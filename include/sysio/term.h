/**
 * @file 
 * @brief Terminaux
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_TERM_H_
#define _SYSIO_TERM_H_
#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */
#include <stdio.h>

/**
 *  @defgroup sysio_term Terminaux
 *
 *  Ce module fournit des fonctions pour les terminaux.
 *  @{
 */

/* internal public functions ================================================ */

/**
 * @brief Test si touche appuyée au clavier
 * Le mode raw doit activer avec raw() pour ne pas bloquer en attente de ENTER
 * @return valeur positive si touche appuyé, sinon 0, -1 pour erreur
 */
int kbhit (void);

/**
 * @brief Activation mode raw de l'entrée standard
 * L'activation du mode raw désactive l'attente d'un ENTER pour valider un appui
 * d'une touche clavier. \n
 * @warning Le mode raw doit être désactiver avant de quiter le programme sous
 * peine de perdre la main sur la ligne de commande.
 * @param true pour activer
 * @return 0, -1 pour erreur
 */
int raw (bool state);

/**
 * @brief Activation mode sans echo de l'entrée standard
 * L'activation du mode raw désactive l'attente d'un ENTER pour valider un appui
 * d'une touche clavier. \n
 * @warning Le mode noecho doit être désactiver avant de quiter le programme sous
 * peine de perdre la main sur la ligne de commande.
 * @param true pour activer
 * @return 0, -1 pour erreur
 */
int noecho (bool state);

/**
 * @}
 */

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /* _SYSIO_TERM_H_ defined */
