/*
 * test/dinput/callback/callback.c
 * @brief Test de callback
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
#include <sysio/dinput.h>

/* constants ================================================================ */
#define BUTTONS 3

/* private variables ======================================================== */
static const xDin xMyButtons[BUTTONS] = {
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
  int id[BUTTONS] = { 0, 1, 2 };

  port = xDinOpen (xMyButtons, 3);
  assert(port);
  for (int i = 0; i < BUTTONS; i++) {

    assert (iDinSetCallback (i, eEdgeBoth, iCallback, &id[i], port) == 0);
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

    // Nothing to do ....
    sleep (-1);
  }

  return 0;
}

/* ========================================================================== */
