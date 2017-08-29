/**
 * Copyright © 2017 epsilonRT, All rights reserved.
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * <http://www.cecill.info>.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 *
 * @file
 * @brief Capteur gp2-i2c
 */
#ifndef _SYSIO_GP2_H_
#define _SYSIO_GP2_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_gp2 Capteur I2C GP2
 *
 *  Ce module permet la commande d'un capteur de particules fines GP2Y1010AU0F
 *  équipé d'un module gp2-i2c <https://github.com/epsilonrt/gp2-i2c>
 *  @{
 *  @example i2c/sysio_demo_i2c_gp2.c
 *  Affichage de la densité de particules fines
 */

/* constants ================================================================ */
/**
 * @brief Adresse I2c par défaut 
 * 
 * La valeur par défaut exprimée en hexadécimal alignée à droite.
 */
#define GP2I2C_I2CADDR  0x46

/* structures =============================================================== */
/**
 * @brief Repère d'étalonnage
 */
struct xG2pSetting {
  double dD1; /**< Densité de particules fines du premier point (min.) en µg/m3 */
  double dV1; /**< Tension en millivolts correspondant au premier point */
  double dD2; /**< Densité de particules fines du deuxième point (max.) en µg/m3 */
  double dV2; /**< Tension en millivolts correspondant au deuxième point */
};
typedef struct xG2pSetting xG2pSetting;

/**
 * @brief Capteur 
 */
struct xG2pSensor;
typedef struct xG2pSensor xG2pSensor;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'un capteur
 * @param sI2cBus nom du fichier d'accès au bus I2C (par exemple /dev/i2c-1)
 * @param iI2cAddr adresse du circuit I2C (alignée à droite)
 * @param xSetting pointeur sur le repère d'étalonnage
 * @return pointeur sur le nouveau capteur, NULL si erreur
 */
xG2pSensor * xGp2Open (const char * sI2cBus, int iI2cAddr, const xG2pSetting * xSetting);

/**
 * @brief Fermeture et destruction d'un capteur
 * @param xSensor pointeur sur le capteur
 * @return 0, -1 si erreur
 */
int iGp2Close (xG2pSensor * xSensor);

/**
 * @brief Modification du repère d'étalonnage
 * @param xSensor pointeur sur le capteur
 * @param xSetting
 */
void vGp2SetSetting (xG2pSensor * xSensor, const xG2pSetting * xSetting);

/**
 * @brief Lecture de la tension moyenne en sortie du capteur
 * @param xSensor pointeur sur le capteur
 * @return tension en millivolts, -1 si erreur
 */
int iGp2ReadVoltage (xG2pSensor * xSensor);

/**
 * @brief Lecture de la densité de particules fines
 * @param xSensor pointeur sur le capteur
 * @return densité de particules fines en µg/m3, -1 si erreur
 */
int iGp2Read (xG2pSensor * xSensor);

/**
 * @brief Convertion d'une tension en densité de particules fines
 * @param iVoltage tension en millivolts
 * @return densité de particules fines en µg/m3
 */
int iGp2VoltageToDensity (int iVoltage, const xG2pSetting * xSetting);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_GP2_H_ defined */
