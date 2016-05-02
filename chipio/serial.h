/**
 * @file sysio/chipio/serial.h
 * @brief Liaison série ChipIo
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_CHIPIO_SERIAL_H_
#define _SYSIO_CHIPIO_SERIAL_H_

#include <stdio.h>
#include <sysio/serial.h>
#include <sysio/dinput.h>
#include <chipio/chipio.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @addtogroup chipio
 *  @{
 *  @defgroup chipio_serial Liaison série
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série
 *  d'un circuit d'entrées-sorties universel ChipIo. Le port se comporte comme
 *  un pseudo-terminal.
 *  @{
 */

/* structures =============================================================== */

/**
 * Port série ChipIo
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xChipIoSerial xChipIoSerial;

/* internal public functions ================================================ */

/**
 * Constructeur
 *
 * @param xChip Pointeur sur objet xChipIo ouvert avec xChipIoOpen()
 * @param xIrqPin Pointeur sur broche d'interruption, NULL si inutilisée
 * @return Pointeur sur le port série, NULL si erreur
 */
xChipIoSerial * xChipIoSerialNew (xChipIo * xChip, xDin * xIrqPin);

/**
 *  Destructeur
 *
 * @param xPort Pointeur sur le port
 */
void vChipIoSerialDelete (xChipIoSerial * xPort);

/**
 * Retourne le nom du port série dans le système
 *
 * Permet une utilisation du port avec l'API de la libc (open(), fopen() ...)
 * ou du module sysio_serial.
 *
 * @param xPort Pointeur sur le port
 * @return le nom du port sous forme d'une chaîne de caractères statique, NULL si erreur
 */
char * sChipIoSerialPortName (xChipIoSerial * xPort);

/**
 * Modification de configuration d'un port série
 *
 * @param xPort Pointeur sur le port
 * @param xIos configuration du port
 * @return 0, -1 si erreur
 */
int iChipIoSerialSetAttr (xChipIoSerial * xPort, const xSerialIos * xIos);

/**
 * Lecture de configuration d'un port série
 *
 * @param xPort Pointeur sur le port
 * @param xIos configuration du port lue
 * @return 0, -1 si erreur
 */
int iChipIoSerialGetAttr (xChipIoSerial * xPort, xSerialIos * xIos);

/**
 *  Vide les buffers de réception et de transmission
 *
 * @param xPort Pointeur sur le port
 * @return 0, -1 si erreur
 */
int iChipIoSerialFlush (xChipIoSerial * port);

/**
 *  @}
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_CHIPIO_SERIAL_H_ */
