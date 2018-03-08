/**
 * @file
 * @author epsilonrt
 * @date 03/08/18
 * @brief Arduino emulation
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <sysio/arduino.h>

using namespace Sysio;
using namespace std;

namespace Arduino {

  Clock * clk;
  Gpio *  gpio;

// -----------------------------------------------------------------------------
  void closeall () {

    if (gpio) {

      gpio->close();
      delete gpio;
      gpio = 0;
    }

    if (clk) {

      delete clk;
      clk = 0;
    }
  }

// -----------------------------------------------------------------------------
// Signal Handler
  void
  sighandler (int sig) {

    closeall();
#ifndef NDEBUG
    cout << endl << "everything was closed." << endl << "Have a nice day !" << endl;
#endif
    exit (EXIT_SUCCESS);
  }

// -----------------------------------------------------------------------------
  void __attribute__ ( (constructor (65535))) begin () {

    gpio = new Gpio;
    clk = new Clock;
    gpio->open();
    // sighandler() intercepts CTRL+C
    signal (SIGINT, sighandler);
    signal (SIGTERM, sighandler);
  }

// -----------------------------------------------------------------------------
  void __attribute__ ( (destructor (65535))) end () {

    closeall();
  }
}

// -----------------------------------------------------------------------------
void pinMode (int n, ArduinoPinMode mode) {
  Pin::Pull p = Pin::PullOff;
  Pin::Mode m = Pin::ModeOutput;

  if (mode != OUTPUT) {

    m = Pin::ModeInput;
    if (mode == INPUT_PULLUP) {
      p = Pin::PullUp;
    }
    else if (mode == INPUT_PULLDOWN) {
      p = Pin::PullDown;
    }
  }

  Arduino::gpio->pin (n).setMode (m);
  Arduino::gpio->pin (n).setPull (p);
}

// -----------------------------------------------------------------------------
void digitalWrite (int n, int value) {

  Arduino::gpio->pin (n).write (value);
}

// -----------------------------------------------------------------------------
void digitalToggle (int n) {

  Arduino::gpio->pin (n).toggle ();
}

// -----------------------------------------------------------------------------
int digitalRead (int n) {

  return Arduino::gpio->pin (n).read();
}

// -----------------------------------------------------------------------------
void attachInterrupt (int n, Pin::Isr isr, ArduinoIntEdge mode) {

  Arduino::gpio->pin (n).attachInterrupt (isr, static_cast<Pin::Edge> (mode));
}

// -----------------------------------------------------------------------------
void detachInterrupt (int n) {

  Arduino::gpio->pin (n).detachInterrupt();
}

// -----------------------------------------------------------------------------
void delay (unsigned long ms) {

  Arduino::clk->delay (ms);
}

// -----------------------------------------------------------------------------
void delayMicroseconds (unsigned long us) {

  Arduino::clk->delayMicroseconds (us);
}

// -----------------------------------------------------------------------------
unsigned long millis() {

  return Arduino::clk->millis();
}

// -----------------------------------------------------------------------------
unsigned long micros() {

  return Arduino::clk->micros();
}
/* ========================================================================== */
