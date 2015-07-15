/**
 * @file sping.c
 * @brief Ping sur liaison série asynchrone
 *
 * Copyright © 2013-2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <sysio/log.h>
#include <sysio/delay.h>
#include <sysio/serial.h>
#include "version-git.h"

/* constants ================================================================ */
// #define VERSION 1.1
#define AUTHORS "Pascal JEAN"
#define WEBSITE "http://www.btssn.net/solarpi"

#define STX 2
#define ETX 3

/* Config. */
#define MIN_PACKET_SIZE 1
#define MAX_PACKET_SIZE 512

/* macros =================================================================== */

/* default values =========================================================== */
#ifdef ARCH_ARM_RASPBERRYPI
#define DEFAULT_DEVICE "/dev/ttyAMA0"
#else
#define DEFAULT_DEVICE "/dev/ttyUSB0"
#endif

#define DEFAULT_BAUDRATE 9600
#define DEFAULT_DATABIT  8
#define DEFAULT_STOPBIT  1
#define DEFAULT_PARITY  'N'
#define DEFAULT_PACKET_SIZE 56
#define DEFAULT_PACKET_COUNT    4
#define DEFAULT_PACKET_TIMEOUT  500
#define DEFAULT_PACKET_INTERVAL 1000

/* extremum values ========================================================== */
// Platform dependent !
#ifdef ARCH_ARM_RASPBERRYPI
// below is for Raspberry Pi
#   define F_CPU 700000000UL // Raspberry Pi System Clock: 700 MHz
#   define BAUDRATE(baudrate_reg) (unsigned long)(F_CPU/(8UL*(baudrate_reg+1UL)))
//
#   define MAX_BAUDRATE BAUDRATE(0)
#   define MIN_BAUDRATE BAUDRATE(0xFFFF)
#   define MIN_DATABIT 7
#   define MAX_DATABIT 8
#else
#   define MAX_BAUDRATE 115200
#   define MIN_BAUDRATE 50
#   define MIN_DATABIT 5
#   define MAX_DATABIT 8
#   define STOPBIT_LIST { 1, 2 }
#   define PARITY_LIST { 'N', 'E', 'O' }
#endif

/* structures =============================================================== */
typedef struct statistics {

  int iTxCount, iRxCount, iErrorCount;
  double dTimeSum;
} statistics;

/* private variables ======================================================== */
static const char * sMyName;
static int iFd;
static char *sDevice;
static bool bDevAlloc;
static char *sPingPacket;

/* statistiques */
static statistics xStats;

/* internal public functions ================================================ */
int iCheckValue (int iValue, const int * iList, int iDefault);
void vSigIntHandler (int);
void vStatistics (void);
void vVersion (void);
void vHelp (FILE *stream, int exit_msg);

