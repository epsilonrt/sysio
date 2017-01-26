/*
 * gpio.c
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/delay.h>
#include <sysio/iomap.h>
#include <sysio/gpio.h>
#include <sysio/rpi.h>
#include <sysio/log.h>

/* constants ================================================================ */
#define ITERATOR_FRONT -1
#define ITERATOR_BACK  -2

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
#define GPIO_SIZE   32

/* TODO:
 * Le BCM2708 a 54 broches mais seule 32 sont accessibles sur le RPi
 * sauf à voir le modèle Compute Module !
 */
//#define GPIO_SIZE   54

/* structures =============================================================== */
typedef struct xPinList {
  const int * pin;
  int size; // Taille du tableau pointé par pin
  int len;  // Nombre de broches valides
  int first;// Premier numéro valide
  int last; // Dernier numéro valide
} xPinList;

typedef struct xGpio {
  xIoMap * map;
  const xPinList * list;
  int i; // iterateur utilisateur
  int j; // iterateur interne
  int pinmode[GPIO_SIZE];
  int pinmode_release[GPIO_SIZE];
  int link;
  bool roc; // release on close
  eGpioNumbering numbering;
} xGpio;


/* macros =================================================================== */
#define GPIO_BASE(_iobase)  ((unsigned long)(_iobase) + 0x200000)

#define GPIO_INP(g)   *(pIo(gpio.map, (g)/10)) &= ~(7<<(((g)%10)*3))
#define GPIO_OUT(g)   *(pIo(gpio.map, (g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_ALT(g,a) *(pIo(gpio.map, (g)/10)) |=  ((a)<<(((g)%10)*3))
#define GPIO_MODE(g)  (((*(pIo(gpio.map, (g)/10))) >> (((g)%10)*3)) & 7)

/* private variables ======================================================== */
static xGpio gpio = { .numbering = -1 }; // Un seul gpio sur RPi !

static const int iLogicalPinsRev1[] = {
  17, 18, 21, 22, 23, 24, 25,  4,  0, 1, 8, 7, 10, 9, 11, 14, 15
};

/*        XX                       X  X                                       */
static const int iLogicalPinsRev2[] = {
  17, 18, 27, 22, 23, 24, 25,  4,  2, 3, 8, 7, 10, 9, 11, 14, 15, 28, 29, 30, 31
};

