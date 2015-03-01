/*
 * gpio.c
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysio/delay.h>
#include <sysio/iomap.h>
#include <sysio/gpio.h>
#include <sysio/rpi.h>

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

/* structures =============================================================== */
typedef struct xGpio {
  xIoMap * map;
  const int * pin;
  int size;
  int * pinmode;
  int * pinmode_release;
  int link;
  bool roc; // release on close
} xGpio;

/* macros =================================================================== */
#define GPIO_INP(g)   *(pIo(gpio.map, (g)/10)) &= ~(7<<(((g)%10)*3))
#define GPIO_OUT(g)   *(pIo(gpio.map, (g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_ALT(g,a) *(pIo(gpio.map, (g)/10)) |=  ((a)<<(((g)%10)*3))
#define GPIO_MODE(g)  (((*(pIo(gpio.map, (g)/10))) >> (((g)%10)*3)) & 7)

/* private variables ========================================================= */
static xGpio gpio;

static const int iPinsPcbRev1[] =
  {17,18,21,22,23,24,25,4,0,1,8,7,10,9,11,15};
static const int iPinsPcbRev2[] =
  {17,18,27,22,23,24,25,4,2,3,8,7,10,9,11,14,16,28,29,30,31};

/* private functions ======================================================== */

#if 0
// -----------------------------------------------------------------------------
static void
vDumpSel(void) {

  for (int i = GFPSEL0; i <= GFPSEL5; i++) {

    printf ("GFPSEL%d=%08X\n", i, *(pIo (gpio.map,  i)));
  }
  putchar('\n');
}
#else
#define vDumpSel()
#endif

// -----------------------------------------------------------------------------
static bool
bIsPwmPin (int p) {

  return (p == GPIO_GEN1);
}

// -----------------------------------------------------------------------------
static bool
bIsOpen (void) {

  return bIoMapIsOpen (gpio.map);
}

// -----------------------------------------------------------------------------
static int
iSetMode (int p, eGpioMode eMode) {

  if ((!bIsOpen()) || (p >= gpio.size)){
    return -1;
  }
  int i = 0;
  int g = gpio.pin[p];

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
      if (!bIsPwmPin(p)) {

        return -1;
      }
      i = iSetMode (p, eModeAlt5);
      break;
    default:
      return -1;
  }
  gpio.pinmode_release[p] = gpio.pinmode[p];
  gpio.pinmode[p] = eMode;
  return i;
}

// -----------------------------------------------------------------------------
static int
iSetPull (int p, eGpioPull ePull) {

  if ((!bIsOpen()) || (p >= gpio.size)){
    return -1;
  }

  int g = gpio.pin[p];
  volatile unsigned int * puPudClk = pIo (gpio.map, GPPUDCLK0);
  if (g > 31) {

    puPudClk++;
    g -= 32;
  }

  *(pIo (gpio.map, GPPUD)) = ePull & 3;
  delay_us (10);
  *puPudClk = 1 << g;
  delay_us (10);
  *(pIo (gpio.map, GPPUD)) = ePullOff;
  *puPudClk = 0;
  return 0;
}

// -----------------------------------------------------------------------------
static int
iRelease (int p) {
  int i = -1;

  if ((bIsOpen()) && (p < gpio.size)){

    i = 0;
    if (gpio.pinmode_release[p] != -1) {

      i = iSetMode (p, gpio.pinmode_release[p]);
      if ((i == 0) && (gpio.pinmode_release[p] == eModeInput)) {
        i = iSetPull (p, ePullDown);
        gpio.pinmode_release[p] = -1;
      }
    }
  }
  return i;
}

