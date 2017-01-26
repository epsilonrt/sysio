/**
 * @file test/gpio/read/read.c
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/gpio.h>
#include <sysio/delay.h>


/* private variables ======================================================== */
static xGpio * gpio;


/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i, v;
  int64_t m, n;
  int t = 1;
  eGpioNumbering eNum = eNumberingLogical;

  printf ("GPIO Read test\nTest %d> ", t);
  gpio = xGpioOpen (NULL);
  assert (gpio != 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioGetSize (gpio);
  assert (i >= 0);
  printf ("Success: %d pins available.\n", i);

  do {
    m = 0;
    printf ("\nTest %d (%s)> ", ++t, sGpioNumberingToStr(eNum));
    i = iGpioSetNumbering (eNum, gpio);
    assert (i == 0);
    printf ("Success\n");

    printf ("Test %d\n", ++t);
    iGpioToFront(gpio);
    while  (bGpioHasNext (gpio) ) {

      i = iGpioNext (gpio);

      v = iGpioRead (i, gpio);
      assert (i >= 0);
      printf ("read[%02d] -> %d\n", i, v);
      if (v) {
        m |= (1LL << i);
      }
    }
    printf ("Success\n");

    printf ("Test %d> ", ++t);
    n = iGpioReadAll (-1, gpio);
    printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
    assert (n == m);
    printf ("Success\n");
  }
  while (eNum++ != eNumberingPhysical);

  printf ("Test %d> ", ++t);
  i = iGpioClose (gpio);
  assert (i == 0);
  printf ("Success\n");

  return 0;
}

/* ========================================================================== */
