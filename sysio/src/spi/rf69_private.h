/**
 * @file
 * @brief Module de transmission UHF RFM69 (Déclarations privées)
 *
 * http://www.hoperf.com/rf_transceiver/modules/RFM69W.html
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _RF69_PRIVATE_H_
#define _RF69_PRIVATE_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include "rf69_registers.h"

/* constants ================================================================ */
/**
 * @enum eRf69Mode
 * @brief Mode de fonctionnement
 */
typedef enum {
  eRf69ModeSleep   =      0, /**< XTAL OFF */
  eRf69ModeStandby =      1, /**< XTAL ON */
  eRf69ModeSynth   =      2, /**< PLL ON */
  eRf69ModeRx      =      3, /**< RX MODE */
  eRf69ModeTx      =      4  /**< TX MODE */
} eRf69Mode;

/* structures =============================================================== */
struct xRf69Header {
  uint8_t payload_len;
  uint8_t dest;
  uint8_t sender;
  uint8_t ctl;
#define RF69_ACK      0x80
#define RF69_ACKREQ  0x40
} __attribute__ ( (__packed__));

/* settings ================================================================= */
/* Bande 315MHz - 290 à 340 MHz pour le RFM69
 * Interdite en Europe (réservé à l'armée !)
 */
#define RF69_FRF_315_MIN 290E6
#define RF69_FRF_315_MAX 340E6
#define RF69_FRF_315_DEF 315E6
#define RF69_FRF_315_STP 25E3

/* Bande 433MHz - 424 à 510 MHz pour le RFM69
 * 69 canaux 25kHz de 433,075 MHz à 434,775 MHz
 * Réglementation européenne ISM
 * http://www.erodocdb.dk/docs/doc98/official/pdf/ERCRep025.pdf
 * Puissance max. 10mW
 */
#define RF69_FRF_433_MIN 433075E3
#define RF69_FRF_433_MAX 434775E3
#define RF69_FRF_433_DEF 433925E3
#define RF69_FRF_433_STP 25E3

/* Bande 868MHz - 862 à 890 MHz pour le RFM69
 * Réglementation européenne ISM
 * 81 canaux 25kHz de 868 MHz à 870 MHz
 * http://www.erodocdb.dk/docs/doc98/official/pdf/ERCRep025.pdf
 * Puissance max. dépendant de la fréquence voir :
 * http://docplayer.fr/docs-images/40/386919/images/6-0.png
 */
#define RF69_FRF_868_MIN 868E6
#define RF69_FRF_868_MAX 870E6
#define RF69_FRF_868_DEF 868E6
//#define RF69_FRF_868_DEF 869250E3
#define RF69_FRF_868_STP 25E3

/* Bande 915MHz - 290 à 340 MHz pour le RFM69
 * Interdite en Europe (réservé à l'armée !)
 */
#define RF69_FRF_915_MIN 890E6
#define RF69_FRF_915_MAX 1020E6
#define RF69_FRF_915_DEF 915E6
#define RF69_FRF_915_STP 25E3

/*
 * Temps d'attente max avant émission
 * La tentative d'émission est infructueuse si le canal ne se libère pas
 * au bout de ce délai
 */
#define RF69_CSMA_TIMEOUT_MS     1000
// Niveau de réception en dessous duquel on considère que le canal est libre
#define RF69_CSMA_MIN_LEVEL_DBM  -90
// Délai max de réponse du module après une demande d'émission
#define RF69_TX_TIMEOUT_MS       1000
// Puissance relative à l'ouverture
#define RF69_DEFAULT_POWER       18 // 0 dBm
// Valeur lu dans le registre RegTemp2 à la température ambiante (20°C)
#define RF69_TEMP_CAL 150

/* constants ================================================================ */
#define RF69_HEADER_SIZE 4
/*
 * Pour tirer avantage du cryptage et de la vérification par CRC, la taille
 * des données ne peut dépasser 61 octets (taille de la FIFO 66 octets - 3
 * octets d'entête - 2 octets de CRC
 */
#define RF69_MAX_DATA_LEN (RF_FIFO_SIZE - 5)


#ifndef __AVRIO__
// -----------------------------------------------------------------------------
// ************   Partie spécifique pour Unix (avec SysIo)          ************
// -----------------------------------------------------------------------------
#include <sysio/rf69.h>
#include <sysio/timer.h>
#include <sysio/dinput.h>
#include <pthread.h>

/* macros =================================================================== */
/**
 * Déclare que la valeur à tester est de type int
 */
#define TRY_INT_INIT() int __error

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours en retournant cette valeur.
 */
#define TRY_INT(__op) do { __error = (__op); if (__error < 0) return __error; } while(0)

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours en retournant __val.
 */
#define TRY_INT_VAL(__op,__val) do { __error = (__op); if (__error < 0) return __val; } while(0)

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours par un return (void).
 */
