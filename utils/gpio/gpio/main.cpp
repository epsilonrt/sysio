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

/* constants ================================================================ */
const string authors = "epsilonRT";
const string website = "http://www.epsilonrt.fr/sysio";

/* types ==================================================================== */
typedef void (*func) (int argc, char * argv[]);

/* private variables ======================================================== */
Gpio * gpio = 0;
GpioPinNumbering numbering = NumberingLogical;
int pinnumber = -1;
int connector = -1;
bool physicalNumbering = false;
GpioPin * pin = 0;
bool debug = false;

/* private functions ======================================================== */
void mode (int argc, char * argv[]);
void pull (int argc, char * argv[]);
void read (int argc, char * argv[]);
void write (int argc, char * argv[]);
void toggle (int argc, char * argv[]);
void blink (int argc, char * argv[]);
void readall (int argc, char * argv[]);
void wfi (int argc, char * argv[]); // TODO
void pwm (int argc, char * argv[]); // TODO

GpioPin * getPin (char * c_str);
void usage ();
void version ();
void sig_handler (int sig);
std::vector<std::string> split (const std::string& s, char seperator);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int opt;
  int ret = 0;
  func do_it;

  std::map<string, func> cmd_map = {
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
    while ( (opt = getopt (argc, argv, "gs1dhv")) != -1) {

      switch (opt) {

        case 'g':
          numbering = NumberingMcu;
          break;

        case 's':
          numbering = NumberingSystem;
          break;

        case '1':
          physicalNumbering = true;
          break;

        case 'd':
          debug = true;
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

    do_it = cmd_map [argv[optind]];
    if (!do_it) {

      throw Exception (Exception::UnknownCommand, argv[optind]);
    }
    optind++;

    gpio = new Gpio;
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
  catch (std::out_of_range& e) {

    cerr << __progname << ": pin number " << pinnumber;
    if (physicalNumbering) {
      cerr  << " or connector " << connector;
    }
    cerr   << " out of range !" << endl;
    cerr   << e.what() << endl;
    ret = -1;
  }
  catch (std::invalid_argument& e) {

    cerr << __progname << ": invalid argument (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (std::domain_error& e) {

    cerr << __progname << ": bad pin type (" << e.what() << ") !" << endl;
    ret = -1;
  }
  catch (std::exception& e) {

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
  readall
    Output a table of all GPIO pins values.
    The values represent the actual values read if the pin is in input mode,
    or the last value written if the pin is in output mode.
 */
void
readall (int argc, char * argv[]) {

  for (auto p = gpio->connector().cbegin(); p != gpio->connector().cend(); ++p) {
    // p est une std::pair: first = numéro et second = connecteur
    cout << p->second << endl;
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
      GpioPinMode m;
      string smode (argv[optind + 1]);
      std::map<string, GpioPinMode> str2mode = {
        {"in",    ModeInput   },
        {"out",   ModeOutput  },
        {"alt0",  ModeAlt0    },
        {"alt1",  ModeAlt1    },
        {"alt2",  ModeAlt2    },
        {"alt3",  ModeAlt3    },
        {"alt4",  ModeAlt4    },
        {"alt5",  ModeAlt5    },
        {"alt6",  ModeAlt6    },
        {"alt7",  ModeAlt7    },
        {"alt8",  ModeAlt8    },
        {"alt9",  ModeAlt9    },
        {"off",   ModeDisabled},
        {"pwm",   ModePwm     }
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
    pin = getPin (argv[optind]);

    if (paramc > 1) {
      GpioPinPull p;
      string pmode (argv[optind + 1]);
      std::map<string, GpioPinPull> str2pull = {
        {"off",   PullOff},
        {"up",    PullUp},
        {"down",  PullDown}
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

  if ( (argc - optind + 1) < 1)    {

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

  if ( (argc - optind + 1) < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = getPin (argv[optind]);

    value = stoi (std::string (argv[optind + 1]));
    if ( (value < 0) || (value > 1)) {

      throw Exception (Exception::NotBinaryValue, value);
    }

    if (pin->mode () != ModeOutput) {

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

  if ( (argc - optind + 1) < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {

    pin = getPin (argv[optind]);
    if (pin->mode () != ModeOutput) {

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

  if ( (argc - optind + 1) < 1)    {

    throw Exception (Exception::PinNumberExpected);
  }
  else {
    int period = 1000;

    gpio->setReleaseOnClose (true);

    pin = getPin (argv[optind]);
    if ( (argc - optind) > 1)    {

      period = stoi (std::string (argv[optind + 1]));
    }

    pin->setMode (ModeOutput);

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

    pin = getPin (argv[optind]);
    cerr << __FUNCTION__ << "() not yet implemented (TODO) !" << endl;
  }
}

/* -----------------------------------------------------------------------------
  pwm <pin> <value>
 */
void
pwm (int argc, char * argv[]) {

  if ( (argc - optind + 1) < 2)    {

    throw Exception (Exception::ArgumentExpected);
  }
  else {
    int value;

    pin = getPin (argv[optind]);
    value = stoi (std::string (argv[optind + 1]));
    if ( (value < 0) || (value > 1023)) {

      throw Exception (Exception::NotPwmValue, value);
    }

    if (pin->mode () != ModePwm) {

      delete gpio;
      throw Exception (Exception::NotPwmPin, pinnumber);
    }

    cerr << __FUNCTION__ << "() not yet implemented (TODO) !" << endl;
  }

}

// -----------------------------------------------------------------------------
std::vector<std::string>
split (const std::string& s, char seperator) {
  std::vector<std::string> output;
  std::string::size_type prev_pos = 0, pos = 0;

  while ( (pos = s.find (seperator, pos)) != std::string::npos) {

    std::string substring (s.substr (prev_pos, pos - prev_pos));
    output.push_back (substring);
    prev_pos = ++pos;
  }

  output.push_back (s.substr (prev_pos, pos - prev_pos)); // Last word
  return output;
}

// -----------------------------------------------------------------------------
GpioPin *
getPin (char * c_str) {
  GpioPin * p;
  string s (c_str);

  string::size_type n = s.find ('.');
  if (n != std::string::npos) {

    physicalNumbering = true;
  }

  if (!physicalNumbering) {

    pinnumber = stoi (s);
    p = gpio->pin (pinnumber);
  }
  else {
    vector<std::string> v = split (s, '.');

    if (v.size() > 1) {

      connector = stoi (v[0]);
      pinnumber = stoi (v[1]);
    }
    else {

      connector = 1;
      pinnumber = stoi (v[0]);
    }
    p = gpio->connector (connector)->pin (pinnumber).get();
  }
  return p;
}

// -----------------------------------------------------------------------------
void
sig_handler (int sig) {

  if (gpio) {

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
  cout << "  readall" << endl;
  cout << "    Output a table of all connectors with pins informations." << endl;
  cout << "  write <pin> <value>" << endl;
  cout << "    Write the given value (0 or 1) to the given pin (output)." << endl;
  cout << "  toggle <pin>" << endl;
  cout << "    Changes the state of a GPIO pin; 0 to 1, or 1 to 0 (output)." << endl;
  cout << "  blink <pin> [period]" << endl;
  cout << "    Blinks the given pin on/off (explicitly sets the pin to output)." << endl;
  cout << "  wfi <pin> <rising/falling/both>" << endl;
  cout << "    Waits  for  the  interrupt  to happen. It's a non-busy wait." << endl;
  cout << "  pwm <pin> <value>" << endl;
  cout << "    Write a PWM value (0-1023) to the given pin (pwm pin only)." << endl;
}
/* ========================================================================== */
