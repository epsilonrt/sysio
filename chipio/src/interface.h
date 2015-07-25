/**
 * @file chipio.h
 * @brief Circuit d'entrées-sorties universel
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _CHIPIO_INTERFACE_H_
#define _CHIPIO_INTERFACE_H_
#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================== */

/* constants ================================================================ */
// Octet de contrôle
#define CHIPIO_CO 0x80 // Bit CO dans l'octet de contrôle
#define CHIPIO_RS 0x40 // Bit RS dans l'octet de contrôle
#define CHIPIO_RA 0x20 // Bit RA (register access): accès par registres
#define CHIPIO_CF 0x10 // Bit CF (config): registre de configuration

/*******************************************************************************
 *                        Interface I²C de ChipIO                              *
 *******************************************************************************
 * L'interface de commande LCD est conforme au protocole utilisé par le
 * contrôleur ST7032.
 *
 * Trames I²C en écriture :
 * ------------------------
 * Ecritue dans les registres -> S SLA+W A CTRL A DATA A DATA A .... P
 * Commande LCD               -> S SLA+W A CTRL A DATA A CTRL A DATA A .... P
 *
 * Dans le cas de commandes LCD, chaque donnée est précédée d'un octet de
 * contrôle. Chaque octet DATA écrit dans un registre provoque le passage au
 * registre suivant (auto-incrémentation) sauf si le registre précisé dans le
 * champ CTRL correspond à eRegSerTx.
 *
 * Légende:
 * - S: Condition de start
 * - SLA+W : Adresse I²C du circuit (TWI_ADDR) avec bit LSB en écriture (0)
 * - A : Bit acknoledge (0)
 * - CTRL : Octet de contrôle. Si le bit CHIPIO_RA est levé, il s'agit d'une
 *          écriture dans les registres, sinon il s'agit de commandes LCD.
 *          - Dans le cas d'une écriture dans un registre, CHIPIO_CF indique
 *            un accès à un registre sauvegardé en EEPROM.
 *          - Dans le cas d'une commande LCD, CHIPIO_RS levé indique un accès
 *            au registre de donnée, baissé au registre d'instruction.
 *            CHIPIO_CO est baissé pour la dernière commande de la trame, sinon
 *            il est levé.
 * - DATA : octets de données suivi par ACK de ChipIO, si ACK = 1 cela indique
 *          que ChipIo ne peut plus accepter de données, la trame doit stopper.
 * - P : Condition de stop
 *
 * Trames I²C en lecture :
 * -----------------------
 * Le LCD est accessible qu'en écriture.
 *
 * Accès simple  -> S SLA+R A DATA A .... DATA /A P
 * Accès combiné -> S SLA+W A CTRL A Rs SLA+R A DATA A .... DATA /A P
 *
 * Un accès combiné permet de présicer l'adresse du premier registre à lire.
 * Chaque lecture provoque le passage au registre suivant (auto-incrémentation)
 * sauf si le registre précisé dans le champ CTRL correspond à eRegSerTx ou
 * eRegSerRx
 *
 * Légende:
 * - S: Condition de start
 * - SLA+W : Adresse I²C du circuit (TWI_ADDR) avec bit LSB en écriture (0)
 * - A : Bit acknoledge (0)
 * - CTRL : Octet de contrôle avec bit CHIPIO_RA est levé, correspond à
 *          l'adresse du premier registre à lire.
 * - Rs: Condition de restart
 * - SLA+R : Adresse I²C du circuit (TWI_ADDR) avec bit LSB en lecture (1)
 * - DATA : octets de données envoyé par ChipIO, si /A (ACK=1) cela indique
 *          que le maître ne veut plus de données (dernière lecture).
 * - P : Condition de stop
 *
 * ================== Plan d'adressage des registres I2C =======================
 *
 * -----------------------------------------------------------------------------
 *                      --- Registres de travail ---
 * Reg    RW  Description
 * -----------------------------------------------------------------------------
 * x20     R   Options disponibles (utilise eChipIoOption)
 * x21     R   Revision logicielle, 4 bits MSB: majeur, 4 bits LSB: mineur
 * x22-23  R   Voie ADC0 (lsb-msb)
 * x24-25  R   Voie ADC1 (lsb-msb)
 * x26-27  R   Voie ADC2 (lsb-msb)
 * x28-29  R   Voie ADC3 (lsb-msb)
 * x2A     RW  Registre d'état série :
 *         R   Bit 0 à 6: taille du tampon de transmission
 *         RW  Bit 7: Bit Busy, à 1 l'interface série n'est pas dispo :
 *             transmission ou configuration en cours. On peut le
 *             remettre à zéro en écrivant un 1 dedans (ce qui a pour
 *             conséquence de vider les buffers).
 * x2B     RW  Registre de transmission série :
 *             Tout caractère qui y est écrit est transmis sur la liaison série
 * x2C     R   Registre d'état réception série :
 *             Contient le nombre de caractères à lire dans le buffer de
 *             réception
 * x2D     R   Registre de réception série :
 *             Dernier caractère reçu sur la liaison série
 *
 * -----------------------------------------------------------------------------
 *        --- Registres de configuration sauvegardés en EEPROM ---
 * Reg    RW  Description
 * -----------------------------------------------------------------------------
 * x30     RW  Options activées (utilise eChipIoOption)
 * x31-32  RW  Registre de contrôle série (lsb-msb)
 * x33-34  RW  Registre de baudrate série en centaines de baud (lsb-msb)
 * x35     RW  Registre de contrôle ADC
 * x36     RW  Registre de contrôle du diviseur d'horloge ADC
 * =============================================================================
 * Les trames I2C ayant une adresse de registre correspondant à eRegSerTx ou
 * eRegSerRx permettent une lecture ou une écriture répétitive dans ces
 * registres (auto-incrémentation dévalidée).
 */
