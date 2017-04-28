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
#include <sysio/vector.h>
#include "../ledrgb_private.h"

/* constants ================================================================ */
#define TLC59116_ADDR_ALL    0x68
#define TLC59116_ADDR_SUB1   0x69
#define TLC59116_ADDR_SUB2   0x6A
#define TLC59116_ADDR_SWRST  0x6B
#define TLC59116_ADDR_SUB3   0x6C

#define TLC59116_SWRST_BYTE1 0xA5
#define TLC59116_SWRST_BYTE2 0x5A

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
  xVector leds;
} xTlc59116Data;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vLedDestroy (void *led) {
  free (led);
}

// -----------------------------------------------------------------------------
// Ecrit un bloc de registre d'un contrôleur
static int
iWriteRegBlock (const xTlc59116Ctrl * ctrl, uint8_t first_reg, uint8_t last_reg) {
  int fd;

  fd = iI2cOpen (ctrl->config.i2c_bus, ctrl->config.i2c_addr);

  if (fd >= 0) {
    uint8_t len;
    uint8_t reg;
    int error, ret;

    len = last_reg - first_reg + 1;
    reg = first_reg | (last_reg > first_reg ? AI_ALL : 0);

    ret = iI2cWriteRegBlock (fd, reg, &ctrl->regs[first_reg], len);
    error = iI2cClose (fd);
    if ( (error == 0) && (ret == len)) {
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
// Lit un bloc de registre d'un contrôleur
static int
iReadRegBlock (xTlc59116Ctrl * ctrl, uint8_t first_reg, uint8_t last_reg) {
  int fd;

  fd = iI2cOpen (ctrl->config.i2c_bus, ctrl->config.i2c_addr);

  if (fd >= 0) {
    uint8_t len;
    uint8_t reg;
    int error, ret;

    len = last_reg - first_reg + 1;
    reg = first_reg | (last_reg > first_reg ? AI_ALL : 0);

    ret = iI2cReadRegBlock (fd, reg, &ctrl->regs[first_reg], len);
    error = iI2cClose (fd);
    if ( (error == 0) && (ret == len)) {
      return 0;
    }
  }
  return -1;
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
iSetPinMode (xTlc59116Ctrl * ctrl, int out, eLedRgbMode mode) {
  uint8_t reg, ls;

  reg = LEDOUT0 + out / 4; // registre LEDOUT correspondant
  ls = (out % 4) * 2; // décalage à gauche
  ctrl->regs[reg] &= ~ (3 << ls); // clear
  ctrl->regs[reg] |= (mode << ls); // set
  return iWriteRegBlock (ctrl, reg, reg);
}

// -----------------------------------------------------------------------------
static int
iSetPinPwm (xTlc59116Ctrl * ctrl, int out, uint8_t dcycle) {
  uint8_t reg;

  reg = PWM0 + out; // registre PWM correspondant
  ctrl->regs[reg] = dcycle; // set
  return iWriteRegBlock (ctrl, reg, reg);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
static int
iClose (xLedRgbDcb * dcb) {
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Ctrl * ctrl = d->ctrl;

  vVectorDestroy (d->leds);
  for (int i = 0; i < d->nof_ctrl; i++) {
    free (ctrl[i]->config.i2c_bus);
  }
  free (ctrl);
  free (d);
  return 0;
}

// -----------------------------------------------------------------------------
static int
iClrError (xLedRgbDcb * dcb) {
  int ret = -1;
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Ctrl * ctrl = d->ctrl;

  for (int i = 0; i < d->nof_ctrl; i++) {

    ctrl[i]->regs[MODE2] |= _BV (EFCLR);
    ret = iWriteRegBlock (&ctrl[i], MODE2, MODE2);
    if (ret != 0) {
      break;
    }
    ctrl[i]->regs[MODE2] &= ~_BV (EFCLR);
    ret = iWriteRegBlock (&ctrl[i], MODE2, MODE2);
  }
  return ret;
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

      config++;
    }
    delay_ms (1); // Délai de démarrage 500 ms

    return iClrError (dcb);
  }

  return -1;
}

// -----------------------------------------------------------------------------
static int
iSetMode (xLedRgbDcb * dcb, int index, eLedRgbMode mode) {
  int ret;
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Led * led = (xTlc59116Led *) pvVectorGet (d->leds, index);
  assert (led);

  ret = iSetPinMode (ctrl[led->red.ctrl], led->red.out, mode);
  if (ret == 0) {
    ret = iSetPinMode (ctrl[led->green.ctrl], led->green.out, mode);
    if (ret == 0) {
      return iSetPinMode (ctrl[led->blue.ctrl], led->blue.out, mode);
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
static int
iAddLed (xLedRgbDcb * dcb, eLedRgbMode mode, void * setup) {
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Led * s = (xTlc59116Led *) setup;

  if ( (s->red.ctrl < d->nof_ctrl) && (s->green.ctrl < d->nof_ctrl) &&
       (s->blue.ctrl < d->nof_ctrl) &&
       (s->red.out < 16) && (s->green.out < 16) && (s->blue.out < 16)) {
    xTlc59116Led * led;

    led = malloc (sizeof (xTlc59116Led));
    assert (led);
    memcpy (led, s, sizeof (xTlc59116Led));

    if (iVectorAppend (&d->leds, led) == 0) {
      int index = iVectorSize (&d->leds) - 1;

      if (iSetMode (dcb, index, mode) == 0) {
        return index;
      }
      iVectorRemove (&d->leds, index);
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iSetColor (xLedRgbDcb * dcb, int index, xRgbColor * color) {
  int ret;
  xTlc59116Data * d = (xTlc59116Data *) dcb->dptr;
  xTlc59116Led * led = (xTlc59116Led *) pvVectorGet (d->leds, index);
  assert (led);

  ret = iSetPinPwm (d->ctrl[led->red.ctrl], led->red.out, color->ucRed);
  if (ret == 0) {
    ret = iSetPinPwm (d->ctrl[led->green.ctrl], led->green.out, color->ucGreen);
    if (ret == 0) {
      return iSetPinPwm (d->ctrl[led->blue.ctrl], led->blue.out, color->ucBlue);
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
static int
iSetDimmer (xLedRgbDcb * dcb, int index, int dimming) {
  int ret = -1;

  if (dimming >= 0) {
    int c[3] = { -1, -1, -1};
    xTlc59116Data * d;
    xTlc59116Led * led;
    xTlc59116Pin * pin;

    d = (xTlc59116Data *) dcb->dptr;
    led = (xTlc59116Led *) pvVectorGet (d->leds, index);
    assert (led);
    pin = &led->red;

    while (dimming > 255) {
      dimming >>= 1;
    }

    for (int i = 0; i < 3; i++) {

      if ( (pin->ctrl != c[0]) && (pin->ctrl != c[1])) {
        xTlc59116Ctrl * ctrl = d->ctrl[pin->ctrl];

        ctrl->regs[GRPPWM] = dimming;
        ret = iWriteRegBlock (ctrl, GRPPWM, GRPPWM);
        if (ret != 0) {
          break;
        }

        ctrl->regs[MODE2] &= ~_BV (DMBLNK);
        ret = iWriteRegBlock (ctrl, MODE2, MODE2);
        if (ret != 0) {
          break;
        }
        c[i] = pin->ctrl;
      }
      pin++;
    }
  }

  return ret;
}

// -----------------------------------------------------------------------------
static int
iSetBlinker (xLedRgbDcb * dcb, int index, int period, int dcycle) {
  int ret = -1;

  if ( (period >= 0) && (dcycle >= 0)) {
    int c[3] = { -1, -1, -1};
    xTlc59116Data * d;
    xTlc59116Led * led;
    xTlc59116Pin * pin;

    d = (xTlc59116Data *) dcb->dptr;
    led = (xTlc59116Led *) pvVectorGet (d->leds, index);
    assert (led);
    pin = &led->red;

    while (dcycle > 255) {
      dcycle >>= 1;
    }

    for (int i = 0; i < 3; i++) {

      if ( (pin->ctrl != c[0]) && (pin->ctrl != c[1])) {
        xTlc59116Ctrl * ctrl = d->ctrl[pin->ctrl];

        if (period < 42) {
          period = 42;
        }
        ctrl->regs[GRPPWM] = dcycle;
        ctrl->regs[GRPFREQ] = (uint8_t) MIN ( (period * 3) / 125 - 1, 255);
        ret = iWriteRegBlock (ctrl, GRPPWM, GRPFREQ);
        if (ret != 0) {
          break;
        }

        ctrl->regs[MODE2] |= _BV (DMBLNK);
        ret = iWriteRegBlock (ctrl, MODE2, MODE2);
        if (ret != 0) {
          break;
        }
        c[i] = pin->ctrl;
      }
      pin++;
    }
  }

  return ret;
}

// -----------------------------------------------------------------------------
static int
iGetError (xLedRgbDcb * dcb, int index) {
  int ret = false;
  xTlc59116Data * d;
  xTlc59116Led * led;
  xTlc59116Pin * pin;

  d = (xTlc59116Data *) dcb->dptr;
  led = (xTlc59116Led *) pvVectorGet (d->leds, index);
  assert (led);
  pin = &led->red;

  for (int i = 0; i < 3; i++) {
    xTlc59116Ctrl * ctrl; 
    uint8_t reg, bit;
    int error;
    
    ctrl = d->ctrl[pin->ctrl];
    reg = EFLAG1 + pin->out / 8;
    bit = pin->out % 8;

    error = iReadRegBlock (ctrl, reg, reg);
    if (error != 0) {
      return error;
    }
    
    if (ctrl->regs[reg] & _BV(bit)) {
      ret = true;
    }
    pin++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
static int
iSetGain (xLedRgbDcb * dcb, int ctl, int gain) {
  xTlc59116Data * d;

  d = (xTlc59116Data *) dcb->dptr;
  if ((ctl >= 0) && (ctl < d->nof_ctrl)) {
    xTlc59116Ctrl * ctrl; 
    
    ctrl = d->ctrl[ctl];
    ctrl->regs [IREF] = (uint8_t) gain;
    return iWriteRegBlock(ctrl, IREF, IREF);
  }
  return -1;
}

// -----------------------------------------------------------------------------
static int
iCtl (xLedRgbDcb * dcb, int c, va_list ap) {
  it ret = 0;

  switch (c) {

    /* int setmode (xLedRgbDcb * dcb, int led, eLedRgbMode mode) */
    case LEDRGB_IOC_SETMODE: {
      int led = va_arg (ap, int);
      eLedRgbMode mode = va_arg (ap, eLedRgbMode);
      ret = iSetMode (dcb, led, mode);
    }
    break;
    /* int setdimmer (xLedRgbDcb * dcb, int led, int dimming) */
    case LEDRGB_IOC_SETDIMMER: {
      int led = va_arg (ap, int);
      int dimming = va_arg (ap, int);
      ret = iSetDimmer (dcb, led, dimming);
    }
    break;
    /* int setblinker (xLedRgbDcb * dcb, int led, int period, int dcycle) */
    case LEDRGB_IOC_SETBLINKER: {
      int led = va_arg (ap, int);
      int period = va_arg (ap, int);
      int dcycle = va_arg (ap, int);
      ret = iSetBlinker (dcb, led, period, dcycle);
    }
    break;
    /* int error (xLedRgbDcb * dcb, int led) */
    case LEDRGB_IOC_GETERROR: {
      int led = va_arg (ap, int);
      ret = iGetError (dcb, led);
    }
    break;
    /* int clrerror (xLedRgbDcb * dcb); */
    case LEDRGB_IOC_CLRERROR: {
      ret = iClrError (dcb);
    }
    break;
    /* int setgain (xLedRgbDcb * dcb, int ctl, int gain); */
    case LEDRGB_IOC_SETGAIN: {
      int ctl = va_arg (ap, int);
      int gain = va_arg (ap, int);
      ret = iSetGain (dcb, ctl, gain);
    }
    break;
    default:
      errno = EINVAL;
      ret = -1;
      break;
  }

  return ret;
}

/* public variables ========================================================= */
xLedRgbOps xTlc59116Ops = {
  .open = iOpen,
  .close = iClose,
  .addled = iAddLed,
  .setcolor = iSetColor,
  .ctl = iCtl
};
/* ========================================================================== */
