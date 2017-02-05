/**
 * @file sysio/spi.h
 * @brief Bus SPI
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_SPI_H_
#define _SYSIO_SPI_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_spi Bus SPI
 *
 *  Ce module fournit les fonctions permettant d'accéder au bus SPI.
 *  @{
 *  @example spi/rfm69/sysio_demo_rfm69.c
 *  Accès aux registres d'un module de transmission
 */

/* constants ================================================================ */
#define SPI_CPHA    0x01 /**< phase horloge: 0 premier front (montant si CPOL=0), 1 deuxième front (descendant si CPOL=0) */
#define SPI_CPOL    0x02 /**< polarité horloge: 0 horloge sur état haut, 1 horloge sur état bas */
#define SPI_REG_RW_BIT 7 /**< bit lecture-écriture pour les fonctions de la famille Reg */
    
/**
 * @enum eSpiMode
 * @brief Mode SPI
 * 
 * Différents modes \n  <hr>
  | SPI Mode | POL | PHA | 1er front                   | 2ème front                  |
  | :------: | :-: | :-: | :-------------------------- | :-------------------------- |
  | 0        |  0  |  0  | montant, échantillonnage    | descendant, changement état |
  | 1        |  0  |  1  | montant, changement état    | descendant, échantillonnage |
  | 2        |  1  |  0  | descendant, échantillonnage | montant, changement état    |
  | 3        |  1  |  1  | descendant, changement état | montant, échantillonnage    |
 * 
 * cf [Wikipedia](https://fr.wikipedia.org/wiki/Serial_Peripheral_Interface) 
 */
typedef enum {
  eSpiMode0 = (0|0),
  eSpiMode1 = (0|SPI_CPHA),
  eSpiMode2 = (SPI_CPOL|0),
  eSpiMode3 = (SPI_CPOL|SPI_CPHA),
  eSpiModeNotSet = -1 /**< Non modifié */
} eSpiMode;

/**
 * @enum eSpiNumbering
 * @brief Ordre des bits
 */
typedef enum {
  eSpiNumberingMsb = 0, /**< MSB en premier */
  eSpiNumberingLsb = 1,/**< LSB en premier */
  eSpiNumberingNotSet = -1 /**< Non modifié */
} eSpiNumbering;

/**
 * @enum eSpiBits
 * @brief Nombre de bits par mot
 */
typedef enum {
  eSpiBits8 = 8, /**< 8 bits */
  eSpiBits7 = 7, /**< 7 bits */
  eSpiBitsNotSet = -1 /**< Non modifié */
} eSpiBits;

/* structures =============================================================== */
/**
 * Configuration d'un port SPI
 */
typedef struct xSpiIos {
  eSpiMode mode; /**< Mode horloge */
  eSpiNumbering lsb; /**< Ordre des bits */
  uint8_t bits; /**< Nombre de bits par mot */
  uint32_t speed; /**< fréquence horloge en Hz, 0 si non spécifié */
} xSpiIos;


/* internal public functions ================================================ */
/**
 * @brief Ouverture d'une connexion vers un circuit SPI
 *
 * @param device nom du fichier d'accès au bus SPI (par exemple /dev/spidev0.1,
 * 0 pour le numéro de bus, 1 pour le numéro de broche CS)
 * @param config configuration de la connexion
 * @return le descripteur de fichier vers la connexion ouverte, -1 si erreur
 */
int iSpiOpen (const char *device);

/**
 * @brief Fermeture d'une connexion vers un circuit SPI
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @return 0, -1 si erreur
 */
int iSpiClose (int fd);

/**
 * @brief Lecture configuration
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param config configuration de la connexion
 * @return 0, -1 si erreur
 */
int iSpiSetConfig (int fd, const xSpiIos * config);

/**
 * @brief Lecture configuration
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param config configuration de la connexion (lue)
 * @return 0, -1 si erreur
 */
int iSpiGetConfig (int fd, xSpiIos * config);

/**
 * @brief Lecture d'un bloc octets
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param len nombre d'octets à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iSpiRead (int fd, uint8_t * buffer, uint8_t len);

/**
 * @brief Ecriture d'un bloc octets
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param buffer pointeur vers la zone de stockage des octets
 * @param len nombre d'octets à écrire
 * @return le nombre d'octets écrits, -1 si erreur
 */
int iSpiWrite (int fd, uint8_t * buffer, uint8_t len);

/**
 * @brief Lecture/Ecriture d'un bloc octets
 * 
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param tx_buffer pointeur vers la zone de stockage des octets
 * @param tx_len nombre d'octets à écrire
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param len nombre d'octets à lire
 * @return
 */
int iSpiXfer (int fd, uint8_t * tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t rx_len);

/**
 * @brief Lecture d'un registre 8 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture d'un octet.\n
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre
 * @return la valeur de l'octet comme un unsigned, -1 si erreur
 */
int iSpiReadReg8 (int fd, uint8_t reg);

/**
 * @brief Lecture d'un registre 16 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture de 2 octets
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre
 * @return la valeur du mot comme un unsigned, -1 si erreur
 */
int iSpiReadReg16 (int fd, uint8_t reg);

/**
 * @brief Lecture d'un bloc de registres
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param len nombre de registres à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iSpiReadRegBlock (int fd, uint8_t reg, uint8_t * buffer, uint8_t len);

/**
 * @brief Ecriture d'un registre 8 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre
 * @param data valeur de l'octet
 * @return nombre d'octets écrits, -1 si erreur
 */
int iSpiWriteReg8 (int fd, uint8_t reg, uint8_t data);

/**
 * @brief Ecriture d'un registre 16 bits
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du registre
 * @param data valeur du mot
 * @return nombre d'octets écrits, -1 si erreur
 */
int iSpiWriteReg16 (int fd, uint8_t reg, uint16_t data);

/**
 * @brief Ecriture d'un bloc de registres
 *
 * @param fd descripteur de fichier vers la connexion ouverte
 * @param reg adresse du premier registre
 * @param buffer pointeur vers la zone de stockage des octets
 * @param len nombre d'octets à écrire
 * @return nombre d'octets écrits, -1 si erreur
 */
int iSpiWriteRegBlock (int fd, uint8_t reg, const uint8_t * buffer, uint8_t len);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_SPI_H_ defined */
