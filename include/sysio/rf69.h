/**
 * @file
 * @brief Module de transmission UHF RFM69
 * 
 * http://www.hoperf.com/rf_transceiver/modules/RFM69W.html
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _RF69_H_
#define _RF69_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include <limits.h>

/**
 * @defgroup sysio_rf69 Module UHF RFM69
 * Ce module permet de gérer la transmission avec des modules sans fil
 * RFM69 de HopeRF (http://www.hoperf.com/rf_transceiver/modules/RFM69W.html). \n
 * La plateforme doit disposer d'un bus SPI et de la gestion des interruptions. \n
 * @{
 * @example spi/rf69-ping/sysio_demo_rf69_ping.c
 * Demo émission-réception RFM69 (module rf69)
 * Ce programme : \n
 * - Transmet des messages 'Hello' à un noeud du réseau (gateway) et attend 
 *   un accusé réception (Ack) et affiche le résultat sur la liaison série.
 * - Affiche les messages reçus (autre que Ack) sur la liaison série.
 * .
 * Il est possible de modifier la puissance d'emission, l'adresse du noeud
 * destinataire, l'activation du mode promiscuité. \n
 * Voilà un exemple d'affichage :
 * @code
    ** RFM69 Ping Demo **
    ----------- Menu -----------
    [space]: start/stop Tx
          p: toggle promiscous
          t: change target node
          w: change power level

    Frf: 868000 kHz - Power Level: 0 dBm - Promiscuous: 1
    Own address: [1]
    Transmitting data on network 1 to node [10]...

    T[1]>[10] 'Hello 0' > Ack [RSSI = -28]
    T[1]>[10] 'Hello 1' > Ack [RSSI = -28]
    ....
    tTarget ID (255 for broadcast)? 255
    Frf: 868000 kHz - Power Level: 0 dBm - Promiscuous: 1
    Own address: [1]
    Transmitting data on network 1 to node [255]...

    T[1]>[255] 'Hello 16' >
 * @endcode
 * @example spi/rf69-gateway/sysio_demo_rf69_gateway.c
 * Demo réception RFM69 (module rf69)
 * Ce programme attend la réception de messages, les affichent sur la liaison
 * série et retourne un accusé réception si demandé. \n
 * Il est possible de modifier la puissance d'emission et  l'activation du mode 
 * promiscuité. \n
 * Voilà un exemple d'affichage :
 * @code
    ** Demo Gateway RFM69 **
    ----------- Menu -----------
    [space]: start/stop Rx
          p: toggle promiscous
          w: change power level

    Frf: 868000 kHz - Power Level: 0 dBm - Promiscuous: 1
    Own address: [10]
    Receiving data on network 1...

    R[10]<[1] < 'Hello 0' > Ack [RSSI = -28]
    R[10]<[1] < 'Hello 1' > Ack [RSSI = -28]
    ....
    wPower level [-18,13]? 3
    Frf: 868000 kHz - Power Level: 3 dBm - Promiscuous: 1
    Own address: [10]
    Receiving data on network 1...
    R[10]<[1] < 'Hello 10' > Ack [RSSI = -28]
 * @endcode
 */
/* constants ================================================================ */
/**
 * @brief Adresse d'appel général (broadcast)
 */
#define RF69_BROADCAST_ADDR 255

/**
 * @enum eRf69Band
 * @brief Bande de fréquence
 */
typedef enum {
  eRf69Band315Mhz = 31,
  eRf69Band433Mhz = 43,
  eRf69Band868Mhz = 86,
  eRf69Band915Mhz = 91,
  eRf69BandUnknown = -1
} eRf69Band;


/* structures =============================================================== */
struct xRf69;

/**
 * @class xRf69
 * @brief Classe permettant de manipuler une liaison RFM69
 *
 * Classe opaque pour l'utilisateur
 */
typedef struct xRf69 xRf69;

/* internal public functions ================================================ */

/**
 * @brief Création d'un objet Rfm69
 *
 * @param iBus
 * @param iDevice
 * @param iIrqPin
 * @return pointeur sur l'objet, NULL si erreur
 */
