/**
 * @file sysio/iaq.h
 * @brief Capteur I2c IAQ de amS
 *
 * Copyright © 2016 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_IAQ_H_
#define _SYSIO_IAQ_H_
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */


/**
 *  @defgroup sysio_iaq Capteur I2C IAQ
 *
 *  Ce module permet la commande d'un capteur I2C IAQ de amS. Ce capteur permet
 *  la mesure de qualité de l'air (CO2 et composés volatiles)
 *  .
 *  @{
 *  @example i2c/iaq/sysio_demo_iaq.c
 *  Affichage du taux de CO2 en ppm (partie par million) et du taux de
 *  composés volatiles en ppb (partie par milliard).
 *  @{
 */

/* constants ================================================================ */
/**
 * @brief Adresse I2c par défaut du circuit IAQ
 * 
 * La valeur par défaut indiquée dans la documentation 0x5A est
 * exprimée en hexadécimal alignée à droite.
 */
#define IAQ_I2CADDR  0x5A

#define IAQ_BUSY  0x01
#define IAQ_WARMUP 0x10 // (module in warm up phase)

/* structures =============================================================== */
/**
 * @brief contexte pour gérer les capteurs IAQ
 * Opaque pour l'utilisateur
 */
struct xIaq;

typedef struct xIaq xIaq;

/**
 * @brief Données de mesure fournies par un capteur IAQ
 */
typedef struct xIaqData {
  uint16_t usCo2;   /**< Taux de Co2 en ppm */
  uint16_t usTvoc;   /**< Taux de composés volatiles en ppb */
} xIaqData;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'un circuit IAQ
 *
 * @param device nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param i2caddr adresse du circuit I2C (alignée à droite)
 * @return pointeur sur le contexte, NULL si erreur
 */
xIaq * xIaqOpen (const char * device, int i2caddr);

/**
 * @brief Fermeture d'un circuit IAQ
 *
 * @param sensor pointeur sur le contexte du circuit IAQ
 * @return 0, -1 si erreur
 */
int iIaqClose (xIaq * sensor);

/**
 * @brief Lecture des derniers résultats de conversion
 * 
 * La version P (Pulse) fournit une mesure toutes les 11 s, inutile de lire
 * le capteur avec une période plus petite. \n
 * Le capteur doit être alimenté pendant 5 min avant de fournir une première
 * mesure, pendant cette phase (Warm up), cette fonction renvoie 
 * 
 * @param sensor pointeur sur le contexte du circuit IAQ
 * @param data Pointeur sur une variable qui contiendra les derniers résultats
 * @return 0 en cas de succès, IAQ_BUSY tant que la mesure n'est pas
 * terminée, une valeur négative en cas d'erreur.
 */
int iIaqRead (xIaq * sensor, xIaqData * data);

/* ========================================================================== */
__END_C_DECLS
#endif  /* _SYSIO_IAQ_H_ defined */
