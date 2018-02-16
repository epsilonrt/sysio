/**
 * @file
 * @brief gpio utility
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/log.h>
#include <sysio/delay.h>
#include <sysio/gpio.h>
#include <sysio/pwm.h>
#include "version.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <exception>
#include <cstdlib>
#include <csignal>
#include <unistd.h>

using namespace std;

/* constants ================================================================ */
const string authors = "epsilonRT";
const string website = "http://www.epsilonrt.fr/sysio";
enum {
  InputMode = 1 << (sizeof (int) * 8 - 1), // Set MSB
  PullMode = InputMode >> 1,
  PwmMode = PullMode >> 1
};

/* types ==================================================================== */
typedef void (*func) (int argc, char * argv[]);

/* private variables ======================================================== */
eGpioNumbering numbering = eNumberingLogical;
Gpio * g;
int pin = -1;

/* private functions ======================================================== */
void mode (int argc, char * argv[]);
void read (int argc, char * argv[]);
void write (int argc, char * argv[]);
void toggle (int argc, char * argv[]);
void blink (int argc, char * argv[]);
void wfi (int argc, char * argv[]); // TODO
void readall (int argc, char * argv[]); // TODO
void pwm (int argc, char * argv[]); // TODO

int toint (char * c_str);
void usage ();
void version ();
void inthandler (int sig);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int opt;
  int ret = 0;
  func f;

  std::map<string, func> cmd_map = {
    {"mode", mode},
    {"read", read},
    {"write", write},
    {"toggle", toggle},
    {"blink", blink},
    {"wfi", wfi},
    {"readall", readall},
    {"pwm", pwm} // TODO
  };

  try {
    /* Traitement options ligne de commande */
    while ( (opt = getopt (argc, argv, "g1phv")) != -1) {

      switch (opt) {

        case 'g':
          numbering = eNumberingMcu;
          break;

        case '1':
          numbering = eNumberingMcu;
          break;

        case 'h':
          usage();
          exit (EXIT_SUCCESS);
          break;

        case 'v':
          version();
          exit (EXIT_SUCCESS);
          break;

        default:
          /* An invalid option has been used, exit with code EXIT_FAILURE */
          exit (EXIT_FAILURE);
          break;
      }
    }

    if (optind >= argc)    {

      throw string ("Command expected");
    }

    f = cmd_map [argv[optind]];
    if (!f) {

      throw string ("Unknown command: ") + argv[optind];
    }
    optind++;

    /* Execute command */
    f (argc, argv);
  }
  catch (int code) {

    cerr << __progname << ": Exception " << code << " !" << endl;
    ret = code;
  }
  catch (string& w) {

    cerr << __progname << ": " << w << " !" << endl;
    ret = -1;
  }
  catch (exception& e) {

    cerr << "ERROR: could not allocate storage\n";
    terminate();
  }
  catch (...) {

    cerr << __progname << "Unknown Exception !!!" << endl;
    ret = -1;
  }

  return ret;
}

/* -----------------------------------------------------------------------------
  mode <pin> <in/out/pwm/up/down/tri>
    Set a pin into input, output or pwm mode.
    Can also use the literals up, down or  tri  to et the internal pull-up,
    pull-down or tristate (off) controls.
    The ALT modes can also be set using alt0, alt1,  ... alt5.
 */
void
mode (int argc, char * argv[]) {

  if ( (argc - optind) <= 1)    {

    throw string ("Arguments expected");
  }
  else {
    int m;
    string smode (argv[optind + 1]);
    std::map<string, int> mmode = {
      {"in",    eModeInput  | InputMode},
      {"out",   eModeOutput | InputMode},
      {"alt0",  eModeAlt0   | InputMode},
      {"alt1",  eModeAlt1   | InputMode},
      {"alt2",  eModeAlt2   | InputMode},
      {"alt3",  eModeAlt3   | InputMode},
      {"alt4",  eModeAlt4   | InputMode},
      {"alt5",  eModeAlt5   | InputMode},
      {"up",    ePullUp     | PullMode},
      {"down",  ePullDown   | PullMode},
      {"tri",   ePullOff    | PullMode},
      {"pwm",   PwmMode}
    };


    pin = toint (argv[optind]);
    m = mmode[smode];
    if (!m) {

      throw string ("Illegal mode: ") + smode;
    }

    g = new Gpio;
    g->setNumbering (numbering);
    g->setReleaseOnClose (false);

    if (m & InputMode) {

      eGpioMode mode = (eGpioMode) (m & ~InputMode);
      g->setMode (pin, mode);
    }

    if (m & PullMode) {

      eGpioPull pull = (eGpioPull) (m & ~PullMode);
      g->setPull (pin, pull);
    }

    if (m & PwmMode) {

      cerr << "Pwm in " << __FUNCTION__ << "() not yet implemented !" << endl;
    }

    delete g;
  }
}

/* -----------------------------------------------------------------------------
  read <pin>
    Read  the  digital  value  of the given pin and print 0 or 1 to represent
    the respective logic levels.
 */
void
read (int argc, char * argv[]) {

  if ( (argc - optind) <= 0)    {

    throw string ("Pin number expected");
  }
  else {

    pin = toint (argv[optind]);
    g = new Gpio;
    g->setNumbering (numbering);

    cout << g->read (pin) << endl;
    delete g;
  }
}

/* -----------------------------------------------------------------------------
  write <pin> <value>
    Write the given value (0 or 1) to the pin.
    You need to set the pin to output mode first.
 */
