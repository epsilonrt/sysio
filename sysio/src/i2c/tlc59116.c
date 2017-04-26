/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <string.h>
#include <sysio/delay.h>
#include <sysio/i2c.h>
#include "../ledrgb_private.h"

#if 0
/**
 * @file avrio-board-ledrgb.h
 * @brief Configuration module LED RGB
 * @author Copyright © 2012-2013 epsilonRT. All rights reserved.
 * @copyright GNU Lesser General Public License version 3
 *            <http://www.gnu.org/licenses/lgpl.html>
 * @version $Id: avrio-board-ledrgb.h 29 2013-02-17 22:19:35Z pjean $
 * Revision History ---
 *    20130129 - Initial version by epsilonRT
 */
#include <avrio/ledrgb_defs.h>

/* constants ================================================================ */
#define LEDRGB_LED_QUANTITY   16 ///< Nombre de LED RGB
#define LEDRGB_ALL_LEDS       0xFFFF
#define LEDRGB_NO_LED         0
#define LEDRGB_DEFAULT_MODE   MODE_BLINK
#define LEDRGB_CTRL           LEDRGB_CTRL_TLC59116 ///< Type de contrôleur

/* types ==================================================================== */
/*
 * Type utilisé pour les masques de LED RGB. 
 * Le nombre de bits doit être supérieur ou égal à LEDRGB_LED_QUANTITY
 */
typedef uint16_t xLedRgbMask;

/* Contrôleur TLC59116 (I²C) =============== */
/** Réglage du gain en courant à l'init. (c.f. p. 19 du datasheet) */
#define TLC59116_DEFAULT_IREF TLC59116_IREF(1,1,63) // CM=1, HC=1, D=63 

/** 
 * Listes d'adresse I²C des contrôleurs (1 liste par couleur)
 * Il doit y avoir un contrôleur TLC59116 de chaque couleur par paquet
 * de 16 leds.
 *                             TLC59116_ADDR(A3,A2,A1,A0) */
#define TLC59116_RED_LIST    { TLC59116_ADDR(0,0,0,0) }
#define TLC59116_GREEN_LIST  { TLC59116_ADDR(0,0,0,1) }
#define TLC59116_BLUE_LIST   { TLC59116_ADDR(0,0,1,0) }

/** Broche de RESET des contrôleurs, si non défini, un RESET SOFT est effectué */
#define TLC59116_RESET 5
#define TLC59116_RESET_PORT PORTD
#define TLC59116_RESET_DDR  DDRD

/* ========================================================================== */
/* _AVRIO_BOARD_LEDRGB_H_ not defined */

#endif
/* constants ================================================================ */
#define TLC59116_CM      7
#define TLC59116_HC      6
#define TLC59116_CC      0x3F
#define TLC59116_IREF(_cm,_hc,_cc) (((_cm)<<TLC59116_CM)+((_hc)<<TLC59116_HC)+((_cc)&TLC59116_CC))

#define TLC59116_SWRST_BYTE1  0xA5
#define TLC59116_SWRST_BYTE2  0x5A

/* Registres du TCL59116 */
#define MODE1       0x00
#define MODE2       0x01
#define PWM0        0x02
#define PWM1        0x03
#define PWM2        0x04
#define PWM3        0x05
#define PWM4        0x06
#define PWM5        0x07
#define PWM6        0x08
#define PWM7        0x09
#define PWM8        0x0A
#define PWM9        0x0B
#define PWM10       0x0C
#define PWM11       0x0D
#define PWM12       0x0E
#define PWM13       0x0F
#define PWM14       0x10
#define PWM15       0x11
#define GRPPWM      0x12
#define GRPFREQ     0x13
#define LEDOUT0     0x14
#define LEDOUT1     0x15
#define LEDOUT2     0x16
#define LEDOUT3     0x17
#define SUBADR1     0x18
#define SUBADR2     0x19
#define SUBADR3     0x1A
#define ALLCALLADR  0x1B
#define IREF        0x1C
#define EFLAG1      0x1D
#define EFLAG2      0x1E

/* Nombre de registre du TCL59116 */
#define REG_BLOCK_SIZE (EFLAG2-MODE1+1)

