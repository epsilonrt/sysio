/*
 * test_chipio_serial.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/chipio/serial.h>
#include <sysio/log.h>

/* constants ================================================================ */
#define I2C_DEVICE      "/dev/i2c-1"
#define I2C_SLAVE       0x46
#define SERIAL_BAUDRATE 115200
#define SERIAL_DATABITS SERIAL_DATABIT_8
#define SERIAL_PARITY   SERIAL_PARITY_NONE
#define SERIAL_STOPBITS SERIAL_STOPBIT_ONE
#define TEST_POLL_DELAY 1

/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
static  xChipIo * xChip;
static  xChipIoSerial * xPort;

/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {
  int iRet;

  vChipIoSerialClose (xPort);
  iRet = iChipIoClose (xChip);
  assert (iRet == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
static void
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

static void
vSetup (void) {
  int iRet;

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

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int iRet, fds;
  FILE * pts;
  fd_set xFdSet;
  struct timeval xTv;
  const char hello[] = "Hello World #%d !\n";
  char msg[256];


  printf ("ChipIo Serial Port Test\n");
  xChip = xChipIoOpen (I2C_DEVICE, I2C_SLAVE);
  assert(xChip);
  printf("ChipIo found on %s at 0x%02X\n", I2C_DEVICE, I2C_SLAVE);
  xPort = xChipIoSerialOpen (xChip);
  assert(xPort);
  printf("Serial port opened %s\n", sChipIoSerialPortName (xPort));

  fds = iChipIoSerialFileNo (xPort);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  vSetup();
  pts = xChipIoSerialFile (xPort);

  printf("Press Ctrl+C to abort ...\n");
  for (int iCount = 0;; iCount++) {


    // Transmission
#if 1
    putchar('.'); fflush(stdout);
    iRet = fprintf (pts, hello, iCount);
    assert(iRet >= 0);
    delay_ms (500);
#endif

#if 0
    printf("> ");
    fgets (msg, 256, stdin);
    printf ("%s", msg);
    iRet = fprintf (pts, "%s", msg);
    assert(iRet >= 0);
#endif

    // Réception
#if 0
    FD_ZERO (&xFdSet);
    FD_SET (fds, &xFdSet);
    xTv.tv_sec = TEST_POLL_DELAY;
    xTv.tv_usec = 0;

    iRet = select (fds + 1, &xFdSet, NULL, NULL, &xTv);
    if (iRet == -1) {

      PERROR ("select()");
    }
    else if (iRet > 0) {

      int c = fgetc (pts);
      if (c > 0) {
        putchar(c);
      }
    }
#endif
  }

  return 0;
}
/* ========================================================================== */
