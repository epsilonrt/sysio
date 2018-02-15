/**
 * @file 
 * @brief Client pour serveur gpsd
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPS_H_
#define _SYSIO_GPS_H_
#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */
#include <gps.h>
#include <math.h>

/**
 * @defgroup sysio_gps GPS avec gpsd
 * 
 * Ce module permet de gérer des récepteurs GPS gérés par gpsd 
 * http://www.catb.org/gpsd/ . Il utilise donc libgps.
 * 
 * Le serveur gpsd et libgps doivent être installés et configurés correctement...
 * 
 * Sous debian, il faut faire quelque chose comme ceci : \n 
 * @code
 * sudo apt-get install gpsd gpsd-clients libgps-dev
 * sudo nano /etc/default/gpsd
 * 
 * # modifier les lignes ci-dessous pour faire correspondre à la liaison série du GPS
 * DEVICES="/dev/ttyUSB0"
 * GPSD_OPTIONS="-b"
 * 
 * sudo service gpsd restart
 * @endcode
 *
 * Pour tester la bonne réception, on peut afficher les trames NMEA 0183 reçues: 
 * @code
 * gpspipe -r localhost
 * @endcode
 *
 * On peut aussi afficher les informations reçues sous forme plus exploitables
 * avec `cgps` ou `xgps`. 
 * @{
 * @example gps/poll/sysio_demo_gps_poll.c
 *  Demo module gps par scrutation
 * 
 * Ce programme permet d'afficher les données GPS reçues par gpsd. \n
 * Le programme scrute si des données GPS sont disponibles avant de les afficher.
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
 * @example gps/callback/sysio_demo_gps_callback.c
 *  Demo module gps par interruption
 * 
 * Ce programme permet d'afficher les données GPS reçues par gpsd. \n
 * Le programme gère la réception par interruption, une fonction de gestion
 * (callback) gère la réception de message de fix et une autre les erreurs.
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
 */

/* macros =================================================================== */
/**
 * @brief Permet de tester si une valeur de type double est NaN (incorrecte)
 */
#define GPS_ISNAN(x) ((x) != (x))

/* constants ================================================================ */
/**
 * @enum eGpsError
 * @brief Liste des erreurs
 */
typedef enum {
  eGpsNoError = 0,
  eGpsArgumentError = -1,
  eGpsReadError = -2,
  eGpsFixError = -3,
  eGpsMemoryError = -4,
  eGpsServerError = -5,
  eGpsThreadError = -6
} eGpsError;

/**
 * @enum eGpsAngleFormat
 * @brief Liste des formats d'affichage de laltitude/longitude
 */
typedef enum {
  eDegDd = 0,       ///< DD.dddddd
  eDegDdMmSs =    (1<<0), ///< DD MM' SS.sss"
  eDegDdMm =      (1<<1), ///< DD MM.mmmm'
  eDegAprsLat =   (1<<2), ///< DDMM.mmX:  X -> N(+) ou S(-)
  eDegAprsLong =  (1<<3), ///< DDDMM.mmX: X -> E(+) ou W(-)
  eDegAprs =  eDegAprsLat | eDegAprsLong  ///< for iGpsPrintData() only
} eGpsAngleFormat;

/**
 * @enum eGpsTimeFormat
 * @brief Liste des formats d'affichage de date/heure
 */
typedef enum {
  eTimeLocal, ///< Heure locale
  eTimeUtc, ///< Heure universelle GMT
  eTimeAprs ///< Heure au format APRS (GMT)
} eGpsTimeFormat;

/**
 * @enum eGpsMode
 * @brief Liste des modes du GPS
 */
typedef enum {
  eModeNotSeen = MODE_NOT_SEEN, ///< Mode pas encore disponible
  eModeNoFix   = MODE_NO_FIX,   ///< Non fixé
  eMode2d      = MODE_2D,       ///< Fixé en 2 dimensions (latitude/longitude)
  eMode3d      = MODE_3D        ///< Fixé en 3 dimensions (latitude/longitude/altitude)
} eGpsMode;

#ifndef STATUS_DGPS_FIX
#define STATUS_DGPS_FIX 2
#endif

/**
 * @enum eGpsStatus
 * @brief Différents états du récepteur GPS
 */
typedef enum {
  eStatusNoFix  = STATUS_NO_FIX,  ///< Non fixé
  eStatusFix    = STATUS_FIX,     ///< Fixé sans GPS différentiel
  eStatsDgpsFix = STATUS_DGPS_FIX ///< Fixé avec GPS différentiel
} eGpsStatus;

/* structures =============================================================== */
/**
 * @class xGps
 * @brief Classe permettant de manipuler un GPS
 * Classe opaque pour l'utilisateur
 */
struct xGps;

/* types ==================================================================== */
/**
 * @brief Fonction de gestion des événements de fix
 * 
 * Ce type de fonction est définie par l'utilisateur et installée à l'aide de
 * iGpsSetFixCallback(). \n
 * La fonction installée sera appelée à chaque message fix correctemen formaté.
 * Elle est exécutée dans un autre thread que le programme appelant.
 */
typedef void (* vGpsFixCallback) (struct xGps * gps);

/**
 * @brief Fonction de gestion des erreurs
 * 
 * Ce type de fonction est définie par l'utilisateur et installée à l'aide de
 * iGpsSetErrorCallback(). \n
 * La fonction installée sera appelée à chaque erreur lors du processus de 
 * réception.
 * Elle est exécutée dans un autre thread que le programme appelant.
 */
typedef void (* vGpsErrorCallback) (int error);

/* internal public functions ================================================ */
/**
 * @brief Création d'un GPS
 * 
 * @param server adresse IP ou hostname du serveur (localhost généralement)
 * @param port port du serveur (DEFAULT_GPSD_PORT généralement)
 * @return pointeur sur le gps initialisé, NULL si erreur
 */
