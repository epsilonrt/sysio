/**
 * @file gpio_private.h
 * @brief GPIO Private
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_PRIVATE_H_
#define _SYSIO_GPIO_PRIVATE_H_

#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */
#include <sysio/iomap.h>
#include <sysio/gpio_c.h>

/* constants ================================================================ */
#define ITERATOR_FRONT -1
#define ITERATOR_BACK  -2

/* structures =============================================================== */
/*
 * @struct xPinList
 * @brief 
 */
typedef struct xPinList {
  const int * pin;
  int size; // Taille du tableau pointé par pin
  int len;  // Nombre de broches valides
  int first;// Premier numéro valide
  int last; // Dernier numéro valide
} xPinList;

/*
 * @struct xConnectorList
 * @brief 
 */
typedef struct xConnectorList {
  const int * size; // Tailles de connecteurs (tableau de nb int)
  int nb; // Nombre de connecteurs
} xConnectorList;

/*
 * @struct xGpio
 * @brief 
 */
typedef struct xGpio {
  xIoMap * map[2];
  const xPinList * list;
  int i; // iterateur utilisateur
  int j; // iterateur interne
  int  * pinmode; // [GPIO_SIZE]
  int  * pinmode_release; // [GPIO_SIZE]
  int  * pinpull; // [GPIO_SIZE]
  int  * pinpull_release; // [GPIO_SIZE]
  int link;
  bool roc; // release on close
  eGpioNumbering numbering;
} xGpio;

/* private functions ======================================================== */

// Retourne le numéro de la broche en numérotation MCU
int iGpioMcuPin (int n, xGpio * gp);
// Partie spécifique à l'architecture
int iArchGpioSetMode (int g, eGpioMode eMode, xGpio * gp);
eGpioMode eArchGpioGetMode (int g, xGpio * gp);
int iArchGpioSetPull (int g, eGpioPull ePull, xGpio * gp);
eGpioPull eArchGpioGetPull (int g, xGpio * gp);
int iArchGpioRelease (int g, xGpio * gp);
int iArchGpioWrite (int g, bool bValue, xGpio * gp);
int iArchGpioRead (int g, xGpio * gp);
int iArchGpioToggle (int g, xGpio * gp);
const xConnectorList * pxArchGpioGetConnSize (xGpio * gp);

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /* #ifndef _SYSIO_GPIO_PRIVATE_H_ ... */
