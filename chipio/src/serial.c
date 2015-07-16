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
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <sysio/delay.h>
#include <sysio/log.h>
#include <sysio/doutput.h>
#include <chipio/serial.h>
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
static int
iBufferSize (xChipIoSerial * port) {

  int iRet = iChipIoReadReg8 (port->chipio, eRegSerTxSr);
  if (iRet >= 0) {
    iRet = MIN((iRet & ~eStatusBusy), CHIPIO_I2C_BLOCK_MAX);
  }
  return iRet;
}

// -----------------------------------------------------------------------------
static int
iChipIsBusy (xChipIoSerial * port) {

  int iRet = iChipIoReadReg8 (port->chipio, eRegSerTxSr);
  if (iRet >= 0) {
    iRet &= eStatusBusy;
  }
  return (iRet != 0);
}


// -----------------------------------------------------------------------------
static int
iChipIoWriteSerialAttr (xChipIoSerial * port, struct termios * newts,
                                              struct termios * oldts) {
  int i, iSerCr;
  bool bCrChanged = false;

  i = iSerialTermiosBaudrate (newts);
  if (i != iSerialTermiosBaudrate (oldts)) {
    if (iChipIoWriteReg16 (port->chipio, eRegSerBaud, i / 100) != 0) {
      return -1;
    }
    PDEBUG("Baudrate changed");
  }

  iSerCr = iChipIoReadReg16 (port->chipio, eRegSerCr);
  if (i < 0) {
    return -1;
  }

  i = iSerialTermiosDataBits (newts);
  if (i != iSerialTermiosDataBits (oldts)) {

    bCrChanged = true;
    iSerCr &= ~eDataBitMask;
    switch (i) {

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
        return -1;
    }
    PDEBUG("Data bits changed");
  }

  i = iSerialTermiosStopBits (newts);
  if (i != iSerialTermiosStopBits (oldts)) {

    bCrChanged = true;
    iSerCr &= ~eStopBitMask;
    switch (i) {

      case SERIAL_STOPBIT_ONE:
        iSerCr |= eStopBit1;
        break;
      case SERIAL_STOPBIT_TWO:
        iSerCr |= eStopBit2;
        break;
      default:
        return -1;
    }
    PDEBUG("Stop bits changed");
  }

  i = iSerialTermiosParity (newts);
  if (i != iSerialTermiosParity (oldts)) {

    bCrChanged = true;
    iSerCr &= ~eParityMask;
    switch (i) {

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
        return -1;
    }
    PDEBUG("Parity changed");
  }

  i = iSerialTermiosFlow (newts);
  if (i != iSerialTermiosFlow (oldts)) {

    bCrChanged = true;
    iSerCr &= ~eFlowMask;
    switch (i) {

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
        return -1;
    }
    PDEBUG("Flow control changed");
  }

  if (bCrChanged) {

    return iChipIoWriteReg16 (port->chipio, eRegSerCr, iSerCr);
  }

  return 0;
}

