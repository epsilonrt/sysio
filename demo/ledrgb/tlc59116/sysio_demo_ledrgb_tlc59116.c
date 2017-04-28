/*
 * sysio_demo_ledrgb_tlc59116.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/ledrgb.h>

/* constants ================================================================ */
#define NOF_LEDS 4
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
static xLedRgbDevice * dev;

/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {
  int ret;

  ret = iLedRgbDeleteDevice (dev);
  if (ret != 0) {

    printf ("\niLedRgbDeleteDevice() failed with %d returned value\n", ret);
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int ret;
  uint64_t leds = 0;
  xTlc59116Config ctrl_list[] = {{.i2c_bus = "/dev/i2c-1", .i2c_addr = TLC59116_ADDR(0,0,0,0) }, /* Fin de la liste */ {.i2c_bus = NULL, 0}};
  int led[NOF_LEDS];
  xTlc59116Led setup[NOF_LEDS] = {
    { .red = { .num = 0, .ctrl = 0 }, .green = { .num =  1, .ctrl = 0 }, .blue = { .num =  2, .ctrl = 0 }},
    { .red = { .num = 3, .ctrl = 0 }, .green = { .num =  4, .ctrl = 0 }, .blue = { .num =  5, .ctrl = 0 }},
    { .red = { .num = 6, .ctrl = 0 }, .green = { .num =  7, .ctrl = 0 }, .blue = { .num =  8, .ctrl = 0 }},
    { .red = { .num = 9, .ctrl = 0 }, .green = { .num = 10, .ctrl = 0 }, .blue = { .num = 11, .ctrl = 0 }}
  };

  dev = xLedRgbNewDevice (eLedRgbDeviceTlc59116, ctrl_list);
  assert(dev);

  for (int i = 0; i < NOF_LEDS; i++) {

    led[i] = iLedRgbAddLed (dev, &setup[i]);
    leds |= 1ULL << i;
  }

  ret = iLedRgbSetMode (dev, leds, eLedRgbModePwm);
  ret _ iLedRgbSetColor(dev, leds, RGB_DEEPPINK);


  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");
  for (;;) {
    // Remplacer les lignes ci-dessous par votre code
    putchar ('.');
    fflush (stdout);
    delay_ms (1000);
  }

  return 0;
}
/* ========================================================================== */
