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
  int (*addled) (xLedRgbDcb * dcb, eLedRgbMode mode, void * led);
  int (*setcolor) (xLedRgbDcb * dcb, int led, xRgbColor * pxColor);
  int (*ctl) (xLedRgbDcb * dcb, int c, ...);
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
