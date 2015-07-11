/**
 * @file sysio/chipio/chipio.h
 * @brief Circuit d'entrées-sorties universel
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_CHIPIO_H_
#define _SYSIO_CHIPIO_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup chipio Circuit d'entrées-sorties universel ChipIo
 *
 *  Ce module fournit les fonctions permettant de contrôler un circuit
 *  d'entrées-sorties universel CipIo.
 *  @{
 */

/* structures =============================================================== */
/**
 * Objet ChipIo
 *
 * Structure opaque.
 */
typedef struct xChipIo xChipIo;

/* internal public functions ================================================ */

/**
 * Ouverture d'un ChipIo sur le port I²C
 *
 * @param sI2cBus nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param iSlaveAddr adresse du circuit I2C (alignée à droite)
 * @return l'objet intialisé et ouvert, NULL si erreur
 */
xChipIo * xChipIoOpen (const char * sI2cBus, int iSlaveAddr);

/**
 *  Fermeture du port série
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @return 0, -1 si erreur
 */
int iChipIoClose (xChipIo * chip);

int iChipIoRevisionMajor (xChipIo * chip);
int iChipIoRevisionMinor (xChipIo * chip);
int iChipIoAvailableOptions (xChipIo * chip);

/**
 * @brief Lecture d'un registre 8 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture d'un octet
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur de l'octet comme un unsigned, -1 si erreur
 */
int iChipIoReadReg8 (xChipIo * chip, uint8_t reg);

/**
 * @brief Lecture d'un registre 16 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture de 2 octets
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur du mot comme un unsigned, -1 si erreur
 */
int iChipIoReadReg16 (xChipIo * chip, uint8_t reg);

/**
 * @brief Lecture d'un bloc de registres
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre (ou octet de contrôle)
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param size nombre de registres à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iChipIoReadRegBlock (xChipIo * chip, uint8_t reg, uint8_t * buffer, uint8_t size);

/**
 * @brief Ecriture d'un registre 8 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur de l'octet
 * @return 0, -1 si erreur
 */
int iChipIoWriteReg8 (xChipIo * chip, uint8_t reg, uint8_t data);

/**
 * @brief Ecriture d'un registre 16 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur du mot
 * @return 0, -1 si erreur
 */
int iChipIoWriteReg16 (xChipIo * chip, uint8_t reg, uint16_t data);

/**
 * @brief Ecriture d'un bloc de registres
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre (ou octet de contrôle)
 * @param buffer pointeur vers la zone de stockage des octets
 * @param size nombre d'octets à écrire
 * @return le nombre d'octets écrits, -1 si erreur
 */
int iChipIoWriteRegBlock (xChipIo * chip, uint8_t reg, const uint8_t * buffer, uint8_t size);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_CHIPIO_H_ */
