/**
 * @file test/doutput/doutput.c
 * @brief
 *
 * Copyright © 2014 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <sysio/delay.h>
#include <sysio/doutput.h>
#include <sysio/rpi.h>

/* constants ================================================================ */
// -----------------------------------------------------------------------------
#define PORT_SIZE   3
#define PORT_MASK ((1<<PORT_SIZE)-1)

static const xDout xMyPins[PORT_SIZE] = {
  { .num = 0, .act = true },
  { .num = 1, .act = true },
  { .num = 2, .act = true }};

/* private variables ======================================================== */
xDoutPort * port;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  
  assert (iDoutClearAll (port) == 0);
  assert (iDoutClose (port) == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i;
  
  printf ("Doutput test\n\n");
  port = xDoutOpen (xMyPins, PORT_SIZE);
  assert(port);

  assert (iDoutPortSize(port) == PORT_SIZE);
  
  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  for (;;) {
    printf("Press Ctrl+C to abort ...\n");

    printf("iDoutSet() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutPortSize(port); led++) {

      assert (iDoutSet (led, port) == 0);
      assert (iDoutRead (led, port) == true);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutClear() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutPortSize(port); led++) {

      assert (iDoutClear (led, port) == 0);
      assert (iDoutRead (led, port) == false);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutToggle() test... "); fflush(stdout);
    for (unsigned led = 0; led < iDoutPortSize(port); led++) {

      assert (iDoutToggle (led, port) == 0);
      assert (iDoutRead (led, port) == true);
      delay_ms (500);
    }
    for (unsigned led = 0; led < iDoutPortSize(port); led++) {

      assert (iDoutToggle (led, port) == 0);
      assert (iDoutRead (led, port) == false);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutSetAll() && iDoutClearAll() test... "); fflush(stdout);
    for (unsigned count = 0; count < 8; count++) {

      assert (iDoutSetAll (port) == 0);
      assert (iDoutReadAll (port) == PORT_MASK);
      delay_ms (500);
      assert (iDoutClearAll (port) == 0);
      assert (iDoutReadAll (port) == 0);
      delay_ms (500);
    }
    printf ("Success.\n");
    delay_ms (1000);

    printf("iDoutToggleAll() test... "); fflush(stdout);
    i = PORT_MASK;
    for (unsigned count = 0; count < 16; count++) {

      assert (iDoutToggleAll (port) == 0);
      assert (iDoutReadAll (port) == i);
      i ^= PORT_MASK;
      delay_ms (100);
    }
    printf ("Success.\n");
  }

  return 0;
}

/* ========================================================================== */