/* Bits du TCL59116 */
#define AI2     7
#define AI1     6
#define AI0     5
#define OSC     4
#define SUB1    3
#define SUB2    2
#define SUB3    1
#define ALLCALL 0
#define EFCLR   7
#define DMBLNK  5
#define OCH     3
#define CM      7
#define HC      6
#define CC0     0
#define CC1     1
#define CC2     2
#define CC3     3
#define CC4     4
#define CC5     5
#define  LDR0_0  0
#define  LDR0_1  1
#define  LDR1_0  2
#define  LDR1_1  3
#define  LDR2_0  4
#define  LDR2_1  5
#define  LDR3_0  6
#define  LDR3_1  7
#define  LDR4_0  0
#define  LDR4_1  1
#define  LDR5_0  2
#define  LDR5_1  3
#define  LDR6_0  4
#define  LDR6_1  5
#define  LDR7_0  6
#define  LDR7_1  7
#define  LDR8_0  0
#define  LDR8_1  1
#define  LDR9_0  2
#define  LDR9_1  3
#define  LDR10_0  4
#define  LDR10_1  5
#define  LDR11_0  6
#define  LDR11_1  7
#define  LDR12_0  0
#define  LDR12_1  1
#define  LDR13_0  2
#define  LDR13_1  3
#define  LDR14_0  4
#define  LDR14_1  5
#define  LDR15_0  6
#define  LDR15_1  7

/* Valeurs d'auto-incrémentation */
#define AI_NONE 0
#define AI_ALL     (_BV(AI2))
#define AI_BRIGHT  (_BV(AI2)|_BV(AI0))
#define AI_GLOBAL  (_BV(AI2)|_BV(AI1))

/* structures =============================================================== */
/*
 * Contexte d'un contrôleur TLC59116
 */
typedef struct xTlc59116Ctrl {
  xTlc59116Config config;
  uint8_t regs[REG_BLOCK_SIZE];
} xTlc59116Ctrl;

/*
 * Données privées du DCB
 */
