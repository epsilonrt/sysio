/*
 * sysio_demo_gp2.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/gp2.h>

/* constants ================================================================ */
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
static xG2pSensor * gp2;

/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  if (iGp2Close (gp2) != 0) {
    perror ("iGp2Close");
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  xG2pSetting setting = {
    .dV1 = 300,
    .dD1 = 0,
    .dV2 = 3000,
    .dD2 = 400
  };
  int v, d;

  gp2 = xGp2Open ("/dev/i2c-1", GP2I2C_I2CADDR, &setting );
  if (gp2 == NULL) {
    perror ("xGp2Open");
    exit (EXIT_FAILURE);
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");
  printf ("v(mV),d(ug/m3)\n");
  for (;;) {

    v = iGp2ReadVoltage (gp2);
    if (v >= 0) {
      d = iGp2VoltageToDensity (v, &setting);
      printf ("%u,%u\n", v, d);
    }
    delay_ms (11000);
  }

  return 0;
}
/* ========================================================================== */
