/**
 * @file
 * @brief Client pour serveur gpsd
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sysio/gps.h>
#include <sysio/log.h>

/* structures =============================================================== */
struct xGps {

  struct gps_data_t * xData;
  pthread_t xFixThread;
  pthread_mutex_t xFixMutex;
  pthread_mutex_t xErrorMutex;
  vGpsFixCallback vFixCallback;
  vGpsErrorCallback vErrorCallback;
  unsigned long ulFixCount;
  int iError;
};

/* private variables ======================================================== */
static const char *cErrorMsg[] = {

  "Illegal argument",
  "gps_read() error",
  "Unable to get a fix",
  "Not enough memory",
  "Unable to connect to server",
  "Can not create a thread"
};
static int iNewError;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vRaiseError (struct xGps * gps, int iError) {

  gps->iError = iError;
  pthread_mutex_unlock (&gps->xErrorMutex);
  if (gps->vErrorCallback) {

    gps->vErrorCallback (iError);
  }
}

// -----------------------------------------------------------------------------
// Fonction de scrutation du gps qui tourne dans un thread
static void *
pvFixLoop (void * p_data) {

  if (!p_data) {

    vLog (LOG_CRIT, "p_data is null !");
  }
  else {
    struct xGps * gps = (struct xGps *) p_data;

    for (;;) {

      // Scrute si des données sont exploitables avec un tiemout de 500 ms
      if (gps_waiting (gps->xData, 5000000) != 0) {

        // des données sont dispo, il faut les lire !
        if (gps_read (gps->xData) == -1) {

          // Erreur de lecture !
          vLog (LOG_ERR, "gps_read(%s)", strerror (errno));
          vRaiseError (gps, eGpsReadError);
        }
        else {

          // Lecture correcte
          if (gps->xData->status > STATUS_NO_FIX) {

            if ( (gps->xData->fix.mode == MODE_2D) ||
                 (gps->xData->fix.mode == MODE_3D)) {

              // Nous avons un fix !
              gps->ulFixCount++;
              pthread_mutex_unlock (&gps->xFixMutex);
              if (gps->vFixCallback) {

                gps->vFixCallback (gps);
              }
            }
          }
        }
      }
      else {

        // gps_waiting a renvoyé 0 ce qui indique que nous ne sommes plus enregistré
        // pour mise à jour, il faut relancer la scrutation
        gps_stream (gps->xData, WATCH_ENABLE | WATCH_JSON, NULL);
      }
    }
  }

  return NULL;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
struct xGps *
xGpsNew (char * server, char * port) {
  struct xGps * gps;
  int err;

  gps = malloc (sizeof (struct xGps));
  assert (gps);
  memset (gps, 0, sizeof (struct xGps));

  gps->xData = malloc (sizeof (struct gps_data_t));
  assert (gps->xData);

  // Connection au serveur gpsd
  err = gps_open (server, port, gps->xData);
  if (err != 0) {

    vLog (LOG_CRIT, "xGpsNew: Unable to connect to server: %s (%d)",
          gps_errstr (err), err);
    free (gps->xData);
    free (gps);
    iNewError = eGpsServerError;
    return NULL;
  }

  // Enregistre notre programme pour une scrutation des données GPS
  gps_stream (gps->xData, WATCH_ENABLE | WATCH_JSON, NULL);

  pthread_mutex_init (&gps->xFixMutex, NULL);
  pthread_mutex_lock (&gps->xFixMutex);
  pthread_mutex_init (&gps->xErrorMutex, NULL);
  pthread_mutex_lock (&gps->xErrorMutex);

  if (pthread_create (&gps->xFixThread, NULL, pvFixLoop, gps) != 0) {

    vLog (LOG_CRIT, "xGpsNew: Can not create a thread");
    vGpsDelete (gps);
    iNewError = eGpsThreadError;
    return NULL;
  }

  return gps;
}

// -----------------------------------------------------------------------------
void
vGpsDelete (struct xGps * gps) {

  if (gps) {

    pthread_cancel (gps->xFixThread);
    pthread_join (gps->xFixThread, NULL);
    // On ferme proprement
    gps_stream (gps->xData, WATCH_DISABLE, NULL);
    gps_close (gps->xData);
    free (gps->xData);
    free (gps);
  }
}

// -----------------------------------------------------------------------------
int
iGpsRead (struct xGps * gps, struct gps_data_t * data) {

  if (!gps) {

    return eGpsArgumentError;
  }
  memcpy (data, gps->xData, sizeof (struct gps_data_t));
  return 0;
}

// -----------------------------------------------------------------------------
bool
bGpsDataAvailable (struct xGps * gps) {

  if (gps) {

    if (pthread_mutex_trylock (&gps->xFixMutex) == 0) {

      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
int
iGpsError (struct xGps * gps) {

  if (gps) {

    if (pthread_mutex_trylock (&gps->xErrorMutex) == 0) {
      return gps->iError;
    }
  }
  else {

    return iNewError;
  }
  return 0;
}

// -----------------------------------------------------------------------------
const char *
cGpsStrError (struct xGps * gps) {
  int iIndex, iError = iGpsError (gps);

  iIndex = - (iError + 1);

  if (iIndex < (sizeof (cErrorMsg) / sizeof (const char *))) {
    return cErrorMsg[iIndex];
  }
  return NULL;
}

// -----------------------------------------------------------------------------
int
iGpsSetFixCallback (struct xGps * gps, vGpsFixCallback callback) {

  if (!gps) {
    return eGpsArgumentError;
  }
  gps->vFixCallback = callback;
  return 0;
}

// -----------------------------------------------------------------------------
void
vGpsClearFixCallback (struct xGps * gps) {

  if (gps) {
    gps->vFixCallback = NULL;
  }
}

// -----------------------------------------------------------------------------
int
iGpsSetErrorCallback (struct xGps * gps, vGpsErrorCallback callback) {

  if (!gps) {
    return eGpsArgumentError;
  }
  gps->vErrorCallback = callback;
  return 0;
}

// -----------------------------------------------------------------------------
void
vGpsClearErrorCallback (struct xGps * gps) {

  if (gps) {
    gps->vErrorCallback = NULL;
  }
}

// -----------------------------------------------------------------------------
eGpsStatus
eGpsFixStatus (struct xGps * gps) {

  if (gps) {
    return (eGpsStatus) gps->xData->status;
  }
  return eStatusNoFix;
}

// -----------------------------------------------------------------------------
bool
bGpsIsFix (struct xGps * gps) {

  if (eGpsFixStatus (gps) != eStatusNoFix) {
    eGpsMode mode = eGpsFixMode (gps);

    if ( (mode == eMode2d) || (mode == eMode3d)) {

      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
eGpsMode eGpsFixMode (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & MODE_SET) {
      return (eGpsMode) gps->xData->fix.mode;
    }
  }
  return eModeNotSeen;
}

// -----------------------------------------------------------------------------
timestamp_t
tGpsTime (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & TIME_SET) {
      return gps->xData->fix.time;
    }
  }
  return -1llu;
}

// -----------------------------------------------------------------------------
double
dGpsTimeError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & TIMERR_SET) {
      return gps->xData->fix.ept;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsLatitude (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & LATLON_SET) {
      return gps->xData->fix.latitude;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsLatitudeError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & HERR_SET) {
      return gps->xData->fix.epy;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsLongitude (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & LATLON_SET) {
      return gps->xData->fix.longitude;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsLongitudeError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & HERR_SET) {
      return gps->xData->fix.epx;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsAltitude (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & ALTITUDE_SET) {
      return gps->xData->fix.altitude;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsAltitudeError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & VERR_SET) {
      return gps->xData->fix.epv;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsTrack (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & TRACK_SET) {
      return gps->xData->fix.track;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsTrackError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & TRACKERR_SET) {
      return gps->xData->fix.epd;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsSpeed (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & SPEED_SET) {
      return gps->xData->fix.speed;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsSpeedError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & SPEEDERR_SET) {
      return gps->xData->fix.eps;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsClimb (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & CLIMB_SET) {
      return gps->xData->fix.climb;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
double
dGpsClimbError (struct xGps * gps) {

  if (gps) {

    if (gps->xData->set & CLIMBERR_SET) {
      return gps->xData->fix.epc;
    }
  }
  return NAN;
}

// -----------------------------------------------------------------------------
int
iGpsPrintData (struct xGps * gps, eGpsTimeFormat eTime, eGpsAngleFormat eDeg) {

  if (!gps) {
    return eGpsArgumentError;
  }

  if (gps->xData->set != 0) {
    const char *msg;
    timestamp_t t;
    double d;

    if ( (gps->ulFixCount % 24UL) == 1) {

      printf ("\n#\tFix\tMode\t#Sat\tDate/Time\t\t\tLat.\t\tLong.\t\tAlt.\t"
              "Track\tSpeed\tClimb\n");
    }
    printf ("%04ld\t", gps->ulFixCount);

    switch (eGpsFixStatus (gps)) {
      case eStatusNoFix:
        msg = "OFF";
        break;
      case eStatusFix:
        msg = "ON";
        break;
      case eStatsDgpsFix:
        msg = "DGPS";
        break;
      default:
        msg = "UN";
        break;
    }
    printf ("%s\t", msg);

    switch (eGpsFixMode (gps)) {
      case eModeNoFix:
        msg = "NO";
        break;
      case eMode2d:
        msg = "2D";
        break;
      case eMode3d:
        msg = "3D";
        break;
      default:
        msg = "UN";
        break;
    }
    printf ("%s\t", msg);

    // TODO: fonction des statistiques satellites
    if (gps->xData->satellites_used > 0) {
      printf ("%d\t", gps->xData->satellites_used);
    }
    else {
      printf ("--\t");
    }

    t = tGpsTime (gps);
    if (t != -1llu) {

      printf ("%s", cGpsTimeToStr (eTime, t));
    }
    else {

      if (eTime != eTimeAprs) {
        printf ("-------");
      }
      else {
        printf ("--------------------------");
      }
    }
    putchar ('\t');
    if (eTime == eTimeAprs) {
      printf ("\t\t\t");
    }

    d = dGpsLatitude (gps);
    if (!GPS_ISNAN (d)) {
      char str[40];

      printf ("%s\t", cGpsAngleToStr (eDeg & ~eDegAprsLong,  d, str, 40));
      d = dGpsLongitude (gps);
      printf ("%s\t", cGpsAngleToStr (eDeg & ~eDegAprsLat, d, str, 40));
    }
    else {
      printf ("-------\t-------\t");
    }

    d = dGpsAltitude (gps);
    if (!GPS_ISNAN (d)) {

      printf ("%g\t", d);
    }
    else {
      printf ("-----\t");
    }

    d = dGpsTrack (gps);
    if (!GPS_ISNAN (d)) {

      printf ("%g\t", d);
    }
    else {
      printf ("-----\t");
    }

    d = dGpsSpeed (gps);
    if (!GPS_ISNAN (d)) {

      printf ("%g\t", d);
    }
    else {
      printf ("-----\t");
    }

    d = dGpsClimb (gps);
    if (!GPS_ISNAN (d)) {

      printf ("%g", d);
    }
    else {
      printf ("-----\t");
    }
    putchar ('\n');
  }
  return 0;
}

// -----------------------------------------------------------------------------
char *
cGpsTimeToStr (eGpsTimeFormat type, timestamp_t ts) {
  struct tm   xT;
  static char str[26];
  time_t t = ts;

  if (type == eTimeLocal) {

    localtime_r (&t, &xT);
  }
  else {

    gmtime_r (&t, &xT);
  }
  if (type == eTimeAprs) {

    (void) snprintf (str, sizeof (str), "%02d%02d%02dZ",
                     xT.tm_mday,  xT.tm_hour,  xT.tm_min);
  }
  else {

    asctime_r (&xT, str);
    str[strlen (str) - 1] = 0; // retrait \n final
  }

  return str;
}

/*
 * -----------------------------------------------------------------------------
 * This function is Copyright (c) 2010 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 *
 * convert double degrees to a static string and return a pointer to it
 *
 * eGpsAngleFormat:
 *  eDegDd     :  return DD.dddddd
 *  eDegDdMm   :  return DD MM.mmmm'
 *  eDegDdMmSs :  return DD MM' SS.sss"
 *  eDegAprsLat:  return DDMM.mmX:  X -> N(+) ou S(-)
 *  eDegAprsLong: return DDDMM.mmX: X -> E(+) ou W(-)
 *
 */
#define DD_FORMAT         "%3d.%06ld"
#define DDMM_FORMAT       "%3d %02d.%04d'"
#define DDMMSS_FORMAT     "%3d %02d' %02d.%03d\""
#define APRS_LAT_FORMAT   "%02d%02d.%02d%c"
#define APRS_LONG_FORMAT  "%03d%02d.%02d%c"
char *
cGpsAngleToStr (eGpsAngleFormat type, double f, char * str, size_t strsize) {
  int dsec, sec, deg, min, sign = 0;
  long frac_deg;
  double fdsec, fsec, fdeg, fmin;

  if (f < 0) {

    f = fabs (f);
    sign = 1;
  }

  if (f > 360) {

    (void) strlcpy (str, "nan", strsize);
    return str;
  }

  //----------------------------------------------------------------------------
  fmin = modf (f, &fdeg);
  deg = (int) fdeg;
  frac_deg = (long) round (fmin * 1000000.0);

  if (eDegDd == type) {

    /* DD.dddddd */
    (void) snprintf (str, strsize, DD_FORMAT, deg, frac_deg);
    return str;
  }

  //----------------------------------------------------------------------------
  fsec = modf (fmin * 60, &fmin);
  min = (int) fmin;
  sec = (int) round (fsec * 10000.0);

  if (eDegDdMm == type) {
    /* DD MM.mmmm */
    (void) snprintf (str, strsize, DDMM_FORMAT, deg, min, sec);
    return str;
  }

  sec = (int) round (fsec * 100.0);
  if (eDegAprsLat == type) {
    /* DDMM.mmX:  X -> N(+) ou S(-) */
    (void) snprintf (str, strsize, APRS_LAT_FORMAT,
                     deg, min, sec, (sign == 0 ? 'N' : 'S'));
    return str;
  }

  if (eDegAprsLong == type) {
    /* DDDMM.mmX: X -> E(+) ou W(-) */
    (void) snprintf (str, strsize, APRS_LONG_FORMAT,
                     deg, min, sec, (sign == 0 ? 'E' : 'W'));
    return str;
  }

  //----------------------------------------------------------------------------
  /* else DD MM SS.sss */
  fdsec = modf (fsec * 60, &fsec);
  sec = (int) fsec;
  dsec = (int) round (fdsec * 1000.0);
  (void) snprintf (str, strsize, DDMMSS_FORMAT, deg, min, sec,
                   dsec);

  return str;
}

/* ========================================================================== */
