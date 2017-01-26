/**
 * @file gpio/mode/mode.c
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
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
  int i;
  int t = 1;
  eGpioNumbering eNum = eNumberingLogical;

  printf ("GPIO get mode test\nTest %d> ", t);
  gpio = xGpioOpen (NULL);
  assert (gpio != 0);
  printf ("Success\n");

  i = iGpioSetNumbering (eNumberingPhysical, gpio);
  assert (i == 0);

  printf ("Test %d> ", ++t);
  i = iGpioGetSize (gpio);
  assert (i >= 0);
  printf ("Success: %d pins available.\n", i);

  do {
    printf ("\nTest %d (%s)> ", ++t, sGpioNumberingToStr(eNum));
    i = iGpioSetNumbering (eNum, gpio);
    assert (i == 0);
    printf ("Success\n");

    iGpioToFront (gpio);
    while  (bGpioHasNext (gpio) ) {
      eGpioMode mode;

      i = iGpioNext (gpio);
      mode = eGpioGetMode (i, gpio);
      assert (mode != eModeError);

      printf ("mode[%02d] -> %s\n", i, sGpioModeToStr (mode) );
    }
  }
  while (eNum++ != eNumberingPhysical);

  printf ("Test %d> ", ++t);
  i = iGpioClose (gpio);
  assert (i == 0);
  printf ("Success\n");
  
  return 0;
}

/* ========================================================================== */
