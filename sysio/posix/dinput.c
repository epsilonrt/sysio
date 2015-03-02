/*
 * @file sysio/dinput.c
 * @brief GPIO Input pin
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sysio/dinput.h>
#include <sysio/delay.h>

/* constants ================================================================ */
#define DINPUT_POLL_DELAY 20
static const char sErrorRange[] = "the pin number is out of range";
static const char sGroupRange[] = "this pin is grouped";

/* structures =============================================================== */

// -----------------------------------------------------------------------------
typedef struct xDinCbContext {
  eDinEdge edge;
  iDinCallback callback;
  void * udata;
  eDinEdge edge_occurred;
  pthread_mutex_t read_mutex;
  pthread_mutex_t write_mutex;
} xDinCbContext;

// -----------------------------------------------------------------------------
typedef struct xDinPort {
  xGpio * gpio;
  const xDin * pins;
  unsigned size;

  xDinCbContext * ctx;
  int prev_values;
  bool run;      /* indique au thread de continuer */
  bool grouped;  /* indique que la gestion est groupée */
  pthread_t thread;
} xDinPort;

// -----------------------------------------------------------------------------
typedef struct xDinGrpData {
  iDinGrpCallback callback;
  void * udata;
  xDinPort * port;
} xDinGrpData;

// -----------------------------------------------------------------------------
typedef struct xDinPinData {
  unsigned pin;
  xDinGrpData * grp;
} xDinPinData;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSetCallback (unsigned p, eDinEdge edge, iDinCallback callback, void *udata, xDinPort * port) {

  pthread_mutex_lock (&port->ctx[p].write_mutex);
  port->ctx[p].callback = callback;
  port->ctx[p].udata = udata;
  port->ctx[p].edge = edge;
  pthread_mutex_unlock (&port->ctx[p].write_mutex);
}

// -----------------------------------------------------------------------------
static void
vClearCallback (unsigned p, xDinPort *port) {

  pthread_mutex_lock (&port->ctx[p].write_mutex);
  port->ctx[p].callback = NULL;
  pthread_mutex_unlock (&port->ctx[p].write_mutex);
}

// -----------------------------------------------------------------------------
static void
vSetEdge (unsigned p, eDinEdge edge, xDinPort * port) {

  pthread_mutex_lock (&port->ctx[p].write_mutex);
  port->ctx[p].edge = edge;
  pthread_mutex_unlock (&port->ctx[p].write_mutex);
}

