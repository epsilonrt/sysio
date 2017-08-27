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

/* constants ================================================================ */
#if defined(BOARD_RASPBERRYPI)
#include <sysio/rpi.h>

#define pxBoardInfo() pxRpiInfo()

/* A+, B+, 2B, 3B, ZERO: 28 broches GPIO dispo */
static const int iPhysListRev3[] = {
  7, 11, 12, 13, 15, 16, 18, 22, 24, 26, 29, 32, 33, 35, 37, 38, 40, -1
};

/* A et B PCB V 2.0: 21 broches GPIO dispo */
static const int iPhysListRev2[] = {
  7, 11, 12, 13, 15, 16, 18, 22, 24, 26, 29, 30, 31, 32, -1
};

/* B PCB V 1.0: 17 broches GPIO dispo */
static const int iPhysListRev1[] = {
  7, 11, 12, 13, 15, 16, 18, 22, 24, 26, -1
};

static const int * iPhysList[] = { iPhysListRev1, iPhysListRev2, iPhysListRev3 };

#elif defined(BOARD_NANOPI_NEO) || defined(BOARD_NANOPI_AIR) || defined(BOARD_NANOPI_M1)
#include <sysio/nanopi.h>

#define pxBoardInfo() pxNanoPiInfo()
static const int iPhysListRev2[] = {
  3, 5, 7, 8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24,
  26, 27, 28, 29, 31, 32, 33, 35, 36, 37, 38, 39, 40, -1
};

static const int iPhysListRev1[] = {
  3, 5, 7, 8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24, -1
};

static const int * iPhysList[] = { iPhysListRev1, iPhysListRev2 };

#else
#error Platform not supported
#endif

/* private variables ======================================================== */
static xGpio * gp;

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i;
  int64_t r = 0, m = 0, a;
  char text[5];
  int t = 1;
  const int * p;
  eGpioMode mode;

  printf ("GPIO Read test\nTest %d> ", t);
  gp = xGpioOpen (NULL);
  assert (gp != 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioGetSize (gp);
  assert (i >= 0);
  printf ("Success: %d pins available.\n", i);

  printf ("\nTest %d (%s)> ", ++t, sGpioNumberingToStr (eNumberingPhysical));
  i = iGpioSetNumbering (eNumberingPhysical, gp);
  assert (i == 0);
  assert (eNumberingPhysical == eGpioGetNumbering (gp));
  printf ("Success\n");

  printf ("Test %d - Setting-up the input pins with pull-up> ", ++t);
  p = iPhysList[pxBoardInfo()->iGpioRev - 1];
  while (*p >= 0) {

    i = iGpioSetMode (*p, eModeInput, gp);
    assert (i == 0);
    mode = eGpioGetMode (*p, gp);
    assert (mode == eModeInput);
    i = iGpioSetPull (*p, ePullUp, gp);
    assert (i == 0);
    p++;
  }
  printf ("Success\n");

  printf ("Test %d - Reading inputs\n", ++t);
  p = iPhysList[pxBoardInfo()->iGpioRev - 1];
  while (*p >= 0) {

    i = iGpioRead (*p, gp);
    assert (i >= 0);
    printf ("[%02d] -> %d\n", *p, i);
    if (i) {
      r |= (1LL << *p);
    }
    m |= (1LL << *p);
    p++;
  }
  assert (m == r);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  a = iGpioReadAll (m, gp);
  printf ("a=%" PRIx64 " / r=%" PRIx64 " : ", a, r);
  assert (a == r);
  printf ("Success\n");

  printf ("Test %d - Setting-up the input pins with pull-down> ", ++t);
  p = iPhysList[pxBoardInfo()->iGpioRev - 1];
  while (*p >= 0) {

    i = iGpioSetPull (*p, ePullDown, gp);
    assert (i == 0);
    p++;
  }
  printf ("Success\n");

  printf ("Test %d - Reading inputs\n", ++t);
  p = iPhysList[pxBoardInfo()->iGpioRev - 1];
  r = 0;
  while (*p >= 0) {

    i = iGpioRead (*p, gp);
    assert (i >= 0);
    printf ("[%02d] -> %d\n", *p, i);
    if (i) {
      r |= (1LL << *p);
    }
    p++;
  }
  assert (r == 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  a = iGpioReadAll (m, gp);
  printf ("a=%" PRIx64 " / r=%" PRIx64 " : ", a, r);
  assert (a == r);
  printf ("Success\n");

  printf ("Do you want individual pin checking ? ");
  fflush (stdin);
  fgets (text, 4, stdin);

  if ( (*text == 'o') || (*text == 'y')) {

    printf ("\nTest %d - Reading inputs\n", ++t);
    p = iPhysList[pxBoardInfo()->iGpioRev - 1];
    while (*p >= 0) {

      i = iGpioSetPull (*p, ePullUp, gp);
      assert (i == 0);
      delay_ms (50);
      i = iGpioRead (*p, gp);
      assert (i == true);
      printf ("[%02d] -> %d ?", *p, i);
      fflush (stdin);
      getchar();
      i = iGpioSetPull (*p, ePullDown, gp);
      assert (i == 0);
      delay_ms (50);
      i = iGpioRead (*p, gp);
      assert (i == false);
      p++;
    }
  }
  putchar ('\n');

  printf ("Test %d> ", ++t);
  i = iGpioClose (gp);
  assert (i == 0);
  printf ("Success\n");

  return 0;
}

/* ========================================================================== */
