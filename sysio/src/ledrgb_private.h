/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_LEDRGB_PRIVATE_H_
#define _SYSIO_LEDRGB_PRIVATE_H_

#include <sysio/defs.h>
#include <sysio/ledrgb.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/* constants ================================================================ */
/* structures =============================================================== */
typedef struct xLedRgbDcb xLedRgbDcb; // déclaration préalable...

/*
 * Opérations réalisées par un driver
 */
typedef struct xLedRgbOps {
  int (*open) (xLedRgbDcb * dcb, void * setup);
  int (*close) (xLedRgbDcb * dcb);
  /*
   * @return numéro led >= 0, négatif si erreur
   */
  int (*addled) (xLedRgbDcb * dcb, void * led);
  int (*setcolor) (xLedRgbDcb * dcb,int led, uint32_t color);
  int (*setmode) (xLedRgbDcb * dcb,int led, eLedRgbMode mode);
  int (*setdimmer) (xLedRgbDcb * dcb,uint16_t dimming);
  int (*setblinker) (xLedRgbDcb * dcb, uint16_t blinking);
  int (*error) (xLedRgbDcb * dcb, int * code);
} xLedRgbOps;

/*
 * Device Control Block
 */
typedef struct xLedRgbDcb {
  xLedRgbOps * ops; // opérations du driver
  void * dptr;  // données du driver
} xLedRgbDcb;

/* public variables ========================================================= */
extern xLedRgbOps xTlc59116Ops;

__END_C_DECLS
/* ========================================================================== */
#endif  /* _SYSIO_LEDRGB_PRIVATE_H_ not defined */