struct xGps * xGpsNew (char * server, char * port);

/**
 * @brief Fermeture et destruction d'un GPS
 * 
 * @param gps pointeur sur objet GPS
 */
void vGpsDelete (struct xGps * gps);

/**
 * @brief Indique que des données de position sont disponibles
 * 
 * @param gps pointeur sur objet GPS
 * @return  true si données disponibles
 */
bool bGpsDataAvailable (struct xGps * gps);

/**
 * @brief Code d'erreur
 * 
 * @param gps pointeur sur objet GPS, peut être NULL (cas où xGpsNew() a échouée)
 * @return code de la dernière erreur
 */
int iGpsError (struct xGps * gps);

/**
 * @brief Message d'erreur
 * 
 * @param gps pointeur sur objet GPS, peut être NULL (cas où xGpsNew() a échouée)
 * @return message d'erreur de la dernière erreur
 */
const char * cGpsStrError (struct xGps * gps);

/**
 * @brief Indique si le GPS est fixé
 * 
 * Cela indique que le GPS a réussi à calculer une position à partir des
 * données reçues des satellites.
 * @param gps pointeur sur objet GPS
 * @return true si fixé
 */
bool bGpsIsFix (struct xGps * gps);

/**
 * @brief Etat du GPS
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée
 */
eGpsStatus eGpsFixStatus (struct xGps * gps);

/**
 * @brief Mode de positionnement (2D/3D)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée
 */
eGpsMode eGpsFixMode (struct xGps * gps);

/**
 * @brief Heure de la position (fix time)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, -1 si pas disponible
 */
timestamp_t tGpsTime (struct xGps * gps);

/**
 * @brief Convertit un temps système en son équivalent humain
 * 
 * @param format
 * @param t temps en microsecondes depuis le 1er janvier 1970
 * @return pointeur sur la chaîne de catactères résulat
 */
char * cGpsTimeToStr (eGpsTimeFormat format, timestamp_t t);

/**
 * @brief Erreur sur l'heure en secondes
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsTimeError (struct xGps * gps);

/**
 * @brief Latitude en degrés décimaux (+N/-S)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsLatitude (struct xGps * gps);

/**
 * @brief Erreur position Y (Latitude) en mètres (+/-)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsLatitudeError (struct xGps * gps);

/**
 * @brief Longitude en degrés décimaux (+E/-W)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsLongitude (struct xGps * gps);

/**
 * @brief Erreur position X (Longitude) en mètres (+/-)
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsLongitudeError (struct xGps * gps);

/**
 * @brief Convertit un angle en degrés décimaux en son équivalent humain
 * 
 * @param format format choisi
 * @param f valeur à convertir
 * @param str buffer sur la chaine de caractère résultat
 * @param strsize taille du buffer str
 * @return pointeur sur la chaîne de catactères résulat
 */
char * cGpsAngleToStr (eGpsAngleFormat format, double f, char * str, size_t strsize);

/**
 * @brief Altitude en mètres
 * Valide uniquement en mode 3D
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsAltitude (struct xGps * gps);

/**
 * @brief Erreur position verticale (Altitude) en mètres (+/-)
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsAltitudeError (struct xGps * gps);

/**
 * @brief Cap/Route en degrés par rapport au Nord vrai
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsTrack (struct xGps * gps);

/**
 * @brief Erreur de cap en degrés (+/-)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsTrackError (struct xGps * gps);

/**
 * @brief Vitesse horizontale par rapport à la terre en mètres par seconde (m/s)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsSpeed (struct xGps * gps);

/**
 * @brief Erreur de vitesse horizontale en m/s (+/-)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsSpeedError (struct xGps * gps);

/**
 * @brief Vitesse verticale par rapport à la terre en mètres par seconde (m/s)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsClimb (struct xGps * gps);

/**
 * @brief Erreur de vitesse verticale en m/s (+/-)
 * 
 * @param gps pointeur sur objet GPS
 * @return la valeur demandée, NaN si pas disponible
 */
double dGpsClimbError (struct xGps * gps);

/**
 * @brief Affiche les données essentielles du dernier fix
 * 
 * @param gps pointeur sur objet GPS
 * @param eTime
 * @param eDeg
 * @return 
 */
int iGpsPrintData (struct xGps * gps, eGpsTimeFormat eTime, eGpsAngleFormat eDeg);

/**
 * @brief Copie les données de position dans une structure gps_data_t
 * 
 * Voir page man 3 de libgps pour utilisation
 * @param gps pointeur sur objet GPS
 * @param data
 * @return 0 ou code d'erreur \c eGpsError
 */
int iGpsRead (struct xGps * gps, struct gps_data_t * data);

/**
 * @brief Installe le gestionnairede fix
 * @param gps pointeur sur objet GPS
 * @param callback
 * @return 0 ou code d'erreur \c eGpsError
 */
int iGpsSetFixCallback (struct xGps * gps, vGpsFixCallback callback);

/**
 * @brief Invalide le gestionnaire de fix
 * @param gps pointeur sur objet GPS
 */
void vGpsClearFixCallback(struct xGps * gps);

/**
 * @brief Installe le gestionnaire d'erreur
 * @param gps pointeur sur objet GPS
 * @param callback
 * @return 0 ou code d'erreur \c eGpsError
 */
int iGpsSetErrorCallback (struct xGps * gps, vGpsErrorCallback callback);

/**
 * @brief Invalide le gestionnaire d'erreur
 * 
 * @param gps pointeur sur objet GPS
 */
void vGpsClearErrorCallback(struct xGps * gps);

/**
 * @}
 */

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /* _SYSIO_GPS_H_ defined */