xRf69 * xRf69New (int iBus, int iDevice, int iIrqPin);

/**
 * @brief Destruction d'une liaison LiveEz
 * @param rf pointeur sur l'objet xRf69
 */
void vRf69Delete (xRf69 * rf);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @param eBand
 * @param ucId
 * @param ucNetId
 * @return 0, valeur négative si erreur
 */
int iRf69Open (xRf69 * rf, eRf69Band eBand, uint8_t ucId, uint8_t ucNetId);

/**
 * @brief Close the input-output layer.
 * @param rf pointeur sur l'objet xRf69
 * @return 0, valeur négative si erreur
 */
int iRf69Close (xRf69 * rf);

/**
 * @brief Vérifie si un paquet a été reçu
 *
 * Si non, active la réception, si oui passe en mode StandBy.
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true, false, valeur négative si erreur
 */
int iRf69ReceiveDone (xRf69 * rf);

/**
 * @brief Indique si le dernier paquet reçu nécessite un ACK (paquet non-broadcast)
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true, false, valeur négative si erreur
 */
int iRf69AckRequested (xRf69 * rf);

/**
 * @brief Envoi d'un paquet ACK suite à demande AckRequested
 *
 * Devrait être appelé immédiatement après réception d'un paquet avec demande d'ACK.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param tx_buffer données à transmettre dans la réponse (NULL si inutile)
 * @param tx_len nombre d'octets de données (0 si inutile)
 * @return true Ok, false timeout, valeur négative si erreur
 */
int iRf69SendAck (xRf69 * rf, const void * tx_buffer, uint8_t tx_len);

/**
 * @brief Adresse de l'émetteur du dernier message reçu
 * @param rf pointeur sur l'objet xRf69
 * @return  Adresse de l'émetteur comme un unsigned int, -1 si erreur
 */
int iRf69SenderId(xRf69 * rf);

/**
 * @brief Adresse du destinataire du dernier message reçu
 * @param rf pointeur sur l'objet xRf69
 * @return  Adresse du destinataire comme un unsigned int, -1 si erreur
 */
int iRf69TargetId(xRf69 * rf);

/**
 * @brief Nombre d'octets du dernier message reçu
 *
 * Les données sont disponibles jusqu'au prochain appel à \c iRf69ReceiveDone()
 * ou jusqu'à l'envoi d'un paquet.
 *
 * @param rf pointeur sur l'objet xRf69
 * @return Nombre d'octets de données utiles (sans en-tête), -1 si erreur
 */
int iRf69DataLength (xRf69 * rf);

/**
 * @brief Données utiles (sans en-tête) du dernier message reçu
 *
 * Les données sont terminées par un octet nul.
 *
 * @param rf pointeur sur l'objet xRf69
 * @return pointeur sur les données
 */
const char * sRf69Data (xRf69 * rf);

/**
 * @brief Vérifie si on peut émettre
 * Si oui, passe en mode StandBy
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true prêt à émettre, false timeout, valeur négative si erreur
 */
int iRf69CanSend (xRf69 * rf);

/**
 * @brief Transmission d'un paquet
 *
 * @param rf pointeur sur l'objet xRf69
 * @param toAddress destinataire ou RF69_BROADCAST_ADDR
 * @param tx_buffer données à transmettre
 * @param tx_len nombre d'octets des données
 * @param bRequestACK demande d'ACK
 * @return true trame envoyée, false timeout, valeur négative si erreur
 */
int iRf69Send (xRf69 * rf, uint8_t toAddress, const void* tx_buffer, uint8_t tx_len, bool bRequestACK);

/**
 * @brief Envoi d'un paquet avec répétition
 *
 * to increase the chance of getting a packet across, call this function instead of send
 * and it handles all the ACK requesting/retrying for you :)
 * The only twist is that you have to manually listen to ACK requests on the other side and send back the ACKs
 * The reason for the semi-automaton is that the lib is interrupt driven and
 * requires user action to read the received data and decide what to do with it
 * replies usually take only 5..8ms at 50kbps@915MHz
 *
 * @param rf pointeur sur l'objet xRf69
 * @param toAddress
 * @param tx_buffer
 * @param tx_len
 * @param retries
 * @param retryWaitTime
 * @return true trame envoyée, false timeout, valeur négative si erreur
 */
