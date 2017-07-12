/**
 * @file
 * @brief ping i2c, accès aux registres d'un capteur IAQ
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <getopt.h>

#include <sysio/delay.h>
#include <sysio/i2c.h>
#include <sysio/log.h>
#include <sysio/string.h>
#include "version-git.h"

/* constants ================================================================ */
#define AUTHORS "epsilonRT"
#define WEBSITE "http://www.epsilonrt.fr/sysio"
#define DEFAULT_INTERVAL 500


/* structures =============================================================== */
typedef struct statistics {

  int iTxCount, iRxCount, iErrorCount;
} statistics;

/* private variables ======================================================== */
const char sDefaultBus[] = "/dev/i2c-1";
static int fd;
/* statistiques */
static statistics stats;
const char * bus = sDefaultBus;
int slave;

/* internal public functions ================================================ */
void vVersion (void);
void vHelp (FILE *stream, int exit_msg);
void vStatistics (void);
void vSigIntHandler (int sig);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int interval = DEFAULT_INTERVAL;

  const char *short_options = "b:i:hv";
  static const struct option long_options[] = {
    {"bus",  required_argument, NULL, 'b'},
    {"interval",  required_argument, NULL, 'i'},
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };

  uint8_t count = 0;
  int i;
  long n;

  do {
    i = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (i) {

      case 'b':
        bus = optarg;
        break;

      case 'i':
        if (iStrToLong (optarg, &n, 0) < 0) {

          vHelp (stderr, EXIT_FAILURE);
        }
        interval = (int) n;
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
  while (i != -1);

  if (optind >= argc)    {

    vHelp (stderr, EXIT_FAILURE);
  }

  if (iStrToLong (argv[optind], &n, 0) < 0) {

    vHelp (stderr, EXIT_FAILURE);
  }

  if ( (n < 0x03) || (n > 0x77) ) {

    vHelp (stderr, EXIT_FAILURE);
  }

  slave = (int) n;
  fd = iI2cOpen (bus, slave);
  if (fd < 0) {

    perror ("Failed to open i2c ! ");
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  signal (SIGTERM, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n\n");

  for (;;) {

    delay_ms (interval);
    if (iI2cWrite (fd, count) < 0) {

      perror ("Unable to write i2c ! ");
      continue;
    }
    stats.iTxCount++;

    i = iI2cRead (fd);
    if (i < 0) {

      perror ("Unable to read i2c ! ");
      continue;
    }
    stats.iRxCount++;

    if (count != (uint8_t) i) {
      printf ("Read value differs from written value, w: 0x%02X, r: 0x%02X\n", count, i);
      stats.iErrorCount++;
      continue;
    }

    putchar ('.');
    fflush (stdout);
    count++;
  }

  return 0;
}

// -----------------------------------------------------------------------------
void
vStatistics (void) {

  if (stats.iTxCount) {
    printf ("\n--- chip 0x%02X ping statistics on %s ---\n", slave, bus);
    printf ("%d bytes transmitted, %d received, %d errors, "
            "%.1f%% byte loss\n",
            stats.iTxCount,
            stats.iRxCount,
            stats.iErrorCount,
            (double) (stats.iTxCount - stats.iRxCount) * 100.0 /
            (double) stats.iTxCount);
  }
  else {

    printf ("no byte transmitted.\n");
  }
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
  fprintf (stream, "usage : %s [ options ] chip-address [ options ]\n\n", __progname);
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "Send then receive ping byte to i2c device.\n"
           "  chip-address specifies the address of the chip on that bus, and is an integer\n"
           "  between 0x03 and 0x77.\n");
  fprintf (stream, "valid options are :\n");
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "  -b  --bus       \tSpecifies the i2c bus.\n"
           "                  \tThe default is %s.\n"
           "  -i  --interval  \tWait interval milliseconds between sending each packet.\n"
           "                  \tThe default is %d ms.\n"
           "  -h  --help      \tPrint this message\n"
           "  -v  --version   \tPrint version and exit\n"
           , sDefaultBus
           , DEFAULT_INTERVAL);
  exit (exit_msg);
}

// -----------------------------------------------------------------------------
void
vSigIntHandler (int sig) {

  if ( (sig == SIGINT) || (sig == SIGTERM) ) {
    vStatistics();
    if (iI2cClose (fd) != 0) {
      perror ("iI2cClose");
      exit (EXIT_FAILURE);
    }
    printf ("\neverything was closed.\nHave a nice day !\n");
    exit (EXIT_SUCCESS);
  }
}

/* ========================================================================== */
