/**
 * @file
 * @brief Leds RGB (Contrôleur TI TLC59116)
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_TLC59116_H_
#define _SYSIO_TLC59116_H_

#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup tlc59116_module Contrôleur de leds RGB TLC59116

 *  Ce module permet de gérer les leds multicolores (Rouge, Verte, Bleue) de
 *  façons indépendantes de la plate-forme matérielle. \n
 *  Son implémentation fait appel à un contrôleur spécialisé (TLC59116...). \n
 *  La macro AVRIO_LEDRGB_ENABLE doit être définie dans avrio-config.h ou dans
 *  le makefile et dans ce cas, la partie interface matérielle doit être
 *  configurée dans le fichier avrio-board-ledrgb.h
 *  @{
 *  @example output/ledrgb/demo_ledrgb.c
 *  Modification des couleurs, de la luminosité et clignotement de leds RGB.
 */

/* constants ================================================================ */
#define TLC59116_ADDR_BASE   0x60
#define TLC59116_ADDR_ALL    0x68
#define TLC59116_ADDR_SUB1   0x69
#define TLC59116_ADDR_SUB2   0x6A
#define TLC59116_ADDR_SWRST  0x6B
#define TLC59116_ADDR_SUB3   0x6C

#define TLC59116_ADDR(_a3,_a2,_a1,_a0) \
  ((TLC59116_ADDR_BASE) + ((_a3)*16)+((_a2)*8)+((_a1)*4)+((_a0)*2))


typedef struct xTlc59116Pin {
  uint8_t num; // numéro de la sortie entre 0 et 15 (/OUT0 à /OUT15)
  uint8_t ctrl;
} xTlc59116Pin;

typedef struct xTlc59116Led {
  xTlc59116Pin red;
  xTlc59116Pin green;
  xTlc59116Pin blue;
} xTlc59116Led;

typedef struct xTlc59116Config {
  char * i2c_bus;
  int i2c_addr;
} xTlc59116Config;
/**
 * @}
 */
__END_C_DECLS
/* ========================================================================== */
#endif  /* _SYSIO_TLC59116_H_ not defined */
