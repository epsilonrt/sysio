/**
 * @file sysio/chipio/serial.h
 * @brief Liaison série Chip
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_CHIPIO_SERIAL_H_
#define _SYSIO_CHIPIO_SERIAL_H_

#include <stdio.h>
#include <sysio/serial.h>
#include <sysio/chipio.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup chipio_serial Liaison série ChipIo
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série
 *  d'un Circuit d'entrées-sorties universel ChipIo.
 *  @{
 */

/* structures =============================================================== */

/**
 * Port série ChipIo
 *
 * La structure est opaque.
 */
typedef struct xChipIoSerial xChipIoSerial;

/* internal public functions ================================================ */

/**
 * Ouverture d'un port série
 *
 * Le port est ouvert en lecture/écriture.
 *
 * @param iChipIoFd descripteur de fichiers du ChipIo ouvert avec iChipIoOpen()
 * @return Pointeur sur le port série ouvert, NULL si erreur
 */
xChipIoSerial * xChipIoSerialOpen (xChipIo * chip);

/**
 *  Fermeture du port série
 *
 * @param xPort Pointeur sur le port
 */
void vChipIoSerialClose (xChipIoSerial * xPort);

int iChipIoSerialBufferSize (xChipIoSerial * xPort);

/**
 * Retourne le flux d'entrée-sortie vers le port série
 *
 * @param xPort Pointeur sur le port
 * @return Pointeur sur le flux ouvert en lecture-écriture, NULL si erreur
 */
FILE * xChipIoSerialFile (xChipIoSerial * xPort);

const char * sChipIoSerialPortName (xChipIoSerial * xPort);

/**
 * Retourne descripteur de fichier du port série
 *
 * @param xPort Pointeur sur le port
 * @return Descripteur de fichier du port
 */
int iChipIoSerialFileNo (xChipIoSerial * xPort);

/**
 *  Lecture de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 */
int iChipIoSerialGetBaudrate (xChipIoSerial * xPort);

/**
 *  Lecture du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 */
eSerialDataBits eChipIoSerialGetDataBits (xChipIoSerial * xPort);

/**
 *  Lecture de la parité
 *
 * @param xPort Pointeur sur le port
 */
eSerialParity eChipIoSerialGetParity (xChipIoSerial * xPort);

/**
 *  Lecture du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 */
eSerialStopBits eChipIoSerialGetStopBits (xChipIoSerial * xPort);


/**
 * Modification de la vitesse de transmission
 *
 * @param xPort Pointeur sur le port
 */
int iChipIoSerialSetBaudrate (xChipIoSerial * xPort, int iBaudrate);

/**
 * Modification du nombre de bits de données
 *
 * @param xPort Pointeur sur le port
 */
eSerialDataBits eChipIoSerialSetDataBits (xChipIoSerial * xPort, eSerialDataBits eDataBits);

/**
 * Modification de la parité
 *
 * @param xPort Pointeur sur le port
 */
eSerialParity eChipIoSerialSetParity (xChipIoSerial * xPort, eSerialParity eParity);

/**
 * Modification du nombre de bits de stop
 *
 * @param xPort Pointeur sur le port
 */
eSerialStopBits eChipIoSerialSetStopBits (xChipIoSerial * xPort, eSerialStopBits eStopBits);

/**
 *  Lecture du type de contrôle de flux en cours
 *
 * @param xPort Pointeur sur le port
 */
eSerialFlow eChipIoSerialGetFlow (xChipIoSerial * xPort);

/**
 *  Modification du type de contrôle de flux
 *
 * @param xPort Pointeur sur le port
 */
eSerialFlow eChipIoSerialSetFlow (xChipIoSerial * xPort, eSerialFlow eNewFlow);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_CHIPIO_SERIAL_H_ */
