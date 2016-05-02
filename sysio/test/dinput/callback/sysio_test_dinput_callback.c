/**
 * @file test/dinput/callback/callback.c
 * @brief Test de callback
 * 
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <assert.h>
#include <sysio/delay.h>
#include <sysio/dinput.h>


/* private variables ======================================================== */
static const xDin xMyButtons[] = {
  { .num = 3, .act = false, .pull = ePullUp },
  { .num = 4, .act = false, .pull = ePullUp },
  { .num = 5, .act = false, .pull = ePullUp }};

xDinPort * port;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  assert (iDinClose (port) == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
int
iCallback (eDinEdge edge, void *udata) {
  static int counter;
  int p = *((int *) udata);

  printf ("%03d - %c: %d\n", counter++, (edge == eEdgeRising) ? 'P' : 'R', p);
  return 0;
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int id[] = { 1, 2, 3 };

  printf ("Dinput with callback test (monitoring by thread)\n\n");
  
  port = xDinOpen (xMyButtons, 3);
  assert(port);
  
  printf("Press press buttons to test: ");
  for (int i = 0; i < iDinPortSize(port); i++) {

    assert (iDinSetCallback (i, eEdgeBoth, iCallback, &id[i], port) == 0);
    printf ("SW%d ", id[i]);
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("\nCtrl+C to abort ...\n");

  for (;;) {

    // Nothing to do ....
    sleep (-1);
  }

  return 0;
}

/* ========================================================================== */
