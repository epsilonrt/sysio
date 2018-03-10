/*
 * arch/arm/rpi/gpio.c
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/delay.h>
#include <sysio/rpi.h>
#include <sysio/log.h>
#include "gpio_private.h"

/* constants ================================================================ */
// Register offsets
#define GFPSEL0     0
#define GFPSEL1     1
#define GFPSEL2     2
#define GFPSEL3     3
#define GFPSEL4     4
#define GFPSEL5     5
// Reserved: 6
#define GPSET0      7
#define GPSET1      8
// Reserved: 9
#define GPCLR0      10
#define GPCLR1      11
// Reserved: 12
#define GPLEV0      13
#define GPLEV1      14
#define GPPUD       37
#define GPPUDCLK0   38

/* TODO:
 * Le BCM2708 a 54 broches mais seule 32 sont accessibles sur le RPi
 * sauf à voir le modèle Compute Module !
 */
#define GPIO_SIZE   32
//#define GPIO_SIZE   54

/* constants ================================================================ */

static const int iLogicalPinsRev1[] = {
  17, 18, 21, 22, 23, 24, 25,  4,
  0,   1,  8,  7, 10,  9, 11, 14,
  15
};

static const int iLogicalPinsRev2[] = {
  17, 18, 27, 22, 23, 24, 25,  4,
  2,  3,  8,  7, 10,  9, 11, 14,
  15, 28, 29, 30, 31
};

static const int iLogicalPinsRev3[] = {
  17, 18, 27, 22, 23, 24, 25,  4,
  2,  3,  8,  7, 10,  9, 11, 14,
  15, -1, -1, -1, -1,  5,  6, 13,
  19, 26, 12, 16, 20, 21,  0,  1
};

static const xPinList pxLogicalPins[] = {
  { .pin = iLogicalPinsRev1, .size = 17, .len = 17, .first = 0, .last = 16 },
  { .pin = iLogicalPinsRev2, .size = 21, .len = 21, .first = 0, .last = 20 },
  { .pin = iLogicalPinsRev3, .size = 32, .len = 28, .first = 0, .last = 31 }
};

static const int iPhysicalPinsRev1[] = {
  -1, -1,  0, -1,  1, -1, 4, 14, -1, 15, 17, 18, 21, -1, 22, 23, -1, 24, 10,
  -1,  9, 25, 11,  8, -1,  7
};

static const int iPhysicalPinsRev2[] = {
  -1, -1,  2, -1,  3, -1, 4, 14, -1, 15, 17, 18, 27, -1, 22, 23, -1, 24, 10,
  -1,  9, 25, 11,  8, -1, 7, /* P5 */ -1, -1, 28, 29, 30, 31, -1, -1,
};

static const int iPhysicalPinsRev3[] = {
  -1, -1,  2, -1,  3, -1, 4, 14, -1, 15, 17, 18, 27, -1, 22, 23, -1, 24, 10,
  -1,  9, 25, 11,  8, -1,  7,
  0,  1,  5, -1,  6, 12, 13, -1, 19, 16, 26, 20, -1, 21
};

static const xPinList pxPhysicalPins[] = {
  { .pin = iPhysicalPinsRev1, .size = 26, .len = 17, .first = 2, .last = 25 },
  { .pin = iPhysicalPinsRev2, .size = 34, .len = 21, .first = 2, .last = 31 },
  { .pin = iPhysicalPinsRev3, .size = 40, .len = 28, .first = 2, .last = 39 }
};

static const int iMcuPinsRev1[] = {
  0, 1, -1, -1, 4, -1, -1, 7, 8, 9, 10, 11, -1, -1, 14, 15, -1, 17, 18, -1, -1,
  21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1
};

static const int iMcuPinsRev2[] = {
  -1, -1, 2, 3, 4, -1, -1, 7, 8, 9, 10, 11, -1, -1, 14, 15, -1, 17, 18, -1, -1,
  -1, 22, 23, 24, 25, -1, 27, 28, 29, 30, 31
};

static const int iMcuPinsRev3[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, -1, -1, -1, -1,
};

static const xPinList pxMcuPins[] = {
  { .pin = iMcuPinsRev1, .size = 32, .len = 17, .first = 0, .last = 25 },
  { .pin = iMcuPinsRev2, .size = 32, .len = 21, .first = 2, .last = 31 },
  { .pin = iMcuPinsRev3, .size = 32, .len = 28, .first = 0, .last = 27 }
};

//------------------------------------------------------------------------------
// 2018-02-18 -->
static const int iPhyConnRev1[] = { 26 };
static const int iPhyConnRev2[] = { 26, 8 }; // 2018-02-18
static const int iPhyConnRev3[] = { 40 }; // 2018-02-18

