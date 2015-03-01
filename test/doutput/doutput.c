/*
 * test/doutput/doutput.c
 * @brief
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <assert.h>
#include <sysio/delay.h>
#include <sysio/doutput.h>

/* private variables ======================================================== */
static const xDout xMyLeds[3] = {
  { .num = 0, .act = true },
  { .num = 1, .act = true },
  { .num = 2, .act = true }};

xDoutPort * port;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  assert (iDoutClose (port) == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  port = xDoutOpen (xMyLeds, 3);
  assert(port);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  for (;;) {
    printf("Press Ctrl+C to abort ...\n");

    printf("iDoutSet() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutMapSize(port); led++) {

      assert (iDoutSet (led, port) == 0);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutClear() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutMapSize(port); led++) {

      assert (iDoutClear (led, port) == 0);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutToggle() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutMapSize(port); led++) {

      assert (iDoutToggle (led, port) == 0);
      delay_ms (500);
    }
    for (unsigned led = 0; led < iDoutMapSize(port); led++) {

      assert (iDoutToggle (led, port) == 0);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutSetAll() && iDoutClearAll() test... "); fflush(stdout);
    for (unsigned count = 0; count < 8; count++) {

      assert (iDoutSetAll (port) == 0);
      delay_ms (500);
      assert (iDoutClearAll (port) == 0);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutToggleAll() test... "); fflush(stdout);
    for (unsigned count = 0; count < 16; count++) {

      assert (iDoutToggleAll (port) == 0);
      delay_ms (100);
    }
    printf ("Success.\n");
  }

  return 0;
}

/* ========================================================================== */
