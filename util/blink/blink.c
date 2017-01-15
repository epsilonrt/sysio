/**
 * @file blink.c
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
#include <sysio/string.h>

/* constants ================================================================ */
// -----------------------------------------------------------------------------
#define PORT_SIZE   1
#define PORT_MASK ((1<<PORT_SIZE)-1)

static xDout xMyPins[PORT_SIZE] = {
  { .num = 0, .act = true }};

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
  long i;
  
	if (argc < 2) {

		printf ("Usage: %s pin\n", __progname);
		exit (EXIT_FAILURE);
	}

	if (iStrToLong (argv[1], &i, 0) != 0) {
		
		printf ("Usage: %s pin\n", __progname);
		exit (EXIT_FAILURE);
	}

  xMyPins[0].num = i;
	port = xDoutOpen (xMyPins, PORT_SIZE);
  assert(port);

  
  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

		iDoutToggle (0, port);
    delay_ms (1000);
  }

  return 0;
}

/* ========================================================================== */
