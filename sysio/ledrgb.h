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
 * @defgroup ledrgb_module Leds RGB

 * Ce module permet de gérer les leds multicolores (Rouge, Verte, Bleue) de
 * façons indépendantes de la plate-forme matérielle et du contrôleur. \n
 * @{
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
 * @defgroup ledrgb_ioctl Codes IOCTL
 * Liste des codes de fonctions pour la fonction \c iLedRgbCtl().
 * @{
 */
#define LEDRGB_IOC_SETMODE    1 /**< int setmode (xLedRgbDcb * dcb, int led, eLedRgbMode mode) */
#define LEDRGB_IOC_SETDIMMER  2 /**< int setdimmer (xLedRgbDcb * dcb, int item, int dimming) */
#define LEDRGB_IOC_SETBLINKER 3 /**< int setblinker (xLedRgbDcb * dcb, int item, int period, int dcycle) */
#define LEDRGB_IOC_SETGAIN    4 /**< int setgain (xLedRgbDcb * dcb, int item, int gain); */
#define LEDRGB_IOC_GETERROR 100 /**< int error (xLedRgbDcb * dcb, int led) */
#define LEDRGB_IOC_CLRERROR 101 /**< int clrerror (xLedRgbDcb * dcb); */
/**
 * @}
 */

/**
 * @enum eLedRgbMode
 * Mode d'allumage d'une LED RGB
 */
typedef enum {
  eLedRgbModeOff = 0,  /**< Led éteinte */
  eLedRgbModeOn = 1,   /**< Led allumée sans PWM, ni réglage de luminosité, ni clignotement. Impossible de régler la couleur dans ce mode ! */
  eLedRgbModePwm = 2,  /**< Led avec PWM permettant le changement de couleur (pas de réglage de luminosité, ni de clignotement). C'est le mode normal. */
  eLedRgbModeFull = 3  /**< Led avec PWM, réglage de luminosité et clignotement */
} eLedRgbMode;

/**
 * @enum eLedRgbDeviceModel
 * @brief Modèle de contrôleur de leds RGB
 */
typedef enum {
  eLedRgbDeviceNone = 0,      /**< Pas de contrôleur */
  eLedRgbDeviceTlc59116 = 1,  /**< Contrôleur TI I²C TLC59116 */
} eLedRgbDeviceModel;

/**
 * @brief Classe de contrôle de leds RGB
 * Cette classe est opaque (privée) pour l'utilisateur.
 */
typedef struct xLedRgbDevice xLedRgbDevice;

/* internal public functions ================================================ */
/**
 * @brief Création et initialisation d'un objet LedRgb
 * Cet objet permet le contrôle des leds RGB
 * @param ctrl_model modèle de contrôleur matériel utilisé
 * @param ctrl_list pointeur sur la liste des contrôleurs matériel, le type de
 * variable pointée dépend du modèle de contrôleur.
 * @return pointeur sur l'objet, NULL si erreur
 */
xLedRgbDevice * xLedRgbNewDevice (eLedRgbDeviceModel ctrl_model, void * ctrl_list);

/**
 * @brief Fermeture et destruction d'un objet LedRgb
 * @param d pointeur sur l'objet
 * @return 0, -1 si erreur
 */
int iLedRgbDeleteDevice (xLedRgbDevice * d);

/**
 * @brief Nombre de leds RGB gérées par un objet LedRgb
 * @param d pointeur sur l'objet
 * @return le nombre de leds
 */
int iLedRgbSize (const xLedRgbDevice * d);

/**
 * @brief Ajout d'une led à un objet LedRgb
 * @param d pointeur sur l'objet
 * @param led_config pointeur sur la configuration de la led. Le type de
 * variable pointée dépend du modèle de contrôleur.
 * @return l'index de la led ajoutée [0...], -1 si erreur
 */
int iLedRgbAddLed (xLedRgbDevice * d, eLedRgbMode mode, void * led_config);

/**
 * @brief Modifie la couleur d'une led
 * 
 * @param d pointeur sur l'objet
 * @param led index de la led
 * @param color nouvelle couleur
 * @return 0, -1 si erreur
 */ 
int iLedRgbSetColor (xLedRgbDevice * d, int led, uint32_t color);

/**
 * @brief Modifie la couleur d'une ou plusieurs leds
 *
 * @param grp masque des leds à modifier. Un bit à 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param color nouvelle couleur
 * @return 0, -1 si erreur
 */
int iLedRgbSetGrpColor (xLedRgbDevice * d, uint64_t grp, uint32_t color);

/**
 * @brief Modifie le mode d'allumage d'une led
 *
 * La liste des modes supportés dépend du modèle de contrôleur.
 * 
 * @param d pointeur sur l'objet
 * @param led index de la led
 * @param mode nouveau mode
 * @return 0, -1 si erreur
 */
int iLedRgbSetMode (xLedRgbDevice * d, int led, eLedRgbMode mode);

/**
 * @brief Modifie le mode d'allumage d'une ou plusieurs leds
 *
 * La liste des modes supportés dépend du modèle de contrôleur.
 * 
 * @param d pointeur sur l'objet
 * @param grp Masque des leds à modifier. Un bit 1 indique que l'on souhaite
 * modifier la led correspondante. Un bit 0 indique que la led correspondante
 * restera inchangée.
 * @param mode nouveau mode
 * @return 0, -1 si erreur
 */
int iLedRgbSetGrpMode (xLedRgbDevice * d, uint64_t grp, eLedRgbMode mode);

/**
 * @brief Modifie la luminosité d'un item
 *
 * Seules les LEDS en mode eLedRgbModeFull sont affectées.
 * Cette fonction est disponible que sur certains contrôleur (comme le TLC59116).
 * 
 * @param d pointeur sur l'objet
 * @param item index de l'item à modifier, en fonction du modèle de contrôleur
 * l'item peut être un contrôleur ou une led.
 * @param dimming nouvelle luminosité (plage acceptée dépendant du modèle de contrôleur)
 * @return 0, -1 si erreur ou non disponible (dans ce dernier cas errno vaut EINVAL)
 */
int iLedRgbSetDimmer (xLedRgbDevice * d, int item, int dimming);

/**
 * @brief Clignotement d'un item
 *
 * Seules les LEDS en mode eLedRgbModeFull sont affectées. \n
 * Cette fonction est disponible que sur certains contrôleur (comme le TLC59116).
 * 
 * @param d pointeur sur l'objet
 * @param item index de l'item à modifier, en fonction du modèle de contrôleur
 * l'item peut être un contrôleur ou une led.
 * @param period période en millisecondes (plage acceptée dépendant du modèle de contrôleur)
 * @param dcycle rapport cyclique (plage acceptée dépendant du modèle de contrôleur)
 * @return 0, -1 si erreur ou non disponible (dans ce dernier cas errno vaut EINVAL)
 */
int iLedRgbSetBlinker (xLedRgbDevice * d, int item, int period, int dcycle);

/**
 * @brief Teste si une led est en défaut
 *
 * En fonction du contrôleur utilisé, celui-ci peut effectuer une surveillance
 * des leds (led en circuit ouvert, surchauffe ...). Si le modèle de 
 * contrôleur n'est pas équipé d'une détection de défauts, cette fonction
 * retourne toujours false. \n
 * 
 * @return true si la led est ou a été en défaut, false si fonctionnelle, -1 si erreur.
 */
int iLedRgbGetError (xLedRgbDevice * d, int led);

/**
 * @brief Effacement des défauts
 * @param d pointeur sur l'objet
 * @return 0, -1 si erreur
 */
int iLedRgbClearError (xLedRgbDevice * d);

/**
 * @brief Appel d'une fonction spécifique à un contrôleur
 * Afin de stabiliser l'interface logicielle d'un contrôleur, les fonctions
 * non supportées par tous les contrôleurs passe par cette fonction.
 * 
 * @param d pointeur sur l'objet
 * @param req code de la fonction \c ledrgb_ioctl
 * @return dépend de la fonction demandée, généralement 0 pour succès, -1 pour erreur
 */
int iLedRgbCtl (xLedRgbDevice * d, int req, ...);

/**
 * @}
 */
__END_C_DECLS
/* ========================================================================== */
#endif  /* _SYSIO_LEDRGB_H_ not defined */
