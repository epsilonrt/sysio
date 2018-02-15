/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_TLC59116_H_
#define _SYSIO_TLC59116_H_

#include <sysio/defs.h>

#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */

/**
 *  @addtogroup sysio_ledrgb
 *  @{
 *  @defgroup sysio_tlc59116 Contrôleur TLC59116

 *  Partie spécifique aux contrôleurs TLC59116. \n
 *  Les TLC59116 supportent :
 *  - la fonction \c iLedRgbSetDimmer() pour le réglage de luminosité qui 
 *  s'applique à toutes les leds reliées à un contrôleur (le paramètre item est
 *  donc le numéro du contrôleur). Le réglage s'effectue entre 0 et 255.
 *  - la fonction \c iLedRgbSetBlinker() pour le clignotement qui 
 *  s'applique à toutes les leds reliées à un contrôleur (le paramètre item est
 *  donc le numéro du contrôleur). Le réglage s'effectue entre 42 et 10000 pour
 *  la période (paramètre period) et entre 0 et 255 pour le rapport cyclique
 *  (paramètre dcycle).
 *  - la fonction \c iLedRgbGetError() qui permet la détection de circuit ouvert et
 *  de surchauffe des leds et du contrôleur.
 *  - la fonction \c iLedRgbClearError() qui permet d'acquiter les défauts après
 *  avoir corrigé la cause du dysfonctionnement.
 *  - la fonction \c iLedRgbSetGain() qui permet de régler le courant max
 *  dans les leds.
 *  .
 *  @{
 * @example i2c/ledrgb-tlc59116/sysio_demo_ledrgb_tlc59116.c
 * Démonstration du module LedRgb avec un contrôleur TLC59116. \n
 * Ce programme allume successivement 4 leds RGB connectées à un TLC59116
 * avec les couleurs de l'arc-en-ciel, puis il modifie la luminosité des
 * leds pour chaque couleur et enfin effectue un test de la fonction
 * clignotement couleur par couleur.
 */

/* constants ================================================================ */
/**
 * @brief Adresse I²C de base de la famille TLC59116
 */
#define TLC59116_ADDR_BASE   0x60

/* macros =================================================================== */
/**
 * @brief Définition de l'adresse I²C d'un circuit TLC59116
 * 
 * @param _a3 état logique de la borche A3 du TLC59116 (0 ou 1)
 * @param _a2 état logique de la borche A2 du TLC59116 (0 ou 1)
 * @param _a1 état logique de la borche A1 du TLC59116 (0 ou 1)
 * @param _a0 état logique de la borche A0 du TLC59116 (0 ou 1)
 * @return adresse I²C du circuit TLC59116 correespondant
 */
#define TLC59116_ADDR(_a3,_a2,_a1,_a0) \
  ((TLC59116_ADDR_BASE) + ((_a3)*16)+((_a2)*8)+((_a1)*4)+((_a0)*2))

/**
 * @brief Définition du gain d'un circuit TLC59116
 * 
 * Permet le calcul du gain fourni à la fonction \c iLedRgbSetGain(). Le
 * calcul du courant max dans les leds dépend de la valeur de ce gain et de
 * la résistance connectée à la broche REXT du TLC59116 tel que défini au § 9.5.9
 * du datasheet p. 20
 * 
 * @param _cm High/Low current multiplier (0 Low, 1 High). 1 après reset
 * @param _hc Voltage Subband  (0 Low, 1 High). 1 après reset
 * @param _cc Current multiplier (0 à 63). 63 après reset
 * @return valeur du gain correspondant
 */
#define TLC59116_IREF(_cm,_hc,_cc) ((((_cm)&1)<<7)+(((_hc)&1)<<6)+((_cc)&0x3F))

/**
 * @brief Broche d'un TLC59116
 * 
 * Utilsé pour la définition de la configuration d'une led.
 */
typedef struct xTlc59116Pin {
  int out;  ///< numéro de la sortie entre 0 et 15 (/OUT0 à /OUT15)
  int ctrl; ///< numéro de contrôleur 0 à (nof_ctrl - 1)
} xTlc59116Pin;

/**
 * @brief Configuration d'une Led connectée à un TLC59116
 * 
 * Cette structure doit être utilisée pour passer la configuration de la
 * led à la fonction iLedRgbAddLed() (paramètre led_config)
 */
typedef struct xTlc59116Led {
  xTlc59116Pin red; ///< broche de commande du rouge
  xTlc59116Pin green; ///< broche de commande du vert
  xTlc59116Pin blue; ///< broche de commande du bleu
} xTlc59116Led;

/**
 * @brief Configuration d'un contrôleur TLC59116
 * 
 * Cette structure doit être utilisée pour passer la configuration du
 * contrôleur à la fonction xLedRgbNewDevice() (paramètre ctrl_list)
 */
typedef struct xTlc59116Config {
  char * i2c_bus;
  int i2c_addr;
} xTlc59116Config;

/**
 *  @}
 * @}
 */
#ifdef __cplusplus
  }
#endif
/* ========================================================================== */
#endif  /* _SYSIO_TLC59116_H_ not defined */
