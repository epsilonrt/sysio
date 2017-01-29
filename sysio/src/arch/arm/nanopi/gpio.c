/*
 * arch/arm/nanopi/gpio.c
 * @brief
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysio/delay.h>
#include <sysio/nanopi.h>
#include <sysio/log.h>
#include "gpio_private.h"

/* structures =============================================================== */
typedef struct xBank {
  uint32_t CFG[4];
  uint32_t DAT;
  uint32_t DRV[2];
  uint32_t PUL[2];
} __attribute__ ( (__packed__))  xBank;

/* constants ================================================================ */
#define PIO_BASE (0x800)
#define GPIO_SIZE   94

/* constants ================================================================ */
static const int iLogicalPinsRev1[] = {
  0, 6, 2, 3, 88, 89, 1, 91, 12, 11, 25, -1, 22, 23, 24, 86, 87
};

static const int iLogicalPinsRev2[] = {
  0, 6, 2, 3, 88, 89, 1, 91, 12, 11, 25, 17, 22, 23, 24, 86, 87,
  20, 21, 8, 16, 9, 7, 13, 15, 14, 19, 18
};

static const xPinList pxLogicalPins[] = {
  { .pin = iLogicalPinsRev1, .size = 17, .len = 16, .first = 0, .last = 16 },
  { .pin = iLogicalPinsRev2, .size = 28, .len = 28, .first = 0, .last = 27 },
};

static const int iPhysicalPinsRev1[] = {
  -1, -1, 12, -1, 11, -1, 91, 86, -1, 87, 0, 6, 2, -1, 3, 88, -1, 89, 22, -1, 23, 1, 24, 25
};

static const int iPhysicalPinsRev2[] = {
  -1, -1, 12, -1, 11, -1, 91, 86, -1, 87, 0, 6, 2, -1, 3, 88, -1, 89, 22, -1, 23,  1, 24, 25,
  -1, 17, 19, 18, 20, -1, 21, 7, 8, -1, 16, 13, 9, 15, -1, 14
};

static const xPinList pxPhysicalPins[] = {
  { .pin = iPhysicalPinsRev1, .size = 24, .len = 16, .first = 2, .last = 23 },
  { .pin = iPhysicalPinsRev2, .size = 40, .len = 28, .first = 2, .last = 39 },
};

static const int iMcuPinsRev1[] = {
  0, 1, 2, 3, -1, -1, 6, -1, -1, -1, -1, 11, 12, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 86, 87, 88, 89, -1, 91, -1, -1
};

static const int iMcuPinsRev2[] = {
  0, 1, 2, 3, -1, -1, 6, 7, 8, 9, -1, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 86, 87, 88, 89, -1, 91, -1, -1
};

static const xPinList pxMcuPins[] = {
  { .pin = iMcuPinsRev1, .size = 94, .len = 16, .first = 0, .last = 91 },
  { .pin = iMcuPinsRev2, .size = 94, .len = 28, .first = 0, .last = 91 },
};

/* private variables ======================================================== */
// Configuration actuelle des broches
static int pinmode[GPIO_SIZE];
static int pinpull[GPIO_SIZE];
// Sauvegarde de la configuration d'origine des broches
static int pinmode_release[GPIO_SIZE];
static int pinpull_release[GPIO_SIZE];
// Un seul gpio sur NanoPi !
static xGpio gpio = { .numbering = -1,
                      .pinmode = pinmode,
                      .pinmode_release = pinmode_release,
                      .pinpull = pinpull,
                      .pinpull_release = pinpull_release
                    };

/* private functions ======================================================== */

#if 0
// -----------------------------------------------------------------------------
static void
vPrintBank (const xBank * b) {

  printf ("\nCFG0=0x%08X - CFG1=0x%08X\n", b->CFG[0], b->CFG[1]);
  printf ("CFG2=0x%08X - CFG3=0x%08X\n", b->CFG[2], b->CFG[3]);
  printf ("DAT =0x%08X\n", b->DAT);
  printf ("DRV0=0x%08X - DRV1=0x%08X\n", b->DRV[0], b->DRV[1]);
  printf ("PUL0=0x%08X - PUL1=0x%08X\n\n", b->PUL[0], b->PUL[1]);
}
#else
#define vPrintBank(b)
#endif

