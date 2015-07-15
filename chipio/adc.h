/**
 * @file sysio/chipio/adc.h
 * @brief Convertisseur analogique-numérique ChipIo
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_CHIPIO_ADC_H_
#define _SYSIO_CHIPIO_ADC_H_

#include <chipio/chipio.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @addtogroup chipio
 *  @{
 *  @defgroup chipio_adc Convertisseur analogique-numérique
 *
 *  Ce module fournit les fonctions permettant de contrôler le CAN
 *  d'un circuit d'entrées-sorties universel ChipIo.
 *  @{
 */

/* constants ================================================================ */
/**
 * Tension de référence du CAN
 */
typedef enum {
  eAdcRefExternal  = 0x00, /**< Tension de référence de la broche AREF */
  eAdcRefVcc       = 0x01, /**< Tension AVCC avec un condensateur sur AREF */
  eAdcRefInternal  = 0x03, /**< Tension de référence interne de 1.1 V */
  eAdcRefUnknown   = -1,
} eAdcRef;

/**
 * Facteur de division d'horloge CAN
 * 
 * La fréquence de l'horloge ChipIo est de 14,7456 MHz. L'horloge du CAN est 
 * dérivée de l'horloge ChipIo, sa fréquence est divisée par un facteur qui
 * peut être modifier. Plus le facteur de division est élevé, plus la mesure 
 * sera précise au détriment de la rapidité de mesure.
 * Une conversion prend 13 cycles d'horloge CAN. Le temps de mesure sera donc,
 * par exemple, de 113 µs pour un facteur de division d'horloge de 128. \n
 * @note Pour garantir un rapport signal/bruit suffisant à une mesure sur 10
 * bits, il faut que le facteur de division soit supérieur ou égal à 16.
 */
typedef enum {
  eAdcDiv2        = 2, 
  eAdcDiv4        = 4, 
  eAdcDiv8        = 8, 
  eAdcDiv16       = 16, 
  eAdcDiv32       = 32, 
  eAdcDiv64       = 64, 
  eAdcDiv128      = 128, 
  eAdcDivUnknown  = -1
} eAdcDiv;


/* structures =============================================================== */

/**
 * CAN ChipIo
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xChipIoAdc xChipIoAdc;

/* internal public functions ================================================ */

/**
 * Ouverture d'un CAN
 *
 * Le port est ouvert en lecture/écriture non bloquant.
 *
 * @param xChip Pointeur sur objet xChipIo ouvert avec xChipIoOpen()
 * @param xIrqPin Pointeur sur broche d'interruption, NULL si inutilisée
 * @return Pointeur sur le CAN ouvert, NULL si erreur
 */
xChipIoAdc * xChipIoAdcOpen (xChipIo * xChip);

/**
 *  Fermeture du CAN
 *
 * @param xAdc Pointeur sur le CAN
 */
void vChipIoAdcClose (xChipIoAdc * xAdc);

/**
 * Retourne la valeur d'une entrée analogique
 *
 * @param input le numéro de l'entrée concernée (0 à 3)
 * @param xAdc Pointeur sur le CAN
 * @return la valeur analogique en LSB sur 10 bits, -1 si erreur
 */
int iChipIoAdcRead (unsigned input, xChipIoAdc * xAdc);

/**
 * Renvoie la tension de référence
 * 
 * @param xAdc Pointeur sur le CAN
 * @return la tension de référence, -1 si erreur
 */
eAdcRef eChipIoAdcRef (xChipIoAdc * xAdc);

/**
 * Modifie la tension de référence
 * 
 * @param eRef choix de la tension de référence
 * @param xAdc Pointeur sur le CAN
 * @return la valeur réglée, -1 si erreur
 */
eAdcRef eChipIoAdcSetRef (eAdcRef eRef, xChipIoAdc * xAdc);

/**
 * Renvoie le facteur de division de l'horloge du CAN
 * 
 * @param xAdc Pointeur sur le CAN
 * @return le facteur de division, -1 si erreur
 */
eAdcDiv eChipIoAdcDiv (xChipIoAdc * xAdc);

/**
 * Modifie le facteur de division de l'horloge du CAN
 * 
 * @param eDiv choix du facteur de division
 * @param xAdc Pointeur sur le CAN
 * @return la valeur réglée, -1 si erreur
 */
eAdcDiv eChipIoAdcSetDiv (eAdcDiv eDiv, xChipIoAdc * xAdc);


#if defined(__DOXYGEN__)
/* macros =================================================================== */
/**
 * @brief Retourne le nombre de LSB associé à une grandeur
 * @param MEASUREMENT Grandeur à convertir
 * @param FULL_SCALE  Pleine échelle de l'ADC exprimée dans la même unité que
 * MEASUREMENT
 * @return le nombre de LSB associé à MEASUREMENT
 */
#define ADC_VALUE(MEASUREMENT,FULL_SCALE)

/**
 * @brief Retourne la grandeur associée à un nombre de LSB
 * @param VALUE Nombre de LSB à convertir
 * @param FULL_SCALE  Pleine échelle de l'ADC exprimée dans la même unité que
 * que la valeur retournée
 * @return La grandeur associée à VALUE
 */
#define ADC_MEASUREMENT(VALUE,FULL_SCALE)

/**
 *  @}
 * @}
 */

#else /* __DOXYGEN__ not defined */
/* ========================================================================== */

/* macros =================================================================== */
#define ADC_VALUE(_MEASUREMENT,_FULL_SCALE) \
  ((uint16_t)(((_MEASUREMENT)*1024.0)/(_FULL_SCALE)))

#define ADC_MEASUREMENT(_VALUE,_FULL_SCALE) \
  ((((double)(_VALUE))*((double)(_FULL_SCALE)))/1024.0)

#endif /* __DOXYGEN__ not defined */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_CHIPIO_ADC_H_ */
