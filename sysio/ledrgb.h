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

__BEGIN_C_DECLS
/* ========================================================================== */
#include <sysio/rgb.h>


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
  MODE_OFF = 0,     ///< Led éteinte
  MODE_ON = 1,      ///< Led allumée sans variation d'intensité
  MODE_BRIGHT = 2,  ///< Led avec variation d'intensité
  MODE_BLINK = 3    ///< Led avec variation d'intensité et clignotement
  } eLedRgbMode;

typedef enum {
  DEVICE_TLC59116 = 0
} eLedRgbDeviceModel;

struct xLedRgbDevice;

typedef struct xLedRgbDevice xLedRgbDevice;

/* internal public functions ================================================ */

xLedRgbDevice * xLedRgbNewDevice (eLedRgbDeviceModel model);
int iLedRgbDeleteDevice (xLedRgbDevice * d);

int iLedRgbAddLed ()

/**
 * @brief Initialise le module
 *
 * Après un initialisation du contrôleur, un test de présence est réalisé avant
 * avant de configurer toutes les leds en mode variation d'intensité et
 * clignotement (MODE_BLINK)
 * @return 0 en cas de succès. Une valeur négative en cas d'erreur.
 */
int8_t iLedRgbInit (void);

/**
 * @brief Modifie la couleur d'une ou plusieurs leds
 *
 * @param xLed Masque des leds à modifier. Un bit 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param ulColor Nouvelle couleur
 */
void vLedRgbSetColor (uint64_t xLed, uint32_t ulColor);

/**
 * @brief Modifie le mode d'allumage d'une ou plusieurs leds
 *
 * @param xLed Masque des leds à modifier. Un bit 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param eMode Nouveau mode
 */
void vLedRgbSetMode  (uint64_t xLed, eLedRgbMode eMode);

/**
 * @brief Modifie la luminosité globale de toutes les leds configurées en mode MODE_BLINK
 *
 * Seules les LEDS en mode MODE_BLINK sont affectées.
 * A l'initialisation la luminosité globale est à sa valeur maximale (255).
 * @param ucDimming Nouvelle luminosité entre 0 et 255
 */
void vLedRgbSetGlobalDimming  (uint8_t ucDimming);

/**
 * @brief Fait clignoter toutes les leds configurées en mode MODE_BLINK
 *
 * Seules les LEDS en mode MODE_BLINK sont affectées. Pour mettre fin au
 * clignotement, il faut faire un appel à vLedRgbSetGlobalDimming()
 * @param usPeriod Période en millisecondes. La valeur minimale et maximale
 * dépend du contrôleur utilisé.
 * @param ucDutyCycle Rapport cyclique entre 0 et 255
 */
void vLedRgbSetGlobalBlinking (uint16_t usPeriod, uint8_t ucDutyCycle);

/**
 * @brief Renvoie les erreurs des leds
 *
 * En fonction du contrôleur utilisé, celui-ci peut effectuer une surveillance
 * des leds (led en circuit ouvert, surchauffe ...)
 * @return les bits d'erreurs pour chaque led multicolore. 1 indique une erreur.
 * Si non implémenté par le contrôleur renvoie toujours 0.
 */
uint64_t xLedRgbError (void);

void vLedRgbClearError (void);

/**
 * @}
 */
__END_C_DECLS
/* ========================================================================== */
#endif  /* _SYSIO_LEDRGB_H_ not defined */
