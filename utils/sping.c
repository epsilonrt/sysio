/**
 * @file sping.c
 * @brief Ping/Pong sur liaison série
 *
 * Copyright © 2013-2016 epsilonRT, All rights reserved.
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
#include "version.h"

/* constants ================================================================ */
#define AUTHORS "epsilonRT"
#define WEBSITE "http://www.epsilonrt.fr/sysio"

#define STX 2
#define ETX 3

/* Config. */
#define MIN_PACKET_SIZE 1
#define MAX_PACKET_SIZE 512

/* default values =========================================================== */
#ifdef BOARD_RASPBERRYPI
#define DEFAULT_DEVICE "/dev/ttyAMA0"
#else
#define DEFAULT_DEVICE "/dev/ttyUSB0"
#endif

#define DEFAULT_BAUDRATE 9600
#define DEFAULT_DATABIT  8
#define DEFAULT_STOPBIT  1
#define DEFAULT_PARITY  'N'
#define DEFAULT_FLOW    SERIAL_FLOW_NONE
#define DEFAULT_PACKET_SIZE 56
#define DEFAULT_PACKET_COUNT    4
#define DEFAULT_PACKET_TIMEOUT  500
#define DEFAULT_PACKET_INTERVAL 1000

/* extremum values ========================================================== */
#define MIN_DATABIT 5
#define MAX_DATABIT 8
#define STOPBIT_LIST { 1, 2 }
#define PARITY_LIST { 'N', 'E', 'O' }
#define MAX_BAUDRATE 921600
#define MIN_BAUDRATE 50

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
static bool bReceive = false;

/* statistiques */
static statistics xStats;

/* internal public functions ================================================ */
int iCheckValue (int iValue, const int * iList, int iDefault);
void vSigIntHandler (int);
void vStatistics (void);
void vVersion (void);
void vHelp (FILE *stream, int exit_msg);
int iCloseAll (void);

