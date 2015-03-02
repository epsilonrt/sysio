/*
 * @file sysio/switch.c
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/switch.h>

/* structures =============================================================== */
typedef struct xSwitchContext {
  iSwitchCallback callback;
  void * udata;
  xDinPort * sw;
} xSwitchContext;

typedef struct xSwitchData {
  unsigned pin;
  xSwitchContext * ctx;
} xSwitchData;

/* private functions ======================================================== */
static int
iSwitchHandler (eDinEdge edge, void *swdata) {
  xSwitchData * d = (xSwitchData *) swdata;

  if (d->ctx->callback) {

    return d->ctx->callback ((unsigned) iDinReadAll (d->ctx->sw), d->pin, d->ctx->udata);
  }
  return 0;
}

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
int
iSwitchSetCallback (iSwitchCallback callback, void *udata, xDinPort * sw) {
  assert (sw);

  xSwitchContext * ctx = malloc (sizeof(xSwitchContext));
  assert (ctx);
  ctx->callback = callback;
  ctx->sw = sw;
  ctx->udata = udata;

  for (unsigned pin = 0; pin < iDinPortSize (sw); pin++) {

    xSwitchData * swdata = malloc (sizeof(xSwitchData));
    assert (swdata);
    swdata->ctx = ctx;
    swdata->pin = pin;

    if (iDinSetCallback (pin, eEdgeBoth, iSwitchHandler, swdata, sw) != 0) {
      return -1;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iSwitchClearCallback (xDinPort * sw) {
  assert (sw);

  if (pDinCallbackData (0, sw) != NULL) {
    xSwitchContext * ctx = 0;

    for (unsigned pin = 0; pin < iDinPortSize (sw); pin++) {

      xSwitchData * d = (xSwitchData *) pDinCallbackData (pin, sw);
      ctx = d->ctx;
      if (iDinClearCallback (pin, sw) != 0) {
        return -1;
      }
      free (d);
    }
    free (ctx);
    return 0;
  }
  return -1;
}
/* ========================================================================== */
