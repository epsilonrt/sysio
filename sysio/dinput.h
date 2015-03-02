/**
 * @file sysio/dinput.h
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_DINPUT_H_
#define _SYSIO_DINPUT_H_
#include <sysio/gpio.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 * @defgroup sysio_dinput Port d'entrée
 *
 * Ce module permet de gérer des entrées GPIO qui sont regroupées de façon
 * logique. \n
 * Les broches sont numérotées de façon indépendantes de la plateforme
 * matérielle sous-jacente. Chaque broche peut avoir un état d'activation
 * différent (actif à l'état haut ou bas) et avoir une configuration de sa
 * résistance de tirage différente. \n
 * Chaque entrée peut être lue par scrutation ou être gérée par un gestionnaire
 * défini par l'utilisateur. Le gestionnaire est exécuté à l'apparition d'un
 * front (montant, descendant ou montant et descendant). Bien que ce
 * fonctionnement puisse s'apparenter à une interruption matérielle, il est en
 * fait implémenté grâce au multi-thread. \n
 * Le nombre maximal de broches d'un port logique est de 31 sur une plateforme
 * 32 bits (63 pour 64 bits ...)
 * @{
 *  @example dinput/poll/poll.c
 *  Utilisation en scrutation
 *  @example dinput/callback/callback.c
 *  Utilisation en "interruption"
 *  @example dinput/group/group.c
 *  Utilisation en "interruption" groupée
 */

/* constants ================================================================ */
/**
 * @enum eDinEdge
 * @brief Front d'entrée
 */
typedef enum {
  eEdgeNone = 0,                          /**< aucun front */
  eEdgeFalling = 1,                       /**< front descendant */
  eEdgeRising  = 2,                       /**< front montant */
  eEdgeBoth = eEdgeFalling + eEdgeRising, /**< front descendant et montant */
} eDinEdge;

/* structures =============================================================== */
/**
 * @brief Broche d'entrée
 */
typedef struct xDin {
  int num;          /**< Numéro de la broche conforme à l'interface définie dans gpio.h */
  bool act;         /**< Etat actif de la broche */
  eGpioPull pull;  /**< Etat de la résistance de tirage */
} xDin;

/**
 * @brief Port d'entrée
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xDinPort xDinPort;

/* types ==================================================================== */
/**
 * Gestionnaire utilisateur d'événement d'une broche d'entrée
 *
 * Déclenché sur modification de niveau d'une ou plusieurs broches.
 * Seuls le ou les fronts valides définis lors de l'appel à iDinSetCallback()
 * déclenchent un traitement.
 *
 * @param edge  front déclenchant
 * @param udata pointeur sur les données utilisateur défini lors de l'appel à iDinSetCallback()
 * @return doit retourner 0 si l'événement a été traité, -1 sinon.
 */
typedef int (*iDinCallback) (eDinEdge edge, void *udata);

/**
 * Gestionnaire d'événement d'un groupe d'entrées
 *
 * Déclenché sur modification de niveau d'une entrée.
 * Seuls les fronts valides déclenchent un traitement.
 *
 * @param uInputMask valeur des entrées (bit 0 pour entrée 0, ...)
 * @param uInputNumber le numéro de l'entrée qui a changée
 * @param edge  front déclenchant
 * @param udata pointeur sur les données utilisateur défini lors de l'appel à iDinSetGrpCallback()
 * @return doit retourner 0 si l'événement a été traité, -1 sinon.
 */
typedef int (*iDinGrpCallback) (unsigned uInputMask, unsigned uInputNumber, eDinEdge edge, void *udata);

/* internal public functions ================================================ */

/**
 * @brief Ouverture d'un port d'entrée
 * @param pins tableau décrivant les broches, le premier élément du tableau
 *         correspondra à la broche 0 du port, le 2ième, la broche 1 ... \n
 *         Cela permet de réorganiser la numérotation de façon logique.
 * @param size nombre de broches de pins
 * @return le port initialisé et ouvert, NULL si erreur
 */
xDinPort * xDinOpen (const xDin * pins, unsigned size);

/**
 * @brief Fermeture d'un port d'entrée
 *
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iDinClose (xDinPort * port);

/**
 * @brief Nombres de broches du port
 *
 * @param port le port d'entrée
 * @return le nombre de broches, -1 si erreur
 */
int iDinPortSize (xDinPort * port);

