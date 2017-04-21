/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 * 
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include "avrio-config.h"

#ifdef AVRIO_LEDRGB_ENABLE
#include "avrio-board-ledrgb.h"


#if LEDRGB_CTRL == LEDRGB_CTRL_TLC59116
/* ========================================================================== */
#include <sysio/delay.h>
#include <sysio/twi.h>
#include <sysio/ledrgb.h>

//#define DEBUG_READ_CTRL() iCheckAllCtrl()
#define DEBUG_READ_CTRL() 

/* constants ================================================================ */

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

/* Nombre de registre du TCL59116 */
#define REG_BLOCK_SIZE (EFLAG2-MODE1+1)

/* Nombre de TCL59116 par couleur */
#define NUMBER_OF_CTRL ((LEDRGB_LED_QUANTITY)/16)

/* private variables ======================================================== */
static xTwiFrame xFrame;
static uint8_t ucRegs[REG_BLOCK_SIZE];
static xTwiDeviceAddr xRedCtrl[NUMBER_OF_CTRL]   = TLC59116_RED_LIST;
static xTwiDeviceAddr xGreenCtrl[NUMBER_OF_CTRL] = TLC59116_GREEN_LIST;
static xTwiDeviceAddr xBlueCtrl[NUMBER_OF_CTRL]  = TLC59116_BLUE_LIST;
static int8_t iGlobalError;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Ecrit un bloc de registre dans un contrôleur
static void
vWriteRegBlock (xTwiDeviceAddr xAddr, uint8_t ucFirst, uint8_t ucLast) {
  uint8_t ucCtrl;
  
  ucCtrl = ucFirst | (ucLast > ucFirst ? AI_ALL : 0);
  xFrame.pxData = &ucRegs[ucFirst];
  xFrame.xLen = ucLast - ucFirst + 1;
  xFrame.xAddr = xAddr;
  iGlobalError = eTwiMem8Write (ucCtrl, &xFrame);
}

// -----------------------------------------------------------------------------
// Lit un bloc de registre dans un contrôleur
static void
vReadRegBlock (xTwiDeviceAddr xAddr, uint8_t ucFirst, uint8_t ucLast) {
  uint8_t ucCtrl;
  
  ucCtrl = ucFirst | (ucLast > ucFirst ? AI_ALL : 0);
  xFrame.pxData = &ucRegs[ucFirst];
  xFrame.xLen = ucLast - ucFirst + 1;
  xFrame.xAddr = xAddr;
  iGlobalError = eTwiMem8Read (ucCtrl, &xFrame);
}

// -----------------------------------------------------------------------------
// Vérifie la présence des contrôleurs sur le bus I2C
static int8_t 
iCheckCtrlAck (xTwiDeviceAddr xCtrlList[]) {

  xFrame.pxData = &ucRegs[MODE1];

  for (uint8_t ucIndex = 0; ucIndex < NUMBER_OF_CTRL; ucIndex++) {
    
    vReadRegBlock (xCtrlList[ucIndex], MODE1, EFLAG2);
    if (iGlobalError)
      break;
    if (ucRegs[SUBADR1] != TLC59116_ADDR_SUB1) {

      iGlobalError = TWI_ERROR_INVALID_DEVICE;
      break;
    }
  }
  return iGlobalError;
}

// -----------------------------------------------------------------------------
static int8_t
iCheckAllCtrl (void) {
  int8_t iError;

  iError = iCheckCtrlAck(xRedCtrl);
  if (iError)
    return iError;
  iError = iCheckCtrlAck(xGreenCtrl);
  if (iError)
    return iError;
  iError = iCheckCtrlAck(xBlueCtrl);
  return iError;
}

