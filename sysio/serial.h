/**
 * @file sysio/serial.h
 * @brief Liaison série
 *
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_SERIAL_H_
#define _SYSIO_SERIAL_H_
#include <termios.h>
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_serial Liaison série
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série.
 *  @{
 */

/**
 * @brief Erreur de baudrate
 */
#define EBADBAUD (-2)

/**
 * @enum eSerialFlow
 * @brief Type de contrôle de flux
 */
typedef enum {

  SERIAL_FLOW_NONE = ' ',
  SERIAL_FLOW_RTSCTS = 'H',
  SERIAL_FLOW_XONXOFF = 'S',
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

/**
 * @enum eSerialDataBits
 * @brief Nombre de bits de données
 */
typedef enum {
  SERIAL_DATABIT_5 = 5,
  SERIAL_DATABIT_6 = 6,
  SERIAL_DATABIT_7 = 7,
  SERIAL_DATABIT_8 = 8,
  SERIAL_DATABIT_UNKNOWN = -1
} eSerialDataBits;

/**
 * @enum eSerialParity
 * @brief Parité
 */
typedef enum {
  SERIAL_PARITY_NONE = 'N',
  SERIAL_PARITY_EVEN = 'E',
  SERIAL_PARITY_ODD  = 'O',
  SERIAL_PARITY_SPACE = 'S',
  SERIAL_PARITY_MARK = 'M',
  SERIAL_PARITY_UNKNOWN = -1
} eSerialParity;

/**
 * @enum eSerialStopBits
 * @brief Nombre de bits de stop
 */
typedef enum {
  SERIAL_STOPBIT_ONE = 1,
  SERIAL_STOPBIT_TWO = 2,
  SERIAL_STOPBIT_ONEHALF = 3,
  SERIAL_STOPBIT_UNKNOWN
} eSerialStopBits;

/* internal public functions ================================================ */

/**
 * Ouverture d'un port série
 *
 * Le port est ouvert en lecture/écriture non bloquante, 8 bits sans parité,
 * 1 bit de stop.
 *
 * @param device le nom du port à ouvrir (/dev/tty...)
 * @param baud vitesse en bauds
 * @return le descripteur de fichier du port ou une valeur négative sur erreur
 */
int iSerialOpen (const char *device, const int baud);

/**
 *  Fermeture du port série
 *
 * @param le descripteur de fichier du port
 */
void vSerialClose (int fd);

/**
 *  Vide les buffers de réception et de transmission
 *
 * Le buffer de transmission est transmis, celui de réception est vidé.
 *
 * @param le descripteur de fichier du port
 */
void vSerialFlush (int fd);

/**
 * Retourne le nombre d'octets en attente de lecture
 *
 * @param xPort Pointeur sur le port
 * @return le nombre d'octets en attente de lecture, NULL si erreur
 */
int iSerialDataAvailable (int fd);

/**
 *  Lecture de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
int iSerialGetBaudrate (int fd);

/**
 *  Lecture du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialDataBits eSerialGetDataBits (int fd);

/**
 *  Lecture de la parité
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialParity eSerialGetParity (int fd);

/**
 *  Lecture du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialStopBits eSerialGetStopBits (int fd);

/**
 *  Lecture du type de contrôle de flux en cours
 *
 * @param xPort Pointeur sur le port
 * @return la valeur, -1 si erreur.
 */
eSerialFlow eSerialGetFlow (int fd);

/**
 * Modification de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetBaudrate (int fd, int iBaudrate);

/**
 * Modification du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetDataBits (int fd, eSerialDataBits eDataBits);

/**
 * Modification de la parité
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetParity (int fd, eSerialParity eParity);

/**
 * Modification du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetStopBits (int fd, eSerialStopBits eStopBits);

/**
 *  Modification du type de contrôle de flux
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur.
 */
int iSerialSetFlow (int fd, eSerialFlow eNewFlow);

/**
 *  Lecture du type de contrôle de flux en cours sous forme "lisible"
 *
 * @param le descripteur de fichier du port
 */
const char * sSerialGetFlowStr (int fd);

/**
 *  Chaîne de caractère décrivant la configuration du port
 *
 * @param le descripteur de fichier du port
 */
const char * sSerialConfigStrShort (int fd);

/**
 *  Renvoie la chaîne de caractère correspondant à une valeur de contrôle de flux
 *
 */
const char * sSerialFlowToStr (eSerialFlow eFlow);

/**
 *  Renvoie la chaîne de caractère correspondant à une valeur de parité
 *
 */
const char * sSerialParityToStr (eSerialParity eParity);

/**
 *  Renvoie la chaîne de caractère correspondant à une valeur de bits de données
 *
 */
const char * sSerialDataBitsToStr (eSerialDataBits eDataBits);

/**
 *  Renvoie la chaîne de caractère correspondant à une valeur de bits de stop
 *
 */
const char * sSerialStopBitsToStr (eSerialStopBits eStopBits);

/**
 *  Renvoie la valeur en baud associée à une constante speed_t
 */
int iSerialSpeedToInt (speed_t speed);

/**
 *  Renvoie la constante speed_t associée à une valeur en baud
 */
speed_t eSerialIntToSpeed (int baud);

int iSerialTermiosBaudrate (const struct termios * ts);
int iSerialTermiosDataBits (const struct termios * ts);
int iSerialTermiosStopBits (const struct termios * ts);
int iSerialTermiosParity (const struct termios * ts);
int iSerialTermiosFlow (const struct termios * ts);
const char * sSerialTermiosToStr  (const struct termios * ts);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_SERIAL_H_ */
