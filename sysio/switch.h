/**
 *  @file sysio/switch.h
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_SWITCH_H_
#define _SYSIO_SWITCH_H_
#include <sysio/dinput.h>

__BEGIN_C_DECLS
/* ========================================================================== */
/**
 * @defgroup sysio_switch Micro-switchs
 *
 * Ce module permet de gérer des micro-switchs qui sont regroupées de façon
 * logique. \n
 * Un micro-switch est un \ref sysio_dinput
 * qui a un type de gestionaire d'interruption différent. \n
 * @warning Un gestionnaire installé avec ce module sur un port d'entrée
 * ne doit pas être modifié avec les fonctions du module \ref sysio_dinput
 * (iDinSetCallback() ou iDinClearCallback())
 * @{
 *  @example switch/switch.c
 *  Utilisation en "interruption"
 */

/* types ==================================================================== */
/**
 * Gestionnaire d'événement d'un ensemble de switchs
 *
 * Déclenché sur modification de niveau d'un switch.
 * Seuls les fronts valides déclenchent un traitement.
 *
 * @param values valeur des switchs (bit 0 pour switch 0, ...)
 * @param sw_changed le numéro du switch qui a changé
 * @return doit retourner 0 si l'événement a été traité, -1 sinon.
 */
typedef int (*iSwitchCallback) (unsigned values, unsigned sw_changed);

/* internal public functions ================================================ */

/**
 * @brief Installation d'un gestionnaire d'interruption utilisateur
 *
 * @param callback nom du gestionnaire (fonction définie par l'utilisateur)
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iSwitchSetCallback (iSwitchCallback callback, xDinPort * sw);

/**
 * @brief Désinstallation d'un gestionnaire d'interruption utilisateur
 *
 * @param input le numéro logique du switch concerné
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iSwitchClearCallback (xDinPort * sw);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_SWITCH_H_ defined */
