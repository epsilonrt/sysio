/**
 * @file
 * @brief Bus I²C
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_I2C_H_
#define _SYSIO_I2C_H_
#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */

/**
 *  @defgroup sysio_i2c Bus I2C
 *
 *  Ce module fournit les fonctions permettant d'accéder au bus I2C.
 *  @{
 *  @example i2c/sensor-iaq/sysio_demo_i2c_iaq.c
 *  Accès aux registres d'un capteur IAQ (qualité de l'air)
 *  @example i2c/mem-eeprom/sysio_demo_i2c_eeprom.c
 *  Ecriture dans de tout l'espace d'une mémoire EEPROM I2c, page par page,
 *  avec une séquence d'octets aléatoires, après écriture, la page est lue et
 *  vérifiée.
 *  @example i2c/rtc-bq32000/sysio_demo_i2c_rtc.c
 *  Accès aux registres d'une RTC BQ32000
 */

/* constants ================================================================ */
/**
 * Nombre max d'octets dans une trame I2C
 */
#define I2C_BLOCK_MAX 32

/* internal public functions ================================================ */
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
 * @warning La transmission s'effectue par bloc qu'au maximum 32 octets.
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
 * @warning La transmission s'effectue par bloc qu'au maximum 32 octets.
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre (ou octet de contrôle)
 * @param buffer pointeur vers la zone de stockage des octets
 * @param size nombre d'octets à écrire
 * @return 0, -1 si erreur
 */
int iI2cWriteRegBlock  (int fd, uint8_t reg, const uint8_t * buffer, uint8_t size);

/**
 *  @defgroup sysio_i2c_mem Mémoires I2C
 *
 *  Ce module fournit les fonctions permettant d'accéder à des mémoires I2c.
 *  Ce module gère les mémoires entre 1 kbits et 4MBits (128 octets à 512 ko).
 *  @{
 */

#define I2CMEM_FLAG_ADDR16    0x80  /**< pointeur d'adresse sur 16 bits */
#define I2CMEM_FLAG_READONLY  0x40  /**< mémoire en lecture seule */

/* structures =============================================================== */
/**
 * @brief contexte pour gérer les mémoire I2C
 * Opaque pour l'utilisateur
 */
struct xI2cMem;

typedef struct xI2cMem xI2cMem;

/**
 * @brief Ouverture d'une mémoire sur bus I2c
 *
 * @param device nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param i2caddr adresse du circuit I2C (alignée à droite)
 * @param mem_size taille de la mémoire
 * @param page_size taille de la page en écriture
 * @param flags drapeaux cf I2CMEM_FLAG_ADDR16, I2CMEM_FLAG_READONLY
 * @return pointeur sur le contexte de la mémoire, NULL si erreur
 */
xI2cMem * xI2cMemOpen (const char * device, int i2caddr, uint32_t mem_size, uint16_t page_size, uint8_t flags);

/**
 * @brief Adresse I2c de la mémoire
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @return adresse I2c alignée à droite
 */
int iI2cMemAddr (const xI2cMem * mem);

/**
 * @brief Taille de la mémoire
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @return taille
 */
uint32_t ulI2cMemSize (const xI2cMem * mem);

/**
 * @brief Taile de la page mémoire pour l'écriture
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @return taille
 */
uint16_t usI2cMemPageSize (const xI2cMem * mem);

/**
 * @brief Drapeaux de la mémoire
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @return drapeaux
 */
uint8_t ucI2cMemFlags (const xI2cMem * mem);


/**
 * @brief Fermeture d'une mémoire sur bus I2c
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @return 0, -1 si erreur
 */
int iI2cMemClose (xI2cMem * mem);


/**
 * @brief Ecriture dans une mémoire sur bus I2c
 *
 * L'écriture se fait page par page,
 * @param mem pointeur sur le contexte de la mémoire
 * @param offset adresse du premier octet à écrire
 * @param buffer pointeur vers les octets à écrire
 * @param size nombre d'octets à écrire
 * @return 0, -1 si erreur
 */
int iI2cMemWrite  (xI2cMem * mem, uint32_t offset, const uint8_t * buffer, uint16_t size);


/**
 * @brief Lecture dans une mémoire sur bus I2c
 *
 * @param mem pointeur sur le contexte de la mémoire
 * @param offset adresse du premier octet à lire
 * @param buffer pointeur vers une zone pour stocker les octets lus
 * @param size nombre d'octets à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iI2cMemRead  (xI2cMem * mem, uint32_t offset, uint8_t * buffer, uint16_t size);

/**
 * @}
 */

/**
 * @}
 */

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /*_SYSIO_I2C_H_ defined */