typedef enum {
  eRegOptAv    = CHIPIO_RA,                 // 0x20
  eRegRev      = CHIPIO_RA + 1,             // 0x21
  eRegAdc0     = CHIPIO_RA + 2,             // 0x22 LSB - 0x23 MSB
  eRegAdc1     = CHIPIO_RA + 4,             // 0x24 LSB - 0x25 MSB
  eRegAdc2     = CHIPIO_RA + 6,             // 0x26 LSB - 0x27 MSB
  eRegAdc3     = CHIPIO_RA + 8,             // 0x28 LSB - 0x29 MSB
  eRegSerSr    = CHIPIO_RA + 0x0A,          // 0x2A
  eRegSerTx    = CHIPIO_RA + 0x0B,          // 0x2B
  eRegSerRxSr  = CHIPIO_RA + 0x0C,          // 0x2C
  eRegSerRx    = CHIPIO_RA + 0x0D,          // 0x2D

  eRegOptEn    = CHIPIO_RA + CHIPIO_CF,     // 0x30
  eRegSerCr    = CHIPIO_RA + CHIPIO_CF + 1, // 0x31 LSB - 0x32 MSB
  eRegSerBaud  = CHIPIO_RA + CHIPIO_CF + 3, // 0x33 LSB - 0x34 MSB
  eRegAdcCr    = CHIPIO_RA + CHIPIO_CF + 5, // 0x35
  eRegAdcDiv   = CHIPIO_RA + CHIPIO_CF + 6, // 0x36
} eChipIoRegister;

// eRegOptions
typedef enum {
  eOptionLcd              = 0x01,
  eOptionAdc              = 0x02,
  eOptionSerial           = 0x04,
  eOptionSerialIrq        = 0x40,
  eOptionLcdSplashScreen  = 0x80
} eChipIoOption;

// eRegSerCr =
//          eChipIoDataBit + eChipIoStopBit + eChipIoParity + eChipIoFlowControl
typedef enum {
  eDataBit5     = 0x0000,
  eDataBit6     = 0x0002,
  eDataBit7     = 0x0004,
  eDataBit8     = 0x0006,
  eDataBitMask  = 0x0007
} eChipIoDataBit;

// eRegSerCr =
//          eChipIoDataBit + eChipIoStopBit + eChipIoParity + eChipIoFlowControl
typedef enum {
  eStopBit1    = 0x0000,
  eStopBit2    = 0x0008,
  eStopBitMask = 0x0008
} eChipIoStopBit;

// eRegSerCr =
//          eChipIoDataBit + eChipIoStopBit + eChipIoParity + eChipIoFlowControl
typedef enum {
  eParityNone = 0x0000,
  eParityEven = 0x0020,
  eParityOdd  = 0x0030,
  eParityMask = 0x0030
} eChipIoParity;

// eRegSerCr =
//          eChipIoDataBit + eChipIoStopBit + eChipIoParity + eChipIoFlowControl
typedef enum {
  eFlowNone  = 0x0000,
  eFlowHard  = 0x0200,
  eFlowSoft  = 0x0400,
  eFlowMask  = 0x0600,
} eChipIoFlowControl;

// eRegSerSr
typedef enum {
  eStatusBusy = 0x80  /* Bit Busy, à 1 l'interface série n'est pas
                       * dispo: transmission ou configuration en cours.
                       * On peut le remettre à zéro en écrivant un 1
                       * dedans (ce qui a pour conséquence de vider
                       * les buffers).
                       */
} eChipIoSerialStatus;

// eRegAdcCr
typedef enum {
  eRefExternal  = 0x00,
  eRefVcc       = 0x01,
  eRefInternal  = 0x03,
} eChipIoRef;


/* Commandes LCD étendues
 * Le passage en mode étendu se fait en appelant FSET avec IS à 1, toutes les
 * commandes suivantes sont alors des commandes étendues jusqu'à ce que FSET
 * soit rappellé avec IS à 0.
 * Commandes LCD étendues:
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  | Commande                 |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  1  |  0  | BL5 | BL4 | BL3 | BL2 | BL1 | BL0 | Set backlight            |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  1  | BON | CT4 | CT3 | CT2 | CT1 | CT0 | Set contrast & Power     |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  1  |  0  |  N  |  F  |  X  | IS  | Function set             |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  0  |  1  |     |     |     |     | Function available 1     |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  0  |  0  |  1  |     |     |     | Function available 2     |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  0  |  0  |  0  |  1  |     |     | Function available 3     |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  0  |  0  |  0  |  0  |  1  |     | Function available 4     |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 * |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  1  | Software RESET           |
 * +-----+-----+-----+-----+-----+-----+-----+-----+--------------------------+
 */
#define CHIPIO_SRST   0x01  /*< Reset Logiciel */
#define CHIPIO_BON    0x20  /*< 0 Alim Lcd Off - 1 Alim Lcd On (CHIPIO_CSET) */
#define CHIPIO_CSET   0x40  /*< Commande Contrast Set */
#define CHIPIO_BSET   0x80  /*< Commande Backlight Set */

/* ========================================================================== */
#ifdef __cplusplus
}
#endif
#endif  /* _CHIPIO_INTERFACE_H_ not defined */
