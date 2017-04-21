/**
 * @file avrio-board-ledrgb.h
 * @brief Configuration module LED RGB
 * @author Copyright � 2012-2013 epsilonRT. All rights reserved.
 * @copyright GNU Lesser General Public License version 3
 *            <http://www.gnu.org/licenses/lgpl.html>
 * @version $Id: avrio-board-ledrgb.h 29 2013-02-17 22:19:35Z pjean $
 * Revision History ---
 *    20130129 - Initial version by epsilonRT
 */
#ifndef _AVRIO_BOARD_LEDRGB_H_
#define _AVRIO_BOARD_LEDRGB_H_

#include <avrio/ledrgb_defs.h>

/* constants ================================================================ */
#define LEDRGB_LED_QUANTITY   16 ///< Nombre de LED RGB
#define LEDRGB_ALL_LEDS       0xFFFF
#define LEDRGB_NO_LED         0
#define LEDRGB_DEFAULT_MODE   MODE_BLINK
#define LEDRGB_CTRL           LEDRGB_CTRL_TLC59116 ///< Type de contr�leur

/* types ==================================================================== */
/*
 * Type utilis� pour les masques de LED RGB. 
 * Le nombre de bits doit �tre sup�rieur ou �gal � LEDRGB_LED_QUANTITY
 */
typedef uint16_t xLedRgbMask;

/* Contr�leur TLC59116 (I�C) =============== */
/** R�glage du gain en courant � l'init. (c.f. p. 19 du datasheet) */
#define TLC59116_DEFAULT_IREF TLC59116_IREF(1,1,63) // CM=1, HC=1, D=63 

/** 
 * Listes d'adresse I�C des contr�leurs (1 liste par couleur)
 * Il doit y avoir un contr�leur TLC59116 de chaque couleur par paquet
 * de 16 leds.
 *                             TLC59116_ADDR(A3,A2,A1,A0) */
#define TLC59116_RED_LIST    { TLC59116_ADDR(0,0,0,0) }
#define TLC59116_GREEN_LIST  { TLC59116_ADDR(0,0,0,1) }
#define TLC59116_BLUE_LIST   { TLC59116_ADDR(0,0,1,0) }

/** Broche de RESET des contr�leurs, si non d�fini, un RESET SOFT est effectu� */
#define TLC59116_RESET 5
#define TLC59116_RESET_PORT PORTD
#define TLC59116_RESET_DDR  DDRD

/* ========================================================================== */
#endif  /* _AVRIO_BOARD_LEDRGB_H_ not defined */
