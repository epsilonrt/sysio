/**
 * @file
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "ledrgb_private.h"

/* macros =================================================================== */
/* constants ================================================================ */
/* structures =============================================================== */
struct xLedRgbDevice {
  xLedRgbDcb * dcb;
  eLedRgbDeviceModel model;
  int size;
};

/* types ==================================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */
/* public variables ========================================================= */

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xLedRgbDevice *
xLedRgbNewDevice (eLedRgbDeviceModel model, void * dev_setup) {
  xLedRgbDevice * d;

  d = calloc (1, sizeof (xLedRgbDevice));
  assert (d);
  d->dcb = calloc (1, sizeof (xLedRgbDcb));
  assert (d->dcb);

  d->model = eLedRgbDeviceNone;

  switch (model) {

    case eLedRgbDeviceTlc59116:
      d->dcb->ops = &xTlc59116Ops;
      d->model = eLedRgbDeviceTlc59116;
      break;

    default:
      break;
  }

  if (d->model != eLedRgbDeviceNone) {

    if (d->dcb->ops->open (d->dcb, dev_setup) != 0) {
      free (d->dcb);
      free (d);
      d = NULL;
    }
  }

  return d;
}

// -----------------------------------------------------------------------------
int
iLedRgbDeleteDevice (xLedRgbDevice * d) {
  int ret;

  ret = d->dcb->ops->close (d->dcb);
  free (d->dcb);
  free (d);
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSize (const xLedRgbDevice * d) {
  return d->size;
}

// -----------------------------------------------------------------------------
int
iLedRgbAddLed (xLedRgbDevice * d, eLedRgbMode mode, void * led_setup) {
  int ret;

  ret = d->dcb->ops->addled (d->dcb, mode, led_setup);
  if (ret >= 0) {
    d->size++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetColor (xLedRgbDevice * d, uint64_t leds, uint32_t color) {
  int ret = 0, led = 0;
  xRgbColor * pxColor = (xRgbColor *) &color;

  while ( (leds) && (ret == 0)) {
    uint64_t mask = 1ULL << led;

    if (leds & mask) {
      leds &= ~mask;
      ret = d->dcb->ops->setcolor (d->dcb, led, pxColor);
    }
    led++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbCtl (xLedRgbDevice * d, int c, ...) {
  int ret = 0;
  va_list ap;

  va_start (ap, c);
  switch (c) {

      // put here the requests should not be transmitted to the layer below.
      // case ...

    default:
      ret = d->dcb->ops->ctl (d->dcb, c, ap);
      if ( (ret == -1) && (errno == EINVAL)) {
        PERROR ("iLedRgbCtl function not supported: %d", c);
      }
      break;
  }
  va_end (ap);
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetMode (xLedRgbDevice * d, uint64_t leds, eLedRgbMode mode) {
  int ret = 0, led = 0;

  while ( (leds) && (ret == 0)) {
    uint64_t mask = 1ULL << led;

    if (leds & mask) {
      ret = iLedRgbCtl (d, LEDRGB_IOC_SETMODE, led, mode);
      leds &= ~mask;
    }
    led++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetDimmer (xLedRgbDevice * d, int dimming) {

  return iLedRgbCtl (d, LEDRGB_IOC_SETDIMMER, dimming);
}

// -----------------------------------------------------------------------------
int
iLedRgbSetBlinker (xLedRgbDevice * d, int period, int dcycle) {

  return iLedRgbCtl (d, LEDRGB_IOC_SETBLINKER, period, dcycle);
}

// -----------------------------------------------------------------------------
int
iLedRgbGetError (xLedRgbDevice * d, int led) {
  int ret;

  ret = iLedRgbCtl (d, LEDRGB_IOC_GETERROR, led);
  if ( (ret == -1) && (errno == EINVAL)) {
    ret = 0;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbClearError (xLedRgbDevice * d) {
  int ret;

  ret = iLedRgbCtl (d, LEDRGB_IOC_CLRERROR);
  if ( (ret == -1) && (errno == EINVAL)) {
    ret = 0;
  }
  return ret;
}

/* ========================================================================== */
