/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 * 
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_LEDRGB_H_
#define _SYSIO_LEDRGB_H_

#include <sysio/defs.h>
#include <sysio/rgb.h>
#include <sysio/tlc59116.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup ledrgb_module Leds RGB

 *  Ce module permet de gérer les leds multicolores (Rouge, Verte, Bleue) de
 *  façons indépendantes de la plate-forme matérielle. \n
 *  Son implémentation fait appel à un contrôleur spécialisé (TLC59116...). \n
 *  La macro AVRIO_LEDRGB_ENABLE doit être définie dans avrio-config.h ou dans
 *  le makefile et dans ce cas, la partie interface matérielle doit être
 *  configurée dans le fichier avrio-board-ledrgb.h
 *  @{
 *  @example output/ledrgb/demo_ledrgb.c
 *  Modification des couleurs, de la luminosité et clignotement de leds RGB.
 */

/* constants ================================================================ */
/**
 * @def LEDRGB_ALL_LEDS
 * @brief Masque de sélection pour toutes les leds
 */
#define LEDRGB_ALL_LEDS -1llu

/**
 * @def LEDRGB_NO_LED
 * @brief Masque de sélection pour aucune led
 */
#define LEDRGB_NO_LED 0

/**
 * @enum eLedRgbMode
 * Mode d'allumage d'une LED RGB
 */
typedef enum {
  eLedRgbModeOff = 0,     ///< Led éteinte
  eLedRgbModeOn = 1,      ///< Led allumée sans variation d'intensité
  eLedRgbModeDimmer = 2,  ///< Led avec variation d'intensité
  eLedRgbModeBlinker = 3    ///< Led avec variation d'intensité et clignotement
  } eLedRgbMode;

/**
 * @enum eLedRgbDeviceModel
 * @brief Modèle de contrôleur de leds RGB
 */
typedef enum {
  eLedRgbDeviceNone = 0,      ///< Pas de contrôleur
  eLedRgbDeviceTlc59116 = 1,  ///< Contrôleur TI I²C TLC59116
} eLedRgbDeviceModel;

struct xLedRgbDevice;

typedef struct xLedRgbDevice xLedRgbDevice;

/* internal public functions ================================================ */

xLedRgbDevice * xLedRgbNewDevice (eLedRgbDeviceModel model, void * dev_setup);
int iLedRgbDeleteDevice (xLedRgbDevice * d);

int iLedRgbSize (const xLedRgbDevice * d);
int iLedRgbAddLed (xLedRgbDevice * d, void * setup);

/**
 * @brief Modifie la couleur d'une ou plusieurs leds
 *
 * @param xLed Masque des leds à modifier. Un bit 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param ulColor Nouvelle couleur
 */
int iLedRgbSetColor (xLedRgbDevice * d, uint64_t leds, uint32_t color);
/**
 * @brief Modifie le mode d'allumage d'une ou plusieurs leds
 *
 * @param xLed Masque des leds à modifier. Un bit 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param eMode Nouveau mode
 */
int iLedRgbSetMode  (xLedRgbDevice * d, uint64_t leds, eLedRgbMode mode);
/**
 * @brief Modifie la luminosité globale de toutes les leds configurées en mode MODE_BLINK
 *
 * Seules les LEDS en mode eLedRgbModeBlinker sont affectées.
 * A l'initialisation la luminosité globale est à sa valeur maximale (255).
 * @param ucDimming Nouvelle luminosité entre 0 et 255
 */
int iLedRgbSetDimmer  (xLedRgbDevice * d, uint16_t dimming);
/**
 * @brief Fait clignoter toutes les leds configurées en mode MODE_BLINK
 *
 * Seules les LEDS en mode eLedRgbModeBlinker sont affectées. Pour mettre fin au
 * clignotement, il faut faire un appel à vLedRgbSetGlobalDimming()
 * @param usPeriod Période en millisecondes. La valeur minimale et maximale
 * dépend du contrôleur utilisé.
 * @param ucDutyCycle Rapport cyclique entre 0 et 255
 */
int iLedRgbSetBlinker  (xLedRgbDevice * d, uint16_t blinking);

/**
 * @brief Renvoie les erreurs des leds
 *
 * En fonction du contrôleur utilisé, celui-ci peut effectuer une surveillance
 * des leds (led en circuit ouvert, surchauffe ...)
 * @return les bits d'erreurs pour chaque led multicolore. 1 indique une erreur.
 * Si non implémenté par le contrôleur renvoie toujours 0.
 */
uint64_t xLedRgbError (xLedRgbDevice * d);

void vLedRgbClearError (xLedRgbDevice * d);

/**
 * @}
 */
__END_C_DECLS
/* ========================================================================== */
#endif  /* _SYSIO_LEDRGB_H_ not defined */
