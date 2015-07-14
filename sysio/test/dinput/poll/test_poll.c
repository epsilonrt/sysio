/**
 * @file test/dinput/poll/poll.c
 * @brief Test de scrutation de boutons poussoir
 * 
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

/* private variables ======================================================== */
static const xDin xMyButtons[3] = {
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

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int mask;
  port = xDinOpen (xMyButtons, 3);
  assert(port);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  printf("Press Ctrl+C to abort ...\n");
  for (;;) {

    mask =  iDinReadAll (port);
    assert (mask >= 0);
    printf ("Buttons = 0x%02X\r", mask);
    fflush (stdout);
    delay_ms (100);
  }

  return 0;
}

/* ========================================================================== */