static const xConnectorList pxPhyConn[] = { // 2018-02-18
  { .size = iPhyConnRev1, .nb = 1 },
  { .size = iPhyConnRev2, .nb = 2 },
  { .size = iPhyConnRev3, .nb = 1 }
};
// <-- 2018-02-18
//------------------------------------------------------------------------------

/* macros =================================================================== */
#define GPIO_BASE(_iobase)  ((unsigned long)(_iobase) + 0x200000)
#define GPIO_INP(g,gp)   *(pIo(gp->map[0], (g)/10)) &= ~(7<<(((g)%10)*3))
#define GPIO_OUT(g,gp)   *(pIo(gp->map[0], (g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_ALT(g,a,gp) *(pIo(gp->map[0], (g)/10)) |=  ((a)<<(((g)%10)*3))
#define GPIO_MODE(g,gp)  (((*(pIo(gp->map[0], (g)/10))) >> (((g)%10)*3)) & 7)

/* private variables ======================================================== */
// Configuration actuelle des broches
static int pinmode[GPIO_SIZE];
// Sauvegarde de la configuration d'origine des broches
static int pinmode_release[GPIO_SIZE];
// Un seul gpio sur RPi !
static xGpio gpio = { .numbering = -1,
                      .pinmode = pinmode,
                      .pinmode_release = pinmode_release
                    };

/* private functions ======================================================== */

#if 0
// -----------------------------------------------------------------------------
static void
vDumpSel (const xGpio * gp) {

  for (int i = GFPSEL0; i <= GFPSEL5; i++) {

    printf ("GFPSEL%d=%08X\n", i, * (pIo (gp->map[0],  i)));
  }
  putchar ('\n');
}
#else
#define vDumpSel()
#endif

// -----------------------------------------------------------------------------
static bool
bIsPwmPin (int g) {
  /*
   * TODO:
   * BCM2708
   *  - PWM0 BCM18 ALT5
   * BCM2709 +
   *  - PWM0 BCM12 ALT0
   *  - PWM1 BCM13 ALT0
   */

  if ( (g == 18) ||
       ( (pxRpiInfo()->eMcu == eRpiMcuBcm2709) && ( (g == 12) || (g == 13)))) {

    return true;
  }
  return false;
}

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
int
iGpioSetNumbering (eGpioNumbering eNum, xGpio * gp) {

  if (eNum != gp->numbering) {
    switch (eNum) {

      case eNumberingLogical:
        gp->list = &pxLogicalPins[pxRpiInfo()->iGpioRev - 1];
        break;

      case eNumberingMcu:
        gp->list = &pxMcuPins[pxRpiInfo()->iGpioRev - 1];
        break;

      case eNumberingPhysical:
        gp->list = &pxPhysicalPins[pxRpiInfo()->iGpioRev - 1];
        break;
    }
    gp->i = gp->j = ITERATOR_FRONT; // ToFront
    gp->numbering = eNum;
  }
  return 0;
}

// -----------------------------------------------------------------------------
xGpio *
xGpioOpen (UNUSED_VAR (void *, setup)) {
  xGpio * gp = &gpio;

  if (!bGpioIsOpen (gp)) {

    if (!pxRpiInfo()) {

      PERROR ("It seems that this system is not a raspberry pi !\n");
      return 0;
    }

    (void) iGpioSetNumbering (eNumberingLogical, gp);

    gp->map[0] = xIoMapOpen (GPIO_BASE (ulRpiIoBase()), BCM270X_BLOCK_SIZE);
    if (gp->map[0]) {

      // Lecture des modes actuels
      for (int g = 0; g < GPIO_SIZE; g++) {

        gp->pinmode_release[g] = -1;
        gp->pinmode[g] = eArchGpioGetMode (g, gp);
      }
      gp->link = 1;
      gp->roc = true;
      return gp;
    }

    // Sortie sur erreur
    return 0;
  }
  gp->link++; // Ajout d'un lien supplémentaire
  return gp;
}

