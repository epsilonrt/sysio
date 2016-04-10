/*
 * sysio_demo_tinfo.c
 * @brief Test liaison Télé-information client des compteurs utilisés par ERDF
 * 
 * Affiche les trames reçus
 * 
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/tinfo.h>

/* constants ================================================================ */
#define DEFAULT_BAUDRATE  1200

/* private variables ======================================================== */
static xTinfo * tinfo;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames normales
static int
iFrameCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Trame normale reçue\n");
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames de changement de période tarifaire (HC...)
static int
iNewPetcCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Changement Période Tarifaire En Cours: %s\n",
          sTinfoPetcToStr (f->blue.ptec));
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire des signaux Unix (pour l'interruption)
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM)) {
    if (iTinfoClose (tinfo)) {

      perror ("iTinfoClose");
      exit (EXIT_FAILURE);
    }
    printf ("\neverything was closed.\nHave a nice day !\n");
    exit (EXIT_SUCCESS);
  }
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  const char * serial_port;
  unsigned long baud = DEFAULT_BAUDRATE;
  
  if (argc < 2) {

    printf("Usage: %s serial_port [baud]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  serial_port = argv[1];
  if (argc > 2) {

    baud = atoi(argv[2]);
  }

  // Ouverture de la liaison
  tinfo = xTinfoOpen (serial_port, baud);
  if (tinfo == NULL) {

    perror ("xTinfoOpen");
    exit (EXIT_FAILURE);
  }
  
  // Installation des gestionnaires d'événements
  vTinfoSetCB (tinfo, eTinfoCbFrame, iFrameCB);
  vTinfoSetCB (tinfo, eTinfoCbPetc, iNewPetcCB);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Démo Liaison de télé-information\nAppuyez sur Ctrl+C pour arrêter ...\n");

  for (;;) {
    
    // Scrutation de la liaison
    if (iTinfoPoll (tinfo) < 0) {

      perror ("iTinfoPoll");
    }
  }

  return 0;
}
/* ========================================================================== */
