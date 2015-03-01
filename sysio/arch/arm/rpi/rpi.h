/*
 * rpi.h
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _LCDIO_RPI_H_
#define _LCDIO_RPI_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/* constants ================================================================ */
#define GPIO_GEN0   0
#define GPIO_GEN1   1
#define GPIO_GEN2   2
#define GPIO_GEN3   3
#define GPIO_GEN4   4
#define GPIO_GEN5   5
#define GPIO_GEN6   6
#define GPIO_GCLK   7

#define I2C_SDA1    8
#define I2C_SCL1    9

#define SPI_CE0_N   10
#define SPI_CE1_N   11
#define SPI_MOSI    12
#define SPI_MISO    13
#define SPI_SCLK    14

#define UART_TXD    15
#define UART_RXD    16

#define GPIO_GEN7   17
#define GPIO_GEN8   18
#define GPIO_GEN9   19
#define GPIO_GEN10  20

/* internal public functions ================================================ */
/*
  Retourne la revision du raspberry, 0 si le système n'est pas un rpi ou -1
  en cas d'erreur.

  Board Revision History
  ------------------------------------------------------------------------------
  Revision  Release Date  Model           PCB Revision  Memory  Notes
  Beta      Q1 2012       B (Beta)        ?             256MB   Beta Board
  0002      Q1 2012       B               1.0           256MB
  0003      Q3 2012       B (ECN0001)     1.0           256MB   Fuses mod and D14 removed
  0004      Q3 2012       B               2.0           256MB   (Mfg by Sony)
  0005      Q4 2012       B               2.0           256MB   (Mfg by Qisda)
  0006      Q4 2012       B               2.0           256MB   (Mfg by Egoman)
  0007      Q1 2013       A               2.0           256MB   (Mfg by Egoman)
  0008      Q1 2013       A               2.0           256MB   (Mfg by Sony)
  0009      Q1 2013       A               2.0           256MB   (Mfg by Qisda)
  000d      Q4 2012       B               2.0           512MB   (Mfg by Egoman)
  000e      Q4 2012       B               2.0           512MB   (Mfg by Sony)
  000f      Q4 2012       B               2.0           512MB   (Mfg by Qisda)
  0010      Q3 2014       B+              1.0           512MB   (Mfg by Sony)
  0011      Q2 2014       Compute Module  1.0           512MB   (Mfg by Sony)
  0012      Q4 2014       A+              1.0           256MB   (Mfg by Sony)
 */
int iRpiRev (void);

/* bcm2708 internals ======================================================== */
#define BCM2708_IO_BASE       (0x20000000)
#define BCM2708_GPIO_BASE     (BCM2708_IO_BASE + 0x200000)
#define BCM2708_PWM_BASE      (BCM2708_IO_BASE + 0x20C000)
#define BCM2708_CLK_BASE      (BCM2708_IO_BASE + 0x101000)
#define BCM2708_PAGE_SIZE     (4*1024)
#define BCM2708_BLOCK_SIZE    (4*1024)

#define BCM2708_PWM_FCLK 19200000.0

/* ========================================================================== */
__END_C_DECLS
#endif /*_LCDIO_RPI_H_ defined */
