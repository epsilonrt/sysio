/*
 * @file sysio/douput.c
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/doutput.h>

/* structures =============================================================== */
typedef struct xDoutPort {
  xGpio * gpio;
  const xDout * pins;
  unsigned size;
  int set_mask;
  int clr_mask;
} xDoutPort;

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xDoutPort *
xDoutOpen (const xDout * pins, unsigned size) {

  xDoutPort * port = malloc (sizeof(xDoutPort));
  assert(port);
  memset (port, 0, sizeof(xDoutPort));

  port->gpio = xGpioOpen (NULL);
  if (!port->gpio) {
    goto xDoutGpioError;
  }
  port->pins = pins;
  port->size = size;

  for (unsigned i = 0; i < size; i++) {

    if (iGpioSetMode (pins[i].num, eModeOutput, port->gpio) != 0) {
      goto xDoutOpenError;
    }
    if (pins[i].act) {
      port->set_mask |= (1<<pins[i].num);
    }
    else {
      port->clr_mask |= (1<<pins[i].num);
    }
  }

  if (iDoutClearAll(port) != 0) {
    goto xDoutOpenError;
  }
  return port;

xDoutOpenError:
  (void) iGpioClose (port->gpio);
xDoutGpioError:
  free (port);
  return 0;
}

// -----------------------------------------------------------------------------
int
iDoutClose (xDoutPort * port) {
  assert(port);
  int i = iGpioClose (port->gpio);

  free (port);
  return i;
}

// -----------------------------------------------------------------------------
int
iDoutPortSize (xDoutPort * port) {
  assert(port);

  return port->size;
}

// -----------------------------------------------------------------------------
int
iDoutClear (unsigned i, xDoutPort * port) {
  assert(port);

  if (i >= port->size) {
    return -1;
  }
  return iGpioWrite (port->pins[i].num, !port->pins[i].act, port->gpio);
}

// -----------------------------------------------------------------------------
int
iDoutSet (unsigned i, xDoutPort * port) {
  assert(port);

  if (i >= port->size) {
    return -1;
  }
  return iGpioWrite (port->pins[i].num, port->pins[i].act, port->gpio);
}

// -----------------------------------------------------------------------------
int
iDoutToggle (unsigned i, xDoutPort * port) {
  assert(port);

  if (i >= port->size) {
    return -1;
  }
  return iGpioToggle (port->pins[i].num, port->gpio);
}

// -----------------------------------------------------------------------------
int
iDoutSetAll (xDoutPort * port) {
  assert(port);

  if (iGpioWriteAll (port->set_mask, true, port->gpio) == 0) {
    return iGpioWriteAll (port->clr_mask, false, port->gpio);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iDoutClearAll (xDoutPort * port) {
  assert(port);

  if (iGpioWriteAll (port->set_mask, false, port->gpio) == 0) {
    return iGpioWriteAll (port->clr_mask, true, port->gpio);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iDoutToggleAll (xDoutPort * port) {
  assert(port);

  return iGpioToggleAll (port->clr_mask | port->set_mask, port->gpio);
}

/* ========================================================================== */
