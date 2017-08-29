/**
 * @file gpio/douput.c
 * @brief
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <string.h>
#include <sysio/doutput.h>
#include <sysio/log.h>

/* structures =============================================================== */
typedef struct xDoutPort {
  xGpio * gpio;
  xDout * pins;
  unsigned size;
  int set_mask;
  int clr_mask;
} xDoutPort;

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xDoutPort *
xDoutOpen (const xDout * pins, unsigned size) {
  assert(pins);
  
  if (size == 0) {
    return 0;
  }
  
  xDoutPort * port = malloc (sizeof(xDoutPort));
  assert(port);
  memset (port, 0, sizeof(xDoutPort));

  port->pins = calloc (size, sizeof(xDout));
  assert(port->pins);
  memcpy (port->pins, pins, size * sizeof(xDout));
  port->size = size;

  port->gpio = xGpioOpen (NULL);
  if (!port->gpio) {
    goto xDoutGpioError;
  }

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
  free(port->pins);
  free (port);
  return 0;
}

// -----------------------------------------------------------------------------
int
iDoutClose (xDoutPort * port) {
  assert(port);
  int i = iGpioClose (port->gpio);

  free (port->pins);
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

// -----------------------------------------------------------------------------
int
iDoutRead (unsigned i, xDoutPort *port) {
  assert (port);

  if (i >= port->size) {
    return -1;
  }

  return (iGpioRead (port->pins[i].num, port->gpio) == port->pins[i].act) ? true : false;
}

// -----------------------------------------------------------------------------
int
iDoutReadAll (xDoutPort *port) {
  int mask = 0;

  for (unsigned i = 0; i < port->size; i++) {

    switch (iDoutRead (i, port)) {
      case true:
        mask |= _BV (i);
        break;
      case false:
        break;
      default:
        return -1;
    }
  }
  return mask;
}

/* ========================================================================== */
