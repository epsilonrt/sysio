/*
 * ssend.c
 * Send a file to the serial port (as binary).
 *
 * Copyright © 2016 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>
#include <sysio/delay.h>
#include <sysio/serial.h>
#include <sysio/log.h>
#include "version-git.h"

/* constants ================================================================ */
#define AUTHORS "Pascal JEAN"
#define WEBSITE "http://gitweb.epsilonrt.com/sysio.git"

/* default values =========================================================== */
#ifdef ARCH_ARM_RASPBERRYPI
#define DEFAULT_PORT "/dev/ttyAMA0"
#else
#define DEFAULT_PORT "/dev/ttyUSB0"
#endif

#define DEFAULT_BAUDRATE 1200
#define DEFAULT_DATABIT  7
#define DEFAULT_STOPBIT  1
#define DEFAULT_PARITY  'E'
#define DEFAULT_FLOW    SERIAL_FLOW_NONE

/* extremum values ========================================================== */
#define MIN_DATABIT 5
#define MAX_DATABIT 8
#define STOPBIT_LIST { 1, 2 }
#define PARITY_LIST { 'N', 'E', 'O' }
#define MAX_BAUDRATE 921600
#define MIN_BAUDRATE 50

/* internal public functions ================================================ */
int iCheckValue (int iValue, const int * iList, int iDefault);
void vVersion (void);
void vHelp (FILE *stream, int exit_msg);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  char c;
  int fd, count = 0;
  int iNextOption;
  FILE * f;
  const char sDefaultPort[] = DEFAULT_PORT;
  const char * sPort = sDefaultPort;
  const char * sFilename = NULL;

  xSerialIos ios;
  const int iStopBitList[] = STOPBIT_LIST;
  const int iParityList[]  = PARITY_LIST;
  const char *short_options = "b:d:s:p:f:D:hv";
  static const struct option long_options[] = {
    {"baudrate",  required_argument, NULL, 'b'},
    {"device",  required_argument, NULL, 'D'},
    {"databit",  required_argument, NULL, 'd'},
    {"stopbit",  required_argument, NULL, 's'},
    {"parity",  required_argument, NULL, 'p'},
    {"flow",  required_argument, NULL, 'f'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };

  /* Paramètres */
  ios.baud =       DEFAULT_BAUDRATE;
  ios.dbits =      DEFAULT_DATABIT;
  ios.sbits =      DEFAULT_STOPBIT;
  ios.parity  =    DEFAULT_PARITY;
  ios.flow =       DEFAULT_FLOW;
  ios.flag =       0;

  /* Traitement paramètres ligne de commande */
  do {
    iNextOption = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (iNextOption) {

      case 'b':
        ios.baud = atoi (optarg);
        if ( ( (unsigned long) ios.baud < MIN_BAUDRATE) ||
             ( (unsigned long) ios.baud > MAX_BAUDRATE)) {
          ios.baud = DEFAULT_BAUDRATE;
          fprintf (stderr, "Warning: baudrate out of range {%d,%d} !\n",
                   MIN_BAUDRATE, MAX_BAUDRATE);
        }
        //printf("baudrate=%d\n", ios.baud);
        break;

      case 'D':
        sPort = optarg;
        break;

      case 'd':
        ios.dbits = atoi (optarg);
        if ( (ios.dbits < MIN_DATABIT) ||
             (ios.dbits > MAX_DATABIT)) {
          ios.dbits = DEFAULT_DATABIT;
          fprintf (stderr, "Warning: databit out of range {%d,%d} !\n",
                   MIN_DATABIT, MAX_DATABIT);
        }
        //printf("databit=%d\n", ios.dbits);
        break;

      case 's':
        ios.sbits = atoi (optarg);
        ios.sbits = iCheckValue (ios.sbits, iStopBitList, DEFAULT_STOPBIT);
        //printf("stopbit=%d\n", ios.sbits);
        break;

      case 'p':
        ios.parity = atoi (optarg);
        ios.parity = iCheckValue (ios.parity, iParityList, DEFAULT_PARITY);
        //printf("parity=%c\n", ios.parity);
        break;

      case 'f':
        if (strcasecmp (optarg, "rtscts") == 0) {
          ios.flow = SERIAL_FLOW_RTSCTS;
        }
        else if (strcasecmp (optarg, "xonxoff") == 0) {
          ios.flow = SERIAL_FLOW_XONXOFF;
        }
        else if (strcasecmp (optarg, "rs485") == 0) {
          ios.flow = SERIAL_FLOW_RS485_RTS_AFTER_SEND;
        }
        else if (strcasecmp (optarg, "rs485on") == 0) {
          ios.flow = SERIAL_FLOW_RS485_RTS_ON_SEND;
        }
        break;

      case 'h':
        vHelp (stdout, EXIT_SUCCESS);
        break;

      case 'v':
        vVersion();
        break;

      case '?': /* An invalide option has been used,
        print help an exit with code EXIT_FAILURE */
        vHelp (stderr, EXIT_FAILURE);
        break;
    }
  }
  while (iNextOption != -1);

  if (optind < argc)    {

    sFilename = argv[optind];
  }
  else {

    fprintf (stderr, "You must provide a file name !\n");
    vHelp (stderr, EXIT_FAILURE);
  }

  /* Début du traitement */
  f = fopen (sFilename, "rb");
  if (f == NULL) {

    perror ("fopen");
    exit (EXIT_FAILURE);
  }

  if ( (fd = iSerialOpen (sPort, &ios)) < 0) {

    perror ("open");
    exit (EXIT_FAILURE);
  }

  printf ("Send %s on %s %s\nPlease wait until the end of the transmission...\n",
          sFilename, sPort, sSerialAttrStr (fd));
  while (fread (&c, 1, 1, f) > 0) {

    if (write (fd, &c, 1) < 0) {
      perror ("write");
      exit (EXIT_FAILURE);
    }
    else {
      
      count++;
    }
  }

  if (close (fd) < 0) {

    perror ("close");
    exit (EXIT_FAILURE);
  }

  printf ("\n%d bytes were sent.\n", count);
  exit (EXIT_SUCCESS);

  return 0;
}

