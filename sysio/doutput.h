/**
 * @file sysio/douput.h
 * @brief Sorties binaires
 * 
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_DOUTPUT_H_
#define _SYSIO_DOUTPUT_H_
#include <sysio/gpio.h>

__BEGIN_C_DECLS
/* ========================================================================== */
/**
 * @defgroup sysio_doutput Port de sortie
 *
 * Ce module permet de gérer des sorties GPIO qui sont regroupées de façon
 * logique. \n
 * Les broches sont numérotées de façon indépendantes de la plateforme
 * matérielle sous-jacente. Chaque broche peut avoir un état d'activation
 * différent (actif à l'état haut ou bas). \n
 * Le nombre maximal de broches d'un port logique est de 31 sur une plateforme
 * 32 bits (63 pour 64 bits ...)
 * @{
 *  @example led/sysio_demo_led.c
 *  Utilisation pour gérer des leds
 */

/* structures =============================================================== */
/**
 * @brief Broche de sortie
 */
typedef struct xDout {
  int num;  /**< Numéro de la broche conforme à l'interface définie dans gpio.h */
  bool act; /**< Etat actif de la broche, true si actif à l'état haut */
} xDout;

/**
 * @brief Port de sortie
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xDoutPort xDoutPort;

/* internal public functions ================================================ */

/**
 * @brief Ouverture d'un port de sortie
 * @param pins tableau décrivant les broches, le premier élément du tableau
 *         correspondra à la broche 0 du port, le 2ième, la broche 1 ... \n
 *         Cela permet de réorganiser la numérotation de façon logique.
 * @param size nombre de broches de pins
 * @return le port initialisé et ouvert, NULL si erreur
 */
xDoutPort * xDoutOpen (const xDout * pins, unsigned size);

/**
 * @brief Fermeture d'un port de sortie
 *
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutClose (xDoutPort * port);

/**
 * @brief Nombres de broches du port
 *
 * @param port le port de sortie
 * @return le nombre de broches, -1 si erreur
 */
int iDoutPortSize (xDoutPort * port);

/**
 * @brief Désactive une sortie
 *
 * Le niveau électrique réel dépend du niveau d'activation défini dans le
 * tableau pins passé à l'ouverture du port.
 *
 * @param output le numéro logique de la sortie
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutClear (unsigned output, xDoutPort * port);

/**
 * @brief Active une sortie
 *
 * Le niveau électrique réel dépend du niveau d'activation défini dans le
 * tableau pins passé à l'ouverture du port.
 *
 * @param output le numéro logique de la sortie
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutSet (unsigned output, xDoutPort * port);

/**
 * @brief Bascule une sortie
 *
 * @param output le numéro logique de la sortie
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutToggle (unsigned output, xDoutPort * port);

/**
 * @brief Lecture de l'état d'une sortie
 *
 * Le niveau électrique réel dépend du niveau d'activation défini dans le
 * tableau pins passé à l'ouverture du port.
 *
 * @param output le numéro logique de la sortie
 * @param port le port de sortie
 * @return true si active, false si inactive, -1 si erreur
 */
int iDoutRead (unsigned output, xDoutPort * port);

/**
 * @brief Désactive toutes les sorties
 *
 * Le niveau électrique réel de chaque sortie dépend de leur niveau d'activation
 * défini dans le tableau pins passé à l'ouverture du port.
 *
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutClearAll (xDoutPort * port);

/**
 * @brief Active toutes les sorties
 *
 * Le niveau électrique réel de chaque sortie dépend de leur niveau d'activation
 * défini dans le tableau pins passé à l'ouverture du port.
 *
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutSetAll (xDoutPort * port);

/**
 * @brief Bascule toutes les sorties
 *
 * @param port le port de sortie
 * @return 0, -1 si erreur
 */
int iDoutToggleAll (xDoutPort * port);

/**
 * @brief Lecture de l'état de toutes les entrées
 *
 * Le niveau électrique réel de chaque broche dépend de leur niveau d'activation
 * défini dans le tableau pins passé à l'ouverture du port.
 *
 * @param port le port de sortie
 * @return masque de bits correspondant à l'état des entrées, le bit 0
 *          correspond à la sortie 0 (0: inactive, 1:active), le bit 1 à la sortie
 *          1 etc ... (-1 si erreur)
 */
int iDoutReadAll (xDoutPort * port);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_DOUTPUT_H_ defined */