// -----------------------------------------------------------------------------
static xBank *
pxGetBank (int bkindex, const xGpio * gp) {
  xBank * bank = NULL;

  if (bkindex < 7) {

    bank = (xBank *) pIo (gp->map, (PIO_BASE + bkindex * 0x24) >> 2);
  }
  return bank;
}

#if 0
// -----------------------------------------------------------------------------
static void
vPrintAllBanks (const xGpio * gp) {

  for (int i = 0; i < 7; i++) {

    if (i != 1) {
      xBank *b = pxGetBank (i, gp);
      printf ("\n---Port %c---\n", 'A' + i);
      vPrintBank (b);
    }
  }
}
#else
#define vPrintAllBanks(g)
#endif

// -----------------------------------------------------------------------------
static xBank *
pxGetPinBank (int * mcupin, const xGpio * gp) {
  const int list[] = { 22, 0, 17, 18, 16, 7, 14, -1 };
  const int *p = list;
  int bkindex = 0;
  int ng = *mcupin;
  xBank * bank = NULL;

  while ( (*p >= 0) && (ng >= *p)) {

    ng -= *p;
    do {
      bkindex++;
      p++;
    }
    while (bkindex == 1);   // saute PortB
  }

  if (bkindex < 7) {

    *mcupin = ng;
    bank = pxGetBank (bkindex, gp);
  }

  return bank;
}

