/*
 * rpi.h
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_RPI_H_
#define _SYSIO_RPI_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_rpi Raspberry Pi
 *
 *  Ce module fournit des éléments spécifiques au Raspberry Pi
 *  @{
 */

/* internal public functions ================================================ */

/**
  @brief Retourne la revision du raspberry
  @return la révision matérielle, 0 si le système n'est pas un rpi ou -1
  en cas d'erreur. \n
  Board Revision History: \n
  <hr>
  <table>
  <th><td>Revision</td><td>Release Date</td><td>Model</td><td>PCB Revision</td><td>Memory</td><td>Notes</td></th>
  <tr><td>Beta</td><td>Q1 2012</td><td>B (Beta)</td><td>?</td><td>256MB</td><td>Beta Board</td></tr>
  <tr><td>0002</td><td>Q1 2012</td><td>B</td><td>1.0</td><td>256MB</td><td></td></tr>
  <tr><td>0003</td><td>Q3 2012</td><td>B (ECN0001)</td><td>1.0</td><td>256MB</td><td>Fuses mod and D14 removed</td></tr>
  <tr><td>0004</td><td>Q3 2012</td><td>B</td><td>2.0</td><td>256MB</td><td>Mfg by Sony</td></tr>
  <tr><td>0005</td><td>Q4 2012</td><td>B</td><td>2.0</td><td>256MB</td><td>Mfg by Qisda</td></tr>
  <tr><td>0006</td><td>Q4 2012</td><td>B</td><td>2.0</td><td>256MB</td><td>Mfg by Egoman</td></tr>
  <tr><td>0007</td><td>Q1 2013</td><td>A</td><td>2.0</td><td>256MB</td><td>Mfg by Egoman</td></tr>
  <tr><td>0008</td><td>Q1 2013</td><td>A</td><td>2.0</td><td>256MB</td><td>Mfg by Sony</td></tr>
  <tr><td>0009</td><td>Q1 2013</td><td>A</td><td>2.0</td><td>256MB</td><td>Mfg by Qisda</td></tr>
  <tr><td>000d</td><td>Q4 2012</td><td>B</td><td>2.0</td><td>512MB</td><td>Mfg by Egoman</td></tr>
  <tr><td>000e</td><td>Q4 2012</td><td>B</td><td>2.0</td><td>512MB</td><td>Mfg by Sony</td></tr>
  <tr><td>000f</td><td>Q4 2012</td><td>B</td><td>2.0</td><td>512MB</td><td>Mfg by Qisda</td></tr>
  <tr><td>0010</td><td>Q3 2014</td><td>B+</td><td>1.0</td><td>512MB</td><td>Mfg by Sony</td></tr>
  <tr><td>0011</td><td>Q2 2014</td><td>Compute Module 1.0</td><td>512MB</td><td>Mfg by Sony</td></tr>
  <tr><td>0012</td><td>Q4 2014</td><td>A+</td><td>1.0</td><td>256MB</td><td>Mfg by Sony</td></tr>
  </table>
 */
int iRpiRev (void);

/**
 *  @defgroup sysio_rpi_pin Raspberry Pi
 *
 *  Ce module décrit la numérotation logique des broches matérielles du
 *  Raspberry Pi
 *  @{
 */
/* constants ================================================================ */
#define GPIO_GEN0  0
#define GPIO_GEN1  1
#define GPIO_GEN2  2
#define GPIO_GEN3  3
#define GPIO_GEN4  4
#define GPIO_GEN5  5
#define GPIO_GEN6  6
#define GPIO_GCLK  7

#define I2C_SDA1  8
#define I2C_SCL1  9

#define SPI_CE0_N  10
#define SPI_CE1_N  11
#define SPI_MOSI  12
#define SPI_MISO  13
#define SPI_SCLK  14

#define UART_TXD  15
#define UART_RXD  16

#define GPIO_GEN7  17
#define GPIO_GEN8  18
#define GPIO_GEN9  19
#define GPIO_GEN10  20

/**
 *  @}
 * @}
 */

#if !defined(__DOXYGEN__)
// -----------------------------------------------------------------------------

/* bcm2708 internals ======================================================== */
#define BCM2708_IO_BASE  (0x20000000)
#define BCM2708_GPIO_BASE  (BCM2708_IO_BASE + 0x200000)
#define BCM2708_PWM_BASE  (BCM2708_IO_BASE + 0x20C000)
#define BCM2708_CLK_BASE  (BCM2708_IO_BASE + 0x101000)
#define BCM2708_PAGE_SIZE  (4*1024)
#define BCM2708_BLOCK_SIZE  (4*1024)

#define BCM2708_PWM_FCLK 19200000.0
// -----------------------------------------------------------------------------
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_RPI_H_ defined */
