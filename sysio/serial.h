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
 * @brief Type de gestion de flux
 */

typedef enum {

  SERIAL_FLOW_NONE,
  SERIAL_FLOW_RTSCTS,
  SERIAL_FLOW_XONXOFF,
  SERIAL_FLOW_UNKNOWN = -1
} eSerialFlow;

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
const char * eSerialGetFlowStr (int fd);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_SERIAL_H_ */
