/**
 * @file sysio/rpi.h
 * @brief Raspberry Pi
 *
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
 *  Ce module fournit des éléments spécifiques au Raspberry Pi. La numérotation
 *  logique SysIo des broches de GPIO est la suivante: \n
  <hr>

  \n Connecteur J8 (Modèle A+, B+, 2B, Zero) \n
  | BCM | SysIo | Name       | Physical | Name       | SysIo | BCM |
  | :-- | :---- | :--------- | :------- | :--------  | :---- | :-- |
  |     |       |    3V3     |  1 || 2  |    5V      |       |     |
  |  2  |   8   | I2C_SDA1   |  3 || 4  |    5V      |       |     |
  |  3  |   9   | I2C_SCL1   |  5 || 6  |    GND     |       |     |
  |  4  |   7   | GPIO_GCLK  |  7 || 8  | UART_TXD0  |  15   | 14  |
  |     |       |    GND     |  9 || 10 | UART_RXD0  |  16   | 15  |
  | 17  |   0   | GPIO_GEN0  | 11 || 12 | GPIO_GEN1  |   1   | 18  |
  | 27  |   2   | GPIO_GEN2  | 13 || 14 |    GND     |       |     |
  | 22  |   3   | GPIO_GEN3  | 15 || 16 | GPIO_GEN4  |   4   | 23  |
  |     |       |    3V3     | 17 || 18 | GPIO_GEN5  |   5   | 24  |
  | 10  |  12   | SPI_MOSI   | 19 || 20 |    GND     |       |     |
  |  9  |  13   | SPI_MISO   | 21 || 22 | GPIO_GEN6  |   6   | 25  |
  | 11  |  14   | SPI_SCLK   | 23 || 24 | SPI_CE0_N  |  10   |  8  |
  |     |       |    GND     | 25 || 26 | SPI_CE1_N  |  11   |  7  |
  |  0  |  30   | I2C_SDA0   | 27 || 28 | I2C_SCL0   |  31   |  1  |
  |  5  |  21   | GPIO_GEN11 | 29 || 30 |    GND     |       |     |
  |  6  |  22   | LAN_RUN    | 31 || 32 | GPIO_GEN15 |       | 12  |
  | 13  |  23   | GPIO_GEN12 | 33 || 34 |    GND     |       |     |
  | 19  |  24   | GPIO_GEN13 | 35 || 36 | STATUS_LED |       | 16  |
  | 26  |  25   | GPIO_GEN14 | 37 || 38 | GPIO_GEN17 |       | 20  |
  |     |       |    GND     | 39 || 40 | GPIO_GEN18 |       | 21  |

  \n Connecteur P1 (Modèle A et B PCB V2.0) \n
  | BCM | SysIo | Name       | Physical | Name       | SysIo | BCM |
  | :-- | :---- | :--------- | :------- | :--------  | :---- | :-- |
  |     |       |    3V3     |  1 || 2  |    5V      |       |     |
  |  2  |   8   | I2C_SDA1   |  3 || 4  |    5V      |       |     |
  |  3  |   9   | I2C_SCL1   |  5 || 6  |    GND     |       |     |
  |  4  |   7   | GPIO_GCLK  |  7 || 8  | UART_TXD0  |  15   | 14  |
  |     |       |    GND     |  9 || 10 | UART_RXD0  |  16   | 15  |
  | 17  |   0   | GPIO_GEN0  | 11 || 12 | GPIO_GEN1  |   1   | 18  |
  | 27  |   2   | GPIO_GEN2  | 13 || 14 |    GND     |       |     |
  | 22  |   3   | GPIO_GEN3  | 15 || 16 | GPIO_GEN4  |   4   | 23  |
  |     |       |    3V3     | 17 || 18 | GPIO_GEN5  |   5   | 24  |
  | 10  |  12   | SPI_MOSI   | 19 || 20 |    GND     |       |     |
  |  9  |  13   | SPI_MISO   | 21 || 22 | GPIO_GEN6  |   6   | 25  |
  | 11  |  14   | SPI_SCLK   | 23 || 24 | SPI_CE0_N  |  10   |  8  |
  |     |       |    GND     | 25 || 26 | SPI_CE1_N  |  11   |  7  |

  \n Connecteur P5 (Modèle A et B PCB V2.0), la broche Physical 27 correspond 
  à la broche 1 du connecteur P5: \n
  | BCM | SysIo | Name       | Physical | Name       | SysIo | BCM |
  | :-- | :---- | :--------- | :------- | :--------- | :---- | :-- |
  |     |       |    5V      | 27 || 28 |    3V3     |       |     |
  | 28  |  17   | GPIO_GEN7  | 29 || 30 | GPIO_GEN8  |  18   | 29  |
  | 30  |  19   | GPIO_GEN9  | 31 || 32 | GPIO_GEN10 |  20   | 31  |
  |     |       |    GND     | 33 || 33 |    GND     |       |     |

  \n Connecteur P1 (Modèle B PCB V1.0, Q3 2012, Board Rev <= 3) \n
  | BCM | SysIo | Name       | Physical | Name       | SysIo | BCM |
  | :-- | :---- | :--------- | :------- | :--------  | :---- | :-- |
  |     |       |    3V3     |  1 || 2  |    5V      |       |     |
  |  0  |   8   | I2C_SDA    |  3 || 4  |    5V      |       |     |
  |  1  |   9   | I2C_SCL    |  5 || 6  |    GND     |       |     |
  |  4  |   7   | GPIO_GCLK  |  7 || 8  | UART_TXD0  |  15   | 14  |
  |     |       |    GND     |  9 || 10 | UART_RXD0  |  16   | 15  |
  | 17  |   0   | GPIO_GEN0  | 11 || 12 | GPIO_GEN1  |   1   | 18  |
  | 21  |   2   | GPIO_GEN2  | 13 || 14 |    GND     |       |     |
  | 22  |   3   | GPIO_GEN3  | 15 || 16 | GPIO_GEN4  |   4   | 23  |
  |     |       |    3V3     | 17 || 18 | GPIO_GEN5  |   5   | 24  |
  | 10  |  12   | SPI_MOSI   | 19 || 20 |    GND     |       |     |
  |  9  |  13   | SPI_MISO   | 21 || 22 | GPIO_GEN6  |   6   | 25  |
  | 11  |  14   | SPI_SCLK   | 23 || 24 | SPI_CE0_N  |  10   |  8  |
  |     |       |    GND     | 25 || 26 | SPI_CE1_N  |  11   |  7  |
 *  @{
 */

