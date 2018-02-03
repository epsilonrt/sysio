/**
 * @file
 * @brief Capteur I2c HIH6130 de Honeywell
 *
 * Copyright © 2016 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_HIH6130_H_
#define _SYSIO_HIH6130_H_
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */


/**
 *  @defgroup sysio_hih6130 Capteur I2C HIH6130
 *
 *  Ce module permet la commande d'un capteur d'humidité et de température
 *  I2C HIH6130 de Honeywell. Ce module est compatible avec les circuits
 *  suivants :
 *  - ChipCap 2
 *  .
 *  @{
 *  @example i2c/sensor-hih6130/sysio_demo_i2c_hih6130.c
 *  Affichage de la température et de l'humidité
 */

/* constants ================================================================ */
/**
 * @brief Adresse I2c par défaut du circuit HIH6130
 * 
 * La valeur par défaut indiquée dans la documentation 0x27 est
 * exprimée en hexadécimal alignée à droite.
 */
#define HIH6130_I2CADDR  0x27

/**
 * @brief Adresse I2c par défaut du circuit ChipCap 2
 * 
 * La valeur par défaut indiquée dans la documentation 0x28 est
 * exprimée en hexadécimal alignée à droite.
 */
#define CHIPCAP2_I2CADDR  0x28

/**
 * @brief Valeur indiquant que la mesure est en cours.
 */
#define HIH6130_BUSY 1

/* structures =============================================================== */
/**
 * @brief contexte pour gérer les capteurs HIH6130
 * Opaque pour l'utilisateur
 */
struct xHih6130;

typedef struct xHih6130 xHih6130;

/**
 * @brief Données de mesure fournies par un capteur HIH6130
 */
typedef struct xHih6130Data {
  double dHum;   /**< Humidité relative en % */
  double dTemp;  /**< Température corrigée de oC */
} xHih6130Data;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'un circuit HIH6130
 *
 * @param device nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param i2caddr adresse du circuit I2C (alignée à droite)
 * @return pointeur sur le contexte, NULL si erreur
 */
xHih6130 * xHih6130Open (const char * device, int i2caddr);

/**
 * @brief Fermeture d'un circuit HIH6130
 *
 * @param sensor pointeur sur le contexte du circuit HIH6130
 * @return 0, -1 si erreur
 */
int iHih6130Close (xHih6130 * sensor);

/**
 * @brief Démarrer la conversion
 * 
 * Le temps de conversion est de 36.65 ms d'après la documentation (vérifié par
 * la mesure). \n
 * iHih6130Read() permet de scruter si la mesure est terminée.
 * @param sensor pointeur sur le contexte du circuit HIH6130
 * @return 0 en cas de succès, une valeur négative en cas d'erreur.
 */
int iHih6130Start (xHih6130 * sensor);

/**
 * @brief Lecture des derniers résultats de conversion
 * 
 * @param sensor pointeur sur le contexte du circuit HIH6130
 * @param data Pointeur sur une variable qui contiendra les derniers résultats
 * @return 0 en cas de succès, HIH6130_BUSY tant que la mesure n'est pas
 * terminée, une valeur négative en cas d'erreur.
 */
int iHih6130Read (xHih6130 * sensor, xHih6130Data * data);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif  /* _SYSIO_HIH6130_H_ defined */
