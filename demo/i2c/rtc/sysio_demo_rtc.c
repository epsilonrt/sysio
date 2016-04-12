/*
 * sysio_demo_rtc.c
 * BQ32000 RTC Demo
 *
 * Reads and displays the RTC registers:
 * n Addr Name        Description
 * 0 0x00 SECONDS     Clock seconds and STOP bit
 * 1 0x01 MINUTES     Clock minutes
 * 2 0x02 CENT_HOURS  Clock hours, century, and CENT_EN bit
 * 3 0x03 DAY         Clock day
 * 4 0x04 DATE        Clock date
 * 5 0x05 MONTH       Clock month
 * 6 0x06 YEARS       Clock years
 * 7 0x07 CAL_CFG1    Calibration and configuration
 * 8 0x08 TCH2        Trickle charge enable
 * 9 0x09 CFG2        Configuration 2
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <sysio/delay.h>
#include <sysio/i2c.h>


/* private variables ======================================================== */
static int fd;

/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM) ) {

    if (iI2cClose (fd) != 0) {
      perror ("iI2cClose");
      exit (EXIT_FAILURE);
    }
    printf ("\neverything was closed.\nHave a nice day !\n");
    exit (EXIT_SUCCESS);
  }
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  uint8_t buf[9];
  time_t t;
  static const char * names[] = { "SECONDS   ",
                                  "MINUTES   ",
                                  "CENT_HOURS",
                                  "DAY       ",
                                  "DATE      ",
                                  "MONTH     ",
                                  "YEARS     ",
                                  "CAL_CFG1  ",
                                  "TCH2      ",
                                  "CFG2      "
                                };

  fd = iI2cOpen ("/dev/i2c-1", 0x68);
  if (fd < 0) {

    perror ("Failed to open i2c ! ");
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  signal (SIGTERM, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n\n");

  for (;;) {

    t = time (NULL);
    printf ("\n%s", ctime (&t) );
    // Lecture d'un bloc de 9 octets
    if (iI2cReadRegBlock (fd, 0, buf, 9) < 0) {

      perror ("Failed to read i2c ! ");
      continue;
    }

    for (int i = 0; i < sizeof (buf); i++) {

      printf ("%s 0x%02X\n", names[i], buf[i]);
    }
    fflush (stdout);
    delay_ms (11000);
  }

  return 0;
}
/* ========================================================================== */
