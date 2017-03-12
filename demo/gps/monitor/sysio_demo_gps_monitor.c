/**
 * @file
 * @brief  Moniteur de réception gps par interruption
 * 
 * Ce programme permet d'afficher les données GPS reçues par gpsd. \n
 * Le programme gère la réception par interruption, une fonction de gestion
 * (callback) gère la réception de message de fix et une autre les erreurs.
 * 
 * @warning le serveur gpsd doit être correctement installé et configuré...
 *
 * Voilà ce que ça donne : \n
 * @code
    ./sysio_demo_gps_monitor localhost
    SysIo GPS monitor
    Press Ctrl+C to abort ...

    #       Fix   Mode  #Sat Date/Time                    Lat.                Long.            Alt.   Track    Speed Climb
    0001    ON    3D    7    Sun Mar 12 11:34:51 2017     43 09' 41.526"      5 55' 50.376"    111    229.33    0    0
    0002    ON    3D    7    Sun Mar 12 11:34:52 2017     43 09' 41.526"      5 55' 50.376"    111    229.33    0    0
    0003    ON    3D    7    Sun Mar 12 11:34:53 2017     43 09' 41.526"      5 55' 50.376"    111    229.33    0    0
    0004    ON    3D    7    Sun Mar 12 11:34:54 2017     43 09' 41.526"      5 55' 50.376"    111    229.33    0    0
    0005    ON    3D    7    Sun Mar 12 11:34:55 2017     43 09' 41.526"      5 55' 50.376"    111    229.33    0    0
    ^C
    Gps closed.
    Have a nice day !
 * @endcode
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <stdio.h>
#include <sysio/gps.h>

/* internal public functions ================================================ */
void vSigIntHandler (int sig);
void vFixCallback (struct xGps * gps);
void vErrorCallback (int iError);

/* private variables ======================================================== */
// Pointeur sur l'objet GPS
static struct xGps * gps;

/* main ===================================================================== */
int main (int argc, char **argv) {
  int ret;
  char * port;

  if (argc < 2) {

    printf ("Usage: %s host [port]\n", argv[0]);
    exit (EXIT_FAILURE);
  }
  if (argc > 2) {

    port = argv[2];
  }
  else {
    
    port = DEFAULT_GPSD_PORT;
  }

  /* Création d'un objet GPS
   * argv[1]: adresse IP ou hostname du serveur gpsd (localhost pour machine locale)
   * port: port d'écoute du serveur gpsd
   */
  gps = xGpsNew (argv[1], port);
  assert (gps);

  // On installe les gestionnaires de fix et d'erreur
  ret = iGpsSetFixCallback (gps, vFixCallback);
  assert (ret == 0);
  ret = iGpsSetErrorCallback (gps, vErrorCallback);
  assert (ret == 0);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf ("SysIo GPS monitor\nPress Ctrl+C to abort ...\n");

  for (;;) {
    // Nous n'avons rien à faire ici, les gestionnaires font le job...
    // Cela libère la boucle principale pour faire autre chose...
  }

  vSigIntHandler (SIGQUIT);
  return 0;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
// Gestionnaire de fix
void
vFixCallback (struct xGps * gps) {

  // Affichage
  iGpsPrintData (gps, eTimeLocal, eDegDdMmSs);
}

// -----------------------------------------------------------------------------
// Gestionnaire d'erreur
void
vErrorCallback (int iError) {

  fprintf (stderr, "Gps Error: %d\n", iError);
  vSigIntHandler (SIGQUIT);
}

// -----------------------------------------------------------------------------
// Fonction de fermeture "propre" appelée par CTRL+C ou à la fin du programme
void
vSigIntHandler (int sig) {

  vGpsDelete (gps);
  printf ("\nGps closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* ========================================================================== */
