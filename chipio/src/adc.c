/**
 * @file chipio/adc.c
 * @brief CAN ChipIo
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <string.h>
#include <sysio/log.h>
#include <chipio/adc.h>
#include "interface.h"

/* structures =============================================================== */
typedef struct xChipIoAdc {
  xChipIo * chipio;   // Objet chipio
} xChipIoAdc;


/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xChipIoAdc *
xChipIoAdcOpen (xChipIo * chip) {
  xChipIoAdc * adc;

  if ( (iChipIoAvailableOptions (chip) & eOptionAdc) == 0) {

    PERROR ("Adc is not available for this chip");
    return NULL;
  }

  adc = malloc (sizeof (xChipIoAdc));
  assert (adc);

  memset (adc, 0, sizeof (xChipIoAdc));
  adc->chipio = chip;

  return adc;
}

// -----------------------------------------------------------------------------
void
vChipIoAdcClose (xChipIoAdc * adc) {

  assert (adc);
  free (adc);
}

// -----------------------------------------------------------------------------
int
iChipIoAdcRead (unsigned input, xChipIoAdc * adc) {

  assert (adc);
  if (input <= 3) {
    int iValue;
    const eChipIoRegister eReg[] = { eRegAdc0, eRegAdc1, eRegAdc2, eRegAdc3 };

    iValue = iChipIoReadReg16 (adc->chipio, eReg[input]);
    return iValue;
  }
  PERROR ("Input out of range");
  return -1;
}

// -----------------------------------------------------------------------------
eAdcRef
eChipIoAdcRef (xChipIoAdc * adc) {

  assert (adc);
  return (eAdcRef) iChipIoReadReg8 (adc->chipio, eRegAdcCr);
}

// -----------------------------------------------------------------------------
eAdcRef
eChipIoAdcSetRef (eAdcRef eRef, xChipIoAdc * adc) {

  assert (adc);
  switch (eRef) {
    case eAdcRefExternal:
    case eAdcRefVcc:
    case eAdcRefInternal:
      if (iChipIoWriteReg8 (adc->chipio, eRegAdcCr, (uint8_t)eRef) == 0) {
        return eRef;
      }
      break;
    default:
      break;
  }
  return eAdcRefUnknown;
}

// -----------------------------------------------------------------------------
eAdcDiv
eChipIoAdcDiv (xChipIoAdc * adc) {

  assert (adc);
  return (eAdcDiv) iChipIoReadReg8 (adc->chipio, eRegAdcDiv);
}

// -----------------------------------------------------------------------------
eAdcDiv
eChipIoAdcSetDiv (eAdcDiv eDiv, xChipIoAdc * adc) {

  assert (adc);
  switch (eDiv) {
    case eAdcDiv2:
    case eAdcDiv4:
    case eAdcDiv8:
    case eAdcDiv16:
    case eAdcDiv32:
    case eAdcDiv64:
    case eAdcDiv128:
      if (iChipIoWriteReg8 (adc->chipio, eRegAdcDiv, (uint8_t)eDiv) == 0) {
        return eDiv;
      }
      break;
    default:
      break;
  }
  return eAdcDivUnknown;
}

/* ========================================================================== */
