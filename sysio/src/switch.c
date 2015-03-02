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
  xDinPort * sw;
} xSwitchContext;

typedef struct xSwitchData {
  unsigned pin;
  xSwitchContext * ctx;
} xSwitchData;

/* private functions ======================================================== */
static int
iSwitchHandler (eDinEdge edge, void *udata) {
  xSwitchData * d = (xSwitchData *) udata;

  if (d->ctx->callback) {

    return d->ctx->callback ((unsigned) iDinReadAll (d->ctx->sw), d->pin);
  }
  return 0;
}

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
int
iSwitchSetCallback (iSwitchCallback callback, xDinPort * sw) {
  assert (sw);

  xSwitchContext * ctx = malloc (sizeof(xSwitchContext));
  assert (ctx);
  ctx->callback = callback;
  ctx->sw = sw;

  for (unsigned pin = 0; pin < iDinPortSize (sw); pin++) {

    xSwitchData * udata = malloc (sizeof(xSwitchData));
    assert (udata);
    udata->ctx = ctx;
    udata->pin = pin;

    if (iDinSetCallback (pin, eEdgeBoth, iSwitchHandler, udata, sw) != 0) {
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
