/**
 * @file sysio/i2c.h
 * @brief Bus I²C
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_I2C_H_
#define _SYSIO_I2C_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_i2c Bus I2C
 *
 *  Ce module fournit les fonctions permettant d'accéder au bus I2C.
 *  @{
 */

/**
 * @brief Ouverture d'une connexion vers un circuit I2C
 *
 * @param device nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param i2caddr adresse du circuit I2C (alignée à droite)
 * @return le descripteur de fichier vers la connexion ouverte, -1 si erreur
 */
int iI2cOpen (const char * device, int i2caddr);

/**
 * @brief Fermeture d'une connexion vers un circuit I2C
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @return 0, -1 si erreur
 */
int iI2cClose (int fd);

/**
 * @brief Lecture d'un octet
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @return la valeur de l'octet comme un unsigned, -1 si erreur
 */
int iI2cRead           (int fd);

/**
 * @brief Lecture d'un bloc octets
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param size nombre d'octets à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iI2cReadBlock      (int fd, uint8_t * buffer, int size);

/**
 * @brief Lecture d'un registre 8 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture d'un octet
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur de l'octet comme un unsigned, -1 si erreur
 */
int iI2cReadReg8       (int fd, uint8_t reg);

/**
 * @brief Lecture d'un registre 16 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture de 2 octets
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @return la valeur du mot comme un unsigned, -1 si erreur
 */
int iI2cReadReg16      (int fd, uint8_t reg);

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
int iI2cReadRegBlock   (int fd, uint8_t reg, uint8_t * buffer, uint8_t size);

/**
 * @brief Ecriture d'un octet
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param data valeur de l'octet
 * @return 0, -1 si erreur
 */
int iI2cWrite          (int fd, uint8_t data);

/**
 * @brief Ecriture d'un bloc octets
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param buffer pointeur vers la zone de stockage des octets
 * @param size nombre d'octets à écrire
 * @return le nombre d'octets écrits, -1 si erreur
 */
int iI2cWriteBlock     (int fd, const uint8_t * buffer, int size);

/**
 * @brief Ecriture d'un registre 8 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur de l'octet
 * @return 0, -1 si erreur
 */
int iI2cWriteReg8      (int fd, uint8_t reg, uint8_t data);

/**
 * @brief Ecriture d'un registre 16 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre (ou octet de contrôle)
 * @param data valeur du mot
 * @return 0, -1 si erreur
 */
int iI2cWriteReg16     (int fd, uint8_t reg, uint16_t data);

/**
 * @brief Ecriture d'un bloc de registres
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre (ou octet de contrôle)
 * @param buffer pointeur vers la zone de stockage des octets
 * @param size nombre d'octets à écrire
 * @return le nombre d'octets écrits, -1 si erreur
 */
int iI2cWriteRegBlock  (int fd, uint8_t reg, const uint8_t * buffer, uint8_t size);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_I2C_H_ defined */
