/*
 * test/serial/serial.c
 * @brief Test liaison série
 *
 * La liaison est bouclée sur elle-même (TXD relié à RXD).
 * Le programme envoie un carcatère sur la liaison série (BYTE2PUT) puis
 * scrute la réception de façon périodique (STEP_MS):
 * - si caractère correct reçu: bascule l'état de la led verte,
 * - si caractère incorrect reçu: allume la led rouge
 * - si aucun caractère au bout du timeout: allume la led jaune
 * 
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <sysio/serial.h>
#include <sysio/doutput.h>
#include <sysio/delay.h>

/* constants ================================================================ */
#define BAUDRATE    38400
#define DEVICE      "/dev/ttyAMA0"
#define BYTE2PUT    'U'
#define TIMEOUT_MS  100
#define STEP_MS     1

#define LED_RED     0
#define LED_YELLOW  1
#define LED_GREEN   2

/* private variables ======================================================== */
static int iSerialFd;
static FILE * xSerialPort;
unsigned uRxError, uTimeoutError, uRxCharCount, uTxCharCount;


/* private variables ======================================================== */
static const xDout xMyLeds[3] = {
  { .num = 0, .act = true },  // red
  { .num = 1, .act = true },  // yellow
  { .num = 2, .act = true }}; // green

xDoutPort * xLedPort;

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Affiche les statistiques
static void
vPrintStats (void) {
  double dRx = uRxCharCount;
  double dTx = uTxCharCount;

  printf ("%u chars transmitted, %u received, %.2f%% char loss\n",
    uTxCharCount,
    uRxCharCount,
    (dTx - dRx) / dTx * 100.0);
}

// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  vPrintStats();
  fclose (xSerialPort);
  printf("\n%s closed.\nHave a nice day !\n", DEVICE);
  exit(EXIT_SUCCESS);
}

// ------------------------------------------------------------------------------
static void
vSetup (int iBaudrate) {


  xLedPort = xDoutOpen (xMyLeds, 3);
  assert(xLedPort);

  if ((iSerialFd = iSerialOpen (DEVICE, iBaudrate)) < 0) {

    perror ("serialOpen failed !");
    exit (EXIT_FAILURE);
  }

  if ((xSerialPort = fdopen (iSerialFd, "w+")) == NULL) {

    perror ("fdopen failed !");
    exit (EXIT_FAILURE);
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);
}

/* internal public functions ================================================ */
int
main (int argc, char **argv) {
  int c;
  int iBaudrate = BAUDRATE; // Valeur par défaut
  int iTimeout = 0;

  if (argc > 1) {

    int iBaud = atoi(argv[1]);
    switch (iBaud) {
      case     50: break;
      case     75: break;
      case    110: break;
      case    134: break;
      case    150: break;
      case    200: break;
      case    300: break;
      case    600: break;
      case   1200: break;
      case   1800: break;
      case   2400: break;
      case   4800: break;
      case   9600: break;
      case  19200: break;
      case  38400: break;
      case  57600: break;
      case 115200: break;
      case 230400: break;

      default:
        fprintf (stderr, "Illegal baudrate: %s\n", argv[1]);
        // exit (EXIT_FAILURE);
        break;
    }
    iBaudrate = iBaud;
  }

  vSetup (iBaudrate);
  printf ("Serial port test in progress on %s (baud=%d).\nPress CTRL+C to quit\n", DEVICE, iBaudrate);

  for (;;) {

    if (iTimeout <= 0) {
      
      // transmission
      if (fputc (BYTE2PUT, xSerialPort) != EOF) {
        
        iTimeout = TIMEOUT_MS;
        iDoutClear (LED_RED, xLedPort);
        iDoutClear (LED_YELLOW, xLedPort);
        uTxCharCount++;
      }
      else {
        // Erreur de transmission
        perror ("fputc");
        clearerr (xSerialPort);
      }
    }
    else {
      
      // Attente réception
      c = fgetc (xSerialPort);
      if (c != EOF) {

        // Caractère reçu sans erreur
        if (c == BYTE2PUT) {

          // C'est le bon
          iDoutToggle (LED_GREEN, xLedPort);
          uRxCharCount++;
          if (uTxCharCount % 1000 == 0) {
            
            vPrintStats();
          }
        }
        else {

          // Caractère incorrect
          uRxError++;
          fprintf (stderr, "E%u: Character received in error, expected %c, %c (0x%X) received\n", uRxError, BYTE2PUT, c, c);
          iDoutSet (LED_RED, xLedPort);
          delay_ms (300);
        }
        iTimeout = 0;
      }
      else {
        
        // pas de caractère ou erreur de réception
        if (ferror (xSerialPort)) {

          // erreur de réception
          perror ("fgetc");
          clearerr (xSerialPort);
        }
        // Temporisation
        delay_ms (STEP_MS);
        iTimeout -= STEP_MS;
        if (iTimeout <= 0) {

          // Erreur de timeout
          uTimeoutError++;
          fprintf (stderr, "E%u: Timeout error, no character received, have you connected TXD to RXD ?\n", uTimeoutError);
          iDoutSet (LED_YELLOW, xLedPort);
          fflush (xSerialPort);
          delay_ms (300);
        }
      }
    }
  }

  return 0;
}
