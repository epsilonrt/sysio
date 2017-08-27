/**
 * @file test/gpio/write/write.c
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  7, 8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24,
  26, 27, 28, 29, 31, 32, 33, 35, 36, 37, 38, 39, 40, -1
};

static const int iPhysListRev1[] = {
  7, 8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24, -1
};

static const int * iPhysList[] = { iPhysListRev1, iPhysListRev2 };

#else
#error Platform not supported
#endif

/* private variables ======================================================== */
static xGpio * gpio;


/* main ===================================================================== */
int
main (int argc, char **argv) {
  char text[5];
  int i;
  int64_t m, n;
  int t = 1;
  const int * p;

  p = iPhysList[pxBoardInfo()->iGpioRev - 1];

  printf ("GPIO Write test\n\n");
  printf ("Test %d> ", t);
  gpio = xGpioOpen (NULL);
  assert (gpio != 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioGetSize (gpio);
  assert (i >= 0);
  printf ("Success: %d pins available.\n", i);

  printf ("\n\n\t\t\t<<<<<<<<<<<<<<<< ATTENTION >>>>>>>>>>>>>>>>\nLes broches {");
  while (*p > 0) {
    printf (" %d,", *p++);
  }
  printf ("} vont être configurées en sortie et testées.\nCes broches doivent "
          "être libres (non connectées à un dispositif extérieur),\nRépondre OUI pour "
          "confirmer le test [n/OUI] ? ");

  fgets (text, 4, stdin);
  if (strcmp (text, "OUI")) {
    return 0;
  }


  printf ("Test %d> ", ++t);
  i = iGpioSetNumbering (eNumberingPhysical, gpio);
  assert (i == 0);
  printf ("Success\n");

  /* Tests write */
  m = 0;
  p = iPhysList[pxBoardInfo()->iGpioRev - 1];
  while (*p >= 0) {
    printf ("\nPin[%d]:\n", *p);

    printf ("\tTest %d> ", ++t);
    i = iGpioSetMode (*p, eModeOutput, gpio);
    assert (i == 0);
    printf ("\tSuccess\n");

    printf ("\tTest %d> ", ++t);
    i = iGpioWrite (*p, false, gpio);
    assert (i == 0);
    i = iGpioRead (*p, gpio);
    assert (i == false);
    printf ("\tSuccess\n");

    printf ("\tTest %d> ", ++t);
    i = iGpioWrite (*p, true, gpio);
    assert (i == 0);
    i = iGpioRead (*p, gpio);
    assert (i == true);
    printf ("\tSuccess\n");

    printf ("\tTest %d> ", ++t);
    i = iGpioToggle (*p, gpio);
    assert (i == 0);
    i = iGpioRead (*p, gpio);
    assert (i == false);
    printf ("\tSuccess\n");

    printf ("\tTest %d> ", ++t);
    i = iGpioToggle (*p, gpio);
    assert (i == 0);
    i = iGpioRead (*p, gpio);
    assert (i == true);
    printf ("\tSuccess\n");

    m |= 1LL << *p;
    p++;
  }

  printf ("\nTest %d> ", ++t);
  n = iGpioReadAll (m, gpio);
  printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
  assert (n == m);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioWriteAll (m, false, gpio);
  assert (i == 0);
  n = iGpioReadAll (m, gpio);
  printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
  assert (n == 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioWriteAll (m, true, gpio);
  assert (i == 0);
  n = iGpioReadAll (m, gpio);
  printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
  assert (n == m);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioToggleAll (m, gpio);
  assert (i == 0);
  n = iGpioReadAll (m, gpio);
  printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
  assert (n == 0);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioToggleAll (m, gpio);
  assert (i == 0);
  n = iGpioReadAll (m, gpio);
  printf ("n=%" PRIx64 " / m=%" PRIx64 " : ", n, m);
  assert (n == m);
  printf ("Success\n");

  printf ("Test %d> ", ++t);
  i = iGpioClose (gpio);
  assert (i == 0);
  printf ("Success\n");

  return 0;
}

/* ========================================================================== */
