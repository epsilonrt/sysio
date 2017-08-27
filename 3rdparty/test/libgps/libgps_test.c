/**
 * @file 
 * @brief Test d'accès au serveur gpsd par libgps
 * 
 * Il faut installer libgps (et gpsd...), sur debian:
 * @code
 * sudo apt-get install libgps-dev
 * @endcode
 *
 * Résultat:
 * @code
    Starting poll...

    #       Date/Time                   Lat.       Long.     Alt.     Track     Speed     Climb
    0000    Sat Mar 11 11:06:17 2017    43.1616    5.9307    110.7    238.71    0.149     0
    0001    Sat Mar 11 11:06:18 2017    43.1616    5.9307    110.3    238.71    0.37      -0.4
    0002    Sat Mar 11 11:06:18 2017    43.1616    5.9307    110.3    238.71    0.37      -0.4
    0003    Sat Mar 11 11:06:19 2017    43.1616    5.9307    109.7    238.71    0.314     -0.6
    0004    Sat Mar 11 11:06:20 2017    43.1616    5.9307    109.5    238.71    0.432     -0.2
    0005    Sat Mar 11 11:06:21 2017    43.1616    5.9307    109.3    238.71    0.082     -0.2
    0006    Sat Mar 11 11:06:22 2017    43.1616    5.9307    109.1    238.71    0.509     -0.2
    0007    Sat Mar 11 11:06:23 2017    43.1616    5.9307    109.1    238.71    0         0
    0008    Sat Mar 11 11:06:23 2017    43.1616    5.9307    109.1    238.71    0         0
    0009    Sat Mar 11 11:06:24 2017    43.1616    5.9307    109      238.71    0.154     -0.1
    0010    Sat Mar 11 11:06:25 2017    43.1616    5.9307    108.8    238.71    0.365     -0.2
    0011    Sat Mar 11 11:06:26 2017    43.1616    5.9307    108.8    238.71    0.005     0
    0012    Sat Mar 11 11:06:27 2017    43.1616    5.9307    108.5    238.71    0.072     -0.3
    0013    Sat Mar 11 11:06:28 2017    43.1616    5.9307    108.5    238.71    0         0
    0014    Sat Mar 11 11:06:28 2017    43.1616    5.9307    108.5    238.71    0         0
    ^C
    Gps closed.
    Have a nice day !

 * @endcode
 * 
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gps.h>
#include <time.h>
#include <pthread.h>

/* internal public functions ================================================ */
void vCloseAll (void);
void * pvPollLoop (void * p_data);
void vGpsPrintData(struct gps_data_t *xGpsData);
void vSigIntHandler (int sig);

/* private variables ======================================================== */
static struct gps_data_t xGpsData;
static pthread_t xPollThread;
static pthread_mutex_t xPollMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t xErrorMutex = PTHREAD_MUTEX_INITIALIZER;

// -----------------------------------------------------------------------------
int main(void) {

  // Connection au serveur gpsd
  if(gps_open("localhost", DEFAULT_GPSD_PORT, &xGpsData) < 0) {

    perror("Connexion au serveur impossible !");
    exit(EXIT_FAILURE);
  }

  // Enregistre notre programme pour une scrutation des données GPS
  gps_stream (&xGpsData, WATCH_ENABLE | WATCH_JSON, NULL);

  if (pthread_create (&xPollThread, NULL, pvPollLoop, "Starting poll...") != 0) {

    fprintf (stderr, "Erreur: pthread_create !\n");
    vCloseAll();
    exit(EXIT_FAILURE);
  }

  // Installs the CTRL + C signal handler
  signal(SIGINT, vSigIntHandler);

  pthread_mutex_lock (&xPollMutex);
  pthread_mutex_lock (&xErrorMutex);

  while (pthread_mutex_trylock (&xErrorMutex) != 0) {

    if (pthread_mutex_trylock (&xPollMutex) == 0) {

      vGpsPrintData(&xGpsData);
    }
  }

  pthread_join (xPollThread, NULL);
  vCloseAll();
  return 0;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
// Fonction de scrutation qui tourne dans un thread
void *
pvPollLoop (void * p_data) {

  if (p_data) {
    printf ("%s\n", (const char *)p_data);
  }

  for (;;) {

    // Scrute si des données sont exploitables avec un tiemout de 500 ms
    if (gps_waiting(&xGpsData, 5000000) != 0) {

      // des données sont dispo, il faut les lire !
      if(gps_read(&xGpsData) == -1) {

        // Erreur de lecture !
        perror("Erreur: gps_read !");
        pthread_mutex_unlock (&xErrorMutex);
        break;
      }
      else {

        // Lecture correcte
        if(xGpsData.status > 0) {

          // Test si la position renvoyée est différente de NaN (Not A Number)
          if(xGpsData.fix.longitude != xGpsData.fix.longitude ||
                        xGpsData.fix.altitude != xGpsData.fix.altitude) {

            printf("Erreur: Impossible d'avoir un fix !\n");
            pthread_mutex_unlock (&xErrorMutex);
            break;
          }
          else {

            // Nous avons un fix !
            pthread_mutex_unlock (&xPollMutex);
          }
        }
      }
    }
    else {

      // gps_waiting a renvoyé 0 ce qui indique que nous ne sommes plus enregistré
      // pour mise à jour, il faut relancer la scrutation
      gps_stream (&xGpsData, WATCH_ENABLE | WATCH_JSON, NULL);
    }
  }
  return NULL;
}

// -----------------------------------------------------------------------------
// Affiche les données
void
vGpsPrintData (struct gps_data_t *xGpsData){
  static int iCount = 0;

  if (xGpsData->set != 0) {

    if ((iCount % 24) == 0) {

      printf("\n#\tDate/Time\t\t\tLat.\tLong.\tAlt.\tTrack\tSpeed\tClimb\n");
    }
    printf("%04d\t", iCount++);

    if (xGpsData->set & TIME_SET) {
      struct tm   utc;
      time_t t = xGpsData->fix.time;
      char buf[26];

      gmtime_r (&t, &utc);
      asctime_r(&utc, buf);
      buf[strlen(buf) - 1] = 0; // retrait \n final

      printf ("%s\t", buf);
    }
    if (xGpsData->set & LATLON_SET) {

      printf( "%g\t%g\t", xGpsData->fix.latitude, xGpsData->fix.longitude);
    }
    else
      printf ("-------\t-------\t");

    if (xGpsData->set & ALTITUDE_SET) {

        printf( "%g\t", xGpsData->fix.altitude);
    }
    else
      printf ("-----\t");

    if (xGpsData->set & TRACK_SET) {

      printf( "%g\t", xGpsData->fix.track);
    }
    else
      printf ("-----\t");

    if (xGpsData->set & SPEED_SET) {

      printf( "%g\t", xGpsData->fix.speed);
    }
    else
      printf ("-----\t");
    if (xGpsData->set & CLIMB_SET) {

      printf( "%g", xGpsData->fix.climb);
    }
    else
      printf ("-----\t");
    putchar('\n');
  }
}

// -----------------------------------------------------------------------------
void vCloseAll (void) {

  // On ferme proprement
  gps_stream(&xGpsData, WATCH_DISABLE, NULL);
  gps_close(&xGpsData);
  printf("\nGps closed.\nHave a nice day !\n");
}

// -----------------------------------------------------------------------------
// Intercepts the CTRL + C signal and closes all properly
void
vSigIntHandler (int sig) {

  pthread_cancel (xPollThread);
  vCloseAll();
  exit(EXIT_SUCCESS);
}

/* ========================================================================== */