/* main ===================================================================== */
int
main (int argc, char **argv) {

  /* Options ligne de commande */
  int iBaudrate, iPacketSize, iPacketCount, iPacketTimeout, iPacketInterval;
  int iDataBit, iStopBit, iParity;

#ifndef ARCH_ARM_RASPBERRYPI
  const int iStopBitList[] = STOPBIT_LIST;
  const int iParityList[]  = PARITY_LIST;
  const char *short_options = "b:d:t:p:s:c:W:i:hv";
  static const struct option long_options[] = {
    {"baudrate",  required_argument, NULL, 'b'},
    {"databit",  required_argument, NULL, 'd'},
    {"stopbit",  required_argument, NULL, 't'},
    {"parity",  required_argument, NULL, 'p'},
    {"packetsize",  required_argument, NULL, 's'},
    {"count",  required_argument, NULL, 'c'},
    {"timeout",  required_argument, NULL, 'W'},
    {"interval",  required_argument, NULL, 'i'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };
#else
  const char *short_options = "b:d:s:c:W:i:hv";
  static const struct option long_options[] = {
    {"baudrate",  required_argument, NULL, 'b'},
    {"databit",  required_argument, NULL, 'd'},
    {"packetsize",  required_argument, NULL, 's'},
    {"count",  required_argument, NULL, 'c'},
    {"timeout",  required_argument, NULL, 'W'},
    {"interval",  required_argument, NULL, 'i'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };
#endif

  /* variables de travail */
  int i, iCount, iPongLength;
  int iNextOption;

  struct timeval tStart;
  struct timeval tStop;
  double dRespTime;
  useconds_t tTransmittTime;

  bool bBad;
  char sPongPacket[1024];
  char *p;
  char c;

  signal (SIGINT, vSigIntHandler);


  /* Paramètres */
  iBaudrate =       DEFAULT_BAUDRATE;
  iDataBit =        DEFAULT_DATABIT;
  iStopBit =        DEFAULT_STOPBIT;
  iParity  =        DEFAULT_PARITY;
  iPacketSize =     DEFAULT_PACKET_SIZE;
  iPacketCount =    DEFAULT_PACKET_COUNT;
  iPacketTimeout  = DEFAULT_PACKET_TIMEOUT;
  iPacketInterval = DEFAULT_PACKET_INTERVAL;

  /* Traitement paramètres ligne de commande */
  sMyName = argv[0];
  do {
    iNextOption = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (iNextOption) {
      case 'b':
        iBaudrate = atoi (optarg);
        if ( ( (unsigned long) iBaudrate < MIN_BAUDRATE) ||
             ( (unsigned long) iBaudrate > MAX_BAUDRATE)) {
          iBaudrate = DEFAULT_BAUDRATE;
          fprintf (stderr, "Warning: baudrate out of range {%d,%d} !\n", MIN_BAUDRATE, MAX_BAUDRATE);
        }
        //printf("baudrate=%d\n", iBaudrate);
        break;
      case 'd':
        iDataBit = atoi (optarg);
        if ( (iDataBit < MIN_DATABIT) ||
             (iDataBit > MAX_DATABIT)) {
          iDataBit = DEFAULT_DATABIT;
          fprintf (stderr, "Warning: databit out of range {%d,%d} !\n", MIN_DATABIT, MAX_DATABIT);
        }
        /***************** TODO *****************/
        fprintf (stderr, "databit: not implemented yet feature !\n");
        //printf("databit=%d\n", iDataBit);
        break;
#ifndef ARCH_ARM_RASPBERRYPI
      case 't':
        iStopBit = atoi (optarg);
        iStopBit = iCheckValue (iStopBit, iStopBitList, DEFAULT_STOPBIT);
        fprintf (stderr, "stopbit: not implemented yet feature !\n");
        //printf("stopbit=%d\n", iStopBit);
        break;
      case 'p':
        iParity = atoi (optarg);
        iParity = iCheckValue (iParity, iParityList, DEFAULT_PARITY);
        fprintf (stderr, "parity: not implemented yet feature !\n");
        //printf("parity=%c\n", iParity);
        break;
#endif
      case 's':
        iPacketSize = atoi (optarg);
        if ( (iPacketSize < MIN_PACKET_SIZE) || (iPacketSize > MAX_PACKET_SIZE)) {
          iPacketSize = DEFAULT_PACKET_SIZE;
        }
        printf ("packetsize=%d\n", iPacketSize);
        break;
      case 'c':
        iPacketCount = atoi (optarg);
        //printf("count=%d\n", iPacketCount);
        break;
      case 'W':
        iPacketTimeout = atoi (optarg);
        //printf("timeout=%d\n", iPacketTimeout);
        break;
      case 'i':
        iPacketInterval = atoi (optarg);
        //printf("interval=%d\n", iPacketInterval);
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

    sDevice = argv[optind];
  }
  else {

    sDevice = malloc (strlen (DEFAULT_DEVICE) + 1);
    strcpy (sDevice, DEFAULT_DEVICE);
    bDevAlloc = true;
  }

  /* Début du traitement */
  iFd = iSerialOpen (sDevice, iBaudrate);
  if (iFd < 0) {

    fprintf (stderr, "Unable to open %s device: %s\n", sDevice, strerror (errno));
    if (bDevAlloc) {
      free (sDevice);
    }
    exit (EXIT_FAILURE);
  }

  sPingPacket = malloc (iPacketSize + 2);

  /* Init contenu paquet */
  sPingPacket[0] = STX;
  for (i = 1, c = 'A'; i <= iPacketSize; i++, c++) {

    if (c > 'Z') {
      c = 'A';
    }
    sPingPacket[i] = c;
  }
  sPingPacket[iPacketSize + 1] = ETX;

  tTransmittTime = ( (iPacketSize + 2) *
                     (iDataBit + iStopBit + (iParity == 'N' ? 0 : 1) + 1) *
                     1E6) / iBaudrate;
  printf ("SERIAL PING %d(%d) bytes of data. %d-%d%c%d. Timeout %d ms.\n",
          iPacketSize,
          iPacketSize + 2,
          iBaudrate,
          iDataBit,
          iParity,
          iStopBit,
          iPacketTimeout);
  //printf ("delay=%lu\n", (unsigned long)tTransmittTime);

  iCount = iPacketCount;
  while (iCount) {

    bBad = true;

    i = write (iFd, sPingPacket, iPacketSize + 2);
    xStats.iTxCount++;
    if (i < 0) {
      fprintf (stderr, "Unable to write %s device: %s\n", sDevice, strerror (errno));
      delay_ms (300);
      continue;
    }

    delay_us (tTransmittTime); // Durée pour l'envoi

    gettimeofday (&tStart, NULL);
    do {

      if (ioctl (iFd, FIONREAD, &iPongLength) == -1) {
        iPongLength = -1 ;
      }
      gettimeofday (&tStop, NULL);
      dRespTime  = (tStop.tv_sec - tStart.tv_sec) * 1000.0;      // sec to ms
      dRespTime += (tStop.tv_usec - tStart.tv_usec) / 1000.0;   // us to ms
      // Si réponse suffisament longue, sortie de boucle
      if (iPongLength >= (iPacketSize + 2)) {
        break;
      }
    }
    while (dRespTime < (double) iPacketTimeout);

    xStats.dTimeSum += dRespTime;

    if (iPongLength == 0) {

      printf ("From %s seq=%d Destination Host Unreachable\n", sDevice, xStats.iTxCount);
    }
    else {

      printf ("%d bytes from %s: seq %d ", iPongLength, sDevice, xStats.iTxCount);
      if (read (iFd, sPongPacket, iPongLength) == -1) {
        fprintf (stderr, "Unable to read %s device: %s\n", sDevice, strerror (errno));
      }

      if (iPongLength >= (iPacketSize + 2)) {

        if (strncmp (sPingPacket, sPongPacket, iPacketSize + 2) == 0) {

          xStats.iRxCount++;
          bBad = false;
        }
      }
    }

    if (bBad) {

      xStats.iErrorCount++;
      if (iPongLength > 0) {

        if (dRespTime < (double) iPacketTimeout) {
          printf ("Error: ");
        }
        else {
          printf ("Timeout: ");
        }
        p = sPongPacket;
        while (iPongLength--) {

          if (isprint (*p)) {
            putchar (*p);
          }
          else {

            printf ("\\x%02X\\", *p);
          }
          p++;
        }
        putchar ('\n');
      }
    }
    else {

      printf ("time=%.3f ms\n", dRespTime);
    }
    if (iCount != -1) {
      iCount--;
    }

    usleep (iPacketInterval * 1000);
  }

  vStatistics();
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vStatistics (void) {


  printf ("--- %s ping statistics ---\n", sDevice);
  printf ("%d packets transmitted, %d received, %d errors, %.1f%% packet loss, time sum. %.2fms - av. %.2fms\n",
          xStats.iTxCount,
          xStats.iRxCount,
          xStats.iErrorCount,
          (double) (xStats.iTxCount - xStats.iRxCount) * 100.0 / (double) xStats.iTxCount,
          xStats.dTimeSum, xStats.dTimeSum / (double) xStats.iTxCount);
  if (bDevAlloc) {
    free (sDevice);
  }
  free (sPingPacket);
  vSerialClose (iFd);
}

// -----------------------------------------------------------------------------
void
vSigIntHandler (int sig) {

  putchar ('\n');
  vStatistics();
  printf ("\n%s closed.\nHave a nice day !\n", sDevice);
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vVersion (void)  {
  printf ("you are running version %.s\n", VERSION);
  printf ("this program was developped by %s\n", AUTHORS);
  printf ("you can find some information on this project page at %s\n", WEBSITE);
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vHelp (FILE *stream, int exit_msg) {
  fprintf (stream, "usage : %s [ options ] [ device ] [ options ]\n\n", sMyName);
  fprintf (stream,
           "Send ping packet to serial device. The packet contains a start byte (STX: 0x02), "
           "packetsize bytes between 'A' and 'Z' and a trailing byte (ETX: 0x03).\n"
           "  device          \tOptionnal serial device name.\n"
           "                  \tThe default is %s.\n\n"
           , DEFAULT_DEVICE);
  fprintf (stream, "valid options are :\n");
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "  -b  --baudrate  \tSpecifies the baudrate.\n"
           "                  \tThe default is %d bauds. The range is from %d to %d bauds.\n"
           "  -c  --count     \tStop after sending count packets.\n"
           "                  \tThe default is %d.\n"
           "  -d  --databit   \tSpecifies the number of data bits.\n"
           "                  \tThe default is %d. The range is from %d to %d bits.\n"
           "  -h  --help      \tPrint this message\n"
           "  -i  --interval  \tWait interval milliseconds between sending each packet.\n"
           "                  \tThe default is %d ms.\n"
#ifndef ARCH_ARM_RASPBERRYPI
           "  -p  --parity    \tSpecifies the parity.\n"
           "                  \tThe default is %c.\n"
           "  -t  --stopbit   \tSpecifies the number of data bits.\n"
           "                  \tThe default is %d.\n"
#endif
           "  -s  --packetsize\tSpecifies the number of data bytes to be sent.\n"
           "                  \tThe default is %d. The range is from %d to %d bytes.\n"
           "  -W  --timeout   \tTime to wait for a response, in milliseconds.\n"
           "                  \tThe default is %d ms.\n"
           "  -v  --version   \tPrint version and exit\n"
           , DEFAULT_BAUDRATE
           , MIN_BAUDRATE
           , MAX_BAUDRATE
           , DEFAULT_PACKET_COUNT
           , DEFAULT_DATABIT
           , MIN_DATABIT
           , MAX_DATABIT
           , DEFAULT_PACKET_INTERVAL
#ifndef ARCH_ARM_RASPBERRYPI
           , DEFAULT_PARITY
           , DEFAULT_STOPBIT
#endif
           , DEFAULT_PACKET_SIZE
           , MIN_PACKET_SIZE
           , MAX_PACKET_SIZE
           , DEFAULT_PACKET_TIMEOUT);
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
