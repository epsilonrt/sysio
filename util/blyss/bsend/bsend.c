/*
 * bsend.c
 * @brief Module blyss en émission
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/blyss.h>
#include <sysio/string.h>

/* constants ================================================================ */
#define DEFAULT_OUT_PIN 0

/* private variables ======================================================== */
static xBlyss * b;
static xBlyssFrame * f;
static const uint8_t my_id[] = {
  //0x79, 0x5F, 0x78 // Micro émetteur @SkyWodd
  //0x39, 0x21, 0xA8 // Télécommande @SkyWodd
  // 0x0E, 0xCB, 0xE8 // Détecteur IR @skywodd
  0x05, 0x08, 0x24 // Télécommande 8 canaux Ref. 582883
};

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  free (f);
  if (iBlyssDelete (b) != 0) {

    printf ("\niBlyssDelete() failed !\n");
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
static void
vExitFailure (void) {

  printf ("Usage: %s channel {on,off,loop} [pin]\n", __progname);
  exit (EXIT_FAILURE);
}
/* main ===================================================================== */
int
main (int argc, char **argv) {
  uint8_t channel;
  long num;
  int out_pin = DEFAULT_OUT_PIN;
  bool loop = false;
  bool state = true;

  if (argc < 3) {

    vExitFailure();
  }
  
  if (iStrToLong (argv[1], &num, 0) < 0) {

    printf ("Error: channel must be a number !\n");
    vExitFailure();
  }
  
  if (!bBlyssChannelIsValid (num)) {

    printf ("Error: illegal channel !\n");
    vExitFailure();
  }

  channel = num;

  if (strcasecmp (argv[2], "on")) {

    if (strcasecmp (argv[2], "off")) {

      if (strcasecmp (argv[2], "loop")) {

        printf ("Error: illegal command !\n");
        vExitFailure();
      }
      else {

        // "loop"
        loop = true;
      }
    }
    else {

      // "off"
      state = false;
    }
  }

  if (argc > 3) {

    if (iStrToLong (argv[3], &num, 0) < 0) {

      printf ("Error: pin must be a number !\n");
      vExitFailure();
    }
    out_pin = num;
  }

  b = xBlyssNew (out_pin, -1);
  assert (b);

  f = xBlyssFrameNew (my_id);
  assert (b);

  vBlyssFrameSetChannel (f, channel);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");

  do {

    vBlyssFrameSetState (f, state);

    if (iBlyssSend (b, f, 4)) {

      printf ("iBlyssSend() failed !\n");
    }
    else {

      vBlyssFramePrint (f);
    }

    if (loop) {
      delay_ms (5000);
      state = !state;
    }
  }
  while (loop);

  vSigIntHandler (SIGQUIT);
  return 0;
}
/* ========================================================================== */
