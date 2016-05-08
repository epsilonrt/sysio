/**
 * @file tinfo.h
 * @brief Télé-information client des compteurs utilisés par ERDF
 *
 * Copyright © 2016 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_TINFO_H_
#define _SYSIO_TINFO_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include <errno.h>
#include <stdio.h>
#include <time.h>

/**
 *  @defgroup sysio_tinfo Télé-information client ERDF
 *
 *  Ce module permet d'analyser les informations transmises sur une liaison de
 *  télé-information client des appareils de comptage électroniques utilisés par
 *  ERDF. \n
 *  Il est compatible avec les trames émises par : \n
 *  - le compteur « Bleu » électronique monophasé multitarif (CBEMM : 2 paliers différents),
 *  - le compteur « Bleu » électronique triphasé multitarif (CBETM).
 *  .
 * Il pourra évoluer pour prendre en charge d'autres compteurs (Jaune, ...). \n
 * L'analyse utilise le document ERDF-NOI-CPT_02E, Version 5. \n
 * Dans la mesure où le compteur LINKY dispose d'un mode « Historique », ce
 * module devrait être en mesure de décoder ses trames dans ce mode (9600 bd). \n
 *
 * L'interface matérielle devra effectuer une démodulation ASK 50 kHz et fournir
 * un signal conforme au paragraphe 1.5 document ERDF-NOI-CPT_02E et reliée
 * au port série fourni lors de l'ouverture.
 *  @{
 *  @example tinfo/sysio_demo_tinfo.c
 *  Affiche les trames reçues.
 */

/* constants ================================================================ */
/**
 * @brief Type de trame
 *
 * Actuellement le module ne sait géré que des trames envoyées par un compteur
 * bleu.
 */
typedef enum  {
  eTinfoFrameUnknown  = 0, /**< Inconnue */
  eTinfoFrameBlue     = 0x0001, /**< Bleue */

#ifndef __DOXYGEN__
  /* Réservé pour évolution future */
  eTinfoFrameYellow   = 0x0002, /**< Jaune, non supporté actuellement */
  eTinfoFrameIce      = 0x0004, /**< Interface Clientèle Emeraude, non supporté actuellement */
  eTinfoFrameIce2Q    = eTinfoFrameIce, /**< Interface Clientèle Emeraude 2 quadrants, non supporté actuellement */
  eTinfoFrameIce4Q    = eTinfoFrameIce + 0x0008, /**< Interface Clientèle Emeraude 4 quadrants, non supporté actuellement */
  eTinfoFramePmePmi   = 0x0010, /**< PME-PMI, non supporté actuellement */
  eTinfoFrameSaphir   = 0x0020, /**< SAPHIR, non supporté actuellement */
#endif /* __DOXYGEN__ not defined */
} eTinfoFrame;

/**
 * @brief Option de tarification
 */
typedef enum  {
  eTinfoOpTarifUnknown = 0, /**< Option tarifaire inconnue */
  eTinfoOpTarifBase,        /**< Option tarifaire de base */
  eTinfoOpTarifHc,          /**< Option tarifaire Heures creuses */
  eTinfoOpTarifEjp,         /**< Option tarifaire EJP (Effacecement des Jours de Pointe)  */
  eTinfoOpTarifTempo,       /**< Option tarifaire Tempo */
} eTinfoOpTarif;

/**
 * @brief Drapeaux de la trame
 */
typedef enum  {
  eTinfoFlagShort         = 0x0001, /**< Trame courte (Triphasé, Dépassement puissance) */
  eTinfoFlagPapp          = 0x0002, /**< Trame contenant une puissance apparente */
  eTinfoFlagAdps          = 0x0100, /**< Avertissement de Dépassement De Puissance Souscrite */
  eTinfoFlagNewPetc       = 0x0200, /**< Changement de période de tarification */
  eTinfoFlagTempoNewColor = 0x0400, /**< Changement de couleur de jour Tempo */
  eTinfoFlagNewMotEtat    = 0x0800, /**< Changement de mot d'état */
} eTinfoFlag;

/**
 * @brief Période Tarifaire En Cours
 */
