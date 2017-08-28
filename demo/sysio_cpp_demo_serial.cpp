/**
 * @file 
 * @brief Démonstration Classe Serial
 * 
 * @todo Passer ce code en C++ pur !
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/serial.hpp>

/* constants ================================================================ */
#define BAUDRATE 38400
#define TRANSMIT_PERIOD 3

/* global variables ========================================================= */
Serial * serial;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Intercepts the CTRL + C signal and closes all properly
static void
vSigIntHandler (int sig) {

  serial->close();
  delete serial;
  printf("\nserial port closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* internal public functions ================================================ */
int
main (int argc, char **argv) {
  int iBaudrate = BAUDRATE;
  const char * cDev = "";
  const char msg[] = "Hello world !";

  if (argc < 2) {

    printf("Usage: %s /dev/ttyAMA0 [baud]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  cDev = argv[1];

  if (argc > 2) {

    iBaudrate = atoi(argv[2]);
  }

  serial = new Serial (cDev, iBaudrate);
  // Opens the serial port
  if (!serial->open()) {

    perror ("iSerialOpen:");
    exit (EXIT_FAILURE);
  }

  
  // Installs the CTRL + C signal handler
  signal(SIGINT, vSigIntHandler);

  printf ("Send on %s (baud=%d).\nPress CTRL+C to quit\n\n", cDev, iBaudrate);

  for (;;) {
    
    write (serial->fileno(), msg, strlen(msg));
    delay_ms(TRANSMIT_PERIOD * 1000);
  }
  return 0;
}

/* ========================================================================== */