void
write (int argc, char * argv[]) {

  if ( (argc - optind) <= 1)    {

    throw string ("Arguments expected");
  }
  else {
    int value;

    pin = toint (argv[optind]);
    value = toint (argv[optind + 1]);
    if ( (value < 0) || (value > 1)) {
      ostringstream s;

      s << value << " is not a binary value.";
      throw s.str();
    }

    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != eModeOutput) {
      ostringstream s;

      delete g;
      s << "Pin #" << pin << " is not an output";
      throw s.str();
    }
    g->write (pin, value);

    delete g;
  }
}

/* -----------------------------------------------------------------------------
  toggle <pin>
    Changes the state of a GPIO pin; 0 to 1, or 1 to 0.

    Note unlike the blink command, the pin must be in output mode first.
 */
void
toggle (int argc, char * argv[]) {

  if ( (argc - optind) <= 0)    {

    throw string ("Pin number expected");
  }
  else {

    pin = toint (argv[optind]);
    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != eModeOutput) {

      delete g;
      ostringstream s;
      s << "Pin #" << pin << " is not an output";
      throw s.str();
    }
    g->toggle (pin);

    delete g;
  }
}

/* -----------------------------------------------------------------------------
  blink <pin> [period]
    Blinks the given pin on/off. Press Control-C to exit.

    Note: This command explicitly sets the pin to output mode.
 */
void
blink (int argc, char * argv[]) {

  if ( (argc - optind) <= 0)    {

    throw string ("Pin number expected");
  }
  else {
    int period = 1000;

    pin = toint (argv[optind]);
    if ( (argc - optind) > 1)    {

      period = toint (argv[optind + 1]);
    }
    g = new Gpio;
    g->setNumbering (numbering);
    g->setReleaseOnClose (true);
    g->setMode (pin, eModeOutput);

    // inthandler() intercepte le CTRL+C
    signal (SIGINT, inthandler);
    signal (SIGTERM, inthandler);
    cout << "Press Ctrl+C to abort ..." << endl;

    for (;;) {

      g->toggle (pin);
      delay_ms (period);
    }
  }
}

/* -----------------------------------------------------------------------------
  wfi <pin> <rising/falling/both>
    This set the given pin to the supplied interrupt mode:  rising,  falling  or  both  then
    waits  for  the  interrupt  to happen. It's a non-busy wait, so does not consume and CPU
    while it's waiting.
 */
void
wfi (int argc, char * argv[]) {

  if ( (argc - optind) <= 1)    {

    throw string ("Arguments expected");
  }
  else {
    string edge (argv[optind + 1]);

    pin = toint (argv[optind]);
    cerr << __FUNCTION__ << "() not yet implemented !" << endl;
  }
}

/* -----------------------------------------------------------------------------
  readall
    Output  a  table of all GPIO pins values. The values represent the actual values read if
    the pin is in input mode, or the last value written if the pin is in output mode.

    The readall command is usable with an extension module (via the -x parameter), but  it's
    unable  to determine pin modes or states, so will perform both a digital and analog read
    on each pin in-turn.
 */
void
readall (int argc, char * argv[]) {

  cerr << __FUNCTION__ << "() not yet implemented !" << endl;
}

/* -----------------------------------------------------------------------------
  pwm <pin> <value>
    Write a PWM value (0-1023) to the given pin.
    The pin needs  to  be  put  into  PWM  mode first.
 */
void
pwm (int argc, char * argv[]) {
  
  if ( (argc - optind) <= 1)    {

    throw string ("Arguments expected");
  }
  else {
    int value;

    pin = toint (argv[optind]);
    value = toint (argv[optind + 1]);
    if ( (value < 0) || (value > 1023)) {
      ostringstream s;

      s << value << " is not a correct value";
      throw s.str();
    }

    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != eModePwm) {
      ostringstream s;

      delete g;
      s << "Pin #" << pin << " is not an pwm output";
      throw s.str();
    }

    cerr << __FUNCTION__ << "() not yet implemented !" << endl;
    delete g;
  }

}

// -----------------------------------------------------------------------------
int
toint (char * c_str) {
  int i;
  string str (c_str);
  istringstream is (str);

  if (! (is >> i)) {

    ostringstream s;
    s << str << " is not an integer";
    throw s.str();
  }
  return i;
}

// -----------------------------------------------------------------------------
void
inthandler (int sig) {

  if (pin > 0) {

    if (g->mode (pin) == eModeOutput) {

      g->write (pin, 0);
    }
  }
  delete g;
  cout << endl << "everything was closed." << endl << "Have a nice day !" << endl;
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
version() {

  cout << VERSION_SHORT << endl;
  // TODO Board info
}

// -----------------------------------------------------------------------------
void
usage () {
  cout << "usage : " << __progname << " [ options ] [ command ]  [ parameters ] [ options ]" << endl;;
  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "Allow the user easy access to the GPIO pins." << endl << endl;

  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "valid options are :" << endl;
  cout << "  -g\tUse the SOC pins numbers rather than SysIo pin numbers." << endl;
  cout << "  -1\tUse the physical pin numbers rather than SysIo pin numbers." << endl;
  cout << "  -v\tOutput the current version including the board informations." << endl;
  cout << "  -h\tPrint this message and exit" << endl << endl;

  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "valid commands are :" << endl;
  cout << "  mode <pin> <in/out/up/down/tri/pwm/alt{0..5}>" << endl;
  cout << "  read <pin>" << endl;
  cout << "  write <pin> <value>" << endl;
  cout << "  toggle <pin>" << endl;
  cout << "  blink <pin> [period]" << endl;
  cout << "  wfi <pin> <rising/falling/both>" << endl;
  cout << "  readall" << endl;
  cout << "  pwm <pin> <value>" << endl;
}
/* ========================================================================== */