/**
 * @brief Modèle de Raspberry Pi
 */
typedef enum  {
  eRpiModelA,
  eRpiModelB,
  eRpiModelAPlus,
  eRpiModelBPlus,
  eRpiModelComputeModule,
  eRpiModel2B,
  eRpiModelZero,
  eRpiModelUnknown = -1
} eRpiModel;

/**
 * @brief Type de MCU SoC
 */
typedef enum  {
  eRpiMcuBcm2708,
  eRpiMcuBcm2709,
  eRpiMcuUnknown = -1
} eRpiMcu;

/**
 * @brief Information sur le Raspberry Pi
 */
typedef struct xRpi {
  int iRev; /**< Numéro de révision identifiant le RPi */
  eRpiModel eModel; /**< Modèle */
  int iGpioRev; /**< Révision du GPIO */
  eRpiMcu eMcu; /**< Type de MCU SoC */
  int iMemMB; /**< Quantité de mémoire en MB */
  int iPcbMajor; /**< Majeur du PCB */
  int iPcbMinor; /**< Mineur du PCB */
  const char * sManufacturer; /**< Nom du fabricant */
} xRpi;

/* internal public functions ================================================ */

/**
  @brief Retourne la revision du raspberry
  @return la révision matérielle, 0 si le système n'est pas un rpi ou -1
  en cas d'erreur. \n
 */
int iRpiRev (void);

