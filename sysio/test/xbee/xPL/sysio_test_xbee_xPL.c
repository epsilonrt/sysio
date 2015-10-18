/**
 * @file sysio_test_xbee_node.c
 * @brief Test noeud XBee
 * - Affiche le contenu des paquets de données reçus
 * - Transmet périodiquement un paquet xPL
 * .
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sysio/xbee.h>
#include <sysio/serial.h>
#include <sysio/delay.h>

#include <assert.h>

/* constants ================================================================ */
#define XBEE_BAUDRATE  38400
#define TX_INTERVAL_DELAY  5

/* private variables ======================================================== */
static xXBee xbee;
static int fd;
static const char * cDev;
static uint32_t ulSH;
static uint32_t ulSL;
static uint16_t usXBeePayloadSize;
static char * cXBeePayload;
static bool bXBeeIsReady = false;
volatile int frame_id = 0;

/* private functions ======================================================== */
int iDataCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);
int iTxStatusCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);
int iLocalAtCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);
void vSigExitHandler (int sig);
void vSigAlarmHandler (int sig);

/* internal public functions ================================================ */
int
main (int argc, char **argv) {
  int ret;
  xSerialIos xIosSet = {
    .baud = XBEE_BAUDRATE, .dbits = SERIAL_DATABIT_8, .parity = SERIAL_PARITY_NONE,
    .sbits = SERIAL_STOPBIT_ONE, .flow = SERIAL_FLOW_RTSCTS, .flag = 0
  };

  if (argc < 2) {

    printf ("Usage: %s device device [baud]\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  cDev = argv[1];
  if (argc > 2) {

    xIosSet.baud = atoi (argv[2]);
  }

  /*
   * Init liaison série vers module XBee
   * Mode lecture/écriture, non bloquant, avec contrôle de flux matériel
   */
  if ( (fd = iSerialOpen (cDev, &xIosSet)) < 0) {

    perror ("serialOpen failed !");
    exit (EXIT_FAILURE);
  }

  printf ("*** SysIO XBee xPL Test ***\n");

  /*
   * Init XBee, mise en place des gestionnaires de réception
   */
  ret = iXBeeInit (&xbee, XBEE_SERIES_S2, fd);
  assert (ret == 0);
  vXBeeSetCB (&xbee, XBEE_CB_DATA, iDataCB);
  vXBeeSetCB (&xbee, XBEE_CB_TX_STATUS, iTxStatusCB);
  vXBeeSetCB (&xbee, XBEE_CB_AT_LOCAL, iLocalAtCB);

  ret = iXBeeSendAt (&xbee, "SH", NULL, 0);
  assert (ret >= 0);
  ret = iXBeeSendAt (&xbee, "SL", NULL, 0);
  assert (ret >= 0);
  ret = iXBeeSendAt (&xbee, "NP", NULL, 0);
  assert (ret >= 0);

  signal (SIGTERM, vSigExitHandler);
  signal (SIGINT,  vSigExitHandler);
  signal (SIGALRM, vSigAlarmHandler);
  alarm (1);

  for (;;) {

    // Scrute la réception des paquets
    ret = iXBeePoll (&xbee, 10);
    assert (ret == 0);
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Gestionnaire de réponse à une commande AT locale
int
iLocalAtCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len) {

  char * cCmd = pcXBeePktCommand (pkt);

  if (iXBeePktStatus (pkt) == XBEE_PKT_STATUS_OK) {
    int ret;

    if (strncmp (cCmd, "SH", 2) == 0) {

      ret = iXBeePktParamGetULong (&ulSH, pkt, 0);
      assert (ret == 0);
    }
    else if (strncmp (cCmd, "SL", 2) == 0) {

      ret = iXBeePktParamGetULong (&ulSL, pkt, 0);
      assert (ret == 0);
      printf ("My serial number is 0x%08x%08x\n", ulSH, ulSL);
    }
    else if (strncmp (cCmd, "NP", 2) == 0) {

      ret = iXBeePktParamGetUShort (&usXBeePayloadSize, pkt, 0);
      assert (ret == 0);
      cXBeePayload = malloc (usXBeePayloadSize);
      assert(cXBeePayload);
      bXBeeIsReady = true;
      printf ("XBee payload size is %d bytes\n", usXBeePayloadSize);
    }
  }
  else {
    // Erreur
    fprintf (stderr, "Local AT command %2s failed with 0x%02X status\n",
             cCmd, iXBeePktStatus (pkt));
  }
  vXBeeFreePkt (xbee, pkt);
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
    uint8_t * src64 = pucXBeePktAddrSrc64 (pkt);

    // Affiche l'adresse 64-bits de la source et le contenu du paquet
    for (int i = 0; i < 8; i++) {

      printf ("%02X", src64[i]);
    }

    // Indique si le paquet est en broadcast
    if (iXBeePktIsBroadcast (pkt)) {
      putchar ('*');
    }

    // puis le contenu du paquet
    p = (char *) pucXBeePktData (pkt);
    p[size] = 0; // Ajout d'un caractère de fin de chaine
    printf (">%s\n", p);
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

      printf (">Err. %d\n",status);
    }
    else {

      printf (">Ok\n");
    }
    frame_id = 0;
  }
  vXBeeFreePkt (xbee, pkt);
  return 0;
}

// -----------------------------------------------------------------------------
void
vSigExitHandler (int sig) {

  vSerialClose (fd);
  free (cXBeePayload);
  printf ("\n%s closed.\nHave a nice day !\n", cDev);
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vSigAlarmHandler (int sig) {

  if (bXBeeIsReady) {
    time_t tNow = time (NULL);
    struct tm * pxDecodedTime;
    char cStrTime[24];

    pxDecodedTime = localtime (&tNow);
    strftime (cStrTime, 24, "%Y%m%d%H%M%S", pxDecodedTime);
    
    snprintf (cXBeePayload, usXBeePayloadSize, 
    "xpl-stat\n{\nhop=1\nsource=episrt-xbee.%08x%08x\ntarget=*\n}\nclock.update\n{\ntime=%s\n}\n", 
    ulSH, ulSL, cStrTime);

    frame_id = iXBeeZbSendToCoordinator (&xbee, cXBeePayload, strlen (cXBeePayload));
    assert (frame_id >= 0);
    printf ("Frm%d[%ld]>\n%s", frame_id, strlen (cXBeePayload), cXBeePayload);
    alarm (TX_INTERVAL_DELAY);
  }
}

/* ========================================================================== */