typedef enum  {
  eTinfoPtecUnknown = 0, /**<  */
  eTinfoPtecTh, /**< Toutes les Heures */
  eTinfoPtecHc, /**< Heures Creuses */
  eTinfoPtecHp, /**< Heures Pleines */
  eTinfoPtecHn, /**< Heures Normales */
  eTinfoPtecPm, /**< Heures de Pointe Mobile */
  eTinfoPtecHcJb, /**< Heures Creuses, Jours Bleus */
  eTinfoPtecHcJw, /**< Heures Creuses, Jours Blancs (White) */
  eTinfoPtecHcJr, /**< Heures Creuses, Jours Rouges */
  eTinfoPtecHpJb, /**< Heures Pleines, Jours Bleus */
  eTinfoPtecHpJw, /**< Heures Pleines, Jours Blancs (White) */
  eTinfoPtecHpJr  /**< Heures Pleines, Jours Rouges*/
} eTinfoPtec;

/**
 * @brief Couleur Tempo
 */
typedef enum  {
  eTinfoColorUnknown = 0, /**<  */
  eTinfoColorBlue, /**< Jour Bleu */
  eTinfoColorWhite, /**< Jour Blanc */
  eTinfoColorRed /**< Jour Rouge */
} eTinfoTempoColor;

/**
 * @brief Type de gestionnaire de réception
 */
typedef enum {
  eTinfoCbFrame   = 0, /**< Réception d'une trame */
  eTinfoCbAdps    = 1, /**< Avertissement de Dépassement de Puissance Souscrite */
  eTinfoCbPetc    = 2, /**< Changement de période tarifaire */
  eTinfoCbMotEtat = 3, /**< Changement du mot d'état */
  eTinfoCbTempo   = 4, /**< Changement de la couleur du jour du lendemain (tempo) */
#ifndef __DOXYGEN__
#define TINFO_CB_FIRST eTinfoCbFrame
#define TINFO_CB_LAST  eTinfoCbTempo
#endif /* __DOXYGEN__ not defined */
} eTinfoCbType;

/* structures =============================================================== */

/**
 * @brief Données de la trame spécifiques à l'option de base
 */
struct xTinfoBase {
  unsigned long index; /**< Index en Wh */
};

/**
 * @brief Données de la trame spécifiques à l'option Heures creuses
 */
struct xTinfoHc {
  unsigned long index_hc; /**< Index Heures Creuses en Wh */
  unsigned long index_hp; /**< Index Heures Pleines en Wh */
  char horaire; /**< Plage horaire Heures Creuses: A, C, D, E, Y (les horaires réels dépendent de la zone et sont fournis par ERDF */
};

/**
 * @brief Données de la trame spécifiques à option Tempo
 */
struct xTinfoTempo {
  unsigned long index_hcjb; /**< Index Heures Creuses, Jours Bleus en Wh */
  unsigned long index_hpjb; /**< Index Heures Pleines, Jours Bleus en Wh */
  unsigned long index_hcjw; /**< Index Heures Creuses, Jours Blancs en Wh */
  unsigned long index_hpjw; /**< Index Heures Pleines, Jours Blancs en Wh */
  unsigned long index_hcjr; /**< Index Heures Creuses, Jours Rouges en Wh */
  unsigned long index_hpjr; /**< Index Heures Pleines, Jours Rouges en Wh */
  eTinfoTempoColor demain;  /**< Couleur de demain */
  char pgm1; /**< Programmes de commande des circuits de sortie à contacts auxiliaires du compteur: A, B, C */
  char pgm2; /**< Programmes de commande des circuits de sortie à contacts auxiliaires du compteur: '0' à '7' */
};

/**
 * @brief Données de la trame spécifiques à option EJP
 */
struct xTinfoEjp {
  unsigned long index_hn; /**< Index Heures Normales en Wh */
  unsigned long index_hpm; /**< Index Heures de Pointe Mobile en Wh */
  unsigned pejp; /**< Préavis Début EJP (30 min) */
};

/**
 * @brief Données de la trame spécifiques à une option de tarification
 * 
 * Permet un accès aux informations optionnelles de façon uniforme
 */
union uTinfoTarif {
  struct xTinfoBase base; /**< Option de base */
  struct xTinfoHc hc; /**< Option Heures Creuses */
  struct xTinfoTempo tempo; /**< Option Tempo*/
  struct xTinfoEjp ejp; /**< Option EJP */
};

/**
 * @brief Trame d'un compteur bleu
 */
