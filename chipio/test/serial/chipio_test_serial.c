/*
 * chipio_test_serial.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sysio/delay.h>
#include <chipio/serial.h>
#include <sysio/log.h>
#include <sysio/rpi.h>

/* constants ================================================================ */
#define I2C_DEVICE      "/dev/i2c-1"
#define I2C_SLAVE       0x46
#define SERIAL_BAUDRATE 38400
#define SERIAL_DATABITS SERIAL_DATABIT_8
#define SERIAL_PARITY   SERIAL_PARITY_NONE
#define SERIAL_STOPBITS SERIAL_STOPBIT_ONE
#define SERIAL_IRQPIN { .num = GPIO_GEN6, .act = true, .pull = ePullOff }

#define TEST_DELAY 200
#define BUFFER_SIZE 256
#define TEST_PING_COUNT 20

//#define TEST_DEBUG
//#define TEST_ALPHABET
//#define TEST_TXOVERFLOW
//#define TEST_TERMINAL
//#define TEST_PING
#define TEST_PONG

/* private variables ======================================================== */
static xChipIo * xChip;
static xChipIoSerial * xPort;
static FILE * pts;
static int fds, iRet;

/* internal public functions ================================================ */
void vSetup (void);
void vPrintSetup (void);
void vSigIntHandler (int sig);
void vTestDebug (void);
void vTestAlphabet (void);
void vTestTerminal (void);
void vTestTxOverflow (void);
void vTestPing (void);
void vTestPong (void);

/* main ===================================================================== */
int
main (int argc, char **argv) {

  vSetup();
  printf("Press Ctrl+C to abort ...\n");

  for (;;) {

    vTestDebug ();
    vTestAlphabet ();
    vTestTxOverflow();
    vTestTerminal ();
    vTestPing();
    vTestPong();
#if TEST_DELAY > 0
    delay_ms (TEST_DELAY);
#endif
    // putchar('.'); fflush(stdout);
  }

  return 0;
}

/* -----------------------------------------------------------------------------
 * Test de debug
 * Envoi d'un U
 */
void
vTestDebug (void) {
#ifdef TEST_DEBUG

  char c = 0x55;
  iRet = write (fds, &c, 1);
  assert (iRet == 1);
#endif
}

/* -----------------------------------------------------------------------------
 * Envoi de l'alphabet A -> Z
 */
void
vTestAlphabet (void) {
#ifdef TEST_ALPHABET
  uint8_t ucCount = 32;
  int c;

  while (ucCount--) {

    c = 'A';
    do {

      iRet = fputc (c, pts);
      assert (iRet == c);
    }
    while (c++ < 'Z');

    c = '\n';
    iRet = fputc (c, pts);
    assert (iRet == c);
    delay_ms (20);
  }
#endif
}

/* -----------------------------------------------------------------------------
 * Envoi d'une longue chaine de caractères (2 fois plus longue que le buffer tx)
 */
void
vTestTxOverflow (void) {
#if defined(TEST_TXOVERFLOW)
  char s[BUFFER_SIZE];
  int iBlockSize;

  iBlockSize = iChipIoSerialGetBufSize (xPort);
  assert(iBlockSize != -1);

  iBlockSize = MIN(BUFFER_SIZE - 1, iBlockSize * 2);

  for (int i = 0; i < iBlockSize ; i++) {

    s[i] = (i % 10) + 0x30;
  }
  s[iBlockSize] = 0;
  iRet = fprintf (pts, "%s\n", s);
  assert (iRet >=  iBlockSize);
#endif
}

/* -----------------------------------------------------------------------------
 * Test Terminal
 * Invite puis attente d'un caractère puis renvoi
 */
void
vTestTerminal (void) {
#ifdef TEST_TERMINAL
  int c;

  iRet = fputs ("\nTerminal Test\n-printf() test\n", pts);
  assert (iRet != EOF);

  for (c = 0; c < 8; c++) {

    iRet = fprintf (pts, "\tTest #0x%02X\n", c);
    assert (iRet > 0);
  }

  fprintf (pts, "\n-fgetc() test: Press any key (ENTER to quit)...\n");
  do {

    c =  fgetc (pts);

    if (c != EOF) {

      if (isprint (c)) {

        iRet = fputc (c, pts);
        assert (iRet == c);
        //fprintf (pts, "%c\\%02X", c, c);
        fflush(pts);
      }
    }
    else {

      delay_ms (10);
    }

  }
  while (c != '\n');    /* Return pour terminer */
  fputc ('\n', pts);
#endif
}

/* -----------------------------------------------------------------------------
 * Test Ping, version avec les fonctions stdio getc et putc
 * Envoi d'un caractère, attente d'une réponse et statistiques
 */
