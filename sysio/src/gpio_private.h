/**
 * @file sysio/gpio_private.h
 * @brief GPIO Private
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_PRIVATE_H_
#define _SYSIO_GPIO_PRIVATE_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include <sysio/iomap.h>
#include <sysio/gpio.h>

/* constants ================================================================ */
#define ITERATOR_FRONT -1
#define ITERATOR_BACK  -2

/* structures =============================================================== */
typedef struct xPinList {
  const int * pin;
  int size; // Taille du tableau pointé par pin
  int len;  // Nombre de broches valides
  int first;// Premier numéro valide
  int last; // Dernier numéro valide
} xPinList;

typedef struct xGpio {
  xIoMap * map;
  const xPinList * list;
  int i; // iterateur utilisateur
  int j; // iterateur interne
  int  * pinmode; // [GPIO_SIZE]
  int  * pinmode_release; // [GPIO_SIZE]
  int link;
  bool roc; // release on close
  eGpioNumbering numbering;
} xGpio;

/* ========================================================================== */
__END_C_DECLS
#endif /* #ifndef _SYSIO_GPIO_PRIVATE_H_ ... */
