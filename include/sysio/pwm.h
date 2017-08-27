/**
 * @file sysio/pwm.h
 * @brief PWM
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_PWM_H_
#define _SYSIO_PWM_H_
#include <sysio/gpio.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_pwm Broche PWM
 *
 *  Ce module fournit les fonctions permettant de contrôler des broches de
 *  sorties PWM.\n
 *  Ce module utilise le réglage "mark-space ratio", c'est à dire que "value"
 *  correspond au nombre de cycles d'horloge à l'état haut alors que "range"
 *  correspond à la période du signal généré en nombre de cycles d'horloge.
 *  @{
 *  @example pwm/sysio_test_pwm.c
 *  Test sortie PWM
 */

/* structures =============================================================== */
/**
 * @brief Port de sortie PWM
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xPwm xPwm;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'une broche PWM
 *
 * @param iPin le numéro de la broche
 * @return le pointeur sur la broche, NULL en cas d'erreur
 */
xPwm * xPwmOpen (int iPin);

/**
 * @brief Fermeture d'une broche PWM
 *
 * @param pwm le pointeur sur la broche
 * @return 0, -1 si erreur
 */
int iPwmClose (xPwm * pwm);

/**
 * @brief Indique si la broche est ouverte
 *
 * @param pwm le pointeur sur la broche
 * @return true si ouverte, false sinon
 */
bool bPwmIsOpen (xPwm * pwm);

/**
 * @brief Démarrage de la génération du signal PWM
 *
 * Les caractéristiques du signal généré sont modifiés à l'aide des fonctions
 * iPwmSetRange(), iPwmSetValue() et iPwmSetClockDiv()
 *
 * @param pwm le pointeur sur la broche
 * @return 0, -1 si erreur
 */
int iPwmRun (xPwm * pwm);

/**
 * @brief Arrêt de la génération du signal PWM
 *
 * Les réglages de la broche ne sont pas modifiés
 *
 * @param pwm le pointeur sur la broche
 * @return 0, -1 si erreur
 */
int iPwmStop (xPwm * pwm);

/**
 * @brief Indique si la génération du signal est démarrée
 *
 * @param pwm le pointeur sur la broche
 * @return true si démarrée, false sinon
 */
bool bPwmIsRunning (xPwm * pwm);

/**
 * @brief Régle la période du signal PWM
 *
 * @param range période du signal en nombre de cycles d'horloge
 * @param pwm le pointeur sur la broche
 * @return 0, -1 si erreur
 */
int iPwmSetRange (int range, xPwm * pwm);

/**
 * @brief Période du signal PWM
 *
 * @param pwm le pointeur sur la broche
 * @return la période en nombre de cycles d'horloge, -1 si erreur
 */
int iPwmRange (xPwm * pwm);

/**
 * @brief Régle la largeur d'impulsion du signal PWM
 *
 * @param value plargeur d'impulsion en nombre de cycles d'horloge
 * @param pwm le pointeur sur la broche
 * @return 0, -1 si erreur
 */
int iPwmSetValue (int value, xPwm * pwm);

/**
 * @brief Largeur d'implusion du signal PWM
 *
 * @param pwm le pointeur sur la broche
 * @return la largeur d'impulsion en nombre de cycles d'horloge, -1 si erreur
 */
int iPwmValue (xPwm * pwm);

/**
 * @brief Valeur de diviseur correspondant à une fréquence d'horloge
 *
 * Les fréquences d'horloges sont réglées grâce à un facteur de division qui
 * est appliqué à une horloge "mère".\n
 * Du fait que ce diviseur est un nombre entier, les fréquences d'horloge
 * ne peuvent être réglés qu'approximativement.
 *
 * @param dFreq fréquence d'horloge
 * @return la valeur du diviseur
 */
int iPwmDivisor (double dFreq);

/**
 * @brief Modification du diviseur d'horloge
 *
 * Ce réglage ne peut être effectué que si au moins une broche PWM est ouverte.
 * Le réglage est commun à toutes les broches PWM. Le nombre de valeurs
 * disponibles dépend de l'architecture, la valeur est ajustée si elle
 * dépasse les bornes (un avertissement est alors affiché)
 *
 * @param iDiv valeur du diviseur
 * @return la valeur de diviseur effectivement réglée, -1 si erreur
 */
int iPwmSetClockDiv (int iDiv);

/**
 * @brief Diviseur d'horloge actuel
 *
 * Cette lecture ne peut être effectuée que si au moins une broche PWM est ouverte.
 *
 * @return la valeur de diviseur effectivement réglée, -1 si erreur
 */
int iPwmClockDiv (void);

/**
 * @brief Valeur de fréquence d'horloge correspondant à un diviseur
 *
 * Un appel avec la valeur 1 permet de connaitre la fréquence de l'horloge "mère"
 *
 * @param iDiv valeur du diviseur
 * @return la fréquence d'horloge
 */
double dPwmFreq (int iDiv);

/**
 * @brief Modification de la fréquence d'horloge
 *
 * Ce réglage ne peut être effectué que si au moins une broche PWM est ouverte.
 * Le réglage est commun à toutes les broches PWM. Le nombre de valeurs
 * disponibles dépend de l'architecture, la valeur est ajustée si elle
 * dépasse les bornes (un avertissement est alors affiché)
 *
 * @param dFreq fréquence d'horloge
 * @return la valeur de diviseur effectivement réglée, -1 si erreur
 */
int iPwmSetClockFreq (double dFreq);

/**
 * @brief Fréquence d'horloge actuelle
 *
 * Cette lecture ne peut être effectuée que si au moins une broche PWM est ouverte.
 *
 * @return la valeur de la fréquence effectivement réglée
 */
double dPwmClockFreq (void);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_PWM_H_ defined */