typedef struct xTlc59116Data {
  xTlc59116Ctrl * ctrl;
  int nof_ctrl;
} xTlc59116Data;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Ecrit un bloc de registre d'un contrôleur
static int
iWriteRegBlock (const xTlc59116Ctrl * ctrl, uint8_t first_reg, uint8_t last_reg) {
  int fd, ret = -1;

  fd = iI2cOpen (ctrl->config.i2c_bus, ctrl->config.i2c_addr);

  if (fd >= 0) {
    uint8_t len;
    uint8_t reg;
    int error;

    len = last_reg - first_reg + 1;
    reg = first_reg | (last_reg > first_reg ? AI_ALL : 0);

    ret = iI2cWriteRegBlock (fd, reg, &ctrl->regs[first_reg], len);
    error = iI2cClose (fd);
    if (error != 0) {
      ret = error;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
// Lit un bloc de registre d'un contrôleur
static int
iReadRegBlock (xTlc59116Ctrl * ctrl, uint8_t first_reg, uint8_t last_reg) {
  int fd, ret = -1;

  fd = iI2cOpen (ctrl->config.i2c_bus, ctrl->config.i2c_addr);

  if (fd >= 0) {
    uint8_t len;
    uint8_t reg;
    int error;

    len = last_reg - first_reg + 1;
    reg = first_reg | (last_reg > first_reg ? AI_ALL : 0);

    ret = iI2cReadRegBlock (fd, reg, &ctrl->regs[first_reg], len);
    error = iI2cClose (fd);
    if (error != 0) {
      ret = error;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
// Vérifie la présence d'un contrôleur sur le bus I2C
static int
iCheckCtrlPresence (const xTlc59116Config * config) {
  int fd = iI2cOpen (config->i2c_bus, config->i2c_addr);

  if (fd >= 0) {
    int subadr1;

    subadr1 = iI2cReadReg8 (fd, SUBADR1);
    iI2cClose (fd);
    if (subadr1 == (TLC59116_ADDR_SUB1 << 1)) {
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
// Software RESET
static int
iSwReset (const char * i2c_bus) {
  int fd;

  fd = iI2cOpen (i2c_bus, TLC59116_ADDR_SWRST);
  if (fd >= 0) {
    int ret;
    uint8_t buffer[] = {TLC59116_SWRST_BYTE1, TLC59116_SWRST_BYTE2};

    ret = iI2cWriteBlock (fd, buffer, sizeof (buffer));
    iI2cClose (fd);
    if (ret == sizeof (buffer)) {
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iClose (xLedRgbDcb * dcb) {
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Ctrl * ctrl = d->ctrl;

  for (int i = 0; i < d->nof_ctrl; i++) {
    free (ctrl->config.i2c_bus);
  }
  free (ctrl);
  free (d);
  return 0;
}

// -----------------------------------------------------------------------------
static int
iOpen (xLedRgbDcb * dcb, void * setup) {
  xTlc59116Config * config;
  int nof_ctrl = 0;

  // Comptage du nombre de contrôleurs TLC591116 à configurer
  config = (xTlc59116Config *) setup;
  while (config->i2c_bus) {

    nof_ctrl++;
    config++;
  }

  if (nof_ctrl > 0) {
    const char * str = "";

    // Allocation de la structure de stockage des données privées et mise à zéro
    xTlc59116Data * d;
    d = calloc (1, sizeof (xTlc59116Data));
    assert (d);

    // Allocation du bloc des contrôleurs et mise à zéro
    d->ctrl = calloc (nof_ctrl, sizeof (xTlc59116Ctrl));
    assert (d->ctrl);
    dcb->dptr = d;

    // Copie des configurations et reset contrôleurs
    config = (xTlc59116Config *) setup;
    for (int i = 0; i < nof_ctrl; i++) {
      xTlc59116Ctrl * ctrl;

      // Software RESET sur chaque bus
      if (strcmp (str, config->i2c_bus) != 0) {
        if (iSwReset (config->i2c_bus) != 0) {

          return -1;
        }
      }

      // Vérification de la présence du contrôleur
      if (iCheckCtrlPresence (config) != 0) {

        iClose (dcb);
        return -1;
      }

      // Contrôleur présent et conforme, on copie des infos de configuration
      ctrl = &d->ctrl[i];

      ctrl->config.i2c_bus = malloc (strlen (config->i2c_bus) + 1);
      assert (ctrl->config.i2c_bus);
      strcpy (ctrl->config.i2c_bus, config->i2c_bus);

      ctrl->config.i2c_addr = config->i2c_addr;
      d->nof_ctrl++;
#if 0
      // Régle le courant max dans les leds
      ucRegs[IREF] = TLC59116_DEFAULT_IREF;
      iWriteRegBlock (TLC59116_ADDR_ALL, IREF, IREF);
      DEBUG_READ_CTRL();

      // Configure les leds en mode brightness
      memset (&ucRegs[LEDOUT0], (LEDRGB_DEFAULT_MODE << LDR3_0) + (LEDRGB_DEFAULT_MODE << LDR2_0) +
              (LEDRGB_DEFAULT_MODE << LDR1_0) + (LEDRGB_DEFAULT_MODE << LDR0_0), 4);
      iWriteRegBlock (TLC59116_ADDR_ALL, LEDOUT0, LEDOUT3);
      DEBUG_READ_CTRL();

      // Démarrage des oscillateurs
      ucRegs[MODE1] = _BV (ALLCALL);
      iWriteRegBlock (TLC59116_ADDR_ALL, MODE1, MODE1);
      delay_ms (1); // Délai de démarrage 500 ṁs
      DEBUG_READ_CTRL();
#endif

      config++;
    }

    return 0;
  }

  return -1;
}

// -----------------------------------------------------------------------------
static int
iAddLed (xLedRgbDcb * dcb, void * led) {
  return 0;
}

// -----------------------------------------------------------------------------
static int
iSetColor (xLedRgbDcb * dcb, int led, uint32_t color) {
  return 0;
}

// -----------------------------------------------------------------------------
static int
iSetMode (xLedRgbDcb * dcb, int led, eLedRgbMode mode) {
  return 0;
}

// -----------------------------------------------------------------------------
static int
iSetDimmer (xLedRgbDcb * dcb, uint16_t dimming) {
  return 0;
}

// -----------------------------------------------------------------------------
static int
iSetBlinker (xLedRgbDcb * dcb, uint16_t blinking) {
  return 0;
}

// -----------------------------------------------------------------------------
static int
iError (xLedRgbDcb * dcb, int * code) {
  return 0;
}

/* public variables ========================================================= */
xLedRgbOps xTlc59116Ops = {
  .open = iOpen,
  .close = iClose,
  .addled = iAddLed,
  .setcolor = iSetColor,
  .setmode = iSetMode,
  .setdimmer = iSetDimmer,
  .setblinker = iSetBlinker,
  .error = iError
};

#if 0

/* private variables ======================================================== */
static xTwiFrame xFrame;
static uint8_t ucRegs[REG_BLOCK_SIZE];
static xTwiDeviceAddr xRedCtrl[NUMBER_OF_CTRL]   = TLC59116_RED_LIST;
static xTwiDeviceAddr xGreenCtrl[NUMBER_OF_CTRL] = TLC59116_GREEN_LIST;
static xTwiDeviceAddr xBlueCtrl[NUMBER_OF_CTRL]  = TLC59116_BLUE_LIST;
static int8_t iGlobalError;

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Modifie la couleur d'un bloc de leds
static void
vSetColorBlock (uint8_t ucCtrlIndex,
                uint8_t ucFirst, uint8_t ucSize, xRgbColor * pxColor) {
  uint8_t ucLast = ucFirst + ucSize - 1;

  memset (&ucRegs[ucFirst], pxColor->ucRed, ucSize);
  iWriteRegBlock (xRedCtrl[ucCtrlIndex], ucFirst, ucLast);
  DEBUG_READ_CTRL();
  memset (&ucRegs[ucFirst], pxColor->ucGreen, ucSize);
  iWriteRegBlock (xGreenCtrl[ucCtrlIndex], ucFirst, ucLast);
  DEBUG_READ_CTRL();
  memset (&ucRegs[ucFirst], pxColor->ucBlue, ucSize);
  iWriteRegBlock (xBlueCtrl[ucCtrlIndex], ucFirst, ucLast);
  DEBUG_READ_CTRL();
}

// -----------------------------------------------------------------------------
// Modifie la couleur d'un bloc de 16 leds
static void
vSetColorCtrl (uint8_t ucCtrlIndex, uint16_t usLed, xRgbColor * pxColor) {
  uint8_t ucReg = PWM0;
  uint8_t ucBlockSize = 0;
  uint16_t usMask = 1;

  while (usMask) {

    if (usLed & usMask) {

      // Bit à 1
      ucBlockSize++;

    }
    else {

      // Bit à 0
      if (ucBlockSize) {
        // Un bloc de 1 précède le 0, il faut envoyé la commande aux leds

        vSetColorBlock (ucCtrlIndex, ucReg, ucBlockSize, pxColor);
        ucReg += ucBlockSize + 1;
        ucBlockSize = 0;
      }
      else {

        ucReg++;
      }
    }
    usMask <<= 1;
  }
  if (ucBlockSize) {
    // Un bloc de 1 termine le mot, il faut envoyé la commande aux leds

    vSetColorBlock (ucCtrlIndex, ucReg, ucBlockSize, pxColor);
  }
}

// -----------------------------------------------------------------------------
// Modifie le mode d'un bloc de 16 leds
static void
vSetModeCtrl (uint8_t ucCtrlIndex, uint16_t usLed, eLedRgbMode eMode) {
  uint8_t ucReg = LEDOUT0;
  uint8_t ucRegMask = _BV (LDR0_1) | _BV (LDR0_0);
  uint8_t ucMode = eMode;
  uint16_t usMask = 1;

  iReadRegBlock (xRedCtrl[ucCtrlIndex], LEDOUT0, LEDOUT3);

  while (usMask) {

    if (ucRegMask == 0) {

      ucRegMask = _BV (LDR0_1) | _BV (LDR0_0);
      ucMode = eMode;
      ucReg++;
    }

    if (usLed & usMask) {

      // Bit à 1
      ucRegs[ucReg] &= ~ucRegMask;
      ucRegs[ucReg] |=  ucMode;
    }
    usMask <<= 1;
    ucRegMask <<= 2;
    ucMode <<= 2;
  }
  iWriteRegBlock (xRedCtrl[ucCtrlIndex],   LEDOUT0, LEDOUT3);
  iWriteRegBlock (xGreenCtrl[ucCtrlIndex], LEDOUT0, LEDOUT3);
  iWriteRegBlock (xBlueCtrl[ucCtrlIndex],  LEDOUT0, LEDOUT3);
}

// -----------------------------------------------------------------------------
static void
vClearErrorCtrl (xTwiDeviceAddr xCtrl) {

  iReadRegBlock (xCtrl, MODE2, MODE2);
  ucRegs[MODE2] |= _BV (EFCLR);
  iWriteRegBlock (xCtrl, MODE2, MODE2);
  ucRegs[MODE2] &= ~_BV (EFCLR);
  iWriteRegBlock (xCtrl, MODE2, MODE2);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int8_t
iLedRgbInit (void) {
  int8_t iError;

#ifdef TLC59116_RESET
  // Hardware RESET
  TLC59116_RESET_DDR  |=  _BV (TLC59116_RESET);
  TLC59116_RESET_PORT &= ~_BV (TLC59116_RESET);
  delay_ms (1);
  TLC59116_RESET_PORT |= _BV (TLC59116_RESET);
  delay_ms (1);
#else
  // Software RESET
  xFrame.xAddr = TLC59116_ADDR_SWRST;
  xFrame.xLen = 2;
  ucRegs[0] = TLC59116_SWRST_BYTE1;
  ucRegs[1] = TLC59116_SWRST_BYTE2;
  xFrame.pxData = &ucRegs[0];
  iGlobalError = eTwiSend (&xFrame);
  if (iGlobalError) {
    return iGlobalError;
  }
#endif

  // Vérifie la présence sur le bus iĠc de tous les contrôleurs
  iError = iCheckAllCtrl();
  if (iError) {
    return iError;
  }

  // Régle le courant max dans les leds
  ucRegs[IREF] = TLC59116_DEFAULT_IREF;
  iWriteRegBlock (TLC59116_ADDR_ALL, IREF, IREF);
  DEBUG_READ_CTRL();

  // Configure les leds en mode brightness
  memset (&ucRegs[LEDOUT0], (LEDRGB_DEFAULT_MODE << LDR3_0) + (LEDRGB_DEFAULT_MODE << LDR2_0) +
          (LEDRGB_DEFAULT_MODE << LDR1_0) + (LEDRGB_DEFAULT_MODE << LDR0_0), 4);
  iWriteRegBlock (TLC59116_ADDR_ALL, LEDOUT0, LEDOUT3);
  DEBUG_READ_CTRL();

  // Démarrage des oscillateurs
  ucRegs[MODE1] = _BV (ALLCALL);
  iWriteRegBlock (TLC59116_ADDR_ALL, MODE1, MODE1);
  delay_ms (1); // Délai de démarrage 500 ṁs
  DEBUG_READ_CTRL();

  return iError;
}


// -----------------------------------------------------------------------------
void
vLedRgbSetColor (uint64_t xLed, uint32_t ulColor) {
  uint8_t ucCtrlIndex;
  uint16_t usLed;
  xRgbColor * pxColor = (xRgbColor *) &ulColor;

  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {

    usLed = (uint16_t) (xLed >> (16 * ucCtrlIndex));
    vSetColorCtrl (ucCtrlIndex, usLed, pxColor);
  }
}

// -----------------------------------------------------------------------------
void
vLedRgbSetMode (uint64_t xLed, eLedRgbMode eMode) {
  uint8_t ucCtrlIndex;
  uint16_t usLed;

  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {

    usLed = (uint16_t) (xLed >> (16 * ucCtrlIndex));
    vSetModeCtrl (ucCtrlIndex, usLed, eMode);
  }
}

// -----------------------------------------------------------------------------
void
vLedRgbSetGlobalDimming (uint8_t ucDimming) {

  ucRegs[GRPPWM] = ucDimming;
  iWriteRegBlock (TLC59116_ADDR_ALL, GRPPWM, GRPPWM);

  iReadRegBlock (xRedCtrl[0], MODE2, MODE2);
  ucRegs[MODE2] &= ~_BV (DMBLNK);
  iWriteRegBlock (TLC59116_ADDR_ALL, MODE2, MODE2);
}

// -----------------------------------------------------------------------------
void
vLedRgbSetGlobalBlinking (uint16_t usPeriod, uint8_t ucDutyCycle) {

  if (usPeriod < 42) {
    usPeriod = 42;
  }

  ucRegs[GRPPWM]  = ucDutyCycle;
  ucRegs[GRPFREQ] = (uint8_t) MIN ( (usPeriod * 3) / 125 - 1, 255);
  iWriteRegBlock (TLC59116_ADDR_ALL, GRPPWM, GRPFREQ);

  iReadRegBlock (xRedCtrl[0], MODE2, MODE2);
  ucRegs[MODE2] |= _BV (DMBLNK);
  iWriteRegBlock (TLC59116_ADDR_ALL, MODE2, MODE2);
}

// -----------------------------------------------------------------------------
uint64_t
xLedRgbError (void) {
  uint64_t xError = 0, xCtrlError;
  uint8_t ucCtrlIndex;

  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {

    xCtrlError = 0;
    iReadRegBlock (xRedCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~ (ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);
    iReadRegBlock (xGreenCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~ (ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);
    iReadRegBlock (xBlueCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~ (ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);

    xError |= (xCtrlError << (16 * ucCtrlIndex));
  }

  return xError;
}

// -----------------------------------------------------------------------------
void
vLedRgbClearError (void) {
  uint8_t ucCtrlIndex;

  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {

    vClearErrorCtrl (xRedCtrl[ucCtrlIndex]);
    vClearErrorCtrl (xGreenCtrl[ucCtrlIndex]);
    vClearErrorCtrl (xBlueCtrl[ucCtrlIndex]);
  }
}
#endif
/* ========================================================================== */
