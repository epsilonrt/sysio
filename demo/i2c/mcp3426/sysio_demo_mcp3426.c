/**
 * @file
 * @brief Démo i2c, Convertisseur analogique-numérique MCP3426
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/i2c.h>

/* constants ================================================================ */

/* private variables ======================================================== */
static int fd;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM)) {

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
  int raw_adc;
  uint8_t buf[2];

  fd = iI2cOpen ("/dev/i2c-1", 0x68);
  if (fd < 0) {

    perror ("Failed to open i2c ! ");
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  signal (SIGTERM, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n\n");
  printf ("Ch1\n");

  // Select configuration command(0x10)
  // Continuous conversion mode, channel-1, 12-bit resolution
  if (iI2cWrite (fd, 0x10) < 0) {

    perror ("Failed to read i2c ! ");
  }
  delay_ms (100);


  for (;;) {

    // Lecture d'un bloc de 2 octets
    // Read 2 bytes of buf from register(0x00)
    // raw_adc msb, raw_adc lsb
    if (iI2cReadBlock (fd, buf, 2) < 0) {

      perror ("Failed to read i2c ! ");
      continue;
    }

    // Extraction des données du bloc
    // Convert the buf to 12-bits
    raw_adc = ( (buf[0] & 0x0F) * 256 + buf[1]);
    if (raw_adc > 2047) {
      raw_adc -= 4095;
    }

    // Output buf to screen
    // Affichage
    printf ("%d\n", raw_adc);
    fflush (stdout);
    delay_ms (1000);
  }

  return 0;
}
/* ========================================================================== */
