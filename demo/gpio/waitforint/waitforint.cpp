/*
 * waitforint.cpp
 * 
 * GPIO Pin::waitForInt() usage demo
 * 
 * Toggles the state of a led at each rising or falling edge sent to an input 
 * used as an interrupt request. 
 * The time difference between 2 successive edges is displayed.
 */
#include <iostream>
#include <sysio/clock.h>
#include <sysio/gpio.h>

using namespace std;
using namespace Sysio;

Clock clk; // the clock of our program used for time calculation... 
Gpio g; // our GPIO port

// Be careful !!!
// Check before launching this program that the pin below is well connected to an LED!
const int ledPin = 0; // Header Pin 11: GPIO17 for RPi, GPIOA0 for NanoPi
const int irqPin = 3; // Header Pin 15: GPIO22 for RPi, GPIOA3 for NanoPi

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  bool b;
  unsigned long t1, t2;

  Pin & led = g.pin(ledPin); // led is a reference on pin 11 of the GPIO
  Pin & irq = g.pin(irqPin); // irq is a reference on pin 15 of the GPIO
  
  g.open(); // Do not forget to open the GPIO before changing the pins !
  led.setMode(Pin::ModeOutput); // the led pin is an output
  led.write(0); // turn off the led
  irq.setPull(Pin::PullUp); // activates the pull-up resistor if the input is 
                            // not connected or connected to a push button.
  irq.setMode(Pin::ModeInput);  // the irq pin is an input that will be used 
                                // as an interrupt request.

  cout << "Press Ctrl+C to abort ..." << endl;
  
  t1 = clk.millis(); // first time
  for (;;) { // Infinite loop, Press Ctrl+C to abort ...
    
    b = irq.waitForInt(Pin::EdgeBoth);  // waiting for a rising or falling edge 
                                        // under interruption
    t2 = clk.millis(); // second time
    led.toggle(); // toggle the led state
    cout << t2 - t1 << ":\t" << b << endl;  // prints the time difference between 
                                            // edges and the state of the irq pin.
    t1 = t2; // the new time becomes the first for the next loop
  }

  return 0;
}
/* ========================================================================== */
