/**
 * @file sysio/gpio.h
 * @brief GPIO
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_H_
#define _SYSIO_GPIO_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_gpio GPIO
 *
 *  Ce module fournit les fonctions permettant de contrôler les broches
 *  d'entrées-sorties à usage général.
 *  @{
 *  @example gpio/gpio.c
 */

/* constants ================================================================ */
/**
 * @enum eGpioMode
 * @brief Type de broche
 */
typedef enum  {
  eModeInput  = 0,  /**< entrée */
  eModeOutput = 1,  /**< sortie */
  eModeAlt0   = 4,  /**< fonction auxiliaire n° 0 (dépend de l'architecture */
  eModeAlt1   = 5,  /**< fonction auxiliaire n° 1 (dépend de l'architecture */
  eModeAlt2   = 6,  /**< fonction auxiliaire n° 2 (dépend de l'architecture */
  eModeAlt3   = 7,  /**< fonction auxiliaire n° 3 (dépend de l'architecture */
  eModeAlt4   = 3,  /**< fonction auxiliaire n° 4 (dépend de l'architecture */
  eModeAlt5   = 2,  /**< fonction auxiliaire n° 5 (dépend de l'architecture */
  eModePwm    = 0x8002  /**< sortie PWM */
} eGpioMode;

/**
 * @enum eGpioMode
 * @brief Activation des résistances de tirage
 */
typedef enum  {
  ePullOff  = 0,  /**< résistance désactivée */
  ePullDown = 1,  /**< résistance de tirage à l'état bas */
  ePullUp   = 2   /**< résistance de tirage à l'état haut */
} eGpioPull;

/* structures =============================================================== */
/**
 * @brief Port d'entrée-sortie à usage général (GPIO)
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xGpio xGpio;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'un GPIO
 *
 * @param setup pointeur sur une variable de configuration dépendant de l'architecture,
 *        NULL si inutilisée
 * @return le pointeur sur le GPIO, NULL en cas d'erreur
 */
xGpio * xGpioOpen (void * setup);

/**
 * @brief Fermeture d'un GPIO
 *
 * Le port est fermé si plus aucun thread ne l'utilise
 *
 * @param gpio le pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioClose (xGpio * gpio);

/**
 * @brief Indique si le GPIO est ouvert
 *
 * @param gpio le pointeur sur le GPIO
 * @return true si ouvert, false sinon
 */
bool bGpioIsOpen (xGpio * gpio);

/**
 * @brief Modifie le type d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param iPin le numéro de la broche
 * @param eMode le type de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioMode (int iPin, eGpioMode eMode, xGpio * gpio);

/**
 * @brief Libère une broche de son utilisation
 *
 * La broche correspondante est remise dans son état de repos qui
 * dépend de l'architecture matérielle.
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioRelease (int iPin, xGpio * gpio);

/**
 * @brief Active ou désactive la résistance d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param iPin le numéro de la broche
 * @param ePull le type de la résistance
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioPull (int iPin, eGpioPull ePull, xGpio * gpio);

/**
 * @brief Modification de l'état binaire d'une sortie
 *
 * @param iPin le numéro de la broche
 * @param bValue valeur binaire à appliquer
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioWrite (int iPin, bool bValue, xGpio * gpio);

/**
 * @brief Bascule de l'état binaire d'une sortie
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioToggle (int iPin, xGpio * gpio);

/**
 * @brief Modification de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param iMask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...)
 * @param bValue valeur binaire à appliquer aux broches
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioWriteAll (int iMask, bool bValue, xGpio * gpio);


/**
 * @brief Bascule de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param iMask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...)
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioToggleAll (int iMask, xGpio * gpio);

/**
 * @brief Lecture de l'état binaire d'une broche
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioRead (int iPin, xGpio * gpio);

/**
 * @brief Lecture de l'état binaire d'un ensemble de broches
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param iMask masque de sélection des broches à lire, un bit à 1 indique
 *        que la broche correspondante doit être lue (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). \n Pour lire toutes les broches
 *        iMask doit être mis à 0.
 * @param gpio pointeur sur le GPIO
 * @return l'état des broches, chaque bit correspond à l'état de la broche
 * correspondante (bit 0 pour la broche 0, bit 1 pour la broche 1 ...)
 */
int iGpioReadAll (int iMask, xGpio * gpio);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_GPIO_H_ defined */
