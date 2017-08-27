/*
 * sysio_test_rpi.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sysio/rpi.h>

/* main ===================================================================== */
int
main (int argc, char **argv) {

  const xRpi * r = pxRpiInfo();
  printf ("Raspberry Pi Hardware\n");
  printf ("------------------------\n");
  if (r) {
    printf ("Revision    : 0x%x\n",r->iRev);
    printf ("Model       : %s\n", sRpiModelToStr(r->eModel));
    printf ("GPIO Rev    : %d\n",r->iGpioRev);
    printf ("Mcu         : %s\n",sRpiMcuToStr(r->eMcu));
    printf ("Memory      : %dMB\n",r->iMemMB);
    printf ("PCB Rev     : %d.%d\n",r->iPcbMajor, r->iPcbMinor);
    printf ("Manufacturer: %s\n",r->sManufacturer);
    return 0;
  }
  printf ("Failed !");
  return 1;
}
/* ========================================================================== */