// -----------------------------------------------------------------------------
int
iGpioClose (xGpio * gp) {

  if (bGpioIsOpen (gp)) {

    gp->link--;

    if (gp->link <= 0) { // Fermeture effective lorsque le nombre de liens est 0
      int error = 0;

      if (gp->roc) {

        // restauration config. si roc == true
        for (int g = 0; g < GPIO_SIZE; g++) {

          (void) iArchGpioRelease (g, gp);
        }
      }
      error = iIoMapClose (gp->map[0]);
      memset (gp, 0, sizeof (xGpio));
      gp->numbering = -1;
      gp->pinmode = pinmode;
      gp->pinmode_release = pinmode_release;

      return error;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
const char *
sGpioModeToStr (eGpioMode mode) {

  switch (mode) {
    case eModeInput:
      return "INPUT";
      break;
    case eModeOutput:
      return "OUTPUT";
      break;
    case eModeAlt0:
      return "ALT0";
      break;
    case eModeAlt1:
      return "ALT1";
      break;
    case eModeAlt2:
      return "ALT2";
      break;
    case eModeAlt3:
      return "ALT3";
      break;
    case eModeAlt4:
      return "ALT4";
      break;
    case eModeAlt5:
      return "ALT5";
      break;
    default:
      break;
  }
  return "!UNK!";
}

/* internal private functions =============================================== */

// -----------------------------------------------------------------------------
eGpioMode
eArchGpioGetMode (int g, xGpio * gp) {

  return GPIO_MODE (g, gp);
}

// -----------------------------------------------------------------------------
eGpioPull
eArchGpioGetPull (int g, xGpio * gp) {
  
  PWARNING("Reading of pull resistors is not supported on raspberry.");
  return 0;
}

// -----------------------------------------------------------------------------
int
iArchGpioSetMode (int g, eGpioMode eMode, xGpio * gp) {
  int i = 0;

  switch (eMode) {
    case eModeInput:
      GPIO_INP (g, gp);
      break;

    case eModeOutput:
      GPIO_INP (g, gp);
      GPIO_OUT (g, gp);
      break;

    case eModeAlt0:
    case eModeAlt1:
    case eModeAlt2:
    case eModeAlt3:
    case eModeAlt4:
    case eModeAlt5:
      GPIO_INP (g, gp);
      vDumpSel();
      GPIO_ALT (g, eMode, gp);
      vDumpSel();
      break;

    case eModePwm:
      if (!bIsPwmPin (g)) {

        return -1;
      }
      i = iArchGpioSetMode (g, eModeAlt5, gp);
      break;

    default:
      return -1;
  }
  if (gp->pinmode_release[g] == -1) { // backup mode initial

    gp->pinmode_release[g] = gp->pinmode[g];
  }
  gp->pinmode[g] = eMode;
  return i;
}

// -----------------------------------------------------------------------------
int
iArchGpioSetPull (int g, eGpioPull ePull, xGpio * gp) {
  volatile unsigned int * puPudClk = pIo (gp->map[0], GPPUDCLK0);
  if (g > 31) {

    puPudClk++;
    g -= 32;
  }

  * (pIo (gp->map[0], GPPUD)) = ePull & 3;
  delay_us (10);
  *puPudClk = 1 << g;
  delay_us (10);
  * (pIo (gp->map[0], GPPUD)) = ePullOff;
  *puPudClk = 0;
  return 0;
}

// -----------------------------------------------------------------------------
int
iArchGpioRelease (int g, xGpio * gp) {
  int i = 0;

  if (gp->pinmode_release[g] != -1) {

    i = iArchGpioSetMode (g, gp->pinmode_release[g], gp);
    if ( (i == 0) && (gp->pinmode_release[g] == eModeInput)) {

      i = iArchGpioSetPull (g, ePullDown, gp);
      gp->pinmode_release[g] = -1;
    }
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iArchGpioWrite (int g, bool bValue, xGpio * gp) {
  volatile unsigned int * puReg = pIo (gp->map[0], 0);

  if (bValue) {

    puReg += GPSET0;
  }
  else {

    puReg += GPCLR0;
  }
  if (g > 31) {

    puReg++;
    g -= 32;
  }
  *puReg = 1 << g;
  return 0;
}


// -----------------------------------------------------------------------------
int
iArchGpioRead (int g, xGpio * gp) {
  volatile unsigned int * puReg = pIo (gp->map[0], GPLEV0);
  if (g > 31) {

    puReg++;
    g -= 32;
  }

  int iValue = ( (*puReg & (1 << g)) != 0) ? true : false;
  return iValue;
}

// -----------------------------------------------------------------------------
int
iArchGpioToggle (int g, xGpio * gp) {
  int iValue = iArchGpioRead (g, gp);

  if (iValue < 0) {

    return -1;
  }

  if (iValue) {

    return iArchGpioWrite (g, false, gp);
  }
  return iArchGpioWrite (g, true, gp);
}

// -----------------------------------------------------------------------------
const xConnectorList *
pxArchGpioGetConnSize (xGpio * gp) {

  return &pxPhyConn[pxRpiInfo()->iGpioRev - 1];
}

/* ========================================================================== */
