/**
 * @file
 * @brief gpio utility
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
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
using namespace Sysio;

/* constants ================================================================ */
const string authors = "epsilonRT";
const string website = "http://www.epsilonrt.fr/sysio";

/* types ==================================================================== */
typedef void (*func) (int argc, char * argv[]);

/* private variables ======================================================== */
Gpio * gpio = 0;
Pin::Numbering numbering = Pin::NumberingLogical;
int pinnumber = -1;
int connector = -1;
bool physicalNumbering = false;
Pin * pin = 0;
bool debug = false;
bool forceSysFs = false;
int useSysFsBeforeWfi = -1;

/* private functions ======================================================== */
void mode (int argc, char * argv[]);
void pull (int argc, char * argv[]);
void read (int argc, char * argv[]);
void write (int argc, char * argv[]);
void toggle (int argc, char * argv[]);
void blink (int argc, char * argv[]);
void readall (int argc, char * argv[]);
void wfi (int argc, char * argv[]);
void pwm (int argc, char * argv[]); // TODO

Pin * getPin (char * c_str);
void usage ();
void version ();
void sig_handler (int sig);
vector<string> split (const string& s, char seperator);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int opt;
  int ret = 0;
  func do_it;

  const map<string, func> str2func = {
    {"mode", mode},
    {"pull", pull},
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
    while ( (opt = getopt (argc, argv, "gs1dhfv")) != -1) {

      switch (opt) {

        case 'g':
          numbering = Pin::NumberingMcu;
          break;

        case 's':
          numbering = Pin::NumberingSystem;
          break;

        case '1':
          physicalNumbering = true;
          break;

        case 'd':
          debug = true;
          break;

        case 'f':
          forceSysFs = true;
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

    do_it = str2func. at (argv[optind]);
    optind++;

    gpio = new Gpio ();
    gpio->setNumbering (numbering);
    gpio->setDebug (debug);
    gpio->open();

    /* Execute command */
    do_it (argc, argv);
  }
  catch (Exception& e) {

    cerr << __progname << ": " << e.what() << " !" << endl;
    cerr << __progname << " -h for help." << endl;
    ret = -1;
  }
  catch (out_of_range& e) {

    cerr << __progname << ": out of range value (" << e.what() << ")" << endl;
    ret = -1;
  }
  catch (invalid_argument& e) {

    cerr << __progname << ": invalid argument (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (domain_error& e) {

    cerr << __progname << ": bad pin type (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (exception& e) {

    cerr << __progname << ": " << e.what() << " !" << endl;
    ret = -1;
  }
  catch (...) {

    cerr << __progname << "Unknown Exception !!!" << endl;
    ret = -1;
  }

  if (gpio) {

    delete gpio;
  }

  return ret;
}

/* -----------------------------------------------------------------------------
  readall [#connector]
    Output a table of all GPIO pins values.
    The values represent the actual values read if the pin is in input mode,
    or the last value written if the pin is in output mode.
 */
void
readall (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc >= 1) {

    int connector = stoi (string (argv[optind]));
    cout << gpio->connector (connector);
  }
  else {
    for (auto p = gpio->connector().cbegin(); p != gpio->connector().cend(); ++p) {
      // p est une pair: first = numéro et second = connecteur
      cout << p->second << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  mode <pin> <in/out/pwm/off/alt{0..9}>
    Get/Set a pin mode into input, output, off, alt0..9 or pwm mode.
 */
void
mode (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {

    pin = getPin (argv[optind]);

    if (paramc > 1) {
      Pin::Mode m;
      string smode (argv[optind + 1]);
      const map<string, Pin::Mode> str2mode = {
        {"in",    Pin::ModeInput   },
        {"out",   Pin::ModeOutput  },
        {"alt0",  Pin::ModeAlt0    },
        {"alt1",  Pin::ModeAlt1    },
        {"alt2",  Pin::ModeAlt2    },
        {"alt3",  Pin::ModeAlt3    },
        {"alt4",  Pin::ModeAlt4    },
        {"alt5",  Pin::ModeAlt5    },
        {"alt6",  Pin::ModeAlt6    },
        {"alt7",  Pin::ModeAlt7    },
        {"alt8",  Pin::ModeAlt8    },
        {"alt9",  Pin::ModeAlt9    },
        {"off",   Pin::ModeDisabled},
        {"pwm",   Pin::ModePwm     }
      };

      m = str2mode.at (smode);

      // Modification à garder après fermeture !
      gpio->setReleaseOnClose (false);
      pin->setMode (m);
    }
    else {
      // Lecture du mode
      cout << pin->modeName() << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  pull <pin> <up/down/off>
    Get/Set the internal pull-up, pull-down or off controls.
 */
void
pull (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    forceSysFs = false;
    pin = getPin (argv[optind]);

    if (paramc > 1) {
      Pin::Pull p;
      string pmode (argv[optind + 1]);
      const map<string, Pin::Pull> str2pull = {
        {"off",   Pin::PullOff},
        {"up",    Pin::PullUp},
        {"down",  Pin::PullDown}
      };

      p = str2pull.at (pmode);

      // Modification à garder après fermeture !
      gpio->setReleaseOnClose (false);
      pin->setPull (p);
    }
    else {
      // Lecture de la résistance
      cout << pin->pullName() << endl;
    }
  }
}

/* -----------------------------------------------------------------------------
  read <pin>
    Read  the  digital value of the given pin (0 or 1)
 */
void
read (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = getPin (argv[optind]);
    cout << pin->read () << endl;
  }
}

/* -----------------------------------------------------------------------------
  write <pin> <value>
    Write the given value (0 or 1) to the given pin (output).
 */
void
write (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = getPin (argv[optind]);

    value = stoi (string (argv[optind + 1]));
    if ( (value < 0) || (value > 1)) {

      throw Exception (Exception::NotBinaryValue, value);
    }

    if (pin->mode () != Pin::ModeOutput) {

      delete gpio;
      throw Exception (Exception::NotOutputPin, pinnumber);
    }
    // Modification à garder après fermeture !
    gpio->setReleaseOnClose (false);
    pin->write (value);
  }
}

/* -----------------------------------------------------------------------------
  toggle <pin>
    Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output).
 */
void
toggle (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = getPin (argv[optind]);
    if (pin->mode () != Pin::ModeOutput) {

      delete gpio;
      throw Exception (Exception::NotOutputPin, pinnumber);
    }
    // Modification à garder après fermeture !
    gpio->setReleaseOnClose (false);
    pin->toggle ();
  }
}

/* -----------------------------------------------------------------------------
  blink <pin> [period]
    Blinks the given pin on/off (explicitly sets the pin to output)
 */
void
blink (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {
    int period = 1000;

    gpio->setReleaseOnClose (true);

    pin = getPin (argv[optind]);
    if (paramc > 1)    {

      period = stoi (string (argv[optind + 1]));
      if (pin->useSysFs() && period < 1) {
        period = 1;
        cout << "Warning: Pin " << pin->name() << " uses SYSFS, the delay has been set to " << period << " ms (min.) !" << endl;
      }
    }

    pin->setMode (Pin::ModeOutput);

    // sig_handler() intercepte le CTRL+C
    signal (SIGINT, sig_handler);
    signal (SIGTERM, sig_handler);
    cout << "Press Ctrl+C to abort ..." << endl;

    for (;;) {

      pin->toggle ();
      delay_ms (period);
    }
  }
}

/* -----------------------------------------------------------------------------
  wfi <pin> <rising/falling/both> [timeout_ms]
    This set the given pin to the supplied interrupt mode:  rising,  falling  or
    both  then waits  for  the  interrupt  to happen.
    It's a non-busy wait, so does not consume and CPU while it's waiting.
 */
void
wfi (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    const map<string, Pin::Edge>  str2edge = {
      { "none", Pin::EdgeNone },
      { "rising", Pin::EdgeRising },
      { "falling", Pin::EdgeFalling },
      { "both", Pin::EdgeBoth }
    };
    long timeout = -1;
    Pin::Edge e;
    string edge (argv[optind + 1]);

    forceSysFs = true;
    pin = getPin (argv[optind]);
    e = str2edge.at (edge);
    if (paramc > 2) {

      timeout = stol (string (argv[optind + 2]));
    }

    // sig_handler() intercepte le CTRL+C
    signal (SIGINT, sig_handler);
    signal (SIGTERM, sig_handler);

    pin->waitForInterrupt (e, timeout);
    cout << pin->read() << endl;
  }
}

/* -----------------------------------------------------------------------------
  pwm <pin> <value>
 */
void
pwm (int argc, char * argv[]) {
  int paramc = (argc - optind);

  if (paramc < 2) {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = getPin (argv[optind]);
    value = stoi (string (argv[optind + 1]));
    if ( (value < 0) || (value > 1023)) {

      throw Exception (Exception::NotPwmValue, value);
    }

    if (pin->mode () != Pin::ModePwm) {

      delete gpio;
      throw Exception (Exception::NotPwmPin, pinnumber);
    }

    cerr << __FUNCTION__ << "() not yet implemented (TODO) !" << endl;
  }

}

// -----------------------------------------------------------------------------
vector<string>
split (const string& s, char seperator) {
  vector<string> output;
  string::size_type prev_pos = 0, pos = 0;

  while ( (pos = s.find (seperator, pos)) != string::npos) {

    string substring (s.substr (prev_pos, pos - prev_pos));
    output.push_back (substring);
    prev_pos = ++pos;
  }

  output.push_back (s.substr (prev_pos, pos - prev_pos)); // Last word
  return output;
}

// -----------------------------------------------------------------------------
Pin *
getPin (char * c_str) {
  Pin * p;
  string s (c_str);

  string::size_type n = s.find ('.');
  if (n != string::npos) {

    physicalNumbering = true;
  }

  if (!physicalNumbering) {

    pinnumber = stoi (s);
    p = &gpio->pin (pinnumber);
  }
  else {
    vector<string> v = split (s, '.');

    if (v.size() > 1) {

      connector = stoi (v[0]);
      pinnumber = stoi (v[1]);
    }
    else {

      connector = 1;
      pinnumber = stoi (v[0]);
    }
    p = &gpio->connector (connector)->pin (pinnumber);
  }
  p->forceUseSysFs (forceSysFs);
  return p;
}

// -----------------------------------------------------------------------------
void
sig_handler (int sig) {

  if (gpio) {

    if (useSysFsBeforeWfi >= 0) {

      pin->forceUseSysFs (useSysFsBeforeWfi != 0);
    }

    delete gpio;
    cout << endl << "everything was closed.";
  }
  cout << endl << "Have a nice day !" << endl;
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
  cout << "  -s\tUse the System pin numbers rather than SysIo pin numbers." << endl;
  cout << "  -f\tForce to use SysFS interface (/sys/class/gpio)." << endl;
  cout << "  -1\tUse the connector pin numbers rather than SysIo pin numbers." << endl;
  cout << "    \ta number is written in the form C.P, eg: 1.5 denotes pin 5 of connector #1." << endl;
  cout << "  -v\tOutput the current version including the board informations." << endl;
  cout << "  -h\tPrint this message and exit" << endl << endl;

  //       01234567890123456789012345678901234567890123456789012345678901234567890123456789
  cout << "valid commands are :" << endl;
  cout << "  mode <pin> <in/out/off/pwm/alt{0..9}>" << endl;
  cout << "    Get/Set a pin mode into input, output, off, alt0..9 or pwm mode." << endl;
  cout << "  pull <pin> <up/down/off>" << endl;
  cout << "    Get/Set the internal pull-up, pull-down or off controls." << endl;
  cout << "  read <pin>" << endl;
  cout << "    Read the digital value of the given pin (0 or 1)" << endl;
  cout << "  readall [#connector]" << endl;
  cout << "    Output a table of all connectors with pins informations." << endl;
  cout << "  write <pin> <value>" << endl;
  cout << "    Write the given value (0 or 1) to the given pin (output)." << endl;
  cout << "  toggle <pin>" << endl;
  cout << "    Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output)." << endl;
  cout << "  blink <pin> [period]" << endl;
  cout << "    Blinks the given pin on/off (explicitly sets the pin to output)." << endl;
  cout << "  wfi <pin> <rising/falling/both> [timeout_ms]" << endl;
  cout << "    Waits  for  the  interrupt  to happen. It's a non-busy wait." << endl;
  cout << "  pwm <pin> <value>" << endl;
  cout << "    Write a PWM value (0-1023) to the given pin (pwm pin only)." << endl;
}
/* ========================================================================== */
