/**
 * @file test/dinput/group/group.c
 * @brief Test d'entrées groupées
 * 
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
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
static const xDin xMySwitches[] = {
  { .num = 3, .act = false, .pull = ePullUp },
  { .num = 4, .act = false, .pull = ePullUp },
  { .num = 5, .act = false, .pull = ePullUp }
};

xDinPort * sw;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  assert (iDinClose (sw) == 0);
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
int
iCallback (unsigned values, unsigned p, eDinEdge edge, void * udata) {
  static int counter;

  printf ("%03d - %s SW: 0x%X (%c%d)\n", counter++, (const char *) udata,
          values, (edge == eEdgeRising) ? 'P' : 'R', p + 1);
  return 0;
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i;
  char msg[] = "Hello !";

  printf ("Dinput with group callback test (monitoring by thread)\n\n");
  sw = xDinOpen (xMySwitches, 3);
  assert (sw);
  i = iDinSetGrpCallback (eEdgeBoth, iCallback, msg, sw);
  assert (i == 0);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf("Press press buttons to test !\n");
  printf ("Ctrl+C to abort ...\n");

  for (;;) {

    // Nothing to do ....
    sleep (-1);
  }

  return 0;
}

/* ========================================================================== */
