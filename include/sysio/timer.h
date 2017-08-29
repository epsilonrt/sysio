/**
 * @file delay.h
 * @brief Temporisation
 *
 * Copyright © 2014 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_TIMER_H_
#define _SYSIO_TIMER_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_timer Timer de temporisation
 *
 *  Ce module fournit les fonctions de temporisation.
 *  @{
 */

/* structures =============================================================== */

struct xTimer;

/**
 * @brief Objet timer
 * Structure opaque pour l'utilisateur
 */
typedef struct xTimer xTimer;

/**
 * Gestionnaire utilisateur d'événement timer
 *
 * Déclenché sur modification de niveau d'une ou plusieurs broches.
 * Seuls le ou les fronts valides définis lors de l'appel à iDinSetCallback()
 * déclenchent un traitement.
 *
 * @param edge  front déclenchant
 * @param udata pointeur sur les données utilisateur défini lors de l'appel à iDinSetCallback()
 * @return doit retourner 0 si l'événement a été traité, -1 sinon.
 */

typedef int (*iTimerCallback) (xTimer * tm);

/* internal public functions ================================================ */

/**
 * @brief Création d'un timer
 * @return pointeur sur le nouveau timer, NULL si erreur
 */
xTimer * xTimerNew (void);

/**
 * @brief Suppression d'un timer
 * @param tm pointeur sur le timer
 * @return 0, -1 si erreur (errno contient l'erreur)
 */
int iTimerDelete (xTimer * tm);

/**
 * @brief Démarre un timer
 * @param tm pointeur sur le timer
 * @return 0, -1 si erreur (errno contient l'erreur)
 */
int iTimerStart (xTimer * tm);

/**
 * @brief Arrête un timer
 * @param tm pointeur sur le timer
 * @return 0, -1 si erreur (errno contient l'erreur)
 */
int iTimerStop (xTimer * tm);

/**
 * @brief Modifie l'intervalle d'un timer
 * @param tm pointeur sur le timer
 * @param msec intervalle en ms
 */
void vTimerSetInterval (xTimer * tm, long msec);

/**
 * @brief Lecture de l'intervalle d'un timer
 * @param tm pointeur sur le timer
 * @return intervalle en ms
 */
long lTimerInterval (const xTimer * tm);

/**
 * @brief Teste si single shot actif
 * @param tm pointeur sur le timer
 * @return true si actif (par défaut)
 */
bool bTimerIsSingleShot (const xTimer * tm);

/**
 * @brief Valide/Invalide le single shot
 * @param tm pointeur sur le timer
 * @param singleShot true pour valider
 */
void vTimerSetSingleShot (xTimer * tm, bool singleShot);

/**
 * @brief Teste si un timer est en marche
 * @param tm pointeur sur le timer
 * @return true si en marche
 */
bool bTimerIsActive (const xTimer * tm);

/**
 * @brief Teste la fin d'un timer single shot
 * @param tm pointeur sur le timer
 * @return true si le décompte est terminé
 */
bool bTimerIsTimeout (xTimer * tm);

/**
 * @brief Attente d'un timer
 * @param tm pointeur sur le timer
 * @return le nombre de déclenchement pendant l'attente, -1 si erreur
 */
int iTimerWait(xTimer * tm);

/**
 * @brief Renvoie la valeur restante de la minuterie en millisecondes
 *
 * Renvoie la valeur restante de la minuterie en millisecondes restant jusqu'à
 * la fin du délai. Si la minuterie est inactive, la valeur renvoyée sera -1.
 * Si la minuterie est en retard, la valeur retournée sera 0.
 * 
 * @param tm pointeur sur le timer
 * @return valeur restante de la minuterie en millisecondes, -1 si erreur
 */
long lTimerRemainingTime (const xTimer * tm);

/**
 * @brief Installation d'un gestionnaire de timeout
 *
 * @param tm pointeur sur le timer
 * @param callback nom du gestionnaire (fonction définie par l'utilisateur)
 * @param udata pointeur sur des données qui seront passées lors de l'exécution
 *         du gestionnaire (NULL si inutilisé)
 * @return 0, -1 si erreur
 */
int iTimerSetCallback (xTimer * tm, iTimerCallback callback, void * udata);

/**
 * @brief Retire un gestionnaire de timeout
 * @param tm pointeur sur le timer
 * @return 0, -1 si erreur
 */
int iTimerUnsetCallback (xTimer * tm);

/**
 * @brief Données utilisateur d'un timer
 * @param tm pointeur sur le timer
 * @return pointeur sur les données utilisateur
 */
void * pvTimerUserData (xTimer * tm);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_TIMER_H_ defined */
