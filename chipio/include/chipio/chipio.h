/**
 * @file 
 * @brief Circuit d'entrées-sorties universel
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _CHIPIO_H_
#define _CHIPIO_H_

#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */

/**
 *  @defgroup chipio Circuit d'entrées-sorties universel ChipIo
 *
 *  Ce module fournit les fonctions permettant de contrôler un circuit
 *  d'entrées-sorties universel ChipIo.
 *  @{
 */

/* constants ================================================================ */
/**
 * Options disponibles
 */
typedef enum {
  eChipIoOptionLcd              = 0x01,
  eChipIoOptionAdc              = 0x02,
  eChipIoOptionSerial           = 0x04,
  eChipIoOptionSerialIrq        = 0x40,
  eChipIoOptionLcdSplashScreen  = 0x80
} eChipIoOptions;

/* structures =============================================================== */
/**
 * Objet ChipIo
 *
 * Structure opaque pour l'utilisateur
 */
typedef struct xChipIo xChipIo;

/* internal public functions ================================================ */

/**
 * Ouverture d'un ChipIo sur le port I²C
 *
 * @param sI2cBus nom du bus I2C (par exemple /dev/i2c-1)
 * @param iSlaveAddr adresse du circuit I2C (alignée à droite)
 * @return l'objet intialisé et ouvert, NULL si erreur
 */
xChipIo * xChipIoOpen (const char * sI2cBus, int iSlaveAddr);

/**
 *  Fermeture du port série
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @return 0, -1 si erreur
 */
int iChipIoClose (xChipIo * chip);

/**
 * Retourne le numéro majeur de la revision du circuit ChipIo
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @return le majeur comme un unsigned, -1 si erreur
 */
int iChipIoRevisionMajor (xChipIo * chip);

/**
 * Retourne le numéro mineur de la revision du circuit ChipIo
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @return le mineur comme un unsigned, -1 si erreur
 */
int iChipIoRevisionMinor (xChipIo * chip);

/**
 * Retourne les options disponibles du circuit ChipIo
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @return le majeur comme un unsigned, -1 si erreur
 */
int iChipIoAvailableOptions (xChipIo * chip);

/**
 * @brief Lecture d'un registre 8 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture d'un octet.
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur de l'octet comme un unsigned, -1 si erreur
 */
int iChipIoReadReg8 (xChipIo * chip, uint8_t reg);

/**
 * @brief Lecture d'un registre 16 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture de 2 octets
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur du mot comme un unsigned, -1 si erreur
 */
int iChipIoReadReg16 (xChipIo * chip, uint8_t reg);

/**
 * @brief Lecture d'un bloc de registres
 *
 * Taille maximal du bloc 32 octets.
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
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
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur de l'octet
 * @return 0, -1 si erreur
 */
int iChipIoWriteReg8 (xChipIo * chip, uint8_t reg, uint8_t data);

/**
 * @brief Ecriture d'un registre 16 bits
 *
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur du mot
 * @return 0, -1 si erreur
 */
int iChipIoWriteReg16 (xChipIo * chip, uint8_t reg, uint16_t data);

/**
 * @brief Ecriture d'un bloc de registres
 *
 * Taille maximal du bloc 32 octets.
 *
 * @note Les accès i2c au ChipIo sont protégés par un mutex.
 *
 * @param chip Pointeur sur le ChipIo ouvert
 * @param reg adresse du premier registre (ou octet de contrôle)
 * @param buffer pointeur vers la zone de stockage des octets
 * @param size nombre d'octets à écrire
 * @return 0, -1 si erreur
 */
int iChipIoWriteRegBlock (xChipIo * chip, uint8_t reg, const uint8_t * buffer, uint8_t size);

/**
 * @}
 */

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /* _CHIPIO_H_ */