static const int iLogicalPinsRev3[] = {
  17, 18, 27, 22, 23, 24, 25,  4,  2, 3, 8, 7, 10, 9, 11, 14, 15, -1, -1, -1, -1,
  5,  6, 13, 19, 26, 12, 16, 20, 21, 0, 1
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

/* private functions ======================================================== */

#if 0
// -----------------------------------------------------------------------------
static void
vDumpSel (void) {

  for (int i = GFPSEL0; i <= GFPSEL5; i++) {

    printf ("GFPSEL%d=%08X\n", i, * (pIo (gpio.map,  i) ) );
  }
  putchar ('\n');
}
#else
#define vDumpSel()
#endif

// -----------------------------------------------------------------------------
static bool
bIsOpen (void) {

  return bIoMapIsOpen (gpio.map);
}

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


  if ( (g == 18) || ( (pxRpiInfo()->eMcu == eRpiMcuBcm2709) && ( (g == 12) || (g == 13) ) ) ) {

    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
static int
iSetMode (int g, eGpioMode eMode) {
  int i = 0;

  if (g < 0) {

    return -1;
  }

  switch (eMode) {
    case eModeInput:
      GPIO_INP (g);
      break;
    case eModeOutput:
      GPIO_INP (g);
      GPIO_OUT (g);
      break;
    case eModeAlt0:
    case eModeAlt1:
    case eModeAlt2:
    case eModeAlt3:
    case eModeAlt4:
    case eModeAlt5:
      vDumpSel();
      GPIO_INP (g);
      vDumpSel();
      GPIO_ALT (g, eMode);
      vDumpSel();
      break;
    case eModePwm:
      if (!bIsPwmPin (g) ) {

        return -1;
      }
      i = iSetMode (g, eModeAlt5);
      break;
    default:
      return -1;
  }
  gpio.pinmode_release[g] = gpio.pinmode[g];
  gpio.pinmode[g] = eMode;
  return i;
}

// -----------------------------------------------------------------------------
static int
iSetPull (int g, eGpioPull ePull) {

  if (g < 0) {

    return -1;
  }
  volatile unsigned int * puPudClk = pIo (gpio.map, GPPUDCLK0);
  if (g > 31) {

    puPudClk++;
    g -= 32;
  }

  * (pIo (gpio.map, GPPUD) ) = ePull & 3;
  delay_us (10);
  *puPudClk = 1 << g;
  delay_us (10);
  * (pIo (gpio.map, GPPUD) ) = ePullOff;
  *puPudClk = 0;
  return 0;
}

// -----------------------------------------------------------------------------
static int
iWrite (int g, bool bValue) {

  if (g < 0) {

    return -1;
  }
  volatile unsigned int * puReg = pIo (gpio.map, 0);

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
static int
iRead (int g) {

  if (g < 0) {

    return -1;
  }

  volatile unsigned int * puReg = pIo (gpio.map, GPLEV0);
  if (g > 31) {

    puReg++;
    g -= 32;
  }

  int iValue = ( (*puReg & (1 << g) ) != 0) ? true : false;
  return iValue;
}

// -----------------------------------------------------------------------------
static eGpioMode
eGetMode (int g) {

  if (g < 0) {

    return -1;
  }

  return GPIO_MODE (g);
}

// -----------------------------------------------------------------------------
static int
iRelease (int g) {

  if (g >= 0) {
    int i = 0;

    if (gpio.pinmode_release[g] != -1) {

      i = iSetMode (g, gpio.pinmode_release[g]);
      if ( (i == 0) && (gpio.pinmode_release[g] == eModeInput) ) {
        i = iSetPull (g, ePullDown);
        gpio.pinmode_release[g] = -1;
      }
    }
    return i;
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iToggle (int g) {
  int iValue = iRead (g);

  if (iValue < 0) {

    return -1;
  }

  if (iValue) {

    return iWrite (g, false);
  }
  return iWrite (g, true);
}

// -----------------------------------------------------------------------------
static void
vSetNumbering (eGpioNumbering eNum) {
  if (eNum != gpio.numbering) {
    switch (eNum) {

      case eNumberingLogical:
        gpio.list = &pxLogicalPins[pxRpiInfo()->iGpioRev - 1];
        break;

      case eNumberingMcu:
        gpio.list = &pxMcuPins[pxRpiInfo()->iGpioRev - 1];
        break;

      case eNumberingPhysical:
        gpio.list = &pxPhysicalPins[pxRpiInfo()->iGpioRev - 1];
        break;
    }
    gpio.i = gpio.j = ITERATOR_FRONT; // ToFront
    gpio.numbering = eNum;
  }
}

// -----------------------------------------------------------------------------
static int
iMcuPin (int n) {

  if (gpio.list) {

    if (gpio.numbering == eNumberingPhysical) {

      n--;
    }
    if ( (n >= gpio.list->first) && (n <= gpio.list->last) ) {
      return gpio.list->pin[n];
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iToFront (void) {

  gpio.i = ITERATOR_FRONT;
  return 0;
}

// -----------------------------------------------------------------------------
static int
iToBack (void) {

  gpio.i = ITERATOR_BACK;
  return 0;
}

// -----------------------------------------------------------------------------
static bool
bHasNext (void) {

  if (gpio.list) {
    return gpio.i != gpio.list->last;
  }
  return false;
}

// -----------------------------------------------------------------------------
static bool
bHasPrevious (void) {

  if (gpio.list) {
    return gpio.i != gpio.list->first;
  }
  return false;
}
#define ITERATOR_FRONT -1
#define ITERATOR_BACK  -2

// -----------------------------------------------------------------------------
static int
iNext (void) {

  if (gpio.list) {

    do {

      if (gpio.i == ITERATOR_FRONT) {

        gpio.i = gpio.list->first;
      }
      else {

        gpio.i++;
      }

      int p = (gpio.numbering == eNumberingPhysical) ? gpio.i + 1 : gpio.i;

      if (iMcuPin (p) >= 0) {

        return p;
      }
    }
    while (bHasNext() );
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iPrevious (void) {

  if (gpio.list) {

    do {

      if (gpio.i == ITERATOR_BACK) {

        gpio.i = gpio.list->last;
      }
      else {

        gpio.i--;
      }

      int p = (gpio.numbering == eNumberingPhysical) ? gpio.i + 1 : gpio.i;

      if (iMcuPin (p) >= 0) {

        return p;
      }
    }
    while (bHasPrevious() );
  }
  return -1;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xGpio *
xGpioOpen (UNUSED_VAR (void *, setup) ) {

  if (!bIsOpen() ) {

    if (!pxRpiInfo() ) {

      PERROR ("It seems that this system is not a raspberry pi !\n");
      return 0;
    }

    vSetNumbering (eNumberingLogical);

    gpio.map = xIoMapOpen (GPIO_BASE (ulRpiIoBase() ), BCM270X_BLOCK_SIZE);
    if (gpio.map) {

      // Lecture des modes actuels
      for (int g = 0; g < GPIO_SIZE; g++) {

        gpio.pinmode_release[g] = -1;
        gpio.pinmode[g] = GPIO_MODE (g);
      }
      gpio.link = 1;
      gpio.roc = true;
      return &gpio;
    }
    // Sortie sur erreur
    return 0;
  }
  gpio.link++; // Ajout d'un lien supplémentaire
  return &gpio;
}

// -----------------------------------------------------------------------------
int
iGpioClose (UNUSED_VAR (xGpio *, unused) ) {

  if (bIsOpen() ) {
    gpio.link--;
    if (gpio.link <= 0) {
      int error = 0;

      if (gpio.roc) {
        // Fermeture effective lorsque le nombre  de liens est 0, si roc true
        for (int g = 0; g < GPIO_SIZE; g++) {
          // Release all used pins
          (void) iRelease (g);
        }
      }
      error = iIoMapClose (gpio.map);
      memset (&gpio, 0, sizeof (xGpio) );
      gpio.numbering = -1;
      return error;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iGpioSetNumbering (eGpioNumbering eNum, UNUSED_VAR (xGpio *, unused) ) {

  vSetNumbering (eNum);
  return 0;
}

// -----------------------------------------------------------------------------
eGpioNumbering
eGpioGetNumbering (UNUSED_VAR (xGpio *, unused) ) {

  return gpio.numbering;
}

// -----------------------------------------------------------------------------
int
iGpioSetReleaseOnClose (bool enable, UNUSED_VAR (xGpio *, unused) ) {

  gpio.roc = enable;
  return 0;
}

// -----------------------------------------------------------------------------
bool
bGpioGetReleaseOnClose (UNUSED_VAR (xGpio *, unused) ) {

  return gpio.roc;
}

// -----------------------------------------------------------------------------
bool
bGpioIsOpen (UNUSED_VAR (xGpio *, unused) ) {

  return bIsOpen();
}

// -----------------------------------------------------------------------------
int
iGpioGetSize (UNUSED_VAR (xGpio *, unused) ) {

  if (!bIsOpen() ) {
    return -1;
  }
  return gpio.list->len;
}

// -----------------------------------------------------------------------------
int
iGpioToFront (UNUSED_VAR (xGpio *, unused) ) {

  return iToFront();
}

// -----------------------------------------------------------------------------
int
iGpioToBack (UNUSED_VAR (xGpio *, unused) ) {

  return iToBack();
}

// -----------------------------------------------------------------------------
bool
bGpioHasNext (UNUSED_VAR (xGpio *, unused) ) {

  return bHasNext();
}

// -----------------------------------------------------------------------------
bool
bGpioHasPrevious (UNUSED_VAR (xGpio *, unused) ) {

  return bHasPrevious();
}

// -----------------------------------------------------------------------------
int
iGpioNext (UNUSED_VAR (xGpio *, unused) ) {

  return iNext();
}

// -----------------------------------------------------------------------------
int
iGpioPrevious (UNUSED_VAR (xGpio *, unused) ) {

  return iPrevious();
}

// -----------------------------------------------------------------------------
int
iGpioSetMode (int p, eGpioMode eMode, UNUSED_VAR (xGpio *, unused) ) {

  return iSetMode (iMcuPin (p), eMode);
}

// -----------------------------------------------------------------------------
eGpioMode
eGpioGetMode (int p, UNUSED_VAR (xGpio *, unused) ) {

  return eGetMode (iMcuPin (p) );
}

// -----------------------------------------------------------------------------
int
iGpioRelease (int p, UNUSED_VAR (xGpio *, unused) ) {

  return iRelease (iMcuPin (p) );
}

// -----------------------------------------------------------------------------
int
iGpioSetPull (int p, eGpioPull ePull, UNUSED_VAR (xGpio *, unused) ) {

  return iSetPull (iMcuPin (p), ePull);
}

// -----------------------------------------------------------------------------
int
iGpioWrite (int p, bool bValue, UNUSED_VAR (xGpio *, unused) ) {

  return iWrite (iMcuPin (p), bValue);;
}

// -----------------------------------------------------------------------------
int
iGpioRead (int p, UNUSED_VAR (xGpio *, unused) ) {

  return iRead (iMcuPin (p) );
}

// -----------------------------------------------------------------------------
int
iGpioToggle (int p, UNUSED_VAR (xGpio *, unused) ) {

  return iToggle (iMcuPin (p) );
}

// -----------------------------------------------------------------------------
bool
bGpioIsValid (int p, UNUSED_VAR (xGpio *, unused) ) {

  return iMcuPin (p) >= 0;
}

// -----------------------------------------------------------------------------
int64_t
iGpioReadAll (int64_t iMask, UNUSED_VAR (xGpio *, unused) ) {

  if (bIsOpen() ) {
    int64_t iValue = 0;
    int64_t iBit = 1;
    int p = 0;
    int size = gpio.list->size + (gpio.numbering == eNumberingPhysical ? 1 : 0);

    if (iMask == 0) {
      iMask = -1;
    }

    while ( (iMask) && (p < size) ) {

      if (iMask & iBit) {

        int g = iMcuPin (p);
        if (g >= 0) {
          switch (iRead (g) ) {
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
iGpioWriteAll (int64_t iMask, bool bValue, UNUSED_VAR (xGpio *, unused) ) {

  if (bIsOpen() ) {
    int p = 0;
    int size = gpio.list->size + (gpio.numbering == eNumberingPhysical ? 1 : 0);

    while ( (iMask) && (p < size) ) {

      int g = iMcuPin (p);
      if (g >= 0) {
        if ( (iMask & 1) && (gpio.pinmode[g] == eModeOutput) ) {

          (void) iWrite (g, bValue);
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
iGpioToggleAll (int64_t iMask, UNUSED_VAR (xGpio *, unused) ) {

  if (bIsOpen() ) {
    int p = 0;
    int size = gpio.list->size + (gpio.numbering == eNumberingPhysical ? 1 : 0);

    while ( (iMask) && (p < size) ) {

      int g = iMcuPin (p);
      if (g >= 0) {
        if ( (iMask & 1) && (gpio.pinmode[g] == eModeOutput) ) {

          (void) iToggle (g);
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

/* ========================================================================== */
