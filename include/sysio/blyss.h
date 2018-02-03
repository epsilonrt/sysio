/**
 * @file
 * @brief Modules Blyss™
 *
 * Copyright © 2016 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_BLYSS_H_
#define _SYSIO_BLYSS_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include <stdio.h>

/**
 * @defgroup sysio_blyss Modules Blyss™
 * Ce module permet de gérer la transmission vers des modules domotiques
 * Blyss™ commerciailisés par Castorama (protocole LiveEz™ 433.92 MHz). \n
 * La plateforme doit disposer d'un GPIO. \n
 * Ce code est basé sur le travail de « reverse engineering » de 7 membres du
 * forum arduino.cc: SkyWodd, Artouste, Al1fch, Barbudor, Bunyman, Trigger et
 * Bidle (https://skyduino.wordpress.com/2012/07/19/hack-partie-2-reverse-engineering-des-interrupteurs-domotique-blyss) \n
 * @todo Seule la partie emission est implémentée pour l'instant.
 * @{
 * @example gpio/blyss-tx/sysio_demo_blyss_tx.c
 * Demo émission Modules Blyss™
 * 
 * Ce programme permet d'envoyer des commandes sur une liaison LiveEz™ 433.92 MHz
 * à l'aide d'un module d'émission connecté à une broche de GPIO. Par exemple,
 * on pourra utiliser le module SeeedStudio du kit :
 * https://www.seeedstudio.com/433Mhz-RF-link-kit-p-127.html#
 * Le module émetteur est alimenté entre GND et +5V, l'entrée DATA sera reliée
 * à une broche de GPIO (3.3 ou 5V). \n
 * Le programme émule par défaut une télécommande 8 canaux Ref. 582883
 * Le synopsis est le suivant :
 * 
 * @code
 * sysio_demo_blyss_tx channel {on,off,loop} [pin]
 * @endcode
 *
 * - `channel` numéro du canal à commander entre 1 et 8, 0 pour tous les canaux
 * - `{on,off,loop}` commande à effectuer : `on` pour marche, `off` pour arrêt et
 * `loop` pour entrer dans une boucle qui bascule l'état du canal toutes les 5
 * secondes (CTRL+C por arrêter)
 * - `[pin]` numéro de la broche à utiliser, optionnel, par défaut le programme
 * utilise la broche 0 (numérotation logique qui dépend de la carte utilisé, voir
 * les modules Raspberry Pi ou NanoPi pour la numérotation.
 * .
 * Voilà ce que ça donne : \n
 * @code
    sysio_demo_blyss_tx 4 on
    Press Ctrl+C to abort ...
    RF frame          : FE508241098E9
    RF footprint      : FE - Ok
    RF global channel : 5
    RF adress         : 0824
    RF channel        : 4
    Light status      : On
    Rolling code      : 98 - Ok
    Token             : E9

    everything was closed.
    Have a nice day !
 * @endcode
 * Si le programme n'est pas installé à l'aide d'un `sudo make install`, la
 * commande est `sudo ./sysio_demo_blyss_tx 4 on`, en effet l'accès au GPIO
 * est réservé à root.
 */
/* macros =================================================================== */
/**
 * @brief Affiche le contenu de la trame pointée par f sur stdout
 * 
 * @param f pointeur sur une trame de type \ref xBlyssFrame
 */
#define vBlyssFramePrint(f) vBlyssFramePrintToFile (f, stdout)

/* constants ================================================================ */

/* structures =============================================================== */
/**
 * @class xBlyssFrame
 * @brief Classe permettant de manipuler une liaison LiveEz
 * Classe opaque pour l'utilisateur
 */
struct xBlyss;

/**
 * @class xBlyssFrame
 * @brief Trame LiveEz
 * 
 * La trame RF est précédée d'un état haut qui dure 2.4 ms et suivie d'un état
 * bas de 24 ms, elle contient les informations suivant:
 * - drapeau de début de trame 0xFE (8 bits),
 * - canal global (4 bits) de la télécommande,
 * - adresse (16 bits) de la télécommande,
 * - sous-canal (4 bits) de l'élément commandé,
 * - état de l'élément commandé (état logique) (4 bits),
 * - rolling code, MSBFIRST (8 bits),
 * - timestamp incrémentiel (0 ~ 255), MSBFIRST (8 bits),
 * - .
 * 
 * Une trame contient donc 13 quartes soit 52 bits stockés dans 7 octets.
 */
struct xBlyssFrame {
  
  uint8_t raw[7];
};


/* types ==================================================================== */
/**
 * @brief xBlyss
 */
typedef struct xBlyss xBlyss;

/**
 * @brief xBlyssFrame
 */
typedef struct xBlyssFrame xBlyssFrame;

/* internal public functions ================================================ */
/**
 * @brief Création d'une liaison LiveEz
 * 
 * @param iOutPin Numéro de la broche reliée à l'émetteur, -1 si pas d'émetteur
 * @param iInPin Numéro de la broche reliée au récepteur, -1 si pas de récepteur
 * @return pointeur sur la liaison initialisée, NULL si erreur
 */
