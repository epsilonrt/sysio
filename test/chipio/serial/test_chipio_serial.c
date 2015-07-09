/*
 * test_chipio_serial.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/chipio/serial.h>

/* constants ================================================================ */
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  printf("Press Ctrl+C to abort ...\n");
  for (;;) {
    // Remplacer les lignes ci-dessous par votre code
    putchar('.'); fflush(stdout);
    delay_ms (1000);
  }

  return 0;
}
/* ========================================================================== */
