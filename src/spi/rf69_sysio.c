/**
 * @file
 * @brief
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include "rf69_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <sysio/spi.h>
#include <sysio/timer.h>

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static int
iIrqCallback (eDinEdge edge, void *udata) {

  xRf69 * rf = (xRf69 *) udata;
  pthread_mutex_lock (&rf->rcv_mutex);
  vRf69Isr (rf);
  pthread_mutex_unlock (&rf->rcv_mutex);
  return 0;
}

/* internal protected functions ============================================= */

// -----------------------------------------------------------------------------
int
iRf69WriteReg (xRf69 * rf, uint8_t reg, uint8_t data) {
  int ret;

  pthread_mutex_lock (&rf->spi_mutex);
  ret = iSpiWriteReg8 (rf->fd, reg, data);
  pthread_mutex_unlock (&rf->spi_mutex);

  return ret;
}

// -----------------------------------------------------------------------------
int
iRf69ReadReg (xRf69 * rf, uint8_t reg) {
  int ret;

  pthread_mutex_lock (&rf->spi_mutex);
  ret = iSpiReadReg8 (rf->fd, reg);
  pthread_mutex_unlock (&rf->spi_mutex);

  return ret;
}
// -----------------------------------------------------------------------------
int
iRf69WriteBlock (xRf69 * rf, uint8_t reg, const uint8_t * buf, uint8_t len) {
  int ret;

  pthread_mutex_lock (&rf->spi_mutex);
  ret = iSpiWriteRegBlock (rf->fd, reg, buf, len);
  pthread_mutex_unlock (&rf->spi_mutex);
  return ret;
}

// -----------------------------------------------------------------------------
int
iRf69ReadBlock (xRf69 * rf, uint8_t reg, uint8_t * buf, uint8_t len) {
  int ret;

  pthread_mutex_lock (&rf->spi_mutex);
  ret = iSpiReadRegBlock (rf->fd, reg, buf, len);
  pthread_mutex_unlock (&rf->spi_mutex);
  return ret;
}

// -----------------------------------------------------------------------------
int
iRf69WriteConstElmt (xRf69 * rf, const struct xRf69Config * config) {

  if (config->reg != 255) {

    return iRf69WriteReg (rf, config->reg, config->data);
  }

  return 0;
}

// -----------------------------------------------------------------------------
xRf69Timer
xRf69TimerNew (void) {

  return xTimerNew();
}

// -----------------------------------------------------------------------------
void
vRf69TimerStart (xRf69Timer t, int timeout) {

  vTimerSetInterval (t, timeout);
  (void) iTimerStart (t);
}

// -----------------------------------------------------------------------------
void
vRf69TimerStop (xRf69Timer t) {

  (void) iTimerStop (t);
}

// -----------------------------------------------------------------------------
void
vRf69TimerDelete (xRf69Timer t) {

  (void) iTimerDelete (t);
}

// -----------------------------------------------------------------------------
bool
bRf69TimerTimeout (xRf69Timer t) {

  return bTimerIsTimeout (t);
}

// -----------------------------------------------------------------------------
int
iRf69ReadIrqPin (xRf69 * rf) {

  return iDinRead (0, rf->port);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xRf69 *
xRf69New (int iBus, int iDevice, int iIrqPin) {
  int i;
  xRf69 * rf = NULL;
  char * device;

  i = snprintf (NULL, 0, "/dev/spidev%d.%d", iBus, iDevice);
  device = malloc (i + 1);
  snprintf (device, i + 1, "/dev/spidev%d.%d", iBus, iDevice);

  i = iSpiOpen (device);
  free (device);

  if (i >= 0) {
    // DIV4: decided to slow down from DIV2 after SPI stalling in some instances,
    // especially visible on mega1284p when RFM69 and FLASH chip both present
    xSpiIos ios = {
      .mode = eSpiMode0,
      .lsb = eSpiNumberingMsb,
      .bits = eSpiBits8,
      .speed = 4000000
    };

    if (iSpiSetConfig (i, &ios) == 0) {

      xDin irqpin = { .num = iIrqPin, .act = true, .pull = ePullUp };
      xDinPort * p =  xDinOpen (&irqpin, 1);
      if (p) {

        rf = calloc (1, sizeof (xRf69));
        rf->fd = i;
        rf->port = p;
        pthread_mutex_init (&rf->spi_mutex, NULL);
        pthread_mutex_init (&rf->rcv_mutex, NULL);

        if (iDinSetCallback (0, eEdgeRising, iIrqCallback, rf, rf->port) == 0) {

          return rf;
        }

        iDinClose (rf->port);
        free (rf);
      }
      // Erreur
      iSpiClose (i);
    }
  }

  return rf;
}

// -----------------------------------------------------------------------------
void
vRf69Delete (xRf69 * rf) {

  if (rf) {

    pthread_mutex_lock (&rf->spi_mutex);
    pthread_mutex_lock (&rf->rcv_mutex);
    iDinClose (rf->port);
    iSpiClose (rf->fd);
    free (rf);
  }
}

// -----------------------------------------------------------------------------
// checks if a packet was received and/or puts transceiver in receive (ie RX or listen) mode
int
iRf69ReceiveDone (xRf69 * rf) {
  int ret = false;

  pthread_mutex_lock (&rf->rcv_mutex);
  if (rf->mode == eRf69ModeRx) {

    if (rf->hdr.payload_len > 0) {

      ret = iRf69SetMode (rf, eRf69ModeStandby);
      if (ret == 0) {

        ret = true;;
      }
    }
  }
  else {

    ret = iRf69StartReceiving (rf);
    if (ret == 0) {

      ret = false;
    }
  }
  pthread_mutex_unlock (&rf->rcv_mutex);

  return ret;
}

/* ========================================================================== */