xBlyss * xBlyssNew (int iOutPin, int iInPin);

/**
 * @brief Fermeture et destruction d'une liaison LiveEz
 * 
 * @param b pointeur sur la liaison
 * @return 0, -1 si erreur
 */
int iBlyssDelete (xBlyss * b);

/**
 * @brief Envoi d'une trame
 * 
 * @param b pointeur sur la liaison
 * @param frame pointeur sur la trame
 * @param repeat nombre de répétition d'nevoi
 * @return 0, -1 si erreur
 */
int iBlyssSend (xBlyss * b, xBlyssFrame * frame, uint8_t repeat);

/**
 * @brief Création et initialisation d'une trame LiveEz
 * 
 * La trame devra être libérée avec free()
 * @param tx_id pointeur sur un tableau de 3 octets contenant dans l'ordre
 * le canal global, le poids fort de l'adresse et le poids faible de l'adresse
 * identifiant l'emetteur.
 * @return pointeur sur la trame initialisée, NULL si ereeur
 */
xBlyssFrame * xBlyssFrameNew (const uint8_t * tx_id);

/**
 * @brief Imprime le contenu d'une trame dans un fichier
 * 
 * Le contenu imprimé pour chaque trame est formaté comme ci-dessous:
 * @code
 * RF frame          : FE5082410987D
 * RF footprint      : FE - Ok
 * RF global channel : 05
 * RF adress         : 0824
 * RF channel        : Ch4
 * Light status      : On
 * Rolling code      : 98 - Ok
 * Token             : 7D
 * @endcode 
 * @param frame pointeur sur la trame
 * @param out pointeur sur le fichier ouvert en écriture
 */
void vBlyssFramePrintToFile (const xBlyssFrame * frame, FILE * out);

/**
 * @brief Modification de l'identifiant de l'émetteur d'une trame
 * 
 * @param frame pointeur sur la trame
 * @param tx_id pointeur sur un tableau de 3 octets contenant dans l'ordre
 * le canal global (4 bits de poids faible), le poids fort de l'adresse et 
 * le poids faible de l'adresse identifiant l'emetteur.
 */
void vBlyssFrameSetTxId (xBlyssFrame * frame, const uint8_t * tx_id);

/**
 * @brief Modification du canal destinataire d'une trame
 * 
 * @param frame pointeur sur la trame
 * @param channel canal destinataire codé sur les 4 bits de poids faible 
 * uniquement, valeur comprise entre 0 et 8, 0 correspondant à tous les canaux
 * (broadcast), 1 au canal 1, 2 au canal 2 ...
 */
void vBlyssFrameSetChannel (xBlyssFrame * frame, uint8_t channel);

/**
 * @brief Modification de l'état du destinataire
 * 
 * @param frame pointeur sur la trame
 * @param state état du destinataire ON = true
 */
void vBlyssFrameSetState (xBlyssFrame * frame, bool state);

/**
 * @brief Canal destinataire de la trame
 * 
 * @param frame pointeur sur la trame
 * @return canal destinataire codé sur les 4 bits de poids faible 
 * uniquement, valeur comprise entre 0 et 8, 0 correspondant à tous les canaux
 * (broadcast), 1 au canal 1, 2 au canal 2 ...
 */
uint8_t ucBlyssFrameChannel (const xBlyssFrame * frame);

/**
 * @brief Etat du destinataire de la trame
 * 
 * @param frame pointeur sur la trame
 * @return état du destinataire ON = true
 */
bool bBlyssFrameState (const xBlyssFrame * frame);

/**
 * @brief Modification du canal global d'une trame
 * 
 * @param frame pointeur sur la trame
 * @param global_channel canal global (4 bits de poids faible)
 */
void vBlyssFrameSetGlobalChannel (xBlyssFrame * frame, uint8_t global_channel);

/**
 * @brief Modification de l'adresse source d'une trame
 * 
 * @param frame pointeur sur la trame
 * @param address adresse de l'émetteur de la trame sur 16 bits
 */
void vBlyssFrameSetAddress (xBlyssFrame * frame, uint16_t address);

/**
 * @brief Canal global d'une trame
 * 
 * @param frame pointeur sur la trame
 * @return canal global (4 bits de poids faible)
 */
uint8_t ucBlyssFrameGlobalChannel (const xBlyssFrame * frame);

/**
 * @brief Adresse source d'une trame
 * 
 * @param frame pointeur sur la trame
 * @return adresse de l'émetteur de la trame sur 16 bits
 */
uint16_t usBlyssFrameAddress (const xBlyssFrame * frame);

/**
 * @brief Test si un buffer contient une trame LiveEz valide
 * 
 * @param buffer pointeur sur une zone contenant les octets
 * @return true si valide
 */
bool bBlyssFrameIsValid (const void * buffer);

/**
 * @brief Test si un canal est valide
 * 
 * @param channel canal à tester
 * @return true si valide
 */
bool bBlyssChannelIsValid (uint8_t channel);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_BLYSS_H_ defined */
