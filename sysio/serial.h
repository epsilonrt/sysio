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

  SERIAL_FLOW_NONE,
  SERIAL_FLOW_RTSCTS,
  SERIAL_FLOW_XONXOFF,
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

/**
 * @enum eSerialDataBits
 * @brief Nombre de bits de données
 */
typedef enum {
  SERIAL_DATABIT_5,
  SERIAL_DATABIT_6,
  SERIAL_DATABIT_7,
  SERIAL_DATABIT_8,
  SERIAL_DATABIT_UNKNOWN = -1
} eSerialDataBits;

/**
 * @enum eSerialParity
 * @brief Parité
 */
typedef enum {
  SERIAL_PARITY_NONE,
  SERIAL_PARITY_EVEN,
  SERIAL_PARITY_ODD,
  SERIAL_PARITY_SPACE,
  SERIAL_PARITY_MARK,
  SERIAL_PARITY_UNKNOWN = -1
} eSerialParity;

/**
 * @enum eSerialStopBits
 * @brief Nombre de bits de stop
 */
typedef enum {
  SERIAL_STOPBIT_ONE,
  SERIAL_STOPBIT_TWO,
  SERIAL_STOPBIT_ONEHALF,
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
 *  Lecture du type de contrôle de flux en cours
 *
 * @param le descripteur de fichier du port
 */
eSerialFlow eSerialGetFlow (int fd);

/**
 *  Modification du type de contrôle de flux
 *
 * @param le descripteur de fichier du port
 */
eSerialFlow eSerialSetFlow (int fd, eSerialFlow eNewFlow);

/**
 *  Lecture du type de contrôle de flux en cours sous forme "lisible"
 *
 * @param le descripteur de fichier du port
 */
const char * sSerialGetFlowStr (int fd);

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
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_SERIAL_H_ */