/* main ===================================================================== */
int
main (int argc, char **argv) {

  /* Options ligne de commande */
  int iPacketSize, iPacketCount, iPacketTimeout, iPacketInterval;
  xSerialIos xIos;

  const int iStopBitList[] = STOPBIT_LIST;
  const int iParityList[]  = PARITY_LIST;
  const char *short_options = "b:d:t:p:s:c:W:i:f:rhv";
  static const struct option long_options[] = {
    {"baudrate",  required_argument, NULL, 'b'},
    {"databit",  required_argument, NULL, 'd'},
    {"stopbit",  required_argument, NULL, 't'},
    {"parity",  required_argument, NULL, 'p'},
    {"flow",  required_argument, NULL, 'f'},
    {"packetsize",  required_argument, NULL, 's'},
    {"count",  required_argument, NULL, 'c'},
    {"timeout",  required_argument, NULL, 'W'},
    {"interval",  required_argument, NULL, 'i'},
    {"receive",  no_argument, NULL, 'r'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };

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
  xIos.baud =       DEFAULT_BAUDRATE;
  xIos.dbits =      DEFAULT_DATABIT;
  xIos.sbits =      DEFAULT_STOPBIT;
  xIos.parity  =    DEFAULT_PARITY;
  xIos.flow =       DEFAULT_FLOW;
  xIos.flag =       0;
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
        xIos.baud = atoi (optarg);
        if ( ( (unsigned long) xIos.baud < MIN_BAUDRATE) ||
             ( (unsigned long) xIos.baud > MAX_BAUDRATE) ) {
          xIos.baud = DEFAULT_BAUDRATE;
          fprintf (stderr, "Warning: baudrate out of range {%d,%d} !\n",
                   MIN_BAUDRATE, MAX_BAUDRATE);
        }
        //printf("baudrate=%d\n", xIos.baud);
        break;

      case 'd':
        xIos.dbits = atoi (optarg);
        if ( (xIos.dbits < MIN_DATABIT) ||
             (xIos.dbits > MAX_DATABIT) ) {
          xIos.dbits = DEFAULT_DATABIT;
          fprintf (stderr, "Warning: databit out of range {%d,%d} !\n",
                   MIN_DATABIT, MAX_DATABIT);
        }
        //printf("databit=%d\n", xIos.dbits);
        break;

      case 't':
        xIos.sbits = atoi (optarg);
        xIos.sbits = iCheckValue (xIos.sbits, iStopBitList, DEFAULT_STOPBIT);
        //printf("stopbit=%d\n", xIos.sbits);
        break;

      case 'p':
        xIos.parity = atoi (optarg);
        xIos.parity = iCheckValue (xIos.parity, iParityList, DEFAULT_PARITY);
        //printf("parity=%c\n", xIos.parity);
        break;

      case 'f':
        if (strcasecmp (optarg, "rtscts") == 0) {
          xIos.flow = SERIAL_FLOW_RTSCTS;
        }
        else if (strcasecmp (optarg, "xonxoff") == 0) {
          xIos.flow = SERIAL_FLOW_XONXOFF;
        }
        else if (strcasecmp (optarg, "rs485") == 0) {
          xIos.flow = SERIAL_FLOW_RS485_RTS_AFTER_SEND;
        }
        else if (strcasecmp (optarg, "rs485on") == 0) {
          xIos.flow = SERIAL_FLOW_RS485_RTS_ON_SEND;
        }
        break;

      case 's':
        iPacketSize = atoi (optarg);
        if ( (iPacketSize < MIN_PACKET_SIZE) ||
             (iPacketSize > MAX_PACKET_SIZE) ) {

          iPacketSize = DEFAULT_PACKET_SIZE;
        }
        //printf ("packetsize=%d\n", iPacketSize);
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

      case 'r':
        bReceive = true;
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
  iFd = iSerialOpen (sDevice, &xIos);
  if (iFd < 0) {

    fprintf (stderr, "Unable to open %s device: %s\n",
             sDevice, strerror (errno) );
    if (bDevAlloc) {
      free (sDevice);
    }
    exit (EXIT_FAILURE);
  }


  if (bReceive == false) {

    /* Mode transmission */
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
                       (xIos.dbits + xIos.sbits +
                        (xIos.parity == 'N' ? 0 : 1) + 1) *
                       1E6) / xIos.baud;
    printf ("SERIAL PING %d(%d) bytes of data. %s. Timeout %d ms.\n",
            iPacketSize,
            iPacketSize + 2, sSerialAttrToStr (&xIos), iPacketTimeout);
    //printf ("delay=%lu\n", (unsigned long)tTransmittTime);

    iCount = iPacketCount;
    while (iCount) {

      bBad = true;

      i = write (iFd, sPingPacket, iPacketSize + 2);
      xStats.iTxCount++;
      if (i < 0) {
        fprintf (stderr, "Unable to write %s device: %s\n",
                 sDevice, strerror (errno) );
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
        if (iPongLength >= (iPacketSize + 2) ) {
          break;
        }
      }
      while (dRespTime < (double) iPacketTimeout);

      xStats.dTimeSum += dRespTime;

      if (iPongLength == 0) {

        printf ("From %s seq=%d Destination Host Unreachable\n",
                sDevice, xStats.iTxCount);
      }
      else {

        printf ("%d bytes from %s: seq %d ", iPongLength,
                sDevice, xStats.iTxCount);
        if (read (iFd, sPongPacket, iPongLength) == -1) {
          fprintf (stderr, "Unable to read %s device: %s\n",
                   sDevice, strerror (errno) );
        }

        if (iPongLength >= (iPacketSize + 2) ) {

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

            if (isprint (*p) ) {
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
  }
  else {
    bool bIsStarted = false;
    int iAvailableSize, iReadSize, iRemainSize = sizeof (sPongPacket) - 1;
    char * pcPkt, * pcEnd, * p = sPongPacket;

    /* évite core dump lors du printf en cas de paquet trop long */
    sPongPacket[iRemainSize] = '\0';

    printf ("SERIAL PONG on %s %s\n"
            "Packet waiting... Press Ctrl+C to abort !\n",
            sDevice, sSerialAttrToStr (&xIos) );

    /* Mode réception */
    for (;;) {

      /* Attente réception */
      iAvailableSize = iSerialPoll (iFd, -1);

      if (iAvailableSize < 0) {

        fprintf (stderr, "Unable to poll %s device: %s\n",
                 sDevice, strerror (errno) );
        delay_ms (300);
        continue;
      }

      /* Lecture */
      iAvailableSize = MIN (iAvailableSize, iRemainSize);
      iReadSize = read (iFd, p,  iAvailableSize);
      if (iReadSize < 0) {

        fprintf (stderr, "Unable to read %s device: %s\n",
                 sDevice, strerror (errno) );
        delay_ms (300);
        continue;
      }

      if (iReadSize > 0) {

        /* Traitement des caractères reçus */
        if (bIsStarted == false) {

          if ( (pcPkt = memchr (p, STX, iReadSize) ) ) {

            bIsStarted = true;
            xStats.iRxCount++;
          }
        }
        else {
          if ( (pcEnd = memchr (p, ETX, iReadSize) ) || (iRemainSize == 0) ) {


            if (pcEnd) {

              i = write (iFd, pcPkt, pcEnd - pcPkt + 1);
              if (i < 0) {

                fprintf (stderr, "Unable to write %s device: %s\n",
                         sDevice, strerror (errno) );
                delay_ms (300);
              }
              *pcEnd = '\0'; /* écrase ETX final */
            }
            else {

              /* Overflow buffer réception */
              xStats.iErrorCount++;
            }

            /* Affiche le paquet reçu sans STX ni ETX */
            printf ("<%s>\n", pcPkt + 1);
            fflush (stdout);

            /* Réinitialise en attente de paquet */
            bIsStarted = false;
            iRemainSize = sizeof (sPongPacket) - 1;
            p = sPongPacket;
            continue;
          }
        }

        iRemainSize -= iReadSize;
        p += iReadSize;
      }
    }
  }

  if (iCloseAll() != 0) {

    perror ("close");
    exit (EXIT_FAILURE);
  }
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vStatistics (void) {

  if (bReceive) {

    printf ("--- %s pong statistics ---\n", sDevice);
    printf ("%d packets received, %d errors.\n",
            xStats.iRxCount,
            xStats.iErrorCount);
  }
  else {
    
    if (xStats.iTxCount) {
      printf ("--- %s ping statistics ---\n", sDevice);
      printf ("%d packets transmitted, %d received, %d errors, "
              "%.1f%% packet loss, time sum. %.2fms - av. %.2fms\n",
              xStats.iTxCount,
              xStats.iRxCount,
              xStats.iErrorCount,
              (double) (xStats.iTxCount - xStats.iRxCount) * 100.0 /
              (double) xStats.iTxCount,
              xStats.dTimeSum, xStats.dTimeSum / (double) xStats.iTxCount);
    }
    else {
      
      printf ("no packet transmitted.\n");
    }
  }
}

// -----------------------------------------------------------------------------
int
iCloseAll (void) {

  if (bDevAlloc) {

    free (sDevice);
  }
  free (sPingPacket);
  return close (iFd);
}

// -----------------------------------------------------------------------------
void
vSigIntHandler (int sig) {

  putchar ('\n');
  vStatistics();
  if (iCloseAll() != 0) {

    perror ("close");
    exit (EXIT_FAILURE);
  }
  printf ("\n%s closed.\nHave a nice day !\n", sDevice);
  exit (EXIT_SUCCESS);
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
  fprintf (stream, "usage : %s [ options ] [ device ] [ options ]\n\n", sMyName);
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "Send or receive ping packet to serial device. The packet contains a\n"
           " start byte (STX: 0x02), packetsize bytes between 'A' and 'Z' and a\n"
           " trailing byte (ETX: 0x03).\n\n"
           "  device          \tOptionnal serial device name.\n"
           "                  \tThe default is %s.\n\n"
           , DEFAULT_DEVICE);
  fprintf (stream, "valid options are :\n");
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "  -r  --receive   \tWait packets and return them, otherwise,\n"
           "                  \t send packets and wait for the response (default).\n"
           "  -b  --baudrate  \tSpecifies the baudrate.\n"
           "                  \tThe default is %d bauds. The range is from %d to %d bauds.\n"
           "  -d  --databit   \tSpecifies the number of data bits.\n"
           "                  \tThe default is %d. The range is from %d to %d bits.\n"
           "  -p  --parity    \tSpecifies the parity.\n"
           "                  \t\t{N, O, E}, the default is %c.\n"
           "  -t  --stopbit   \tSpecifies the number of data bits.\n"
           "                  \t\t{1, 2}, the default is %d.\n"
           "  -f  --flow      \tSpecifies the flow control.\n"
           "                  \t\t{none, rtscts, xonxoff, rs485, rs485on}\n"
           "                  \tThe default is %s.\n"
           "  -c  --count     \tStop after sending count packets.\n"
           "                  \tThe default is %d.\n"
           "  -i  --interval  \tWait interval milliseconds between sending each packet.\n"
           "                  \tThe default is %d ms.\n"
           "  -s  --packetsize\tSpecifies the number of data bytes to be sent.\n"
           "                  \tThe default is %d. The range is from %d to %d bytes.\n"
           "  -W  --timeout   \tTime to wait for a response, in milliseconds.\n"
           "                  \tThe default is %d ms.\n"
           "  -h  --help      \tPrint this message\n"
           "  -v  --version   \tPrint version and exit\n"
           , DEFAULT_BAUDRATE
           , MIN_BAUDRATE
           , MAX_BAUDRATE
           , DEFAULT_DATABIT
           , MIN_DATABIT
           , MAX_DATABIT
           , DEFAULT_PARITY
           , DEFAULT_STOPBIT
           , sSerialFlowToStr (DEFAULT_FLOW)
           , DEFAULT_PACKET_COUNT
           , DEFAULT_PACKET_INTERVAL
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
