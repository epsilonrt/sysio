/**
 * @file
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <chipio/adc.h>

/* constants ================================================================ */
#define I2C_DEVICE  "/dev/i2c-1"
#define I2C_SLAVE   0x46
#define ADC_AREF    eAdcRefInternal
#define ADC_VREF    1.1
#define ADC_CKDIV   eAdcDiv32
#define ADC_CHAN_QUANTITY 2
#define ADC_FULLSCALE_LIST { (39000.0 + 11000.0) / 11000.0 * ADC_VREF, (16000.0 + 2200.0) / 2200.0 * ADC_VREF}
#define TEST_DELAY 200

/* private variables ======================================================== */
static xChipIo * xChip;
static xChipIoAdc * xAdc;
static const double dAdcFullScale[] = ADC_FULLSCALE_LIST;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  vChipIoAdcClose (xAdc);
  int i = iChipIoClose (xChip);
  assert (i == 0);
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i;

  printf ("ChipIo ADC Demo\n");
  xChip = xChipIoOpen (I2C_DEVICE, I2C_SLAVE);
  assert (xChip);
  printf ("ChipIo found on %s at 0x%02X\n", I2C_DEVICE, I2C_SLAVE);

  xAdc = xChipIoAdcOpen (xChip);
  assert (xAdc);

  i = eChipIoAdcSetRef (ADC_AREF, xAdc);
  assert (i == ADC_AREF);

  i = eChipIoAdcSetDiv (ADC_CKDIV, xAdc);
  assert (i == ADC_CKDIV);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");
  for (i = 0; i < ADC_CHAN_QUANTITY; i++) {

    printf ("V%d\tADC%d\t", i, i);
  }
  putchar ('\n');

  for (;;) {
    for (i = 0; i < ADC_CHAN_QUANTITY; i++) {
      
      int iAdc  = iChipIoAdcRead (i, xAdc);

      // on affiche la tension et la valeur du registre ADC
      printf ("%.3f\t%04d\t",
              ADC_MEASUREMENT (iAdc, dAdcFullScale[i]), iAdc);
    }
    putchar ('\n');
    delay_ms (TEST_DELAY);
  }

  return 0;
}
/* ========================================================================== */
