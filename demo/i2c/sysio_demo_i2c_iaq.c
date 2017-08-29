/*
 * Démonstration utilisation capteur IAQ
 * Effectue des mesures et les affichent sur le terminal, les valeurs
 * sont affichées de façon tabulaire afin de pouvoir être traitée par un
 * tableur, voilà un exemple d'affichage:
 *  IAQ Demo
 *  Co2(ppm) Voc(ppb)
 *  21.2 53.5
 *  ...
 * Le logiciel KST https://kst-plot.kde.org/ peut être utilisé pour afficher les
 * mesures sous forme de graphe.
 */
#include <sysio/delay.h>
#include <sysio/iaq.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>

/* constants ================================================================ */
#define DEFAULT_I2C_BUS       "/dev/i2c-1"

/* private variables ======================================================== */
xIaq * sensor;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM)) {

    if (iIaqClose (sensor) != 0) {
      perror ("iIaqClose");
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
  bool bIsWarmUp = false;
  xIaqData data;

  printf ("\n# IAQ Demo\n");

  sensor = xIaqOpen (DEFAULT_I2C_BUS, IAQ_I2CADDR);
  assert (sensor);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  signal (SIGTERM, vSigIntHandler);
  printf ("# Press Ctrl+C to abort ...\n");
  // Affichage entête
  printf ("# Co2(ppm) Voc(ppb)\n");

  for (;;) {


    // Test 1 - Mesure
    ret = iIaqRead (sensor, &data);

    switch (ret) {
      case 0:
        printf ("%d %d\n", data.usCo2, data.usTvoc);
        break;
        
      case IAQ_WARMUP:
        if (bIsWarmUp == false) {
          printf ("# Warming, please wait of about 5 minutes...\n");
          bIsWarmUp = true;
        }
        break;
        
      default:
        printf ("iIaqRead() return %d\n", ret);
        break;
    }

    delay_ms (11000);
  }
  return 0;
}
/* ========================================================================== */
