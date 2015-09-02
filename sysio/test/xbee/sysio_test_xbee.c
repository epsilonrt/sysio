/**
 * @file sysio_test_xbee.c
 * @brief Test du module XBee
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sysio/xbee.h>
#include <sysio/serial.h>
#include <sysio/delay.h>

/* constants ================================================================ */
#define DEVICE "/dev/ttyUSB0"
#define BAUDRATE  38400
#define DATABITS  SERIAL_DATABIT_8
#define PARITY    SERIAL_PARITY_NONE
#define STOPBITS  SERIAL_STOPBIT_ONE
#define FLOW      SERIAL_FLOW_RTSCTS

/* private variables ======================================================== */
static xXBee xbee;
static FILE * xSerialPort;
volatile int frame_id = 0;

/* private functions ======================================================== */
int iDataCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);
int iTxStatusCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);

// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  fclose (xSerialPort);
  printf ("\n%s closed.\nHave a nice day !\n", DEVICE);
  exit (EXIT_SUCCESS);
}

/* internal public functions ================================================ */
int
main (void) {
  int fd;
  static bool isStarted = false;
  xSerialIos xIosSet = {
    .baud = BAUDRATE, .dbits = DATABITS, .parity = PARITY,
    .sbits = STOPBITS, .flow = FLOW, .flag = 0
  };

  /*
   * Init liaison série vers module XBee
   * Mode lecture/écriture, non bloquant, avec contrôle de flux matériel
   */
  if ( (fd = iSerialOpen (DEVICE, &xIosSet)) < 0) {

    perror ("serialOpen failed !");
    exit (EXIT_FAILURE);
  }

  if ( (xSerialPort = fdopen (fd, "r+")) == 0) {

    perror ("serialOpen failed !");
    exit (EXIT_FAILURE);
  }

  /*
   * Init XBee, mise en place des gestionnaires de réception
   */
  vXBeeInit (&xbee, xSerialPort);
  vXBeeSetCB (&xbee, XBEE_CB_DATA, iDataCB);
  vXBeeSetCB (&xbee, XBEE_CB_TX_STATUS, iTxStatusCB);

  for (;;) {

    // Scrute la réception des paquets
    iXBeePoll (&xbee);

    frame_id = iXBeeZbSendToCoordinator (&xbee, message, strlen (message));
    assert (frame_id >= 0);
  }
  
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réception des paquets de données
int
iDataCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len) {
  int size;

  size = iXBeePktDataLen (pkt);
  if (size > 0) {
    volatile char * p;
    uint8_t * addr64 = pucXBeePktAddrSrc64 (pkt);

    // Affiche l'adresse 64-bits de la source et le contenu du paquet
    for (int i = 0; i < 8; i++) {

      printf ("%02X", addr64[i]);
    }
    p = (char *) pucXBeePktData (pkt);
    p[size] = 0;
    printf ("%s\n", p);
  }
  vXBeeFreePkt (xbee, pkt);
  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de status de fin de transmission
int
iTxStatusCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len) {

  if (iXBeePktFrameId (pkt) == frame_id) {
    int status = iXBeePktStatus (pkt);

    if (status) {

      printf ("Tx%d Err. %d", frame_id, status);
    }
    else {

      printf ("Tx%d Ok", frame_id);
    }
    frame_id = 0;
  }
  vXBeeFreePkt (xbee, pkt);
  return 0;
}

/* -----------------------------------------------------------------------------
 * Vérifie que la condition passée est vraie, sinon fait clignoter rapidement
 * la led 1 à l'infinie
 */
void
vLedAssert (int i) {

  if (!i) {

    for (;;) {

      vLedSet (LED_LED1);
      delay_ms (5);
      vLedClear (LED_LED1);
      delay_ms (25);
    }
  }
}
/* ========================================================================== */