// -----------------------------------------------------------------------------
static int
iWrite (int p, bool bValue) {

  if ((!bIsOpen()) || (p >= gpio.size)){

    return -1;
  }

  int g = gpio.pin[p];
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
iRead (int p) {

  if ((!bIsOpen()) || (p >= gpio.size)){

    return -1;
  }
  int g = gpio.pin[p];
  volatile unsigned int * puReg = pIo (gpio.map, GPLEV0);
  if (g > 31) {

    puReg++;
    g -= 32;
  }

  int iValue = ((*puReg & (1 << g)) != 0) ? true : false;
  return iValue;
}

// -----------------------------------------------------------------------------
static int
iToggle (int p) {
  int iValue = iRead(p);

  if (iValue < 0){

    return -1;
  }

  if (iValue) {

    return iWrite (p, false);
  }
  return iWrite (p, true);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xGpio *
xGpioOpen (UNUSED_VAR(void *, setup)) {

  if (!bIsOpen()) {
    int iRev = iRpiRev();

    if (iRev <= 0) {

      PDEBUG ("It seems that this system is not a raspberry pi !\n");
      return 0;
    }

    if (iRev <= 3) {

      gpio.pin = iPinsPcbRev1;
      gpio.size = sizeof(iPinsPcbRev1) / sizeof(int);
    }
    else {

      gpio.pin = iPinsPcbRev2;
      gpio.size = sizeof(iPinsPcbRev2) / sizeof(int);
    }

    gpio.map = xIoMapOpen (BCM2708_GPIO_BASE, BCM2708_BLOCK_SIZE);
    if (gpio.map) {

      gpio.pinmode = malloc (gpio.size * sizeof(int));
      gpio.pinmode_release = malloc (gpio.size * sizeof(int));
      if ((gpio.pinmode) && (gpio.pinmode_release)) {

        memset (gpio.pinmode_release, -1, gpio.size * sizeof(int));

        // Lecture des modes actuels
        for (int p = 0; p < gpio.size; p++) {

          gpio.pinmode[p] = GPIO_MODE(gpio.pin[p]);
        }
        gpio.link = 1;
        gpio.roc = true;
        return &gpio;
      }
      (void) iIoMapClose (gpio.map);
    }
    // Sortie sur erreur
    return 0;
  }
  gpio.link++; // Ajout d'un lien supplémentaire
  return &gpio;
}

// -----------------------------------------------------------------------------
int
iGpioClose (UNUSED_VAR(xGpio *, unused)) {

  if (bIsOpen()) {
    gpio.link--;
    if (gpio.link <= 0) {
      int error = 0;

      if (gpio.roc) {
        // Fermeture effective lorsque le nombre  de liens est 0, si roc true
        for (int i = 0; i < gpio.size; i++) {
          // Release all used pins
            (void)iRelease (i);
        }
      }
      error = iIoMapClose (gpio.map);
      free (gpio.pinmode);
      memset (&gpio, 0, sizeof (xGpio));
      return error;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iGpioSetReleaseOnClose (bool enable, UNUSED_VAR(xGpio *, unused)) {
  gpio.roc = enable;
  return 0;
}

// -----------------------------------------------------------------------------
bool
bGpioReleaseOnClose (UNUSED_VAR(xGpio *, unused)) {
  return gpio.roc;
}

// -----------------------------------------------------------------------------
bool
bGpioIsOpen (UNUSED_VAR(xGpio *, unused)) {

  return bIsOpen();
}

// -----------------------------------------------------------------------------
int
iGpioMode (int p, eGpioMode eMode, UNUSED_VAR(xGpio *, unused)) {

  return iSetMode (p, eMode);
}

// -----------------------------------------------------------------------------
int
iGpioRelease (int p, UNUSED_VAR(xGpio *, unused)) {

  return iRelease (p);
}

// -----------------------------------------------------------------------------
int
iGpioPull (int p, eGpioPull ePull, UNUSED_VAR(xGpio *, unused)) {

  return iSetPull (p, ePull);
}

// -----------------------------------------------------------------------------
int
iGpioWrite (int p, bool bValue, UNUSED_VAR(xGpio *, unused)) {

  return iWrite (p, bValue);;
}

// -----------------------------------------------------------------------------
int
iGpioRead (int p, UNUSED_VAR(xGpio *, unused)) {

  return iRead (p);
}

// -----------------------------------------------------------------------------
int
iGpioToggle (int p, UNUSED_VAR(xGpio *, unused)) {

  return iToggle (p);
}

// -----------------------------------------------------------------------------
int
iGpioReadAll (int iMask, UNUSED_VAR(xGpio *, unused)) {

  if (bIsOpen()) {
    int iValue = 0;
    int iBit = 1;
    int p = 0;

    if (iMask == 0) {
      iMask = -1;
    }
    while ((iMask) && (p < gpio.size)) {

      if (iMask & iBit) {
        switch (iRead (p)) {
          case true:
            iValue |= iBit;
            break;
          case false:
            break;
          default:
            return -1;
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
iGpioWriteAll (int iMask, bool bValue, UNUSED_VAR(xGpio *, unused)) {

  if (bIsOpen()) {
    int p = 0;

    while ((iMask) && (p < gpio.size)) {

      if ((iMask & 1) && (gpio.pinmode[p] == eModeOutput)) {

        (void) iWrite (p, bValue);
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
iGpioToggleAll (int iMask, UNUSED_VAR(xGpio *, unused)){

  if (bIsOpen()) {
    int p = 0;

    while ((iMask) && (p < gpio.size)) {

      if ((iMask & 1) && (gpio.pinmode[p] == eModeOutput)) {

        (void) iToggle (p++);
      }
      iMask >>= 1;
    }
    return 0;
  }
  return -1;
}

/* ========================================================================== */