/**
 * @brief Lecture de l'état d'une entrée
 *
 * Le niveau électrique réel dépend du niveau d'activation défini dans le
 * tableau pins passé à l'ouverture du port.
 *
 * @param input le numéro logique de l'entrée
 * @param port le port d'entrée
 * @return true si active, false si inactive, -1 si erreur
 */
int iDinRead (unsigned input, xDinPort * port);

/**
 * @brief Lecture de l'état de toutes les entrées
 *
 * Le niveau électrique réel de chaque broche dépend de leur niveau d'activation
 * défini dans le tableau pins passé à l'ouverture du port.
 *
 * @param port le port d'entrée
 * @return masque de bits correspondant à l'état des entrées, le bit 0
 *          correspond à l'entrée 0 (0: inactive, 1:active), le bit 1 à l'entrée
 *          1 etc ... (-1 si erreur)
 */
int iDinReadAll (xDinPort * port);

/**
 * @brief Réglage du ou des front(s) déclenchant d'une entrée
 *
 * Cette fonction peut être utilisée même si aucun gestionnaire d'interruption
 * n'est défini pour la broche. Il est alors possible de scruter l'apparition
 * de front grâce à iDinEdgeOccured().
 *
 * @param input le numéro logique de l'entrée concernée
 * @param edge front déclenchant (eEdgeNone désactive la détection de front et
 *              l'éxécution du gestionnaire d'interruption)
 * @param port le port d'entrée
 * @return le ou les fronts déclenchants, eEdgeNone si erreur
 */
int iDinSetEdge (unsigned input, eDinEdge edge, xDinPort * port);

/**
 * @brief Front(s) déclenchant d'une entrée
 *
 * @param input le numéro logique de l'entrée concernée
 * @param port le port d'entrée
 * @return le ou les fronts déclenchants, eEdgeNone si erreur
 */
eDinEdge eDinGetEdge (unsigned input, xDinPort *port);


/**
 * @brief Indique si un front déclenchant est survenu
 *
 * Cette fonction est opérationnelle dès qu'un type de front a été défini pour
 * la broche concernée grâce à iDinSetEdge() (qu'un gestionnaire d'interruption
 * ait été défini ou pas). \n
 * La valeur lue est remise à eEdgeNone après chaque appel.
 *
 * @param input le numéro logique de l'entrée concernée
 * @param port le port d'entrée
 * @return le type de front (eEdgeRising, EEdgeFalling), eEdgeNone si aucun
 *          front détecté, -1 si erreur
 */
int iDinEdgeOccured (unsigned input, xDinPort * port);

/**
 * @brief Installation d'un gestionnaire d'interruption utilisateur
 *
 * @param input le numéro logique de l'entrée concernée
 * @param edge front déclenchant de l'entrée cocnernée
 * @param callback nom du gestionnaire (fonction définie par l'utilisateur)
 * @param udata pointeur sur des données qui seront passées lors de l'exécution
 *         du gestionnaire (NULL si inutilisé)
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iDinSetCallback (unsigned input, eDinEdge edge, iDinCallback callback, void *udata, xDinPort * port);

/**
 * @brief Installation d'un gestionnaire de groupe d'entrées
 *
 * @param callback nom du gestionnaire (fonction définie par l'utilisateur)
 * @param udata pointeur sur des données qui seront passées lors de l'exécution
 *         du gestionnaire (NULL si inutilisé)
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iDinSetGrpCallback (eDinEdge edge, iDinGrpCallback callback, void *udata, xDinPort * port);

/**
 * @brief Désinstallation d'un gestionnaire d'interruption utilisateur
 *
 * @param input le numéro logique de l'entrée concernée
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iDinClearCallback (unsigned input, xDinPort * port);

/**
 * @brief Désinstallation d'un gestionnaire de groupe d'entrées
 *
 * @param port le port d'entrée
 * @return 0, -1 si erreur
 */
int iDinClearGrpCallback (xDinPort * port);

/**
 * @brief Indique si un gestionnaire d'interruption utilisateur est installé
 *
 * @param input le numéro logique de l'entrée concernée
 * @param port le port d'entrée
 * @return true si oui, false si non, -1 si erreur
 */
int iDinCallbackInstalled (unsigned input, xDinPort *port);

/**
 * @brief Données d'un gestionnaire d'interruption utilisateur
 *
 * @param input le numéro logique de l'entrée concernée
 * @param port le port d'entrée
 * @return pointeur sur les données, NULL si pas de données ou si erreur
 */
void * pDinCallbackData (unsigned input, xDinPort *port);

/**
 * @}
 */
/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_DINPUT_H_ defined */