// -----------------------------------------------------------------------------
static bool
bIsPwmPin (int g) {

  return g == 6; // PWM1/GPIO_A6
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iGpioSetNumbering (eGpioNumbering eNum, xGpio * gp) {

  if (eNum != gp->numbering) {
    switch (eNum) {

      case eNumberingLogical:
        gp->list = &pxLogicalPins[pxNanoPiInfo()->iGpioRev - 1];
        break;

      case eNumberingMcu:
        gp->list = &pxMcuPins[pxNanoPiInfo()->iGpioRev - 1];
        break;

      case eNumberingPhysical:
        gp->list = &pxPhysicalPins[pxNanoPiInfo()->iGpioRev - 1];
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

    if (!pxNanoPiInfo()) {

      PERROR ("It seems that this system is not a NanoPi board !\n");
      return 0;
    }

    (void) iGpioSetNumbering (eNumberingLogical, gp);

    gp->map = xIoMapOpen (ulNanoPiIoBase(), H3_BLOCK_SIZE);
    if (gp->map) {

      // Lecture des modes actuels
      for (int g = 0; g < GPIO_SIZE; g++) {

        gp->pinmode_release[g] = -1;
        gp->pinmode[g] = eArchGpioGetMode (g, gp);
        gp->pinpull_release[g] = -1;
        gp->pinpull[g] = eArchGpioGetPull (g, gp);
      }
      gp->link = 1;
      gp->roc = true;

      vPrintAllBanks (gp);
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
      vPrintAllBanks (gp);
      error = iIoMapClose (gp->map);
      memset (gp, 0, sizeof (xGpio));
      gp->numbering = -1;
      gp->pinmode = pinmode;
      gp->pinmode_release = pinmode_release;
      gp->pinpull = pinpull;
      gp->pinpull_release = pinpull_release;

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
      return "INP";
      break;
    case eModeOutput:
      return "OUT";
      break;
    case eModeFunc2:
      return "FC2";
      break;
    case eModeFunc3:
      return "FC3";
      break;
    case eModeFunc4:
      return "FC4";
      break;
    case eModeFunc5:
      return "FC5";
      break;
    case eModeFunc6:
      return "FC6";
      break;
    case eModeDisabled:
      return "DIS";
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
  xBank * b;

  b = pxGetPinBank (&g, gp);
  if (b) {

    int i, r;

    r = g >> 3;
    i = (g - (r * 8)) * 4;
    return b->CFG[r] >> i & 7;
  }

  return -1;
}

// -----------------------------------------------------------------------------
eGpioPull
eArchGpioGetPull (int g, xGpio * gp) {
  xBank * b;

  b = pxGetPinBank (&g, gp);
  if (b) {
    eGpioPull p;
    int i, r;

    r = g >> 4;
    i = (g - (r * 16)) * 2;
    p = (eGpioPull) (b->PUL[r] >> i & 3);
    if (p) {

      p ^= 3; // Pull-up et Pull-down inversé par rapport au codage eGpioPull
    }
    return p;
  }

  return -1;
}

// -----------------------------------------------------------------------------
int
iArchGpioSetMode (int g, eGpioMode eMode, xGpio * gp) {
  xBank * b;
  int f = g;

  b = pxGetPinBank (&f, gp);
  if (b) {
    int i, r;

    r = f >> 3;
    i = (f - (r * 8)) * 4;

    switch (eMode) {
      case eModeInput:
        b->CFG[r] &= ~ (0b1111 << i);
        break;

      case eModeOutput:
      case eModeFunc2:
      case eModeFunc3:
      case eModeFunc4:
      case eModeFunc5:
      case eModeFunc6:
      case eModeDisabled:
        b->CFG[r] &= ~ (0b1111 << i); // clear config
        b->CFG[r] |= (eMode << i);
        break;

      case eModePwm:
        if (!bIsPwmPin (g)) {

          return -1;
        }
        b->CFG[r] &= ~ (0b1111 << i); // clear config
        b->CFG[r] |= (eModeFunc3 << i); // TODO
        eMode = eModeFunc3;
        break;

      default:
        return -1;
    }

    if (gp->pinmode_release[g] == -1) { // backup mode initial

      gp->pinmode_release[g] = gp->pinmode[g];
    }
    gp->pinmode[g] = eMode;
    return 0;
  }

  return -1;
}

// -----------------------------------------------------------------------------
int
iArchGpioSetPull (int g, eGpioPull ePull, xGpio * gp) {
  xBank * b;
  int f = g;

  b = pxGetPinBank (&f, gp);
  if (b) {
    int i, r;

    if (gp->pinpull_release[g] == -1) { // backup mode initial

      gp->pinpull_release[g] = gp->pinpull[g];
    }
    gp->pinpull[g] = ePull;
    if (ePull) {

      ePull ^= 3; // Pull-up et Pull-down inversé par rapport au codage eGpioPull
    }

    r = f >> 4;
    i = (f - (r * 16)) * 2;
    b->PUL[r] &= ~ (0b11 << i); // clear config -> disable
    delay_us (10);
    b->PUL[r] |= (ePull << i);
    delay_us (10);

    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iArchGpioRelease (int g, xGpio * gp) {
  int i = 0;

  if (gp->pinmode_release[g] != -1) {

    i = iArchGpioSetMode (g, gp->pinmode_release[g], gp);
    gp->pinmode_release[g] = -1;
  }

  if ( (i == 0) && (gp->pinpull_release[g] != -1)) {

    i = iArchGpioSetPull (g, gp->pinpull_release[g], gp);
    gp->pinpull_release[g] = -1;
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iArchGpioWrite (int g, bool bValue, xGpio * gp) {
  xBank * b;

  b = pxGetPinBank (&g, gp);
  if (b) {

    if (bValue) {

      b->DAT |= 1 << g;;
    }
    else {

      b->DAT &= ~ (1 << g);
    }
    return 0;
  }

  return -1;
}

// -----------------------------------------------------------------------------
int
iArchGpioRead (int g, xGpio * gp) {
  xBank * b;

  b = pxGetPinBank (&g, gp);
  if (b) {

    return b->DAT & (1 << g) ? true : false;
  }

  return -1;
}

// -----------------------------------------------------------------------------
int
iArchGpioToggle (int g, xGpio * gp) {
  xBank * b;

  b = pxGetPinBank (&g, gp);
  if (b) {

    b->DAT ^= 1 << g;;
    return 0;
  }

  return -1;
}

/* ========================================================================== */