/**
 * @brief Lecture des informations sur le Raspberry Pi
 * @return Pointeur sur les informations, NULL si erreur
  Board Revision History (from <A HREF="http://elinux.org/RPi_HardwareHistory">elinux.org</A>) : \n
  <hr>
  | Revision | Release Date | Model          | PCB Revision | Memory | Notes                     |
  | :------- | :----------- | :------------- | :----------- | :----- | :------------------------ |
  | Beta     | Q1 2012      | B (Beta)       | ?            | 256MB  | Beta Board                |
  | 0002     | Q1 2012      | B              | 1.0          | 256MB  |                           |
  | 0003     | Q3 2012      | B (ECN0001)    | 1.0          | 256MB  | Fuses mod and D14 removed |
  | 0004     | Q3 2012      | B              | 2.0          | 256MB  | Mfg by Sony               |
  | 0005     | Q4 2012      | B              | 2.0          | 256MB  | Mfg by Qisda              |
  | 0006     | Q4 2012      | B              | 2.0          | 256MB  | Mfg by Egoman             |
  | 0007     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Egoman             |
  | 0008     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Sony               |
  | 0009     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Qisda              |
  | 000d     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Egoman             |
  | 000e     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Sony               |
  | 000f     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Qisda              |
  | 0010     | Q3 2014      | B+             | 1.0          | 512MB  | Mfg by Sony               |
  | 0011     | Q2 2014      | Compute Module | 1.0          | 512MB  | Mfg by Sony               |
  | 0012     | Q4 2014      | A+             | 1.0          | 256MB  | Mfg by Sony               |
  | 0013     | Q1 2015      | B+             | 1.2          | 512MB  | ?                         |
  | a01041   | Q1 2015      | 2 Model B      | 1.1          | 1GB    | Mfg by Sony               |
  | a21041   | Q1 2015      | 2 Model B      | 1.1          | 1GB    | Mfg by Embest, China      |
  | 900092   | Q4 2015      | Zero           | 1.2          | 512MB  | Mfg by Sony               |
 */
const xRpi * pxRpiInfo (void);


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

#define I2C_SDA   8
#define I2C_SCL   9
#define I2C_SDA1  8
#define I2C_SCL1  9
#define I2C_SDA0  30
#define I2C_SCL0  31

#define SPI_CE0_N 10
#define SPI_CE1_N 11
#define SPI_MOSI  12
#define SPI_MISO  13
#define SPI_SCLK  14

#define UART_TXD  15
#define UART_RXD  16
#define UART_TXD0 15
#define UART_RXD0 16

// Models A & B PCB V 2.0 only
#define GPIO_GEN7  17
#define GPIO_GEN8  18
#define GPIO_GEN9  19
#define GPIO_GEN10 20

// Models A+, B+, 2B, Zero only
#define GPIO_GEN11 21
#define LAN_RUN    22
#define GPIO_GEN12 23
#define GPIO_GEN13 24
#define GPIO_GEN14 25
#define GPIO_GEN15 26
#define STATUS_LED 27
#define GPIO_GEN17 28
#define GPIO_GEN18 29

#define GPIO17  0
#define GPIO18  1
#define GPIO27  2 // Models A & B PCB V2.0, A+, B+, 2B, Zero 
#define GPIO22  3
#define GPIO23  4
#define GPIO24  5
#define GPIO25  6
#define GPIO4   7

#define GPIO2   8  // Models A & B PCB V2.0, A+, B+, 2B, Zero 
#define GPIO3   9  // Models A & B PCB V2.0, A+, B+, 2B, Zero 
#define GPIO0   30
#define GPIO1   31

#define GPIO8   10
#define GPIO7   11
#define GPIO10  12
#define GPIO9   13
#define GPIO11  14

#define GPIO14  15
#define GPIO15  16

// Models A & B PCB V 2.0 only
#define GPIO28  17
#define GPIO29  18
#define GPIO30  19
#define GPIO31  20

// Models A+, B+, 2B, Zero only
#define GPIO5   21
#define GPIO6   22
#define GPIO13  23
#define GPIO19  24
#define GPIO26  25
#define GPIO12  26
#define GPIO16  27
#define GPIO20  28
#define GPIO21  29

/**
 *  @}
 */

#if defined(__DOXYGEN__)
/**
 * @brief Adresse de base des entrées-sorties
 */
static inline  unsigned long ulRpiIoBase(void);

/**
 * @}
 */

#else /* ! defined(__DOXYGEN__) */
// -----------------------------------------------------------------------------
/*
  GPIO_PADS     = RASPBERRY_PI_PERI_BASE + 0x00100000 ;
  GPIO_TIMER    = RASPBERRY_PI_PERI_BASE + 0x0000B000 ;
 */
/* bcm2708 internals ======================================================== */
#define BCM2708_IO_BASE    (0x20000000)
#define BCM2709_IO_BASE    (0x3F000000)
#define BCM270X_PAGE_SIZE  (4*1024)
#define BCM270X_BLOCK_SIZE (4*1024)

// -----------------------------------------------------------------------------
INLINE unsigned long
ulRpiIoBase(void) {
  
  return (pxRpiInfo()->eMcu == eRpiMcuBcm2708) ? BCM2708_IO_BASE : BCM2709_IO_BASE;
}
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_RPI_H_ defined */
