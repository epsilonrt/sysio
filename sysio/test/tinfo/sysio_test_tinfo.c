/*
 * sysio_test_tinfo.c
 * @brief Test liaison Télé-information client des compteurs utilisés par ERDF
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
#define PORT      "/dev/ttyUSB0"
#define BAUDRATE  1200

/* private variables ======================================================== */
static xTinfo * tinfo;

/* private functions ======================================================== */
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
// Gestionnaire de réception de trames de dépassement de puissance souscrite
static int
iAdpsCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Avertissement de Dépassement de Puissance Souscrite: ");
  if (f->blue.flag & eTinfoFlagTri) {
    for (int i = 0; i < 3; i++) {
      printf ("Ph%d: %d A\t", i + 1, f->blue_short.adir[i]);
    }
    putchar('\n');
  }
  else {

    printf ("%d A\n", f->blue.adps);
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames de changement du mot d'état compteur
static int
iNewMotEtatCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Nouveau Mot d'état: 0x%06X\n",  f->blue.motdetat);
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames de changement de couleur du jour Tempo (demain)
static int
iNewTempoColorCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Changement couleur demain: %s\n",
          sTinfoTempoColorToStr (f->blue.tarif.tempo.demain));
  return 0;
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  // Ouverture de la liaison
  tinfo = xTinfoOpen (PORT, BAUDRATE);
  if (tinfo == NULL) {

    perror ("xTinfoOpen");
    exit (EXIT_FAILURE);
  }
  
  // Installation des gestionnaires d'événements
  vTinfoSetCB (tinfo, eTinfoCbFrame, iFrameCB);
  vTinfoSetCB (tinfo, eTinfoCbAdps, iAdpsCB);
  vTinfoSetCB (tinfo, eTinfoCbPetc, iNewPetcCB);
  vTinfoSetCB (tinfo, eTinfoCbMotEtat, iNewMotEtatCB);
  vTinfoSetCB (tinfo, eTinfoCbTempo, iNewTempoColorCB);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Test Liaison de télé-information\nAppuyez sur Ctrl+C pour arrêter ...\n");

  for (;;) {
    
    // Scrutation de la liaison
    if (iTinfoPoll (tinfo) < 0) {

      perror ("iTinfoPoll");
    }
  }

  return 0;
}
/* ========================================================================== */
