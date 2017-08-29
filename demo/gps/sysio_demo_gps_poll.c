/**
 * @file
 * @brief Demo module gps par scrutation
 * 
 * Ce programme permet d'afficher les données GPS reçues par gpsd. \n
 * Le programme scrute si des données GPS sont disponibles avant de les afficher.
 * 
 * @warning le serveur gpsd doit être correctement installé et configuré...
 * 
 * Voilà ce que ça donne : \n
 * @code
    Press Ctrl+C to abort ...

    Fix,Date/Time,Latitude,Longitude
    ON,Sat Mar 11 16:29:44 2017,43.1615,5.93072
    ON,Sat Mar 11 16:29:45 2017,43.1615,5.93071
    ON,Sat Mar 11 16:29:46 2017,43.1616,5.9307
    ON,Sat Mar 11 16:29:47 2017,43.1616,5.9307
    ON,Sat Mar 11 16:29:48 2017,43.1616,5.9307
    ON,Sat Mar 11 16:29:48 2017,43.1616,5.9307
    ON,Sat Mar 11 16:29:49 2017,43.1616,5.93071
    ON,Sat Mar 11 16:29:50 2017,43.1616,5.93072
    ON,Sat Mar 11 16:29:51 2017,43.1616,5.93072
    ON,Sat Mar 11 16:29:52 2017,43.1616,5.93071
    ON,Sat Mar 11 16:29:53 2017,43.1616,5.9307
    ^C
    Gps closed.
    Have a nice day !
 * @endcode
 * 
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sysio/gps.h>

/* internal public functions ================================================ */
void vSigIntHandler (int sig);

/* private variables ======================================================== */
// Pointeur sur l'objet GPS
static struct xGps * gps;

/* main ===================================================================== */
int main (void) {

  /* Création d'un objet GPS
   * localhost: adresse IP ou hostname du serveur gpsd (localhost pour machine locale)
   * DEFAULT_GPSD_PORT: port d'écoute du serveur gpsd
   */
  gps = xGpsNew ("localhost", DEFAULT_GPSD_PORT);
  assert(gps);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n");
  printf("\nFix,Date/Time,Latitude,Longitude\n");
  
  while (iGpsError (gps) == 0) {

    if (bGpsDataAvailable (gps)) { // Test si données GPS dispo ?
        timestamp_t t;
        double latitude, longitude;
        bool fix;
        
        // Lecture des données
        fix = bGpsIsFix(gps);
        t = tGpsTime(gps);
        latitude = dGpsLatitude(gps);
        longitude= dGpsLongitude(gps);
        
        // Affichage
        printf ("%s,%s,%g,%g\n", (fix ? "ON" : "OFF"), cGpsTimeToStr(eTimeLocal, t), latitude, longitude);
        
        // Pour un affichage plus complet on peut utiliser iGpsPrintData()
        // iGpsPrintData (gps, eTimeLocal, eDegDdMmSs); 
    }
  }

  vSigIntHandler (SIGQUIT);
  return 0;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
// Fonction de fermeture "propre" appelée par CTRL+C ou à la fin du programme
void
vSigIntHandler (int sig) {

  vGpsDelete (gps);
  printf ("\nGps closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* ========================================================================== */
