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
#define STR_SIZE 40

//#define TEST_DEBUG
#define TEST_ALPHABET
//#define TEST_TXOVERFLOW
//#define TEST_TERMINAL
//#define TEST_PING

/* macros =================================================================== */
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
#if TEST_DELAY > 0
    delay_ms (TEST_DELAY);
#endif
    putchar('.'); fflush(stdout);
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
    //delay_ms (100);
  }
#endif
}

/* -----------------------------------------------------------------------------
 * Envoi d'une longue chaine de caractères (2 fois plus longue que le buffer tx)
 */
void
vTestTxOverflow (void) {
#if defined(TEST_TXOVERFLOW) && defined(SERIAL_TXBUFSIZE)
  #define BUFSIZE (SERIAL_TXBUFSIZE * 2)
  char s[BUFSIZE];
  for (int i = 0; i < BUFSIZE - 1; i++) {

    s[i] = (i % 10) + 0x30;
  }
  s[BUFSIZE - 1] = 0;
  vSerialPutString (s);
#endif
}

/* -----------------------------------------------------------------------------
 * Test Terminal
 * Invite puis attente d'un caractère puis renvoi
 */
void
vTestTerminal (void) {
#ifdef TEST_TERMINAL
  uint16_t usCount = 0;
  bool isWait = true;

  puts_P (PSTR ("\nStdio Test\n-printf() test"));
  for (c = 0; c < 8; c++) {

    printf_P (PSTR ("\tTest #0x%02X\r"), c);
  }

  printf_P (PSTR ("\n-getchar() test: Press any key (ENTER to quit)..."));
  do {

    c =  getchar ();;
    iRet = ferror (stdin);
    assert (iRet == 0);
    if ( (c != EOF) && (feof (stdin) == 0)) {

      if (isWait) {
        iRet = putchar ('\n');
        assert (iRet != EOF);
        iRet = ferror (stdout);
        assert (iRet == 0);
        isWait = false;
      }

      iRet = putchar (c);
      assert (iRet == c);
      iRet = ferror (stdout);
      assert (iRet == 0);
      vLedToggle (LED_LED1);
    }
    else {
      if ( (isWait) && ( (usCount++ % 10000) == 0)) {

        iRet = putchar ('.');
        assert (iRet != EOF);
        iRet = ferror (stdout);
        assert (iRet == 0);
      }
    }

  }
  while (c != '\r');    /* Return pour terminer */
  putchar ('\n');
#endif
}

/* -----------------------------------------------------------------------------
 * Test Ping, version avec les fonctions stdio getc et putc
 * Boucle infinie d'attente d'un caractère puis renvoi
 */
void
vTestPing (void) {
#ifdef TEST_PING

  // iRet = puts ("\nPing Stdio Test\nPress any key...");
  // assert (iRet >= 0);
  for (;;) {

    c = getc (&xSerialPort);
    iRet = ferror (&xSerialPort);
    assert (iRet == 0);
    if ( (c != EOF) && (feof (&xSerialPort) == 0)) {

      iRet = putc (c, &xSerialPort);
      assert (iRet == c);
      iRet = ferror (&xSerialPort);
      assert (iRet == 0);
      vLedToggle (LED_LED1);
    }
  }
#endif
}


// -----------------------------------------------------------------------------
void
vSetup (void) {
  int iRet;
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
  int iRet;

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

#if 0
  int iRet, fds;
  fd_set xFdSet;
  struct timeval xTv;
  //const char hello[] = "Hello World #%d !\n";
  const char hello[] = "#%d#\n";
  char msg[256];
    // Transmission
    putchar('.'); fflush(stdout);
#if 1
    iRet = fputc ('A', pts); fflush (pts);
    assert(iRet >= 0);
#endif

#if 0
    iRet = fprintf (pts, hello, iCount);
    assert(iRet >= 0);
#endif

#if 0
    for (int i = 0; i < STR_SIZE; i++) {
      msg[i] = i % 10 + 0x30;
    }
    msg[STR_SIZE] = 0;
    iRet = fprintf (pts, "%s\n", msg);
    assert(iRet >= 0);
#endif

#if 0
    printf("> ");
    fgets (msg, 256, stdin);
    printf ("%s", msg);
    iRet = fprintf (pts, "%s", msg);
    assert(iRet >= 0);
#endif
    delay_ms (20);

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
#endif

/* ========================================================================== */