#define TRY_INT_VOID(__op) do { __error = (__op); if (__error < 0) return; } while(0)

/**
 * Déclare que la valeur à tester est de type pointeur
 */
#define TRY_PTR_INIT() void * __ptr

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie un pointeur null
 * quitte la fonction en cours en retournant NULL.
 */
#define TRY_PTR(__op) do { __ptr = (__op); if (__ptr == NULL) return NULL; } while(0)

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie un pointeur null
 * quitte la fonction en cours en retournant __val.
 */
#define TRY_PTR_VAL(__op,__val) do { __ptr = (__op); if (__ptr == NULL) return __val; } while(0)

#ifndef PROGMEM
// pas de mémoire flash sur un système Unix !
#define PROGMEM
#endif

/* types ==================================================================== */
typedef xTimer * xRf69Timer;

/* structures =============================================================== */
/**
 * @class xRf69
 * @brief
 */
struct xRf69 {
  uint8_t data[RF_FIFO_SIZE];
  uint8_t data_len;
  struct xRf69Header hdr;
  int rssi;
  uint8_t mode;
  union {
    uint8_t flags;
    struct {
      uint8_t promiscuous: 1;
      uint8_t is_rfm69hw: 1;
      uint8_t is_open: 1;
    };
  };
  uint8_t node_id;
  uint8_t power_level;
  int fd; // descripteur de fichier vers spidev
  pthread_mutex_t spi_mutex;
  pthread_mutex_t rcv_mutex;
  xDinPort * port;
};

// -----------------------------------------------------------------------------

#else
// -----------------------------------------------------------------------------
// ************ Partie spécifique pour AVR (avec AvrIo)             ************
// -----------------------------------------------------------------------------
#include <avrio/rf69.h>
#include <avrio/task.h>
#include <avr/pgmspace.h>

/* macros =================================================================== */
/**
 * Déclare que la valeur à tester est de type int
 */
#define TRY_INT_INIT()

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours en retournant cette valeur.
 */
#define TRY_INT(__op) __op

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours en retournant __val.
 */
#define TRY_INT_VAL(__op,__val) __op

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie une valeur négative,
 * quitte la fonction en cours par un return (void).
 */
#define TRY_INT_VOID(__op) __op

/**
 * Déclare que la valeur à tester est de type pointeur
 */
#define TRY_PTR_INIT()

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie un pointeur null
 * quitte la fonction en cours en retournant NULL.
 */
#define TRY_PTR(__op) __op

/**
 * Essaye d'éxécuter l'opération __op, si __op renvoie un pointeur null
 * quitte la fonction en cours en retournant __val.
 */
#define TRY_PTR_VAL(__op,__val) __op

/* types ==================================================================== */
typedef xTaskHandle xRf69Timer;

/* structures =============================================================== */
/**
 * @class xRf69
 * @brief
 */
struct xRf69 {
  uint8_t data[RF_FIFO_SIZE]; // tx/rx buffer, modifié sous interruption
  uint8_t data_len; // modifié sous interruption
  struct xRf69Header hdr; // received header, modifié sous interruption
  int rssi; // modifié sous interruption
  uint8_t mode; // modifié sous interruption
  union {
    uint8_t flags;
    struct {
      uint8_t promiscuous: 1; // lu sous interruption
      uint8_t is_rfm69hw: 1; // lu sous interruption
      uint8_t is_open: 1;
    };
  };
  uint8_t node_id; // lu sous interruption
  uint8_t power_level;
  int irqpin;
};
// -----------------------------------------------------------------------------
#endif

/* structures =============================================================== */

/**
 * @class xRf69Config
 * @brief
 */
struct xRf69Config {
  uint8_t reg;
  uint8_t data;
};

/* protected functions ====================================================== */
// -----------------------------------------------------------------------------
// ************ Fonctions réservées à un usage interne              ************
// -----------------------------------------------------------------------------
/**
 * @brief Routine d'interruption
 * @param rf pointeur sur l'objet xRf69
 */
void vRf69Isr (struct xRf69 * rf);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @param eNewMode
 * @return
 */
int iRf69SetMode (xRf69 * rf, eRf69Mode eNewMode);

/**
 * @brief Attente de passage de la broche DIO0 (Irq) à l'état haut
 * @param rf pointeur sur l'objet xRf69
 * @param timeout en ms
 * @return true DIO0 à l'état haut, false si timeout, -1 si erreur
 */
int iRf69WaitIrq (xRf69 * rf, int timeout);

/**
 * @brief Attente module prêt
 * @param rf pointeur sur l'objet xRf69
 * @param timeout en ms
 * @return true module prêt, false si timeout, -1 si erreur
 */
int iRf69WaitForReady (xRf69 * rf, int timeout);