struct xTinfoFrameBlue  {
  eTinfoFrame frame;  /**< Type de trame (eTinfoFrameBlue) */
  time_t time; /**< date/heure de réception du STX */
  eTinfoFlag flag;    /**< Drapeaux */
  unsigned long long adco; /**< Adresse du compteur */
  eTinfoOpTarif optarif; /**< Option tarifaire choisie */
  eTinfoPtec ptec; /**< Période Tarifaire en cours */
  unsigned isousc; /**< Intensité souscrite en A */
  unsigned nph; /**< Nombre de phases (1 pour monophasé, 3 pour le triphasé) */
  unsigned iinst[3]; /**< Intensités Instantanées en A (index 0 pour le monophasé) */
  unsigned adps; /**<  Avertissement de Dépassement De Puissance Souscrite en A (monophasé) */
  unsigned imax[3]; /**< Intensités maximales appelées (index 0 pour le monophasé) */
  unsigned pmax; /**< Puissance maximale triphasée atteinte en W */
  unsigned papp; /**< Puissance apparente en VA */
  unsigned motdetat; /**< Mot d'état du compteur (hexadécimal) */
  unsigned ppot; /**< Présence des potentiels triphasés (hexadécimal) */
  union uTinfoTarif tarif; /**< Indexes et données spécifiques à une option tarifaire*/
};

/**
 * @brief Trame courte d'un compteur bleu triphasé
 * Emise lors d'un dépassement d'intensité souscrite sur une ou plusieurs phases
 */
struct xTinfoFrameBlueShort  {
  eTinfoFrame frame; /**< Type de trame (eTinfoFrameBlue) */
  time_t time; /**< date/heure de réception du STX */
  eTinfoFlag flag; /**< Drapeaux */
  unsigned long adco; /**< Adresse du compteur */
  unsigned nph; /**< Nombre de phases: toujours 3 puisque triphasé */
  unsigned iinst[3]; /**< Intensités Instantanées en A */
  unsigned adir[3]; /**< Avertissement de Dépassement d'intensité de réglage par phase */
};

/**
 * @brief Trame brute
 * Permet de tester le type de trame
 */
struct xTinfoFrameRaw  {
  eTinfoFrame frame; /**< Type de trame (eTinfoFrameBlue) */ 
  time_t time; /**< date/heure de réception du STX */
  uint8_t raw[1]; /**< Accès aux données par octets (usage réservé) */
};

/**
 * @brief Trame de télé-information
 *
 * Cette union permet de stocker les différents type de trame, le champ frame
 * est commun à toutes les trames et peut être testé grace à la structure raw.
 */
typedef union xTinfoFrame {

  struct xTinfoFrameBlue blue; /**< Trame bleue */
  struct xTinfoFrameBlueShort blue_short; /**< Trame bleue courte */
  struct xTinfoFrameRaw raw; /**< Trame brute */
} xTinfoFrame;

/**
 * @brief Contexte d'une liaison de télé-information
 *
 * Cette structure est opaque pour l'utilisateur
 */
typedef struct xTinfo xTinfo;


/* types ==================================================================== */
/**
 * @brief Prototype d'un gestionnaire de réception
 *
 * @param tinfo pointeur sur le contexte du module tinfo initialisé
 * @param frame pointeur sur la trame reçue (ne doit pas être libérée)
 * @return Si il réussit un gestionnaire doit renvoyer 0, sinon c'est une erreur
 */
typedef int (*iTinfoCb) (struct xTinfo *tinfo, union xTinfoFrame *frame);

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'une liaison de télé-information
 *
 * Cette fonction doit être appellée avant toute utilisation du contexte tinfo.
 *
 * @param port port série à utiliser (/dev/ttyUSB0...)
 * @param baudrate vitesse en bauds, devrait être 1200 sauf pour Linky (9600)
 * @return le contexte, NULL si erreur
 */
xTinfo * xTinfoOpen (const char * port, unsigned long baudrate);

/**
 * @brief Scrute la liaison de télé-information en attente de réception d'octet
 *
 * Cette fonction doit être appellée dans la boucle principale aussi souvent que
 * possible. \n
 * @warning Il faut appeller cette fonction sinon aucune réception n'est possible.
 * @return 0, -1 si erreur
 */
int iTinfoPoll (xTinfo *tinfo);

/**
 * @brief Fermeture d'une liaison de télé-information
 * @param tinfo pointeur sur l'objet Tinfo
 * @return 0, -1 si erreur
 */
int iTinfoClose (xTinfo *tinfo);

