/**
 * @file
 * @brief gpio utility
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <sysio/log.h>
#include <sysio/delay.h>
#include <sysio/gpio.h>
#include <sysio/pwm.h>
#include "exception.h"
#include "version.h"

using namespace std;

/* constants ================================================================ */
const string authors = "epsilonRT";
const string website = "http://www.epsilonrt.fr/sysio";
enum {
  PinMode =   1 << 16,
  PullMode =  2 << 16,
  PwmMode =   4 << 16
};

/* types ==================================================================== */
typedef void (*func) (int argc, char * argv[]);

/* private variables ======================================================== */
Gpio * g;
GpioPinNumbering numbering = NumberingLogical;
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
  
  if (getuid() != 0) {
    
    cerr << "You is not a root user !" << endl;
    exit(EXIT_FAILURE);
  }

  try {
    /* Traitement options ligne de commande */
    while ( (opt = getopt (argc, argv, "g1phv")) != -1) {

      switch (opt) {

        case 'g':
          numbering = NumberingMcu;
          break;

        case '1':
          numbering = NumberingMcu;
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

      throw Exception (Exception::CommandExpected);
    }

    f = cmd_map [argv[optind]];
    if (!f) {

      throw Exception (Exception::UnknownCommand, argv[optind]);
    }
    optind++;

    /* Execute command */
    f (argc, argv);
  }
  catch (Exception& e) {

    cerr << __progname << ": " << e.what() << " !" << endl;
    cerr << __progname << " -h for help." << endl;
    ret = -1;
  }
  catch (std::exception& e) {

    cerr << __progname << ": " << e.what() << " !" << endl;
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
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int m = 0;

    pin = toint (argv[optind]);

    if (paramc > 1) {
      string smode (argv[optind + 1]);
      std::map<string, int> mmode = {
        {"in",    ModeInput  | PinMode},
        {"out",   ModeOutput | PinMode},
        {"alt0",  ModeAlt0   | PinMode},
        {"alt1",  ModeAlt1   | PinMode},
        {"alt2",  ModeAlt2   | PinMode},
        {"alt3",  ModeAlt3   | PinMode},
        {"alt4",  ModeAlt4   | PinMode},
        {"alt5",  ModeAlt5   | PinMode},
        {"alt6",  ModeAlt6   | PinMode},
        {"alt7",  ModeAlt7   | PinMode},
        {"alt8",  ModeAlt8   | PinMode},
        {"alt9",  ModeAlt9   | PinMode},
        {"off",   ModeDisabled | PinMode},
        {"up",    ePullUp     | PullMode},
        {"down",  ePullDown   | PullMode},
        {"tri",   ePullOff    | PullMode},
        {"pwm",   PwmMode}
      };

      m = mmode[smode];
      if (!m) {

        throw Exception (Exception::IllegalMode, smode);
      }
    }

    g = new Gpio;
    g->setNumbering (numbering);

    if (paramc > 1) {

      // Modification du mode
      g->setReleaseOnClose (false);

      if (m & PinMode) {

        GpioPinMode mode = (GpioPinMode) (m & ~PinMode);
        g->setMode (pin, mode);
      }

      if (m & PullMode) {

        GpioPinPull pull = (GpioPinPull) (m & ~PullMode);
        g->setPull (pin, pull);
      }

      if (m & PwmMode) {

        cerr << "Pwm in " << __FUNCTION__ << "() not yet implemented !" << endl;
      }

    }
    else {
      // Lecture du mode
      cout << g->pin(pin)->modeName() << endl;
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

  if ( (argc - optind + 1) < 1)    {

    throw Exception (Exception::PinNumberExpected);
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

  if ( (argc - optind + 1) < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = toint (argv[optind]);
    value = toint (argv[optind + 1]);
    if ( (value < 0) || (value > 1)) {

      throw Exception (Exception::NotBinaryValue, value);
    }

    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != ModeOutput) {

      delete g;
      throw Exception (Exception::NotOutputPin, pin);
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

  if ( (argc - optind + 1) < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = toint (argv[optind]);
    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != ModeOutput) {

      delete g;
      throw Exception (Exception::NotOutputPin, pin);
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

  if ( (argc - optind + 1) < 1)    {

    throw Exception (Exception::PinNumberExpected);
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
    g->setMode (pin, ModeOutput);

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

  if ( (argc - optind + 1) < 2)    {

    throw Exception (Exception::ArgumentExpected);
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

  if ( (argc - optind + 1) < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = toint (argv[optind]);
    value = toint (argv[optind + 1]);
    if ( (value < 0) || (value > 1023)) {

      throw Exception (Exception::NotPwmValue, value);
    }

    g = new Gpio;
    g->setNumbering (numbering);
    if (g->mode (pin) != ModePwm) {

      delete g;
      throw Exception (Exception::NotPwmPin, pin);
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

    if (g->mode (pin) == ModeOutput) {

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
  cout << "  mode <pin> <in/out/up/down/tri/pwm/alt{0..5}/off>" << endl;
  cout << "  read <pin>" << endl;
  cout << "  write <pin> <value>" << endl;
  cout << "  toggle <pin>" << endl;
  cout << "  blink <pin> [period]" << endl;
  cout << "  wfi <pin> <rising/falling/both>" << endl;
  cout << "  readall" << endl;
  cout << "  pwm <pin> <value>" << endl;
}
/* ========================================================================== */
