/**
 * @file chipio/serial.c
 * @brief Liaison série ChipIo
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <sysio/delay.h>
#include <sysio/log.h>
#include <sysio/i2c.h>
#include <sysio/doutput.h>
#include <chipio/serial.h>
#include "interface.h"

/* setup ==================================================================== */
#define LED_DEBUG 0
#define THREAD_POLL_DELAY 10

/* constants ================================================================ */
#define LED_RED     0
#define LED_YELLOW  1
#define LED_GREEN   2

/* structures =============================================================== */
typedef struct xChipIoSerial {
  xChipIo * chipio;   // Objet chipio
  int fdm;            // descripteur de fichier pseudo-terminal maître
  pthread_t thread;   // thread de surveillance
  xDinPort * irq;     // Port pour la broche d'interruption
} xChipIoSerial;

#if LED_DEBUG
// Les leds sont utilisées pour la mise au point du thread
/* private variables ======================================================== */
static xDoutPort * led;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static inline void
vLedDebugInit (void) {
  const xDout xMyLeds[3] = {
    { .num = 0, .act = true },
    { .num = 1, .act = true },
    { .num = 2, .act = true }
  };

  led = xDoutOpen (xMyLeds, 3);
  assert (led);
  iDoutClearAll (led);
}

// -----------------------------------------------------------------------------
static void
vLedDebugClear (unsigned i) {
  (void) iDoutClear (i, led);
}

// -----------------------------------------------------------------------------
static void
vLedDebugSet (unsigned i) {
  (void) iDoutSet (i, led);
}


// -----------------------------------------------------------------------------
static void
vLedDebugToggle (unsigned i) {
  (void) iDoutToggle (i, led);
}
#else
#define vLedDebugInit()
#define vLedDebugSet(i)
#define vLedDebugClear(i)
#define vLedDebugToggle(i)
#endif /* LED_DEBUG */

#if 0
// -----------------------------------------------------------------------------
static int
iBufferSize (xChipIoSerial * port) {

  int iRet = iChipIoReadReg8 (port->chipio, eRegSerSr);
  if (iRet >= 0) {
    iRet = MIN ( (iRet & ~eStatusBusy), I2C_BLOCK_MAX);
  }
  return iRet;
}
#endif

// -----------------------------------------------------------------------------
static int
iChipIsBusy (xChipIoSerial * port) {

  int iRet = iChipIoReadReg8 (port->chipio, eRegSerSr);
  if (iRet >= 0) {
    iRet &= eStatusBusy;
  }
  return (iRet != 0);
}

/* -----------------------------------------------------------------------------
 * Retourne true si la broche irq est active ou si il n'y a pas de broche irq
 */
