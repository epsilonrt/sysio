/*
 * rpi-info.c
 * @brief Lecture des informations matérielles d'un Raspberry Pi
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <sysio/rpi.h>
#include <sysio/log.h>
#include "version-git.h"

/* constants ================================================================ */
#define AUTHORS "epsilonRT"
#define WEBSITE "http://www.epsilonrt.fr/sysio"

#define OPT_CPU 0x01
#define OPT_REV 0x02
#define OPT_GPIO 0x04
#define OPT_MEM 0x08
#define OPT_PCB 0x10
#define OPT_MAN 0x20
#define OPT_NAME 0x40
#define OPT_ALL 0xFFFF

/* internal public functions ================================================ */
void vVersion (void);
void vHelp (FILE *stream, int exit_msg);
void vPrintWithLabels (const xRpi * rpi, uint16_t flags);

/* main ===================================================================== */
int
main (int argc, char **argv) {
  const char *short_options = "crgmpbnahv";
  static const struct option long_options[] = {
    {"cpu",  no_argument, NULL, 'c'},       // OPT_CPU
    {"revision",  no_argument, NULL, 'r'},  // OPT_REV
    {"gpio",  no_argument, NULL, 'g'},      // OPT_GPIO
    {"mem",  no_argument, NULL, 'm'},       // OPT_MEM
    {"pcb",  no_argument, NULL, 'p'},       // OPT_PCB
    {"builder",  no_argument, NULL, 'b'},   // OPT_MAN
    {"name",  no_argument, NULL, 'n'},     // OPT_NAME
    {"all",  no_argument, NULL, 'a'},       // OPT_ALL
    {"help",  no_argument, NULL, 'h'},
    {"version",  no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0} /* End of array need by getopt_long do not delete it*/
  };
  const xRpi * r;
  int i;
  uint16_t flags = 0, count = 0;

  do {
    i = getopt_long (argc, argv, short_options, long_options, NULL);

    switch (i) {

      case 'c':
        count++;
        flags |= OPT_CPU;
        break;

      case 'r':
        count++;
        flags |= OPT_REV;
        break;

      case 'g':
        count++;
        flags |= OPT_GPIO;
        break;

      case 'm':
        count++;
        flags |= OPT_MEM;
        break;

      case 'p':
        count++;
        flags |= OPT_PCB;
        break;

      case 'b':
        count++;
        flags |= OPT_MAN;
        break;

      case 'n':
        count++;
        flags |= OPT_NAME;
        break;

      case 'a':
        count = -1;
        flags = 0xFFFF;
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

  r = pxRpiInfo();
  if (r) {

    if (count == 0) {
      // Pas d'option fournie, all par défaut
      flags = OPT_ALL;
      count = -1;
    }

    if (count > 1) {

      vPrintWithLabels (r, flags);
    }
    else {
      // Une seule info demandée, affichage brut
      switch (flags) {
        case OPT_CPU:
          printf ("%s\n", sRpiMcuToStr (r->eMcu) );
          break;
        case OPT_REV:
          printf ("0x%x\n", r->iRev);
          break;
        case OPT_GPIO:
          printf ("%d\n", r->iGpioRev);
          break;
        case OPT_MEM:
          printf ("%d\n", r->iMemMB);
          break;
        case OPT_PCB:
          printf ("%d.%d\n", r->iPcbMajor, r->iPcbMinor);
          break;
        case OPT_MAN:
          printf ("%s\n", r->sManufacturer);
          break;
        case OPT_NAME:
          printf ("%s\n", sRpiModelToStr (r->eModel) );
          break;
        default:
          exit (EXIT_FAILURE);
          break;
      }
    }
    return 0;
  }
  fprintf (stderr, "This board is not a raspberry pi !");
  return 1;
}

// -----------------------------------------------------------------------------
void
vPrintWithLabels (const xRpi * r, uint16_t flags) {

  if (flags & OPT_NAME) {
    printf ("Name        : %s\n", sRpiModelToStr (r->eModel) );
  }
  if (flags & OPT_CPU) {
    printf ("Mcu         : %s\n", sRpiMcuToStr (r->eMcu) );
  }
  if (flags & OPT_REV) {
    printf ("Revision    : 0x%x\n", r->iRev);
  }
  if (flags & OPT_GPIO) {
    printf ("GPIO Rev    : %d\n", r->iGpioRev);
  }
  if (flags & OPT_PCB) {
    printf ("PCB Rev     : %d.%d\n", r->iPcbMajor, r->iPcbMinor);
  }
  if (flags & OPT_MEM) {
    printf ("Memory      : %dMB\n", r->iMemMB);
  }
  if (flags & OPT_MAN) {
    printf ("Manufacturer: %s\n", r->sManufacturer);
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
  fprintf (stream, "usage : %s [ options ]\n\n", __progname);
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "Retrieves information from the raspberry pi board.\n"
           "If only one information is requested, it is displayed without formatting,\n"
           "otherwise each information is displayed on a line preceded by its name.\n"
           "Return value: 0 if the board is a rasperry pi, 1 otherwise\n"
          );
  fprintf (stream, "valid options are :\n");
  fprintf (stream,
           //01234567890123456789012345678901234567890123456789012345678901234567890123456789
           "  -a  --all       \tPrints all informations (default)\n"
           "  -n  --name      \tPrints the \"human-readable\" name of the board.\n"
           "  -c  --cpu       \tPrints the MCU model (bcm2708 ...)\n"
           "  -r  --revision  \tPrints the revision number of the board in hexadecimal (prefixed by 0x).\n"
           "  -g  --gpio      \tPrints the revision number of the GPIO in decimal\n"
           "  -m  --mem       \tPrints the RAM size in megabytes\n"
           "  -p  --pcb       \tPrints the revision number of the PCB in the form M.m\n"
           "  -b  --builder   \tPrints the name of the manufacturer.\n"
           "  -h  --help      \tPrints this message\n"
           "  -v  --version   \tPrints version and exit\n"
          );
  exit (exit_msg);
}

/* ========================================================================== */