// -----------------------------------------------------------------------------
// Modifie la couleur d'un bloc de leds
static void
vSetColorBlock (uint8_t ucCtrlIndex, 
                uint8_t ucFirst, uint8_t ucSize, xRgbColor * pxColor) {
  uint8_t ucLast = ucFirst + ucSize - 1;

  memset (&ucRegs[ucFirst], pxColor->ucRed, ucSize);
  vWriteRegBlock (xRedCtrl[ucCtrlIndex], ucFirst, ucLast);
  DEBUG_READ_CTRL();
  memset (&ucRegs[ucFirst], pxColor->ucGreen, ucSize);
  vWriteRegBlock (xGreenCtrl[ucCtrlIndex], ucFirst, ucLast);
  DEBUG_READ_CTRL();
  memset (&ucRegs[ucFirst], pxColor->ucBlue, ucSize);
  vWriteRegBlock (xBlueCtrl[ucCtrlIndex], ucFirst, ucLast);
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
  uint8_t ucRegMask = _BV(LDR0_1)|_BV(LDR0_0);
  uint8_t ucMode = eMode;
  uint16_t usMask = 1;
  
  vReadRegBlock (xRedCtrl[ucCtrlIndex], LEDOUT0, LEDOUT3);
  
  while (usMask) {
  
    if (ucRegMask == 0) {
    
      ucRegMask = _BV(LDR0_1)|_BV(LDR0_0);
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
  vWriteRegBlock (xRedCtrl[ucCtrlIndex],   LEDOUT0, LEDOUT3);
  vWriteRegBlock (xGreenCtrl[ucCtrlIndex], LEDOUT0, LEDOUT3);
  vWriteRegBlock (xBlueCtrl[ucCtrlIndex],  LEDOUT0, LEDOUT3);
}

// -----------------------------------------------------------------------------
static void
vClearErrorCtrl (xTwiDeviceAddr xCtrl) {

  vReadRegBlock (xCtrl, MODE2, MODE2);
  ucRegs[MODE2] |= _BV(EFCLR);
  vWriteRegBlock (xCtrl, MODE2, MODE2);
  ucRegs[MODE2] &= ~_BV(EFCLR);
  vWriteRegBlock (xCtrl, MODE2, MODE2);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int8_t 
iLedRgbInit (void) {
  int8_t iError;
  
#ifdef TLC59116_RESET
  // Hardware RESET
  TLC59116_RESET_DDR  |=  _BV(TLC59116_RESET);
  TLC59116_RESET_PORT &= ~_BV(TLC59116_RESET);
  delay_ms(1);
  TLC59116_RESET_PORT |= _BV(TLC59116_RESET);
  delay_ms(1);
#else
  // Software RESET
  xFrame.xAddr = TLC59116_ADDR_SWRST;
  xFrame.xLen = 2;
  ucRegs[0] = TLC59116_SWRST_BYTE1;
  ucRegs[1] = TLC59116_SWRST_BYTE2;
  xFrame.pxData = &ucRegs[0];
  iGlobalError = eTwiSend (&xFrame);
  if (iGlobalError)
    return iGlobalError;
#endif
  
  // Vérifie la présence sur le bus iĠc de tous les contrôleurs
  iError = iCheckAllCtrl();
  if (iError)
    return iError;
   
  // Régle le courant max dans les leds
  ucRegs[IREF] = TLC59116_DEFAULT_IREF;
  vWriteRegBlock (TLC59116_ADDR_ALL, IREF, IREF);
  DEBUG_READ_CTRL();
  
  // Configure les leds en mode brightness
  memset (&ucRegs[LEDOUT0], (LEDRGB_DEFAULT_MODE<<LDR3_0) + (LEDRGB_DEFAULT_MODE<<LDR2_0) +
                            (LEDRGB_DEFAULT_MODE<<LDR1_0) + (LEDRGB_DEFAULT_MODE<<LDR0_0), 4);
  vWriteRegBlock (TLC59116_ADDR_ALL, LEDOUT0, LEDOUT3);
  DEBUG_READ_CTRL();
  
  // Démarrage des oscillateurs
  ucRegs[MODE1] = _BV(ALLCALL);
  vWriteRegBlock (TLC59116_ADDR_ALL, MODE1, MODE1);
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
vLedRgbSetMode  (uint64_t xLed, eLedRgbMode eMode) {
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
  vWriteRegBlock (TLC59116_ADDR_ALL, GRPPWM, GRPPWM);
  
  vReadRegBlock (xRedCtrl[0], MODE2, MODE2);
  ucRegs[MODE2] &= ~_BV(DMBLNK);
  vWriteRegBlock (TLC59116_ADDR_ALL, MODE2, MODE2);
}

// -----------------------------------------------------------------------------
void 
vLedRgbSetGlobalBlinking (uint16_t usPeriod, uint8_t ucDutyCycle) {

  if (usPeriod < 42)
    usPeriod = 42;
    
  ucRegs[GRPPWM]  = ucDutyCycle;
  ucRegs[GRPFREQ] = (uint8_t) MIN((usPeriod * 3) / 125 - 1, 255);
  vWriteRegBlock (TLC59116_ADDR_ALL, GRPPWM, GRPFREQ);
  
  vReadRegBlock (xRedCtrl[0], MODE2, MODE2);
  ucRegs[MODE2] |= _BV(DMBLNK);
  vWriteRegBlock (TLC59116_ADDR_ALL, MODE2, MODE2);
}

// -----------------------------------------------------------------------------
uint64_t 
xLedRgbError (void) {
  uint64_t xError = 0, xCtrlError;
  uint8_t ucCtrlIndex;
  
  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {
  
    xCtrlError = 0;
    vReadRegBlock (xRedCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~(ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);
    vReadRegBlock (xGreenCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~(ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);
    vReadRegBlock (xBlueCtrl[ucCtrlIndex], EFLAG1, EFLAG2);
    xCtrlError |= ~(ucRegs[EFLAG2] << 8 | ucRegs[EFLAG1]);
    
    xError |= (xCtrlError << (16 * ucCtrlIndex));
  }
  
  return xError;
}

// -----------------------------------------------------------------------------
void 
vLedRgbClearError (void) {
  uint8_t ucCtrlIndex;
  
  for (ucCtrlIndex = 0; ucCtrlIndex < NUMBER_OF_CTRL; ucCtrlIndex++) {
  
    vClearErrorCtrl (  xRedCtrl[ucCtrlIndex]);
    vClearErrorCtrl (xGreenCtrl[ucCtrlIndex]);
    vClearErrorCtrl ( xBlueCtrl[ucCtrlIndex]);
  }
}

/* ========================================================================== */
#endif  /* LEDRGB_CTRL == LEDRGB_CTRL_TLC59116 */
#endif /* AVRIO_LEDRGB_ENABLE defined */

/* ========================================================================== */
