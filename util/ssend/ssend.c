/*
 * serial_test.c
 * Serial port test
 * - Transmits all the keys pressed on the serial port (no echo)
 * - Displays all characters received from the serial port
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sysio/delay.h>
#include <sysio/serial.h>

/* constants ================================================================ */
#define DEFAULT_PORT "/dev/ttyUSB0"
#define BAUDRATE  1200
#define DATABITS  SERIAL_DATABIT_7
#define PARITY    SERIAL_PARITY_EVEN
#define STOPBITS  SERIAL_STOPBIT_ONE
#define FLOW      SERIAL_FLOW_NONE

/* main ===================================================================== */
int
main (int argc, char **argv) {
  char c;
  int fd;
  FILE * f;
  const char sDefaultPort[] = DEFAULT_PORT;
  const char * sPort = sDefaultPort;
  xSerialIos ios = { .baud = BAUDRATE, .dbits = DATABITS, .parity = PARITY,
                     .sbits = STOPBITS, .flow = FLOW
                   };

  if (argc < 2) {

    printf ("Usage: %s file [port]\n", argv[0]);
    exit (EXIT_FAILURE);
  }
  if (argc > 2) {

    sPort = argv[2];
  }

  f = fopen (argv[1], "rb");
  if (f == NULL) {

    perror ("fopen");
    exit (EXIT_FAILURE);
  }

  if ( (fd = iSerialOpen (sPort, &ios)) < 0) {

    perror ("open");
    exit (EXIT_FAILURE);
  }

  printf ("Serial port %s %s\nPress Ctrl+C to abort ...\n", sPort, sSerialAttrStr (fd));
  while (fread (&c, 1, 1, f) > 0) {

    if (write (fd, &c, 1) < 0) {
      perror ("write");
      exit (EXIT_FAILURE);
    }
  }

  if (close (fd) < 0) {

    perror ("close");
    exit (EXIT_FAILURE);
  }

  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);

  return 0;
}
/* ========================================================================== */
