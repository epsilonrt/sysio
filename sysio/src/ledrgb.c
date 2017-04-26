/**
 * @file
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
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
      free (d);
      d = NULL;
    }
  }
  
  return d;
}

// -----------------------------------------------------------------------------
int
iLedRgbDeleteDevice (xLedRgbDevice * d) {
  int ret = d->dcb->ops->close(d->dcb);
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
iLedRgbAddLed (xLedRgbDevice * d, void * led_setup) {
  int ret = d->dcb->ops->addled (d->dcb, led_setup);
  if (ret == 0) {
    d->size++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetColor (xLedRgbDevice * d, uint64_t leds, uint32_t color) {
  int ret = 0, led = 0;

  while ( (leds) && (ret == 0)) {
    uint64_t mask = 1ULL << led;

    if (leds & mask) {
      leds &= ~mask;
      ret = d->dcb->ops->setcolor (d->dcb, led, color);
    }
    led++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetMode (xLedRgbDevice * d, uint64_t leds, eLedRgbMode mode) {
  int ret = 0, led = 0;

  while ( (leds) && (ret == 0)) {
    uint64_t mask = 1ULL << led;

    if (leds & mask) {
      leds &= ~mask;
      ret = d->dcb->ops->setmode (d->dcb, led, mode);
    }
    led++;
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetDimmer (xLedRgbDevice * d, uint16_t dimming) {
  
  if (d->dcb->ops->setdimmer) {
    return d->dcb->ops->setdimmer (d->dcb, dimming);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iLedRgbSetBlinker (xLedRgbDevice * d, uint16_t blinking) {
  
  if (d->dcb->ops->setblinker) {
    return d->dcb->ops->setblinker (d->dcb, blinking);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iLedRgbError (xLedRgbDevice * d, int * code) {
  
  if (d->dcb->ops->error) {
    return d->dcb->ops->error (d->dcb, code);
  }
  return 0;
}

// -----------------------------------------------------------------------------
void
vLedRgbClearError (xLedRgbDevice * d) {
}

/* ========================================================================== */