/**
 * @brief wait to send
 * @param rf pointeur sur l'objet xRf69
 * @param timeout
 * @return 0 succès, -1 timeout
 */
int iRf69WaitToSend (xRf69 * rf, int timeout);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @param timeout
 * @return 0 succès, -1 timeout
 */
int iRf69WriteRegWithCheck (xRf69 * rf, uint8_t reg, uint8_t data, int timeout);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @param toAddress
 * @param tx_buffer
 * @param tx_len
 * @param bRequestACK
 * @param bSendAck
 * @return
 */
int iRf69SendFrame (xRf69 * rf, uint8_t toAddress, const void * tx_buffer, uint8_t tx_len, bool bRequestACK, bool bSendAck);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @return
 */
int iRf69AvoidRxDeadLocks (xRf69 * rf) ;

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @return
 */
int iRf69StartReceiving (xRf69 * rf);

/**
 * @brief
 * @param rf pointeur sur l'objet xRf69
 * @param bOn
 * @return
 */
int iRf69SetHighPowerRegs (xRf69 * rf, bool bOn);

/* protected functions ====================================================== */
// -----------------------------------------------------------------------------
// Fonctions devant être implémentées par la partie spécifique à la plateforme
// -----------------------------------------------------------------------------
// ---------------------------- Accès au bus SPI -------------------------------
/**
 * @brief Ecriture d'un registre 8 bits
 *
 * @param rf pointeur sur l'objet xRf69
 * @param reg adresse du registre
 * @param data valeur de l'octet
 * @return nombre d'octets écrits, -1 si erreur
 */
int iRf69WriteReg (xRf69 * rf, uint8_t reg, uint8_t data);

/**
 * @brief Ecriture d'un registre 8 bits sous forme de stucture constante
 *
 * Sur AVR par exemple, la configuration initiale est stockée en FLASH ce qui
 * nécessite un traitement particulier.
 *
 * @param rf pointeur sur l'objet xRf69
 * @param elmt adresse et valeur du registre sous forme de stucture constante
 * @return 1 si le registre a été écrit, 0 si elmt pointe sur le registre 255
 * (fin de configuration)
 */
int iRf69WriteConstElmt (xRf69 * rf, const struct xRf69Config * elmt);

/**
 * @brief Ecriture d'un bloc de registres
 *
 * @param rf pointeur sur l'objet xRf69
 * @param reg adresse du premier registre
 * @param buffer pointeur vers la zone de stockage des octets
 * @param len nombre d'octets à écrire
 * @return nombre d'octets écrits, -1 si erreur
 */
int iRf69WriteBlock (xRf69 * rf, uint8_t reg, const uint8_t * buffer, uint8_t len);

/**
 * @brief Lecture d'un registre 8 bits
 *
 * Cette fonction réalise une transmission de l'adresse du registre à lire,
 * suivie d'une lecture d'un octet.\n
 * @param rf pointeur sur l'objet xRf69
 * @param reg adresse du registre
 * @return la valeur de l'octet comme un unsigned int, -1 si erreur
 */
int iRf69ReadReg (xRf69 * rf, uint8_t reg);

/**
 * @brief Lecture d'un bloc de registres
 *
 * @param rf pointeur sur l'objet xRf69
 * @param reg adresse du premier registre
 * @param buffer pointeur vers la zone de stockage des octets, la taille doit
 *        être suffisante pour y stocker le nombre d'octets demandés.
 * @param len nombre de registres à lire
 * @return le nombre d'octets lus, -1 si erreur
 */
int iRf69ReadBlock (xRf69 * rf, uint8_t reg, uint8_t * buf, uint8_t len);

// ------------------------- Broche d'interruption -----------------------------
/**
 * @brief Lecture de l'état de DIO0 (Irq)
 *
 * @param rf pointeur sur l'objet xRf69
 * @return true si active, false si inactive, -1 si erreur
 */
int iRf69ReadIrqPin (xRf69 * rf);

// ---------------------------------- Timers -----------------------------------
/**
 * @brief Création d'un timer
 * @return Le nouveau timer
 */
xRf69Timer xRf69TimerNew (void);

/**
 * @brief Suppression d'un timer
 * @param t le timer
 */
void vRf69TimerDelete (xRf69Timer t);

/**
 * @brief Démarre un timer
 * @param t le timer
 * @param timeout timeout en ms
 */
void vRf69TimerStart (xRf69Timer t, int timeout);

/**
 * @brief Arrête un timer
 * @param t le timer
 */
void vRf69TimerStop (xRf69Timer t);

/**
 * @brief Teste si le timeout est atteint
 * @param t le timer
 * @return true timeout, false timer en fonctionnement ou arrêté
 */
bool bRf69TimerTimeout (xRf69Timer t);

/* ========================================================================== */
__END_C_DECLS
#endif /*_RF69_PRIVATE_H_ defined */