// -----------------------------------------------------------------------------
void
vVersion (void)  {
  printf ("you are running version %s\n", VERSION_SHORT);
  printf ("this program was developped by %s.\n", AUTHORS);
  printf ("you can find some information on this project page at %s\n", WEBSITE);
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vHelp (FILE *stream, int exit_msg) {
  fprintf (stream, "usage : %s [ options ] filename [ options ]\n\n", __progname);
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "Send a file to the serial port (as binary).\n\n");
  fprintf (stream, "valid options are :\n");
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "  -D  --device    \tSpecifies the serial device name (port).\n"
           "                  \tThe default is %s.\n"
           "  -b  --baudrate  \tSpecifies the baudrate.\n"
           "                  \tThe default is %d bauds. The range is from %d to %d bauds.\n"
           "  -d  --databit   \tSpecifies the number of data bits.\n"
           "                  \tThe default is %d. The range is from %d to %d bits.\n"
           "  -p  --parity    \tSpecifies the parity.\n"
           "                  \t\t{N, O, E}, the default is %c.\n"
           "  -s  --stopbit   \tSpecifies the number of data bits.\n"
           "                  \t\t{1, 2}, the default is %d.\n"
           "  -f  --flow      \tSpecifies the flow control.\n"
           "                  \t\t{none, rtscts, xonxoff, rs485, rs485on}\n"
           "                  \tThe default is %s.\n"
           "  -h  --help      \tPrint this message\n"
           "  -v  --version   \tPrint version and exit\n"
           , DEFAULT_PORT
           , DEFAULT_BAUDRATE
           , MIN_BAUDRATE
           , MAX_BAUDRATE
           , DEFAULT_DATABIT
           , MIN_DATABIT
           , MAX_DATABIT
           , DEFAULT_PARITY
           , DEFAULT_STOPBIT
           , sSerialFlowToStr (DEFAULT_FLOW));
  exit (exit_msg);
}

// -----------------------------------------------------------------------------
int
iCheckValue (int iValue, const int * piList, int iDefault) {

  while (*piList != -1) {

    if (*piList++ == iValue) {
      return iValue;
    }
  }
  return iDefault;
}

/* ========================================================================== */
