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
#define LED1        0
#define LED2        1
#define LED3        2
#define LED_ALL     ((1 << LED1) | (1 << LED2) | (1 << LED3))
#define SW1         3
#define SW2         4
#define SW3         5
#define SW_ALL      ((1 << SW1) | (1 << SW2) | (1 << SW3))

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
  assert (iGpioSetMode (LED1, eModeOutput, gpio) == 0);
  assert (iGpioSetMode (LED2, eModeOutput, gpio) == 0);
  assert (iGpioSetMode (LED3, eModeOutput, gpio) == 0);

  assert (iGpioSetMode (SW1, eModeInput, gpio) == 0);
  assert (iGpioSetMode (SW2, eModeInput, gpio) == 0);
  assert (iGpioSetMode (SW3, eModeInput, gpio) == 0);
  assert (iGpioSetPull (SW1, ePullUp, gpio) == 0);
  assert (iGpioSetPull (SW2, ePullUp, gpio) == 0);
  assert (iGpioSetPull (SW3, ePullUp, gpio) == 0);
  assert (iGpioRead (SW1, gpio) == true);
  assert (iGpioRead (SW2, gpio) == true);
  assert (iGpioRead (SW3, gpio) == true);
  assert (iGpioReadAll (SW_ALL, gpio) == SW_ALL);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

    for (int i = 0; i < 3; i++) {
      assert (iGpioWrite (i, true, gpio) == 0);
      delay_ms(200);
    }
    delay_ms(1000);

    for (int i = 0; i < 3; i++) {
      assert (iGpioWrite (i, false, gpio) == 0);
      delay_ms(200);
    }
    delay_ms(1000);

    for (int i = 0; i < 3; i++) {
      assert (iGpioToggle (i, gpio) == 0);
      delay_ms(200);
      assert (iGpioToggle (i, gpio) == 0);
      delay_ms(200);
    }
    delay_ms(1000);

    for (int i = 0; i < 8; i++) {
      assert (iGpioWriteAll (LED_ALL, true, gpio) == 0);
      delay_ms(200);
      assert (iGpioWriteAll (LED_ALL, false, gpio) == 0);
      delay_ms(200);
    }
    delay_ms(1000);

    for (int i = 0; i < 80; i++) {
      assert (iGpioToggleAll (LED_ALL, gpio) == 0);
      delay_ms(20);
      assert (iGpioToggleAll (LED_ALL, gpio) == 0);
      delay_ms(20);
    }
    delay_ms(1000);

  }
  return 0;
}

/* ========================================================================== */