static bool
bIrqIsSet (xChipIoSerial * port) {
  bool bIsSet = true;

  if (port->irq) {
    // Il y a une broche d'interruption
    int iRet;

    iRet = iDinRead (0, port->irq);
    if (iRet >= 0) {
      // Interrruption lue avec succès, recopie de l'état
      bIsSet = iRet;
    }
    else {
      // Erreur de lecture
      bIsSet = false;
    }
  }
  return bIsSet;
}
// -----------------------------------------------------------------------------
// Thread de surveillance du port
static void *
pvSerialThread (void * xContext) {
  int iLen, iRet;
  int iBytesAvailable;
  uint8_t buffer[I2C_BLOCK_MAX];
  int iBlockSize = I2C_BLOCK_MAX;
  unsigned uDelay;

  xChipIoSerial * port = (xChipIoSerial *) xContext;

  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  vLedDebugInit();

  for (;;) {

    vLedDebugToggle (LED_GREEN);

    /* Surveillance des octets reçus par la liaison série et envoi vers ptm
     * -----------------------------------------------------------------------*/
    if (bIrqIsSet (port)) {
      /* Interrruption active ou pas de broche d'interruption, on vient lire
       * le nombre de caractères en attente */

      if (iChipIsBusy (port) == false) {
        int iBytesAvailable, iBytesRead = 0;
        uint8_t * p = buffer;

        vLedDebugSet (LED_YELLOW);
        while (bIrqIsSet (port)) {

          // Boucle de réception tant que la broche irq est active
          iBytesAvailable = iChipIoReadReg8 (port->chipio, eRegSerRxSr);
          if (iBytesAvailable <= 0) {
            /* Sortie plus de caractères à lire, cas où la broche IRQ n'est
             * pas activée */
            // PDEBUG("Exit %d", iBytesAvailable);
            break;
          }

          // le port série est prêt, 32 octets max.
          iBlockSize = MIN (I2C_BLOCK_MAX, iBytesAvailable);

          // lecture d'un bloc de données en réception
          iRet = iChipIoReadRegBlock (port->chipio, eRegSerRx, p, iBlockSize);
          if (iRet > 0) {

            iBytesRead += iRet;
            p += iRet;
            // PDEBUG("Recv %d car", iRet);
          }
          delay_ms (THREAD_POLL_DELAY);
        }

        if (iBytesRead > 0) {

          // transmission vers l'esclave
          if (write (port->fdm, buffer, iBytesRead) != iRet) {

            PERROR ("Unable to write to pty");
          }
          else {
            // PDEBUG("Write %d car", iBytesRead);
          }
        }
      }
      vLedDebugClear (LED_YELLOW);
    }

    /* Surveillance des octets reçus de ptm et envoi vers la liaison série
     * -----------------------------------------------------------------------*/
    iLen = 0;

    while ( (iBytesAvailable = iSerialDataAvailable (port->fdm)) > 0) {

      // Des données sont disponibles pour envoi

      if (iChipIsBusy (port) == false) {

        // le port série est prêt à transmettre, 32 octets max.
        vLedDebugSet (LED_RED);

        iBlockSize = MIN (I2C_BLOCK_MAX, iBytesAvailable);
        if ( (iLen = read (port->fdm, buffer, iBlockSize)) > 0) {

          // on envoie un bloc de données
          iRet = iChipIoWriteRegBlock (port->chipio, eRegSerTx, buffer, iLen);
          // attente d'une durée correspondant à la trame
          uDelay = dSerialFrameDuration (port->fdm, iLen) * 1000.;
          delay_ms (uDelay);
        }
        vLedDebugClear (LED_RED);
      }
      else {

        uDelay = dSerialFrameDuration (port->fdm, MAX (iLen, I2C_BLOCK_MAX)) * 1000.;
        delay_ms (uDelay);
      }
    }
    pthread_testcancel();
    delay_ms (THREAD_POLL_DELAY);
  }
  return NULL;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xChipIoSerial *
xChipIoSerialNew (xChipIo * chip, xDin * xIrqPin) {
  xChipIoSerial * port;

  if ( (iChipIoAvailableOptions (chip) & eOptionSerial) == 0) {

    PERROR ("Serial port is not available for this chip");
    return NULL;
  }
  if ( ( (iChipIoAvailableOptions (chip) & eOptionSerialIrq) == 0) && (xIrqPin)) {

    PERROR ("No interrupt pin for this serial port");
    return NULL;
  }

  port = malloc (sizeof (xChipIoSerial));
  assert (port);

  memset (port, 0, sizeof (xChipIoSerial));
  port->chipio = chip;

  // Création du pseudo-terminal maître
  if ( (port->fdm = getpt ()) < 0) {

    goto open_error_exit;
  }

  // Ajout de l'interface esclave qui sera fournie à l'utilisateur
  if ( (grantpt (port->fdm)) != 0) {

    goto open_error_exit;
  }

  if ( (unlockpt (port->fdm)) != 0) {

    goto open_error_exit;
  }

  /* Configuration en mode Raw, sans cela le pseudo-terminal est en mode
   * canonique, c'est à dire que les échanges se font lignes par lignes */
  struct termios ts;

  if (tcgetattr (port->fdm, &ts)) {

    goto open_error_exit;
  }

  cfmakeraw (&ts);
  ts.c_cc [VMIN]  = 0;
  ts.c_cc [VTIME] = 0;

  tcflush (port->fdm, TCIOFLUSH);
  tcsetattr (port->fdm, TCSANOW, &ts);

  // Création du port de broche d'interruption si nécessaire
  if (xIrqPin) {
    port->irq = xDinOpen (xIrqPin, 1);
    if (port->irq == NULL) {

      goto open_error_exit;
    }
  }

  // Création du thread
  if (pthread_create (&port->thread, NULL, pvSerialThread, port) != 0) {

    goto open_error_exit;
  }

  return port;

open_error_exit:
  free (port);
  return NULL;
}

// -----------------------------------------------------------------------------
void
vChipIoSerialDelete (xChipIoSerial * port) {
  UNUSED_VAR (int, iRet);

  assert (port);

  // Arrêt du thread
  iRet = pthread_cancel (port->thread);
  assert (iRet == 0);
  iRet = pthread_join (port->thread, NULL);
  assert (iRet == 0);

  // Fermeture des éléments
  if (port->irq) {

    iRet = iDinClose (port->irq);
    assert (iRet == 0);
  }

  iRet = close (port->fdm);
  assert (iRet == 0);

  free (port);
}


// -----------------------------------------------------------------------------
int
iChipIoSerialGetAttr (xChipIoSerial * port, xSerialIos * ios) {
  int iRet;

  // Baudrate
  iRet = iChipIoReadReg16 (port->chipio, eRegSerBaud);
  if (iRet < 0) {
    return iRet;
  }
  ios->baud = iRet * 100;

  iRet = iChipIoReadReg16 (port->chipio, eRegSerCr);
  if (iRet < 0) {
    return iRet;
  }

  // Data bits
  switch (iRet & eDataBitMask) {
    case eDataBit5:
      ios->dbits = SERIAL_DATABIT_5;
      break;
    case eDataBit6:
      ios->dbits = SERIAL_DATABIT_6;
      break;
    case eDataBit7:
      ios->dbits = SERIAL_DATABIT_7;
      break;
    case eDataBit8:
      ios->dbits = SERIAL_DATABIT_8;
      break;
    default:
      ios->dbits = SERIAL_DATABIT_UNKNOWN;
      break;
  }

  // Stop bits
  switch (iRet & eStopBitMask) {
    case eStopBit1:
      ios->sbits = SERIAL_STOPBIT_ONE;
      break;
    case eStopBit2:
      ios->sbits = SERIAL_STOPBIT_TWO;
      break;
    default:
      ios->sbits = SERIAL_STOPBIT_UNKNOWN;
      break;
  }

  // Parity
  switch (iRet & eParityMask) {
    case eParityNone:
      ios->parity = SERIAL_PARITY_NONE;
      break;
    case eParityEven:
      ios->parity = SERIAL_PARITY_EVEN;
      break;
    case eParityOdd:
      ios->parity = SERIAL_PARITY_ODD;
      break;
    default:
      ios->parity = SERIAL_PARITY_UNKNOWN;
      break;
  }

  // Flow control
  switch (iRet & eFlowMask) {
    case eFlowNone:
      ios->flow = SERIAL_FLOW_NONE;
      break;
    case eFlowHard:
      ios->flow = SERIAL_FLOW_RTSCTS;
      break;
    case eFlowSoft:
      ios->flow = SERIAL_FLOW_XONXOFF;
      break;
    default:
      ios->flow = SERIAL_FLOW_UNKNOWN;
      break;
  }
  ios->flag = 0;
  return 0;
}

// -----------------------------------------------------------------------------
int
iChipIoSerialSetAttr (xChipIoSerial * port, const xSerialIos * ios) {
  int iRet;
  xSerialIos old;

  if ( (iRet = iChipIoSerialGetAttr (port, &old)) == 0) {
    bool bIsChanged = false;

    if (old.baud != ios->baud) {
      if ( (iRet = iChipIoWriteReg16 (port->chipio, eRegSerBaud, ios->baud / 100)) != 0) {

        return iRet;
      }
      PDEBUG ("Baudrate changed %d", ios->baud);
      old.baud = ios->baud;
      bIsChanged = true;
    }

    if (memcmp (ios, &old, sizeof (xSerialIos))) {
      int iSerCr;

      if ( (iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr)) < 0) {

        return iSerCr;
      }

      if (ios->dbits != old.dbits) {

        iSerCr &= ~eDataBitMask;
        switch (ios->dbits) {

          case SERIAL_DATABIT_5:
            iSerCr |= eDataBit5;
            break;
          case SERIAL_DATABIT_6:
            iSerCr |= eDataBit6;
            break;
          case SERIAL_DATABIT_7:
            iSerCr |= eDataBit7;
            break;
          case SERIAL_DATABIT_8:
            iSerCr |= eDataBit8;
            break;
          default:
            PERROR ("Illegal data bits: %d", ios->dbits);
            return -1;
        }
        PDEBUG ("Data bits changed: %s", sSerialDataBitsToStr (ios->dbits));
      }

      if (ios->sbits != old.sbits) {

        iSerCr &= ~eStopBitMask;
        switch (ios->sbits) {

          case SERIAL_STOPBIT_ONE:
            iSerCr |= eStopBit1;
            break;
          case SERIAL_STOPBIT_TWO:
            iSerCr |= eStopBit2;
            break;
          default:
            PERROR ("Illegal stop bits: %d", ios->sbits);
            return -1;
        }
        PDEBUG ("Stop bits changed: %s", sSerialStopBitsToStr (ios->sbits));
      }

      if (ios->parity != old.parity) {

        iSerCr &= ~eParityMask;
        switch (ios->parity) {

          case SERIAL_PARITY_NONE:
            iSerCr |= eParityNone;
            break;
          case SERIAL_PARITY_EVEN:
            iSerCr |= eParityEven;
            break;
          case SERIAL_PARITY_ODD:
            iSerCr |= eParityOdd;
            break;
          default:
            PERROR ("Illegal parity: %d", ios->parity);
            return -1;
        }
        PDEBUG ("Parity changed: %s", sSerialParityToStr (ios->parity));
      }

      if (ios->flow != old.flow) {

        iSerCr &= ~eFlowMask;
        switch (ios->flow) {

          case SERIAL_FLOW_NONE:
            iSerCr |= eFlowNone;
            break;
          case SERIAL_FLOW_RTSCTS:
            iSerCr |= eFlowHard;
            break;
          case SERIAL_FLOW_XONXOFF:
            iSerCr |= eFlowSoft;
            break;
          default:
            PERROR ("Illegal flow control: %d", ios->flow);
            return -1;
        }
        PDEBUG ("Flow control changed: %s", sSerialFlowToStr (ios->flow));
      }

      bIsChanged = true;
      iRet = iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);
    }
    // Attente de la fin de la configuration
    while (bIsChanged == true) {
      delay_ms (THREAD_POLL_DELAY);
      bIsChanged = iChipIsBusy(port);
    }
  }
  return iRet;
}

// -----------------------------------------------------------------------------
char *
sChipIoSerialPortName (xChipIoSerial * port) {

  assert (port);
  return ptsname (port->fdm);
}

/* ========================================================================== */