// -----------------------------------------------------------------------------
static int
iDinGrpHandler (eDinEdge edge, void *pdata) {
  xDinPinData * d = (xDinPinData *) pdata;

  if (d->grp->callback) {

    return d->grp->callback ( (unsigned) iDinReadAll (d->grp->port),
                              d->pin, edge, d->grp->udata);
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Thread de surveillance des entrées du port
static void *
pvDinPoll (void * xContext) {

  xDinPort *port = (xDinPort *) xContext;

  while (port->run) {
    int values = iDinReadAll (port);

    if (values != port->prev_values) {
      // l'état des entrées a changé ?

      delay_ms (DINPUT_POLL_DELAY); // Anti-rebond
      if (values == iDinReadAll (port)) {
        // le changement est confirmé
        int mask = 1;
        int diff = values ^ port->prev_values;
        int p = 0;

        // Recherche des bits modifiés et décodage des fronts
        while (diff) {

          if (diff & mask) {
            // Ce bit a changé
            int ret;
            eDinEdge edge;

            if (values & mask) {
              // Il est passé à 1

              edge = eEdgeRising;  // front montant
            }
            else {
              // Il est passé à 0

              edge = eEdgeFalling; // front descendant
            }
            diff &= ~mask;  // clear du bit traité

            pthread_mutex_lock (&port->ctx[p].read_mutex);
            port->ctx[p].edge_occurred = edge & port->ctx[p].edge;
            pthread_mutex_unlock (&port->ctx[p].read_mutex);

            pthread_mutex_lock (&port->ctx[p].write_mutex);
            if ( (port->ctx[p].edge & edge) && (port->ctx[p].callback)) {
              // Le front est valide et le callback est présent
              ret = port->ctx[p].callback (edge, port->ctx[p].udata);
            }
            else {
              ret = 0;
            }
            pthread_mutex_unlock (&port->ctx[p].write_mutex);

            if (ret != 0) {

              port->run = false;
              break;
            }
          }
          mask <<= 1;
          p++;
        }
        port->prev_values = values;
      }
    }
    delay_ms (DINPUT_POLL_DELAY);
  }
  return NULL;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xDinPort *
xDinOpen (const xDin * pins, unsigned size) {

  xDinPort * port = malloc (sizeof (xDinPort));
  assert (port);
  memset (port, 0, sizeof (xDinPort));

  port->gpio = xGpioOpen (NULL);
  if (!port->gpio) {
    goto xDinOpenError;
  }
  port->pins = pins;
  port->size = size;

  for (unsigned i = 0; i < size; i++) {

    if (iGpioSetMode (pins[i].num, eModeInput, port->gpio) != 0) {
      goto xDinGpioError;
    }
    if (iGpioSetPull (pins[i].num, pins[i].pull, port->gpio) != 0) {
      goto xDinGpioError;
    }
  }

  port->ctx = calloc (size, sizeof (xDinCbContext));
  assert (port->ctx);
  memset (port->ctx, 0, sizeof (xDinCbContext) * size);

  port->run = true;
  if (pthread_create (&port->thread, NULL, pvDinPoll, port) != 0) {

    goto xDinTheadError;
  }

  return port;

xDinTheadError:   // Erreur lors de la création du thread
  free (port->ctx);
xDinGpioError:    // Erreur lors de l'accès au gpio
  (void) iGpioClose (port->gpio);
xDinOpenError:    // Erreur lors de l'ouverture du gpio
  free (port);
  return 0;
}

// -----------------------------------------------------------------------------
int
iDinRead (unsigned i, xDinPort *port) {
  assert (port);

  if (i >= port->size) {
    return -1;
  }

  return (iGpioRead (port->pins[i].num, port->gpio) == port->pins[i].act) ? true : false;
}

// -----------------------------------------------------------------------------
int
iDinReadAll (xDinPort *port) {
  int mask = 0;

  for (unsigned i = 0; i < port->size; i++) {

    switch (iDinRead (i, port)) {
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

// -----------------------------------------------------------------------------
int
iDinPortSize (xDinPort * port) {
  assert (port);

  return port->size;
}

// -----------------------------------------------------------------------------
int
iDinClose (xDinPort * port) {
  assert (port);

  port->run = false;
  pthread_join (port->thread, NULL);
  int i = iGpioClose (port->gpio);

  free (port);
  return i;
}

// -----------------------------------------------------------------------------
int
iDinSetCallback (unsigned p, eDinEdge edge, iDinCallback callback, void *udata, xDinPort * port) {
  assert (port);

  if ( (p >= port->size) || (port->grouped)) {
    PERROR ("%s or %s", sErrorRange, sGroupRange);
    return -1;
  }

  vSetCallback (p, edge, callback, udata, port);
  return 0;
}

// -----------------------------------------------------------------------------
int
iDinClearCallback (unsigned p, xDinPort *port) {
  assert (port);

  if ( (p >= port->size) || (port->grouped)) {
    PERROR ("%s or %s", sErrorRange, sGroupRange);
    return -1;
  }

  vClearCallback (p, port);
  return 0;
}

// -----------------------------------------------------------------------------
eDinEdge
eDinGetEdge (unsigned p, xDinPort *port) {
  assert (port);

  if (p >= port->size) {
    PERROR ("%s", sErrorRange);
    return eEdgeNone;
  }
  return port->ctx[p].edge;
}

// -----------------------------------------------------------------------------
int
iDinSetEdge (unsigned p, eDinEdge edge, xDinPort * port) {
  assert (port);

  if (p >= port->size) {
    PERROR ("%s", sErrorRange);
    return -1;
  }
  if (port->grouped) {
    for (p = 0; p < port->size; p++) {
      vSetEdge (p, edge, port);
    }
  }
  else {
    vSetEdge (p, edge, port);
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iDinEdgeOccured (unsigned p, xDinPort *port) {
  assert (port);

  if (p < port->size) {
    int ret;

    pthread_mutex_lock (&port->ctx[p].read_mutex);
    ret = port->ctx[p].edge_occurred;
    port->ctx[p].edge_occurred = eEdgeNone;
    pthread_mutex_unlock (&port->ctx[p].read_mutex);
    return ret;
  }
  PERROR ("%s", sErrorRange);
  return -1;
}

// -----------------------------------------------------------------------------
void *
pDinCallbackData (unsigned p, xDinPort *port) {
  assert (port);

  if (p >= port->size) {

    PERROR ("%s", sErrorRange);
    return NULL;
  }
  void * udata = port->ctx[p].udata;

  if (port->grouped) {
    xDinPinData * d = (xDinPinData *) udata;
    udata = d->grp->udata;
  }
  return udata;
}

// -----------------------------------------------------------------------------
int
iDinCallbackInstalled (unsigned p, xDinPort *port) {
  assert (port);

  if (p >= port->size) {
    PERROR ("%s", sErrorRange);
    return -1;
  }
  return port->ctx[p].callback != NULL;
}

// -----------------------------------------------------------------------------
int
iDinSetGrpCallback (eDinEdge edge, iDinGrpCallback callback, void *udata, xDinPort * port) {
  assert (port);

  if (port->grouped) {

    (void) iDinClearGrpCallback (port);
  }
  xDinGrpData * grp = malloc (sizeof (xDinGrpData));
  assert (grp);
  grp->callback = callback;
  grp->port = port;
  grp->udata = udata;

  for (unsigned pin = 0; pin < port->size; pin++) {

    xDinPinData * pdata = malloc (sizeof (xDinPinData));
    assert (pdata);
    pdata->grp = grp;
    pdata->pin = pin;
    vSetCallback (pin, edge, iDinGrpHandler, pdata, port);
  }
  port->grouped = true;
  return 0;
}

// -----------------------------------------------------------------------------
int
iDinClearGrpCallback (xDinPort * port) {
  assert (port);

  if (port->grouped) {
    xDinGrpData * grp = 0;

    for (unsigned pin = 0; pin < port->size; pin++) {

      xDinPinData * d = (xDinPinData *) port->ctx[pin].udata;
      grp = d->grp;
      vClearCallback (pin, port);
      free (d);
    }
    free (grp);
    port->grouped = false;
    return 0;
  }
  return -1;
}

/* ========================================================================== */
