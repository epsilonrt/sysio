// Blink example
// by epsilonrt <https://github.com/epsilonrt>

// This program flashes an LED connected to a GPIO pin with a frequency of 2 Hz.

// Created 6 March 2018

// This example code is in the public domain.
#include <iostream>
#include <sysio/clock.h>
#include <sysio/gpio.h>

using namespace std;
using namespace Sysio;

Clock clk; // the clock of our program used for time calculation... 
Gpio g; // our GPIO port

// !!! Be careful !!!
// Check before launching this program that the pin below is well connected to an LED!
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {

  Pin & led = g.pin(ledPin); // led is a reference on pin 11 of the GPIO
  
  g.open(); // Do not forget to open the GPIO before changing the pins !
  led.setMode(Pin::ModeOutput); // the led pin is an output
  led.write(0); // turn off the led

  // Replace the lines below with your code.
  cout << "Press Ctrl+C to abort ..." << endl;
  for (;;) {

    led.toggle(); // blinking led
    clk.delay (1000);
    cout << '.' << flush; // print one point per second
  }

  return 0;
}
/* ========================================================================== */
