/**
 * @file sysio/gpio/gpio.c
 * @brief GPIO
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio_private.h"

/* private variables ======================================================== */
static const char * sGpioNumberingStr[] = {
  "Logical",
  "Mcu",
  "Physical"
};

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Retourne le numéro de la broche en numérotation MCU
int
iGpioMcuPin (int n, xGpio * gp) {

  if (gp->list) {

    if (gp->numbering == eNumberingPhysical) {

      n--;
    }

    if ( (n >= gp->list->first) && (n <= gp->list->last)) {

      return gp->list->pin[n];
    }
  }
  return -1;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iGpioRelease (int p, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioRelease (g, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eGpioMode
eGpioGetMode (int p, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return eArchGpioGetMode (g, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eGpioPull
eGpioGetPull (int p, xGpio * gp) {

  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return eArchGpioGetPull (g, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioSetMode (int p, eGpioMode eMode, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioSetMode (g, eMode, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioSetPull (int p, eGpioPull ePull, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioSetPull (g, ePull, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioWrite (int p, bool bValue, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioWrite (g, bValue, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioRead (int p, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioRead (g, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioToggle (int p, xGpio * gp) {
  int g;

  g = iGpioMcuPin (p, gp);
  if (g >= 0) {

    return iArchGpioToggle (g, gp);
  }
  return -1;
}

// -----------------------------------------------------------------------------
bool
bGpioIsValid (int p, xGpio * gp) {

  return iGpioMcuPin (p, gp) >= 0;
}

// -----------------------------------------------------------------------------
eGpioNumbering
eGpioGetNumbering (const xGpio * gp) {

  return gp->numbering;
}

// -----------------------------------------------------------------------------
int
iGpioSetReleaseOnClose (bool enable, xGpio * gp) {

  gp->roc = enable;
  return 0;
}

// -----------------------------------------------------------------------------
bool
bGpioGetReleaseOnClose (const xGpio * gp) {

  return gp->roc;
}

// -----------------------------------------------------------------------------
bool
bGpioIsOpen (const xGpio * gp) {

  return bIoMapIsOpen (gp->map);
}

// -----------------------------------------------------------------------------
int
iGpioGetSize (const xGpio * gp) {

  if (!bGpioIsOpen (gp)) {

    return -1;
  }
  return gp->list->len;
}

// -----------------------------------------------------------------------------
int64_t
iGpioReadAll (int64_t iMask, xGpio * gp) {

  if (bGpioIsOpen (gp)) {
    int64_t iValue = 0;
    int64_t iBit = 1;
    int p = 0;
    int size = gp->list->size + (gp->numbering == eNumberingPhysical ? 1 : 0);

    if (iMask == 0) {

      iMask = -1;
    }

    while ( (iMask) && (p < size)) {

      if (iMask & iBit) {

        if (bGpioIsValid (p, gp)) {
          switch (iGpioRead (p, gp)) {
            case true:
              iValue |= iBit;
              break;
            case false:
              break;
            default:
              return -1;
          }
        }
        iMask &= ~iBit; // bit lu
      }
      iBit <<= 1;
      p++;
    }
    return iValue;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioWriteAll (int64_t iMask, bool bValue, xGpio * gp) {

  if (bGpioIsOpen (gp)) {
    int p = 0;
    int size = gp->list->size + (gp->numbering == eNumberingPhysical ? 1 : 0);

    while ( (iMask) && (p < size)) {

      if (bGpioIsValid (p, gp)) {
        if ( (iMask & 1) && (eGpioGetMode (p, gp) == eModeOutput)) {

          int ret = iGpioWrite (p, bValue, gp);
          if (ret != 0) {

            return ret;
          }
        }
      }
      p++;
      iMask >>= 1;
    }
    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioToggleAll (int64_t iMask, xGpio * gp) {

  if (bGpioIsOpen (gp)) {
    int p = 0;
    int size = gp->list->size + (gp->numbering == eNumberingPhysical ? 1 : 0);

    while ( (iMask) && (p < size)) {

      if (bGpioIsValid (p, gp)) {

        if ( (iMask & 1) && (eGpioGetMode (p, gp) == eModeOutput)) {

          int ret = iGpioToggle (p, gp);
          if (ret != 0) {

            return ret;
          }
        }
      }
      p++;
      iMask >>= 1;
    }
    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioToFront (xGpio * gp) {

  gp->i = ITERATOR_FRONT;
  return 0;
}

// -----------------------------------------------------------------------------
int
iGpioToBack (xGpio * gp) {

  gp->i = ITERATOR_BACK;
  return 0;
}

// -----------------------------------------------------------------------------
bool
bGpioHasNext (const xGpio * gp) {

  if (gp->list) {

    return gp->i != gp->list->last;
  }
  return false;
}

// -----------------------------------------------------------------------------
bool
bGpioHasPrevious (const xGpio * gp) {

  if (gp->list) {

    return gp->i != gp->list->first;
  }
  return false;
}

// -----------------------------------------------------------------------------
int
iGpioNext (xGpio * gp) {

  if (gp->list) {

    do {

      if (gp->i == ITERATOR_FRONT) {

        gp->i = gp->list->first;
      }
      else {

        gp->i++;
      }

      int p = (gp->numbering == eNumberingPhysical) ? gp->i + 1 : gp->i;

      if (bGpioIsValid (p, gp)) {

        return p;
      }
    }
    while (bGpioHasNext (gp));
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iGpioPrevious (xGpio * gp) {

  if (gp->list) {

    do {

      if (gp->i == ITERATOR_BACK) {

        gp->i = gp->list->last;
      }
      else {

        gp->i--;
      }

      int p = (gp->numbering == eNumberingPhysical) ? gp->i + 1 : gp->i;

      if (bGpioIsValid (p, gp)) {

        return p;
      }
    }
    while (bGpioHasPrevious (gp));
  }
  return -1;
}

// -----------------------------------------------------------------------------
const char *
sGpioNumberingToStr (eGpioNumbering num) {

  if (num > eNumberingPhysical) {

    return "!UNK!";
  }
  return sGpioNumberingStr[num];
}

/* ========================================================================== */
