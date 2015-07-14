/**
 * @file gpio/mode/mode.c
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sysio/gpio.h>
#include <sysio/delay.h>

/* constants ================================================================ */

/* private variables ======================================================== */
static xGpio * gpio;

/* main ===================================================================== */
int
main (int argc, char **argv) {

  gpio = xGpioOpen(NULL);
  assert (gpio != 0);

  for (int i = 0; i < iGpioGetSize (gpio); i++) {
    const char * name;

    eGpioMode mode= eGpioGetMode (i, gpio);
    assert (mode != eModeError);

    switch (mode) {
      case eModeInput:
        name = "INPUT";
        break;
      case eModeOutput:
        name = "OUTPUT";
        break;
      case eModeAlt0:
        name = "ALT0";
        break;
      case eModeAlt1:
        name = "ALT1";
        break;
      case eModeAlt2:
        name = "ALT2";
        break;
      case eModeAlt3:
        name = "ALT3";
        break;
      case eModeAlt4:
        name = "ALT4";
        break;
      case eModeAlt5:
        name = "ALT5";
        break;
      default:
        return -1;
    }
    printf ("pin[%d]\t-> %s\n", i, name);
  }
  assert (iGpioClose(gpio) == 0);
  return 0;
}

/* ========================================================================== */
