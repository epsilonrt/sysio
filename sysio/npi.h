/**
 * @file sysio/rpi.h
 * @brief NanoPi
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_NPI_H_
#define _SYSIO_NPI_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_npi NanoPi
 *
 *  Ce module fournit des éléments spécifiques au NanoPi.
 *  @{
 * La numérotation logique SysIo des broches de GPIO est la suivante: \n

  \n Connecteur CON1 (Modèle Neo, Neo Air) \n  <hr>
  | H3  | SysIo | Name             | Physical | Name             | SysIo | H3  |
  | :-: | :---: | :--------------: | :------: | :--------------: | :---: | :-: |
  |     |       |       3V3        |  1 - 2   |       5V         |       |     |
  | A12 |   8   | I2C0_SDA         |  3 - 4   |       5V         |       |     |
  | A11 |   9   | I2C0_SCL         |  5 - 6   |       GND        |       |     |
  | G11 |   7   | GPIOG11          |  7 - 8   | UART1_TX/GPIOG6  |  G6   | 14  |
  |     |       |       GND        |  9 - 10  | UART1_RX/GPIOG7  |  G7   | 15  |
  | A0  |   0   | UART2_TX/GPIOA0  | 11 - 12  | PWM1/GPIOA6      |  A6   | 18  |
  | A2  |   2   | UART2_RTS/GPIOA2 | 13 - 14  |       GND        |       |     |
  | A3  |   3   | UART2_CTS/GPIOA3 | 15 - 16  | UART1_RTS/GPIOG8 |  G8   | 23  |
  |     |       |       3V3        | 17 - 18  | UART1_CTS/GPIOG9 |  G9   | 24  |
  | C0  |  12   | SPI0_MOSI/GPIOC0 | 19 - 20  |       GND        |       |     |
  | C1  |  13   | SPI0_MISO/GPIOC1 | 21 - 22  | UART2_RX/GPIOA1  |  A1   | 25  |
  | C2  |  14   | SPI0_CLK/GPIOC29 | 23 - 24  | SPI0_CS/GPIOC3   |  C3   |  8  |
 */

/**
 * @brief Modèle de NanoPi
 */
typedef enum  {
  eNpiModelNeo,
  eNpiModelNeoAir,
  eNpiModelNeoM1,
  eNpiModelUnknown = -1
} eNpiModel;

/**
 * @brief Type de MCU SoC
 */
typedef enum  {
  eNpiMcuH3,
  eNpiMcuUnknown = -1
} eNpiMcu;

/**
 * @brief Information sur le NanoPi
 */
typedef struct xNpi {
  int iRev; /**< Numéro de révision identifiant le NPi */
  eNpiModel eModel; /**< Modèle */
  int iGpioRev; /**< Révision du GPIO */
  eNpiMcu eMcu; /**< Type de MCU SoC */
  int iMemMB; /**< Quantité de mémoire en MB */
  int iPcbMajor; /**< Majeur du PCB */
  int iPcbMinor; /**< Mineur du PCB */
  const char * sManufacturer; /**< Nom du fabricant */
} xNpi;

/* internal public functions ================================================ */

/**
  @brief Retourne la revision du raspberry
  @return la révision matérielle, 0 si le système n'est pas un rpi ou -1
  en cas d'erreur. \n
 */
int iNpiRev (void);

/**
 * @brief Lecture des informations sur le NanoPi
 * @return Pointeur sur les informations, NULL si erreur
 */
const xNpi * pxNpiInfo (void);

/**
 * @brief Chaîne de caractères correspondant à un modèle
 * 
 * @param eModel modèle
 * @return string correspondante
 */
const char * sNpiModelToStr (eNpiModel eModel);

/**
 * @brief Chaîne de caractères correspondant à un mcu
 * 
 * @param eMcu mcu
 * @return string correspondante
 */
const char * sNpiMcuToStr (eNpiMcu eMcu);


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

#define SPI_CE0_N 10
#define SPI_CE1_N 11
#define SPI_MOSI  12
#define SPI_MISO  13
#define SPI_SCLK  14

#define UART_TXD  15
#define UART_RXD  16
#define UART_TXD1 15
#define UART_RXD1 16

// Models M1 only
#define GPIO_GEN7  17
#define GPIO_GEN8  18
#define GPIO_GEN9  19
#define GPIO_GEN10 20

// Models A+, B+, 2B, 3B, Zero only
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
#define GPIO27  2 // Models A & B PCB V2.0, A+, B+, 2B, 3B, Zero 
#define GPIO22  3
#define GPIO23  4
#define GPIO24  5
#define GPIO25  6
#define GPIO4   7

#define GPIO2   8  // Models A & B PCB V2.0, A+, B+, 2B, 3B, Zero 
#define GPIO3   9  // Models A & B PCB V2.0, A+, B+, 2B, 3B, Zero 
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

// Models A+, B+, 2B, 3B, Zero only
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
static inline  unsigned long ulNpiIoBase(void);

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
ulNpiIoBase(void) {
  
  return (pxNpiInfo()->eMcu == eNpiMcuBcm2708) ? BCM2708_IO_BASE : BCM2709_IO_BASE;
}
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_NPI_H_ defined */