void
vTestPing (void) {
#ifdef TEST_PING
  fd_set xFdSet;
  struct timeval xTv;
  int iTxCount;
  int iRxCount = 0;
  int iErrorCount = 0;
  int c = 0x55;

  printf("Ping Test\n");
  for (iTxCount = 0; iTxCount < TEST_PING_COUNT; iTxCount++) {

    putchar('.'); fflush(stdout);
    iRet = fputc (c, pts);
    assert (iRet == c);
    fflush (pts);

    FD_ZERO (&xFdSet);
    FD_SET (fds, &xFdSet);
    xTv.tv_sec = 0;
    xTv.tv_usec = TEST_DELAY * 1000UL;

    iRet = select (fds + 1, &xFdSet, NULL, NULL, &xTv);
    if (iRet == -1) {

      PERROR ("select()");
    }
    else if (iRet > 0) {

      int rc = fgetc (pts);
      if (rc == c) {
        iRxCount++;
        continue;
      }
    }
    iErrorCount++;
  }
  delay_ms (TEST_DELAY);
  printf ("\n--- %s ping statistics ---\n", sChipIoSerialPortName (xPort));
  printf ("%d packets transmitted, %d received, %d errors, %.1f%% packet loss\n",
    iTxCount,
    iRxCount,
    iErrorCount,
    (double)(iTxCount - iRxCount) * 100.0 / (double)iTxCount);

#endif
}

/* -----------------------------------------------------------------------------
 * Test Pong, version avec les fonctions read et write
 * Boucle infinie d'attente d'un caractère puis renvoi
 */
void
vTestPong (void) {
#ifdef TEST_PONG
  fd_set xFdSet;
  struct timeval xTv;
  char buffer[BUFFER_SIZE];

  printf("Pong Test\nWaiting for data...\n");
  for (;;) {

    FD_ZERO (&xFdSet);
    FD_SET (fds, &xFdSet);
    xTv.tv_sec = 0;
    xTv.tv_usec = TEST_DELAY * 1000UL;

    iRet = select (fds + 1, &xFdSet, NULL, NULL, &xTv);
    if (iRet == -1) {

      PERROR ("select()");
    }
    else if (iRet > 0) {
      int iBytesAvailable;

      // Lecture du nombre de caractères à transmettre
      iRet = ioctl (fds, FIONREAD, &iBytesAvailable);
      if (iRet == -1) {
        PERROR ("ioctl()");
      }
      else {
        if (iBytesAvailable) {

          int iBytesRead = read (fds, buffer, iBytesAvailable);
          if (iBytesRead == -1) {
            PERROR ("read()");
          }
          else if (iBytesRead > 0) {
            int iBytesWritten = write (fds, buffer, iBytesRead);
            if (iBytesWritten == iBytesRead) {
              putchar('.'); fflush(stdout);
            }
          }
        }
      }
    }
  }
#endif
}

// -----------------------------------------------------------------------------
void
vSetup (void) {
  xDin xIrqPin = SERIAL_IRQPIN;

  printf ("ChipIo Serial Port Test\n");
  xChip = xChipIoOpen (I2C_DEVICE, I2C_SLAVE);
  assert(xChip);
  printf("ChipIo found on %s at 0x%02X\n", I2C_DEVICE, I2C_SLAVE);
  xPort = xChipIoSerialOpen (xChip, &xIrqPin);
  assert(xPort);
  printf("Serial port opened %s\n", sChipIoSerialPortName (xPort));

  fds = iChipIoSerialFileNo (xPort);
  pts = xChipIoSerialFile (xPort);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  printf("Old setting-up:\n");
  vPrintSetup();
#ifdef SERIAL_BAUDRATE
  iRet = iChipIoSerialSetBaudrate (xPort, SERIAL_BAUDRATE);
  assert (iRet == SERIAL_BAUDRATE);
#endif
#ifdef SERIAL_DATABITS
  iRet = eChipIoSerialSetDataBits (xPort, SERIAL_DATABITS);
  assert (iRet == SERIAL_DATABITS);
#endif
#ifdef SERIAL_PARITY
  iRet = eChipIoSerialSetParity (xPort, SERIAL_PARITY);
  assert (iRet == SERIAL_PARITY);
#endif
#ifdef SERIAL_STOPBITS
  iRet = eChipIoSerialSetStopBits (xPort, SERIAL_STOPBITS);
  assert (iRet == SERIAL_STOPBITS);
#endif
#if defined(SERIAL_BAUDRATE) || defined(SERIAL_DATABITS) || defined(SERIAL_PARITY) || defined(SERIAL_STOPBITS)
  printf("New setting-up:\n");
  vPrintSetup();
#endif
}

// -----------------------------------------------------------------------------
void
vSigIntHandler (int sig) {

  vChipIoSerialClose (xPort);
  iRet = iChipIoClose (xChip);
  assert (iRet == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
void
vPrintSetup (void) {

  int iBaudrate = iChipIoSerialGetBaudrate (xPort);
  eSerialDataBits eDataBits = eChipIoSerialGetDataBits (xPort);
  eSerialParity eParity = eChipIoSerialGetParity (xPort);
  eSerialStopBits eStopBits= eChipIoSerialGetStopBits (xPort);

  printf ("\tBaudrate: %d\n", iBaudrate);
  printf ("\tData bits: %s\n", sSerialDataBitsToStr (eDataBits));
  printf ("\tParity: %s\n", sSerialParityToStr (eParity));
  printf ("\tStop bits: %s\n", sSerialStopBitsToStr (eStopBits));
}

/* ========================================================================== */