// -----------------------------------------------------------------------------
static int
iChipIoReadSerialAttr (xChipIoSerial * port, struct termios * ts) {
  int i;

  // Baudrate
  i = iChipIoReadReg16 (port->chipio, eRegSerBaud);
  if (i < 0) {
    return i;
  }

  speed_t baud;
  switch (i) {
    case 2:
      baud = B200;
      break;
    case 3:
      baud = B300;
      break;
    case 6:
      baud = B600;
      break;
    case 12:
      baud = B1200;
      break;
    case 18:
      baud = B1800;
      break;
    case 24:
      baud = B2400;
      break;
    case 48:
      baud = B4800;
      break;
    case 96:
      baud = B9600;
      break;
    case 192:
      baud = B19200;
      break;
    case 384:
      baud = B38400;
      break;
    case 576:
      baud = B57600;
      break;
    case 1152:
      baud = B115200;
      break;
    case 2304:
      baud = B230400;
      break;
    case 4608:
      baud = B460800;
      break;
    case 9216:
      baud = B921600;
      break;
    default:
      baud = B0;
      break;
  }

  if (baud != B0) {
    if (cfsetispeed (ts, baud) != 0) {
      return -1;
    }
    if (cfsetospeed (ts, baud) != 0) {
      return -1;
    }
  }
  else {
    // TODO Custom baudrate
    return EBADBAUD;
  }

  // Clear des paramètres
  ts->c_cflag &= ~(PARENB | PARODD);
  ts->c_cflag &= ~CSTOPB;
  ts->c_cflag &= ~CSIZE;
  ts->c_cflag &= ~CRTSCTS;
  ts->c_iflag &= ~(IXON | IXOFF | IXANY);

  i = iChipIoReadReg16 (port->chipio, eRegSerCr);
  if (i < 0) {
    return i;
  }

  // Data bits
  switch (i & eDataBitMask) {
    case eDataBit5:
      ts->c_cflag |= CS5;
      break;
    case eDataBit6:
      ts->c_cflag |= CS6;
      break;
    case eDataBit7:
      ts->c_cflag |= CS7;
      break;
    case eDataBit8:
      ts->c_cflag |= CS8;
      break;
    default:
      break;
  }

  // Stop bits
  switch (i & eStopBitMask) {
    case eStopBit2:
      ts->c_cflag |= CSTOPB;
      break;
    default:
      break;
  }

  // Parity
  switch (i & eParityMask) {
    case eParityEven:
      ts->c_cflag |= PARENB;
      break;
    case eParityOdd:
      ts->c_cflag |= PARENB | PARODD;
      break;
    default:
      break;
  }

  // Flow control
  switch (i & eFlowMask) {
    case eFlowHard:
      ts->c_cflag |= CRTSCTS;
      break;
    case eFlowSoft:
      ts->c_iflag |= IXON | IXOFF | IXANY;
      break;
    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// Thread de surveillance du port
static void *
pvSerialThread (void * xContext) {
  int iLen, iRet;
  fd_set xFdSet;
  struct timeval xTv;
  uint8_t buffer[CHIPIO_I2C_BLOCK_MAX];
  bool bIrqPending = false;
  struct termios xCurrentTs, xNewTs;

  xChipIoSerial * port = (xChipIoSerial *) xContext;
  tcgetattr (port->fdm, &xCurrentTs);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  vLedDebugInit();

  for (;;) {

    vLedDebugToggle (LED_GREEN);

    // Surveillance configuration
    tcgetattr (port->fdm, &xNewTs);
    if (memcmp (&xCurrentTs, &xNewTs, sizeof(struct termios)) != 0) {

      PDEBUG ("Config changed ! %s", sSerialTermiosToStr (&xNewTs)); fflush(stdout);
      iChipIoWriteSerialAttr (port, &xNewTs, &xCurrentTs);
      memcpy (&xCurrentTs, &xNewTs, sizeof(struct termios));
    }

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

      if  (iChipIsBusy(port) == false) {
        int iBytesAvailable = iChipIoReadReg8 (port->chipio, eRegSerRxSr);

        if (iBytesAvailable > 0) {
          int iBlockSize;

          vLedDebugSet (LED_YELLOW);

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
      vLedDebugClear (LED_YELLOW);
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

        if  (iChipIsBusy(port) == false) {
          int iBlockSize;

          // le port série est prêt à transmettre, 32 octets max.
          vLedDebugSet (LED_RED);

          iBlockSize = MIN(CHIPIO_I2C_BLOCK_MAX, iBytesAvailable);
          if  ((iLen = read (port->fdm, buffer, iBlockSize)) > 0) {

            // on envoie un bloc de données
            iRet = iChipIoWriteRegBlock (port->chipio, eRegSerTx, buffer, iLen);
          }
          vLedDebugClear (LED_RED);
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
xChipIoSerialNew (xChipIo * chip, xDin * xIrqPin) {
  xChipIoSerial * port;
  char * name;

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

  /* Configuration en mode Raw, sans cela le pseudo-terminal est en mode
   * canonique, c'est à dire que les échanges se font lignes par lignes */
  struct termios ts;

  if (tcgetattr (port->fdm, &ts)) {

    goto open_error_exit;
  }

  cfmakeraw (&ts);
  ts.c_cc [VMIN]  = 0;
  ts.c_cc [VTIME] = 0;

  if (iChipIoReadSerialAttr (port, &ts) != 0) {

    goto open_error_exit;
  }

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

  iRet = close (port->fdm);
  assert (iRet == 0);

  free (port);
}

// -----------------------------------------------------------------------------
const char *
sChipIoSerialPortName (xChipIoSerial * port) {

  assert (port);
  return ptsname (port->fdm);
}

/* ========================================================================== */