/**
 * @brief Modifie un gestionnaire de réception
 *
 * Permet d'installer qui sera appelée lors de l'événement corresondant à type.
 * @param tinfo pointeur sur l'objet Tinfo
 * @param type type de l'événement géré
 * @param cb fonction de l'utilisateur gérant l'événement
 */
void vTinfoSetCB (xTinfo *tinfo, eTinfoCbType type, iTinfoCb cb);

/**
 * @brief Renvoie le pointeur sur les données de l'utilisateur
 *
 * Le contexte xTinfo dispose d'un pointeur void * permettant à l'utilisateur
 * d'y attacher des donnnées spécifiques. Comme les gestionnaires d'événements
 * reçoivent le contexte xTinfo, il leur est possible de le récupérer.
 *
 * @param tinfo pointeur sur l'objet Tinfo
 * @return pointeur sur les données de l'utilisateur
 */
void * pvTinfoGetUserContext (const xTinfo *tinfo);

/**
 * @brief Modifie le contexte utilisateur
 *
 * Le contexte xTinfo dispose d'un pointeur void * permettant à l'utilisateur
 * d'y attacher des donnnées spécifiques. Comme les gestionnaires d'événements
 * reçoivent le contexte xTinfo, il leur est possible de le récupérer.
 *
 * @param tinfo pointeur sur l'objet Tinfo
 * @param pointeur sur les données de l'utilisateur
 */
void vTinfoSetUserContext (xTinfo *tinfo, void * udata);

/**
 * @brief Chaîne de caractère correspondant à une période de tarification
 * 
 * @param ptec période tarifaire en cours
 * @return chaîne de caractère string ci-dessous: \n
  | string | ptec           | période tarifaire en cours            | 
  | :----- | :------------- | :------------------------------------ |
  | th     | eTinfoPtecTh   |  Toutes les Heures                    | 
  | hc     | eTinfoPtecHc   |  Heures Creuses                       | 
  | hp     | eTinfoPtecHp   |  Heures Pleines                       | 
  | hn     | eTinfoPtecHn   |  Heures Normales                      | 
  | hpm    | eTinfoPtecPm   |  Heures de Pointe Mobile              | 
  | hcjb   | eTinfoPtecHcJb |  Heures Creuses, Jours Bleus          | 
  | hcjw   | eTinfoPtecHcJw |  Heures Creuses, Jours Blancs (White) | 
  | hcjr   | eTinfoPtecHcJr |  Heures Creuses, Jours Rouges         | 
  | hpjb   | eTinfoPtecHpJb |  Heures Pleines, Jours Bleus          | 
  | hpjw   | eTinfoPtecHpJw |  Heures Pleines, Jours Blancs (White) | 
  | hpjr   | eTinfoPtecHpJr |  Heures Pleines, Jours Rouges         | 
  | ----   | autre valeur   |  Valeur inconnue (4 tirets -)         | 
 */
const char * sTinfoPtecToStr (eTinfoPtec ptec);

/**
 * @brief Chaîne de caractère correspondant à une couleur Tempo
 * 
 * @param color couleur tempo du jour
 * @return chaîne de caractère string ci-dessous: \n
  | string | color            | couleur tempo du jour         | 
  | :----- | :--------------- | :---------------------------- |
  | bleu   | eTinfoColorBlue  |  Bleu                         | 
  | blanc  | eTinfoColorWhite |  Blanc                        | 
  | rouge  | eTinfoColorRed   |  Rouge                        | 
  | ----   | autre valeur     |  Valeur inconnue (4 tirets -) | 
 */
const char * sTinfoTempoColorToStr (eTinfoTempoColor color);

/**
 * @brief Chaîne de caractère correspondant à une option tarifaire
 * 
 * @param tarif option tarifaire
 * @return chaîne de caractère string ci-dessous: \n
  | string | tarif              | Option tarifaire                        | 
  | :----- | :----------------- | :-------------------------------------- |
  | base   | eTinfoOpTarifBase  |  de Base                                | 
  | hc     | eTinfoOpTarifHc    |  Heures creuses                         | 
  | ejp    | eTinfoOpTarifEjp   |  EJP (Effacecement des Jours de Pointe) | 
  | tempo  | eTinfoOpTarifTempo |  Tempo                                  | 
  | ----   | autre valeur       |  Valeur inconnue (4 tirets -)           | 
 */
const char * sTinfoOpTarifToStr (eTinfoOpTarif tarif);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*  _SYSIO_TINFO_H_ defined */
