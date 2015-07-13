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
#include <sysio/doutput.h>
#include <sysio/chipio/serial.h>
#include "interface.h"

/* macros =================================================================== */
/* constants ================================================================ */
#define THREAD_POLL_DELAY 10
#define LED_DEBUG 1

#define LED_RED     0
#define LED_YELLOW  1
#define LED_GREEN   2

/* structures =============================================================== */
typedef struct xChipIoSerial {
  xChipIo * chipio;   // Objet chipio
  int fdm;            // descripteur de fichier pseudo-terminal maître
  FILE * pts;         // flux pseudo-terminal esclave
  char name[NAME_MAX];// Nom du pseudo-terminal esclave
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
    { .num = 2, .act = true }};

  led = xDoutOpen (xMyLeds, 3);
  assert(led);
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

// -----------------------------------------------------------------------------
// Thread de surveillance du port
static void *
pvSerialThread (void * xContext) {
  int iLen, iRet;
  fd_set xFdSet;
  struct timeval xTv;
  uint8_t buffer[CHIPIO_I2C_BLOCK_MAX];
  bool bIrqPending = false;

  xChipIoSerial * port = (xChipIoSerial *) xContext;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  vLedDebugInit();

  for (;;) {

    vLedDebugToggle (LED_GREEN);

    /* Surveillance des octets reçus par la liaison série et envoi vers ptm
     * -----------------------------------------------------------------------*/
    if (port->irq) {
      // Il y a une broche d'interruption
      if (iDinRead (0, port->irq) == true) {
        // Interrruption active
        bIrqPending = true;
      }
    }

    if ((port->irq == NULL) || (bIrqPending == true)) {
      // Interrruption active ou pas de broche d'interruption, on vient lire
      // le nombre de caractères en attente

      if  (iChipIoSerialIsBusy(port) == false) {
        int iBytesAvailable = iChipIoReadReg8 (port->chipio, eRegSerRxSr);

        if (iBytesAvailable > 0) {
          int iBlockSize;

          vLedDebugSet (LED_RED);

          // le port série est prêt, 32 octets max.
          iBlockSize = MIN(CHIPIO_I2C_BLOCK_MAX, iBytesAvailable);

          // lecture d'un bloc de données en réception
          iRet = iChipIoReadRegBlock (port->chipio, eRegSerRx, buffer, iBlockSize);

          if (iRet > 0) {

            // transmission vers l'esclave
            if (write (port->fdm, buffer, iRet) != iRet) {
              PWARNING ("write()");
            }
          }
        }
      }
      bIrqPending = false; /* Interruption traitée, si il y a encore des données
                            * du côté de ChipIo, la broche d'interruption va
                            * rester active et un nouveau bloc sera lu à la
                            * prochaine itération
                            */
      vLedDebugClear (LED_RED);
    }

    /* Surveillance des octets reçus de ptm et envoi vers la liaison série
     * -----------------------------------------------------------------------*/
    xTv.tv_sec = 0;
    xTv.tv_usec = THREAD_POLL_DELAY * 1000UL;

    FD_ZERO (&xFdSet);
    FD_SET (port->fdm, &xFdSet);
    iRet = select (port->fdm + 1, &xFdSet, NULL, NULL, &xTv);
    // Considérer xTv comme indéfini maintenant !

    if (iRet > 0) {
      int iBytesAvailable;

      // Lecture du nombre de caractères à transmettre
      iRet = ioctl (port->fdm, FIONREAD, &iBytesAvailable);

      if ((iRet != -1) && (iBytesAvailable)) {
        // Des données sont disponibles pour envoi

        if  (iChipIoSerialIsBusy(port) == false) {
          int iBlockSize;

          // le port série est prêt à transmettre, 32 octets max.
          vLedDebugSet (LED_YELLOW);

          iBlockSize = MIN(CHIPIO_I2C_BLOCK_MAX, iBytesAvailable);
          if  ((iLen = read (port->fdm, buffer, iBlockSize)) > 0) {

            // on envoie un bloc de données
            iRet = iChipIoWriteRegBlock (port->chipio, eRegSerTx, buffer, iLen);
          }
          vLedDebugClear (LED_YELLOW);
        }
        else {
          delay_ms (THREAD_POLL_DELAY * 5);
        }
      }
    }
    pthread_testcancel();
  }
  return NULL;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xChipIoSerial *
xChipIoSerialOpen (xChipIo * chip, xDin * xIrqPin) {
  xChipIoSerial * port;
  int fds;

  if ((iChipIoAvailableOptions(chip) & eOptionSerial) == 0) {

    PERROR("Serial port is not available for this chip");
    return NULL;
  }
  if (((iChipIoAvailableOptions(chip) & eOptionSerialIrq) == 0) && (xIrqPin)) {

    PERROR("No interrupt pin for this serial port");
    return NULL;
  }

  port = malloc (sizeof (xChipIoSerial));
  assert(port);

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

  // Lecture du nom du device esclave dans /dev/pts
  if (ptsname_r  (port->fdm, port->name, NAME_MAX) != 0) {

    goto open_error_exit;
  }

  // Ouverture du descripteur de fichier côté esclave en mode non bloquant
  if ( (fds = open (port->name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {

    goto open_error_exit;
  }

  // Ouverture du fichier côté esclave
  if ( (port->pts = fdopen (fds, "r+")) == NULL) {

    goto open_error_exit;
  }

 if (xIrqPin) {
    // Création du port de broche d'interruption
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
vChipIoSerialClose (xChipIoSerial * port) {
  int iRet;

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

  iRet = fclose (port->pts);
  assert (iRet == 0);

  iRet = close (port->fdm);
  assert (iRet == 0);

  free (port);
}

// -----------------------------------------------------------------------------
int
iChipIoSerialGetBufSize (xChipIoSerial * port) {

  assert (port);
  int iRet = iChipIoReadReg8 (port->chipio, eRegSerTxSr);
  if (iRet >= 0) {
    iRet = MIN((iRet & ~eStatusBusy), CHIPIO_I2C_BLOCK_MAX);
  }
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoSerialIsBusy (xChipIoSerial * port) {

  assert (port);
  int iRet = iChipIoReadReg8 (port->chipio, eRegSerTxSr);
  if (iRet >= 0) {
    iRet &= eStatusBusy;
  }
  return (iRet != 0);
}

// -----------------------------------------------------------------------------
FILE *
xChipIoSerialFile (xChipIoSerial * port) {

  assert (port);
  return port->pts;
}

// -----------------------------------------------------------------------------
int
iChipIoSerialFileNo (xChipIoSerial * port) {

  assert (port);
  return fileno (port->pts);
}

// -----------------------------------------------------------------------------
const char *
sChipIoSerialPortName (xChipIoSerial * port) {

  assert (port);
  return port->name;
}

// -----------------------------------------------------------------------------
int
iChipIoSerialGetBaudrate (xChipIoSerial * port) {
  int iBaudrate;

  assert (port);


  iBaudrate = iChipIoReadReg16 (port->chipio, eRegSerBaud);

  if (iBaudrate > 0) {

    iBaudrate *= 100;
  }
  return iBaudrate;
}

// -----------------------------------------------------------------------------
eSerialDataBits
eChipIoSerialGetDataBits (xChipIoSerial * port) {
  eSerialDataBits eDataBits = SERIAL_DATABIT_UNKNOWN;
  int iSerCr;

  assert (port);

  iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


  if (iSerCr > 0) {
    switch (iSerCr & eDataBitMask) {
      case eDataBit5:
        eDataBits = SERIAL_DATABIT_5;
        break;
      case eDataBit6:
        eDataBits = SERIAL_DATABIT_6;
        break;
      case eDataBit7:
        eDataBits = SERIAL_DATABIT_7;
        break;
      case eDataBit8:
        eDataBits = SERIAL_DATABIT_8;
        break;
      default:
        break;
    }
  }
  return eDataBits;
}

// -----------------------------------------------------------------------------
eSerialStopBits
eChipIoSerialGetStopBits (xChipIoSerial * port) {
  eSerialStopBits eStopBits = SERIAL_PARITY_UNKNOWN;
  int iSerCr;

  assert (port);

  iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


  if (iSerCr > 0) {
    switch (iSerCr & eStopBitMask) {
      case eStopBit1:
        eStopBits = SERIAL_STOPBIT_ONE;
        break;
      case eStopBit2:
        eStopBits = SERIAL_STOPBIT_TWO;
        break;
      default:
        break;
    }
  }
  return eStopBits;
}

// -----------------------------------------------------------------------------
eSerialParity
eChipIoSerialGetParity (xChipIoSerial * port) {
  eSerialParity eParity = SERIAL_PARITY_UNKNOWN;
  int iSerCr;

  assert (port);

  iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


  if (iSerCr > 0) {
    switch (iSerCr & eParityMask) {
      case eParityNone:
        eParity = SERIAL_PARITY_NONE;
        break;
      case eParityEven:
        eParity = SERIAL_PARITY_EVEN;
        break;
      case eParityOdd:
        eParity = SERIAL_PARITY_ODD;
        break;
      default:
        break;
    }
  }
  return eParity;
}

// -----------------------------------------------------------------------------
eSerialFlow
eChipIoSerialGetFlow (xChipIoSerial * port) {
  eSerialParity eFlow = SERIAL_FLOW_UNKNOWN;
  int iSerCr;

  assert (port);

  iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


  if (iSerCr > 0) {
    switch (iSerCr & eFlowMask) {
      case eFlowNone:
        eFlow = SERIAL_FLOW_NONE;
        break;
      case eFlowHard:
        eFlow = SERIAL_FLOW_RTSCTS;
        break;
      case eFlowSoft:
        eFlow = SERIAL_FLOW_XONXOFF;
        break;
      default:
        break;
    }
  }
  return eFlow;
}

// -----------------------------------------------------------------------------
int
iChipIoSerialSetBaudrate (xChipIoSerial * port, int iBaudrate) {
  int iRet;
  assert (port);

  iRet = iChipIoWriteReg16 (port->chipio, eRegSerBaud, iBaudrate / 100);

  if (iRet == 0) {

    return iChipIoSerialGetBaudrate (port);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eSerialDataBits
eChipIoSerialSetDataBits (xChipIoSerial * port, eSerialDataBits eDataBits) {

  assert (port);
  if (eChipIoSerialGetDataBits (port) != eDataBits) {
    int iSerCr, iRet;


    iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


    if (iSerCr < 0) {
      return SERIAL_DATABIT_UNKNOWN;
    }

    iSerCr &= ~eDataBitMask;
    switch (eDataBits) {

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
        return SERIAL_DATABIT_UNKNOWN;
    }

    iRet = iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);

    if (iRet < 0) {
      return SERIAL_DATABIT_UNKNOWN;
    }
  }
  return eDataBits;
}

// -----------------------------------------------------------------------------
eSerialStopBits
eChipIoSerialSetStopBits (xChipIoSerial * port, eSerialStopBits eStopBits) {

  assert (port);
  if (eChipIoSerialGetStopBits (port) != eStopBits) {
    int iSerCr, iRet;


    iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


    if (iSerCr < 0) {
      return SERIAL_STOPBIT_UNKNOWN;
    }

    iSerCr &= ~eStopBitMask;
    switch (eStopBits) {

      case SERIAL_STOPBIT_ONE:
        iSerCr |= eStopBit1;
        break;
      case SERIAL_STOPBIT_TWO:
        iSerCr |= eStopBit2;
        break;
      default:
        return SERIAL_STOPBIT_UNKNOWN;
    }

    iRet = iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);

    if (iRet < 0) {
      return SERIAL_STOPBIT_UNKNOWN;
    }
  }
  return eStopBits;
}

// -----------------------------------------------------------------------------
eSerialParity
eChipIoSerialSetParity (xChipIoSerial * port, eSerialParity eParity) {

  assert (port);
  if (eChipIoSerialGetParity (port) != eParity) {
    int iSerCr, iRet;


    iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


    if (iSerCr < 0) {
      return SERIAL_PARITY_UNKNOWN;
    }

    iSerCr &= ~eParityMask;
    switch (eParity) {

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
        return SERIAL_PARITY_UNKNOWN;
    }

    iRet = iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);

    if (iRet < 0) {
      return SERIAL_PARITY_UNKNOWN;
    }
  }
  return eParity;
}

// -----------------------------------------------------------------------------
eSerialFlow
eChipIoSerialSetFlow (xChipIoSerial * port, eSerialFlow eFlow) {

  assert (port);
  if (eChipIoSerialGetFlow (port) != eFlow) {
    int iSerCr, iRet;


    iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);


    if (iSerCr < 0) {
      return SERIAL_FLOW_UNKNOWN;
    }

    iSerCr &= ~eFlowMask;
    switch (eFlow) {

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
        return SERIAL_FLOW_UNKNOWN;
    }

    iRet = iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);

    if (iRet < 0) {
      return SERIAL_FLOW_UNKNOWN;
    }
  }
  return eFlow;
}

/* ========================================================================== */
