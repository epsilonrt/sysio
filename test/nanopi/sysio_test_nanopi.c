/*
 * sysio_test_nanopi.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sysio/nanopi.h>

/* main ===================================================================== */
int
main (int argc, char **argv) {

  const xNanoPi * r = pxNanoPiInfo();
  printf ("NanoPi Hardware\n");
  printf ("------------------------\n");
  if (r) {
    printf ("Model       : %s\n", sNanoPiModelToStr (r->eModel));
    printf ("GPIO Rev    : %d\n", r->iGpioRev);
    printf ("Mcu         : %s\n", sNanoPiMcuToStr (r->eMcu));
    printf ("Memory      : %dMB\n", r->iMemMB);
    printf ("Manufacturer: %s\n", r->sManufacturer);
    return 0;
  }
  printf ("Failed !");
  return 1;
}
/* ========================================================================== */
