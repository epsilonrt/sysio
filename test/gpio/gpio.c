/*
 * gpio.c
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/gpio.h>
#include <sysio/delay.h>

/* constants ================================================================ */
#define LED 0
#define SW  3

/* private variables ======================================================== */
static xGpio * gpio;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  assert (iGpioClose (gpio) == 0);
  printf("\ngpio closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  gpio = xGpioOpen(NULL);
  assert (gpio != 0);
  assert (iGpioMode (LED, eModeOutput, gpio) == 0);

  assert (iGpioMode (SW, eModeInput, gpio) == 0);
  assert (iGpioPull (SW, ePullUp, gpio) == 0);
  assert (iGpioRead (SW, gpio) == true);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

    assert (iGpioToggle (LED, gpio) == 0);
    //delay_us(75);
    delay_ms(100);
  }
  return 0;
}

/* ========================================================================== */
