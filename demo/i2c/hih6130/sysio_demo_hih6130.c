/*
 * Démonstration utilisation capteur HIH6130
 * Effectue des mesures et les affichent sur le terminal, les valeurs
 * sont affichées de façon tabulaire afin de pouvoir être traitée par un
 * tableur, voilà un exemple d'affichage:
 *  # HIH6130 Demo
 *  # T(oC) H(%)
 *  21.2 53.5
 *  ...
 * Le logiciel KST https://kst-plot.kde.org/ peut être utilisé pour afficher les 
 * mesures sous forme de graphe.
 */
#include <sysio/delay.h>
#include <sysio/hih6130.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>

/* constants ================================================================ */
#define DEFAULT_I2CBUS       "/dev/i2c-1"
#define DEFAULT_I2CADDR      HIH6130_I2CADDR
//#define DEFAULT_I2CADDR      CHIPCAP2_I2CADDR

/* private variables ======================================================== */
xHih6130 * sensor;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM)) {

    if (iHih6130Close (sensor) != 0) {
      perror ("iHih6130Close");
      exit (EXIT_FAILURE);
    }
    printf ("\n# everything was closed.\n# Have a nice day !\n");
    exit (EXIT_SUCCESS);
  }
}

/* main ===================================================================== */
int
main (void) {
  int ret;
  xHih6130Data data;

  printf ("\n# HIH6130 Demo\n");

  // sensor = xHih6130Open (DEFAULT_I2CBUS, HIH6130_I2CADDR);
  sensor = xHih6130Open (DEFAULT_I2CBUS, CHIPCAP2_I2CADDR);
  assert (sensor);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  signal (SIGTERM, vSigIntHandler);
  printf ("# Press Ctrl+C to abort ...\n");
  // Affichage entête
  printf ("# T(°C) H(%%)\n");

  for (;;) {


    // Test 1 - Mesure
    // Démarrage mesure et vérification succès
    ret = iHih6130Start (sensor);
    assert (ret == 0);

    do {

      // Lecture
      ret = iHih6130Read (sensor, &data);
      // Vérif absence d'erreur
      assert (ret >= 0);
    }
    while (ret == HIH6130_BUSY);

    // Affichage mesures
    printf ("%.1f %.1f\n", data.dTemp, data.dHum);
    delay_ms (1000);
  }
  return 0;
}
/* ========================================================================== */
