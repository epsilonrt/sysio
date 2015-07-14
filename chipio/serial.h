/**
 * @file sysio/chipio/serial.h
 * @brief Liaison série ChipIo
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
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
 *  @defgroup chipio_serial Liaison série ChipIo
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série
 *  d'un circuit d'entrées-sorties universel ChipIo.
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
 * Ouverture d'un port série
 *
 * Le port est ouvert en lecture/écriture non bloquant.
 *
 * @param xChip Pointeur sur objet xChipIo ouvert avec xChipIoOpen()
 * @param xIrqPin Pointeur sur broche d'interruption, NULL si inutilisée
 * @return Pointeur sur le port série ouvert, NULL si erreur
 */
xChipIoSerial * xChipIoSerialOpen (xChipIo * xChip, xDin * xIrqPin);

/**
 *  Fermeture du port série
 *
 * @param xPort Pointeur sur le port
 */
void vChipIoSerialClose (xChipIoSerial * xPort);

/**
 * Retourne le flux d'entrée-sortie vers le port série
 *
 * Permet des accès avec les fonctions de haut niveau de stdio.h (fprintf() ...)
 *
 * @param xPort Pointeur sur le port
 * @return Pointeur sur le flux ouvert en lecture-écriture, NULL si erreur
 */
FILE * xChipIoSerialFile (xChipIoSerial * xPort);

/**
 * Retourne descripteur de fichier du port série
 *
 * Permet des accès avec les fonctions de bas niveau (read(), write() ...)
 *
 * @param xPort Pointeur sur le port
 * @return Descripteur de fichier du port
 */
int iChipIoSerialFileNo (xChipIoSerial * xPort);

/**
 * Retourne le nom du port série dans le système
 *
 * @param xPort Pointeur sur le port
 * @return le nom du port sous forme d'une chaîne de caractères statique, NULL si erreur
 */
const char * sChipIoSerialPortName (xChipIoSerial * xPort);

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

#if !defined(__DOXYGEN__)
/*
 * Indique si le port est occupé
 *
 * Cette fonction est à usage interne. Elle est utilisée par le thread qui
 * gère la transmission et la réception vers le ChipIo.
 *
 * @note Il n'est pas nécessaire de tester si le port est occupé avec d'y écrire
 * ou lire des données
 *
 * @param xPort Pointeur sur le port
 * @return true si occupé, false si prêt, -1 si erreur
 */
int iChipIoSerialIsBusy (xChipIoSerial * xPort);

/*
 *  Retourne la taille du buffer
 *
 * Cette fonction est à usage interne. Elle est utilisée par le thread qui
 * gère la transmission et la réception vers le ChipIo.
 *
 * @param xPort Pointeur sur le port
 * @return la taille du buffer en octets (valeur positive), -1 si erreur
 */
int iChipIoSerialGetBufSize (xChipIoSerial * xPort);
#endif /* __DOXYGEN__ not defined */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_CHIPIO_SERIAL_H_ */
