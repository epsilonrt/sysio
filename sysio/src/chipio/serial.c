/**
 * @file sysio/chipio/serial.c
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sysio/delay.h>
#include <sysio/log.h>
#include <sysio/chipio/serial.h>
#include "interface.h"

/* macros =================================================================== */
/* constants ================================================================ */
#define CHIPIO_SERIAL_POLL_DELAY 20
#define CHIPIO_SERIAL_BUFFER_SIZE 64

/* structures =============================================================== */
typedef struct xChipIoSerial {
  xChipIo * chipio;   // Objet chipio
  int fdm;            // descripteur de fichier pseudo-terminal maître
  FILE * pts;         // flux pseudo-terminal esclave
  pthread_t thread;   // thread de surveillance
  bool run;           // indique au thread de continuer
} xChipIoSerial;

/* types ==================================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Thread de surveillance du port
static void *
pvSerialThread (void * xContext) {
  int iLen, iRet;
  fd_set xFdSet;
  struct timeval xTv;
  uint8_t buffer[CHIPIO_SERIAL_BUFFER_SIZE];

  xChipIoSerial * port = (xChipIoSerial *) xContext;
  FD_ZERO (&xFdSet);
  FD_SET (port->fdm, &xFdSet);

  while (port->run) {

    // Surveillance des octets reçus par la liaison série et envoi vers ptm
    // TODO utilisation de la broche IRQ
    
    iLen = iChipIoReadReg8 (port->chipio, eRegSerSr);
    
    if (iLen < 0) {

      PERROR ("iChipIoReadReg8()");
    }
    else if (iLen > 0) {

      
      iRet = iChipIoReadRegBlock (port->chipio, eRegSerRx, buffer, iLen);
      
      if (iRet < 0) {

        PERROR ("iChipIoReadRegBlock()");
      }
      else if (iRet > 0) {
        if (write (port->fdm, buffer, iRet) != iRet) {
          PERROR ("write()");
        }
      }
    }

    // Surveillance des octets reçus de ptm et envoi vers la liaison série
    xTv.tv_sec = 0;
    xTv.tv_usec = CHIPIO_SERIAL_POLL_DELAY * 1000UL;

    iRet = select (port->fdm + 1, &xFdSet, NULL, NULL, &xTv);
    // Considérer xTv comme indéfini maintenant ! 

    if (iRet == -1) {
      
      PERROR ("select()");
    }
    else if (iRet > 0) {
      
      // Des données sont disponibles maintenant
      while ( (iLen = read (port->fdm, buffer, CHIPIO_SERIAL_BUFFER_SIZE)) > 0) {

        
        iRet = iChipIoWriteRegBlock (port->chipio, eRegSerTx, buffer, iLen);
        
        if (iRet < 0) {

          PERROR ("iChipIoWriteRegBlock()");
        }
      }
    }
  }
  return NULL;
}

/* public variables ========================================================= */

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xChipIoSerial *
xChipIoSerialOpen (xChipIo * chip) {
  xChipIoSerial * port = malloc (sizeof (xChipIoSerial));

  if (port) {
    char * pts;

    port->chipio = chip;
    if ( (port->fdm = getpt ()) < 0) {

      goto error_exit;
    }
    
    // Partie à mettre dans le thread ?
    if ( (grantpt (port->fdm)) != 0) {

      goto error_exit;
    }
    if ( (unlockpt (port->fdm)) != 0) {

      goto error_exit;
    }
    if ( (pts = ptsname (port->fdm)) == NULL) {

      goto error_exit;
    }
    if ( (port->pts = fopen (pts, "r+")) == NULL) {

      goto error_exit;
    }
    // --- Fin de partie à mettre dans le thread
    
    // Création du thread
    port->run = true;
    if (pthread_create (&port->thread, NULL, pvSerialThread, port) != 0) {

      goto error_exit;
    }

  }
  return port;

error_exit:
  free (port);
  return NULL;
}

// -----------------------------------------------------------------------------
void
vChipIoSerialClose (xChipIoSerial * port) {

  assert (port);
  port->run = false;
  pthread_join (port->thread, NULL);
  fclose (port->pts);
  close (port->fdm);
  free (port);
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
