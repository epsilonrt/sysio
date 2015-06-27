/*
 * test/gpio/read/read.c
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
  int buttons;

  gpio = xGpioOpen(NULL);
  assert (gpio != 0);

  for (int i = SW1; i <= SW3; i++) {
    assert (iGpioSetMode (i, eModeInput, gpio) == 0);
    assert (iGpioSetPull (i, ePullUp, gpio) == 0);
    assert (iGpioRead (i, gpio) == true);
  }
  assert (iGpioReadAll (SW_ALL, gpio) == SW_ALL);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

    buttons = iGpioReadAll (SW_ALL, gpio);
    assert (buttons >= 0);
    buttons = (buttons ^ SW_ALL) >> SW1;
    printf ("Buttons: 0x%X\r", buttons); fflush(stdout);
  }
  return 0;
}

/* ========================================================================== */