int iRf69SendWithRetry (xRf69 * rf, uint8_t toAddress,
                        const void * tx_buffer, uint8_t tx_len,
                        uint8_t retries, int retryWaitTime);

/**
 * @brief Vérifie la réception d'un ACK
 *
 * Vérifie la réception d'un ACK en réponse à l'envoi d'un paquet avec
 * demande d'ACK. \n
 * Devrait être scruté immédiatement après l'envoi d'un paquet avec
 * demande d'ACK.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param fromNodeId Adresse du noeud dont on attend la réponse
 * @return true si Ack reçu, valeur négative si erreur
 */
int iRf69AckReceived (xRf69 * rf, uint8_t fromNodeId);

/**
 * @brief Attente de la réception d'un ACK
 *
 * Vérifie la réception d'un ACK en réponse à l'envoi d'un paquet avec
 * demande d'ACK. \n
 * Devrait être utilisé immédiatement après l'envoi d'un paquet avec
 * demande d'ACK.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param fromNodeId Adresse du noeud dont on attend la réponse
 * @return true si Ack reçu, false, valeur négative si erreur
 */
int iRf69WaitAckReceived (xRf69 * rf, uint8_t fromNodeId, int timeout);

/**
 * @brief Modification de notre identifiant
 *
 * @param rf pointeur sur l'objet xRf69
 * @param ucNodeId nouvel identifiant
 * @return 0, valeur négative si erreur
 */
int iRf69SetNodeId (xRf69 * rf, uint8_t ucNodeId);

/**
 * @brief Lecture de notre identifiant
 *
 * @param rf pointeur sur l'objet xRf69
 * @return identifiant, valeur négative si erreur
 */
int iRf69NodeId (xRf69 * rf);

/**
 * @brief Modification de notre réseau
 *
 * @param rf pointeur sur l'objet xRf69
 * @param ucNetId identifiant du nouveau réseau
 * @return 0, valeur négative si erreur
 */
int iRf69SetNetworkId (xRf69 * rf, uint8_t ucNetId);

/**
 * @brief Lecture de notre réseau
 * @param rf pointeur sur l'objet xRf69
 * @return identifiant de notre réseau, valeur négative si erreur
 */
int iRf69NetworkId (xRf69 * rf);

/**
 * @brief Lecture de la fréquence d'émission/réception
 *
 * @param rf pointeur sur l'objet xRf69
 * @return La fréquence en Hz, valeur négative si erreur
 */
long lRf69Frequency (xRf69 * rf);

/**
 * @brief Modification de la fréquence d'émission/réception
 *
 * @param rf pointeur sur l'objet xRf69
 * @param lFreq fréquence en Hz
 * @return 0, valeur négative si erreur
 */
int iRf69SetFrequency (xRf69 * rf, long lFreq);

/**
 * @brief Modification du mode haute puissance d'émission
 *
 * Disponible uniquement sur RFM69HW. Dans ce mode, la puissance d'émission
 * varie entre 5 et 20 dBm, au lieu de -18 à 13 dBm.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param bOn nouvel état
 * @return 0, valeur négative si erreur
 */
int iRf69SetHighPower (xRf69 * rf, bool bOn);

/**
 * @brief Indique si le mode  haute puissance d'émission est activé
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true, false, valeur négative si erreur
 */
bool bRf69isHighPower (xRf69 * rf);

