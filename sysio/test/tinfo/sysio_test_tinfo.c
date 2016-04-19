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
#define DEFAULT_BAUDRATE  1200

/* private variables ======================================================== */
static xTinfo * tinfo;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames normales
static int
iFrameCB (struct xTinfo * t, union xTinfoFrame * f) {

  if (f->raw.frame == eTinfoFrameBlue) {

    printf ("\n%s", ctime (&f->blue.time) );
    printf ("--------------------------------<<< Compteur Bleu >>>--------------------------------\n");
    printf ("Adresse du compteur: %012lld\n", f->blue.adco);
    printf ("Intensité souscrite (A): %d\n\n", f->blue.isousc);


    if (f->blue.nph == 3) {

      printf ("Alimentation triphasée\n");
      printf ("Puissance maximale triphasée atteinte (W): %d\n", f->blue.pmax);
    }
    else {

      printf ("Alimentation monophasée\n");
      if (f->blue.flag & eTinfoFlagAdps) {

        printf ("<< Avertissement >> Dépassement De Puissance Souscrite (A): %d\n", f->blue.adps);
      }
    }
    // La puissance apparente n'est pas présente pour tous les compteurs
    if (f->blue.papp > 0) {

      printf ("Puissance apparente (VA): %d\n", f->blue.papp);
    }

    // Intensités sur la ou les phases
    for (int i = 0; i < f->blue.nph; i++) {

      printf ("Intensité instantanée      Ph. %d (A): %d\n", i + 1, f->blue.iinst[i]);
      printf ("Intensité maximale appelée Ph. %d (A): %d\n", i + 1, f->blue.imax[i]);
    }

    printf ("\nOption tarifaire choisie    : %s\n", sTinfoOpTarifToStr (f->blue.optarif) );
    printf ("Période Tarifaire en cours  : %s\n", sTinfoPetcToStr (f->blue.ptec) );
    switch (f->blue.optarif) {

      case eTinfoOpTarifBase:
        printf ("Index (Wh): %ld\n", f->blue.tarif.base.index);
        break;

      case eTinfoOpTarifHc:
        printf ("Plage horaire Heures Creuses: %c\n", f->blue.tarif.hc.horaire);
        printf ("Index Heures Creuses (Wh): %ld\n", f->blue.tarif.hc.index_hc);
        printf ("Index Heures Pleines (Wh): %ld\n", f->blue.tarif.hc.index_hp);
        break;

      case eTinfoOpTarifEjp:
        printf ("Index Heures Normales         (Wh): %ld\n", f->blue.tarif.ejp.index_hn);
        printf ("Index Heures de Pointe Mobile (Wh): %ld\n", f->blue.tarif.ejp.index_hpm);
        break;

      case eTinfoOpTarifTempo:
        printf ("Couleur de demain: %s\n", sTinfoTempoColorToStr (f->blue.tarif.tempo.demain) );
        printf ("Index Heures Creuses, Jours Bleus  (Wh): %ld\n", f->blue.tarif.tempo.index_hcjb);
        printf ("Index Heures Pleines, Jours Bleus  (Wh): %ld\n", f->blue.tarif.tempo.index_hpjb);
        printf ("Index Heures Creuses, Jours Blancs (Wh): %ld\n", f->blue.tarif.tempo.index_hcjw);
        printf ("Index Heures Pleines, Jours Blancs (Wh): %ld\n", f->blue.tarif.tempo.index_hpjw);
        printf ("Index Heures Creuses, Jours Rouges (Wh): %ld\n", f->blue.tarif.tempo.index_hcjr);
        printf ("Index Heures Pleines, Jours Rouges (Wh): %ld\n", f->blue.tarif.tempo.index_hpjr);
        break;

      default:
        break;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réception de trames de changement de période tarifaire (HC...)
static int
iNewPetcCB (struct xTinfo * t, union xTinfoFrame * f) {

  printf ("Changement Période Tarifaire En Cours: %s\n",
          sTinfoPetcToStr (f->blue.ptec) );
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire des signaux Unix (pour l'interruption)
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM) ) {
    if (iTinfoClose (tinfo) ) {

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

    printf ("Usage: %s serial_port [baud]\n", argv[0]);
    exit (EXIT_FAILURE);
  }
  serial_port = argv[1];
  if (argc > 2) {

    baud = atoi (argv[2]);
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
