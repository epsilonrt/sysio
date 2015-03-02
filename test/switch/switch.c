/*
 * test/switch/switch.c
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
#include <sysio/switch.h>

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
iCallback (unsigned values, unsigned p, void * udata) {
  static int counter;

  printf ("%03d - %s SW: 0x%X (%d)\n", counter++, (const char *) udata, values, p);
  return 0;
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  const char msg[] = "Hello !";

  sw = xDinOpen (xMySwitches, 3);
  assert (sw);
  assert (iSwitchSetCallback (iCallback, sw, msg) == 0);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n");

  for (;;) {

    // Nothing to do ....
    sleep (-1);
  }

  return 0;
}

/* ========================================================================== */