/**
 * @brief Modification de la puissance d'émission
 *
 * La puissance fournie est une puissance relative à la puissance de référence
 * du module utilisé. Pour les modules ne disposant pas du mode haute puissance
 * la puissance réelle varie entre -18 et 13 dBm pour une puissance relative
 * entre 0 et 31 dBm (la puissance de référence est donc de -18 dBm). \n
 * Pour un module haute puissance (HW) avec \c iRf69SetHighPower() activé
 * la puissance réelle varie entre 5 et 20 dBm pour une puissance relative
 * entre 0 et 31 dBm (la puissance de référence est donc de 5 dBm avec un pas
 * de 0.5 dBm). \n
 * @param rf pointeur sur l'objet xRf69
 * @param level nouvelle puissance relative (0 à 31 dBm)
 * @return 0, valeur négative si erreur
 */
int iRf69SetPowerLevel (xRf69 * rf, int level);

/**
 * @brief Lecture de la puissance d'émission
 *
 * @param rf pointeur sur l'objet xRf69
 * @return puissance relative (0 à 31 dBm)
 */
int iRf69PowerLevel (xRf69 * rf);

/**
 * @brief Lecture de la température du capteur 8 bits embarqué
 *
 * @param rf pointeur sur l'objet xRf69
 * @param iCalFactor correction de zéro (suite à étalonnage)
 * @return La température en °C, INT_MAX si erreur
 */
int iRf69Temperature (xRf69 * rf, int iCalFactor);

/**
 * @brief Lecture du niveau de réception
 *
 * Cette fonction retourne le niveau de réception en dBm
 *
 * @param rf pointeur sur l'objet xRf69
 * @param bForceTrigger force le module à effectuer la mesure, si false, c'est
 * le niveau de réception correspondant à la dernière réception qui est lu.
 * @return Le niveau de réception en dBm, INT_MAX si erreur
 */
int iRf69Rssi (xRf69 * rf, bool bForceTrigger);

/**
 * @brief Modification de la clé d'encryptage
 * 
 * Tous les noeuds du réseau doivent utiliser la même clé.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param sKey clé de cryptage doit avoir 16 caractères de long, mettre NULL
 * pour désactiver le cryptage.
 * @return 0, valeur négative si erreur
 */
int iRf69SetEncryptKey (xRf69 * rf, const char * sKey);

/**
 * @brief Indique si la transmission est cryptée
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true, false, valeur négative si erreur
 */
int iRf69isEncrypted (xRf69 * rf);

/**
 * @brief Modification du mode promiscuité
 *
 * @param rf pointeur sur l'objet xRf69
 * @param bOn false réception des seuls paquets correspondant à notre adresse ou
 * au broadcast, true réception de tous les paquets.
 * @return 0, valeur négative si erreur
 */
int iRf69SetPromiscuous (xRf69 * rf, bool bOn);

/**
 * @brief Modification du mapping d'une broche DIO
 *
 * @param rf pointeur sur l'objet xRf69
 * @param dio numéro de broche à modifier (1,2,3,5). Le mapping de la broche DIO0
 * ne peut pas être modifié.
 * @param map configuration de mapping choisie conformémement au tableau 22, 
 * page 48 du datasheet.
 * @return 0, valeur négative si erreur
 */
int iRf69SetDioMapping (xRf69 * rf, uint8_t dio, uint8_t map);

/**
 * @brief Lecture du mode promiscuité
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true, false, valeur négative si erreur
 */
bool bRf69isPromiscuous (xRf69 * rf);

/**
 * @brief Passage en mode sommeil
 *
 * Afin d'économiser de l'énergie, un appel à iRf69ReceiveDone() réveille le
 * module.
 *
 * @param rf pointeur sur l'objet xRf69
 * @return 0, valeur négative si erreur
 */
int iRf69Sleep (xRf69 * rf);

/**
 * @brief Calibrage de l'oscilateur RC interne
 *
 * Cet oscillateur est calibré automatiquement au démarrage de l'alimentation,
 * Cette fonction est utile pour une utilisation sur une plage de température
 * plus large ou si le module est démarré sur une longue période (pour corriger
 * le glissement).
 *
 * @param rf pointeur sur l'objet xRf69
 * @return 0, valeur négative si erreur
 */
int iRf69RcCalibration (xRf69 * rf);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_RF69_H_ defined */
