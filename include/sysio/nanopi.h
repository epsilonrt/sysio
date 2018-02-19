/**
 * @file nanopi.h
 * @brief NanoPi
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_NANOPI_H_
#define _SYSIO_NANOPI_H_
#include <sysio/defs.h>
#ifdef __cplusplus
  extern "C" {
#endif
/* ========================================================================== */

/**
 *  @defgroup sysio_nanopi NanoPi
 *
 *  Ce module fournit des éléments spécifiques au NanoPi.
 *  @{
 * La numérotation logique SysIo des broches de GPIO est la suivante: \n

  \n Modèle Neo, Neo2, Neo2 Plus, Neo Air\n  <hr>
  
  \n Connecteur CON1 (2x12) \n 
  | Mcu | SysIo | Name             | Physical | Name             | SysIo | Mcu |
  | :-: | :---: | :--------------: | :------: | :--------------: | :---: | :-: |
  |     |       |       3V3        |  1 - 2   |       5V         |       |     |
  | 12  |   8   | I2C0_SDA/GPIOA12 |  3 - 4   |       5V         |       |     |
  | 11  |   9   | I2C0_SCL/GPIOA11 |  5 - 6   |       GND        |       |     |
  | 91  |   7   | GPIOG11          |  7 - 8   | UART1_TX/GPIOG6  |  15   | 86  |
  |     |       |       GND        |  9 - 10  | UART1_RX/GPIOG7  |  16   | 87  |
  | 0   |   0   | UART2_TX/GPIOA0  | 11 - 12  | PWM1/GPIOA6      |  1    | 6   |
  | 2   |   2   | UART2_RTS/GPIOA2 | 13 - 14  |       GND        |       |     |
  | 3   |   3   | UART2_CTS/GPIOA3 | 15 - 16  | UART1_RTS/GPIOG8 |  4    | 88  |
  |     |       |       3V3        | 17 - 18  | UART1_CTS/GPIOG9 |  5    | 89  |
  | 22  |  12   | SPI0_MOSI/GPIOC0 | 19 - 20  |       GND        |       |     |
  | 23  |  13   | SPI0_MISO/GPIOC1 | 21 - 22  | UART2_RX/GPIOA1  |  6    | 1   |
  | 24  |  14   | SPI0_CLK/GPIOC2  | 23 - 24  | SPI0_CS/GPIOC3   |  10   | 25  |

  \n Connecteur Debug UART (1x4) \n 

  | Mcu | SysIo | Name                         | Physical |
  | :-: | :---: | :--------------------------: | :------: |
  |     |       | GND                          | 1 (101)  |
  |     |       | 5V                           | 2 (102)  |
  | 4   | 17    | UART0_TX/GPIOA4              | 3 (103)  |
  | 5   | 18    | UART0_RX/GPIOA5              | 4 (104)  |

  \n Composants implantés sur la carte \n

  | Mcu | SysIo | Name                         | Physical |
  | :-: | :---: | :--------------------------: | :------: |
  | 10  | 19    | STATUS-LED/GPIOA10 (ACT:H)   | 1 (201)  |
  | 104 | 32    | POWER-LED/GPIOL10  (ACT:H)   | 2 (202)  |

  \n Connecteur CON2 (1x12) \n
  
  | Mcu | SysIo | Name                         | Physical |
  | :-: | :---: | :--------------------------: | :------: |
  |     |       | 5V                           | 1 (301)  |
  |     |       | USB1+                        | 2 (302)  |
  |     |       | USB1-                        | 3 (303)  |
  |     |       | USB2+                        | 4 (304)  |
  |     |       | USB2-                        | 5 (305)  |
  | 105 | 20    | IR-RX/GPIOL11                | 6 (306)  |
  | 17  | 11    | SPDIF-OUT/GPIOA17            | 7 (307)  |
  | 18  | 31    | PCM0_SYNC/I2S0_LRCK/GPIOA18  | 8 (308)  |
  | 19  | 30    | PCM0_CLK/I2S0_BCK/GPIOA19    | 9 (309)  |
  | 20  | 21    | PCM0_DOUT/I2S0_SDOUT/GPIOA20 | 10 (310) |
  | 21  | 22    | PCM0_DIN/I2S0_SDIN/GPIOA21   | 11 (311) |
  |     |       | GND                          | 12 (312) |


  \n Modèle M1, M1 Plus \n  <hr>
  
  \n Connecteur CON1 (2x20) \n
  | H3  | SysIo | Name                        | Physical | Name                        | SysIo | H3  |
  | :-: | :---: | :-------------------------: | :------: | :-------------------------: | :---: | :-: |
  |     |       |       3V3                   |  1 - 2   |       5V                    |       |     |
  | 12  |   8   | I2C0_SDA/GPIOA12            |  3 - 4   |       5V                    |       |     |
  | 11  |   9   | I2C0_SCL/GPIOA11            |  5 - 6   |       GND                   |       |     |
  | 91  |   7   | GPIOG11                     |  7 - 8   | UART1_TX/GPIOG6             |  15   | 86  |
  |     |       |       GND                   |  9 - 10  | UART1_RX/GPIOG7             |  16   | 87  |
  | 0   |   0   | UART2_TX/GPIOA0             | 11 - 12  | PWM1/GPIOA6                 |  1    | 6   |
  | 2   |   2   | UART2_RTS/GPIOA2            | 13 - 14  |       GND                   |       |     |
  | 3   |   3   | UART2_CTS/GPIOA3            | 15 - 16  | UART1_RTS/GPIOG8            |  4    | 88  |
  |     |       |       3V3                   | 17 - 18  | UART1_CTS/GPIOG9            |  5    | 89  |
  | 22  |  12   | SPI0_MOSI/GPIOC0            | 19 - 20  |       GND                   |       |     |
  | 23  |  13   | SPI0_MISO/GPIOC1            | 21 - 22  | UART2_RX/GPIOA1             |  6    | 1   |
  | 24  |  14   | SPI0_CLK/GPIOC2             | 23 - 24  | SPI0_CS/GPIOC3              |  10   | 25  |
  |     |       |       GND                   | 25 - 26  | SPDIF-OUT/GPIOA17           |  11   | 17  |
  | 19  |  30   | I2C1_SDA/GPIOA19            | 27 - 28  | I2C1_SCL/GPIOA18            |  31   | 18  |
  | 20  |  21   | GPIOA20                     | 29 - 30  |       GND                   |       |     |
  | 21  |  22   | GPIOA21                     | 31 - 32  | GPIOA7                      |  26   | 7   |
  | 8   |  23   | GPIOA8                      | 33 - 34  |       GND                   |       |     |
  | 16  |  24   | UART3_CTS/SPI1_MISO/GPIOA16 | 35 - 36  | UART3_TX/SPI1_CS/GPIOA13    |  27   | 13  |
  | 9   |  25   | GPIOA9                      | 37 - 38  | UART3_RTS/SPI1_MOSI/GPIOA15 |  28   | 15  |
  |     |       |       GND                   | 39 - 40  | UART3_RX/SPI1_CLK/GPIOA14   |  29   | 14  |

  \n Connecteur Debug UART (1x4) \n 

  | Mcu | SysIo | Name                         | Physical |
  | :-: | :---: | :--------------------------: | :------: |
  |     |       | GND                          | 1 (101)  |
  |     |       | 5V                           | 2 (102)  |
  | 4   | 17    | UART0_TX/GPIOA4              | 3 (103)  |
  | 5   | 18    | UART0_RX/GPIOA5              | 4 (104)  |

  \n Composants implantés sur la carte \n

  | Mcu | SysIo | Name                         | Physical |
  | :-: | :---: | :--------------------------: | :------: |
  | 10  | 19    | STATUS-LED/GPIOA10 (ACT:H)   | 1 (201)  |
  | 104 | 32    | POWER-LED/GPIOL10  (ACT:H)   | 2 (202)  |
  | 97  | 33    | K1/GPIOL3          (ACT:L)   | 3 (203)  |
  | 105 | 20    | IR-RX/GPIOL11      (ACT:L)   | 4 (204)  |
 */

/**
 * @brief Modèle de NanoPi
 */
typedef enum  {
  eNanoPiModelNeo,
  eNanoPiModelNeo2,
  eNanoPiModelNeo2Plus,
  eNanoPiModelNeoAir,
  eNanoPiModelNeoM1,
  eNanoPiModelNeoM1Plus,
  eNanoPiModelUnknown = -1
} eNanoPiModel;

/**
 * @brief Type de MCU SoC
 */
typedef enum  {
  eNanoPiMcuH3,
  eNanoPiMcuH5,
  eNanoPiMcuUnknown = -1
} eNanoPiMcu;

/* structures =============================================================== */
/**
 * @brief Information sur le NanoPi
 */
typedef struct xNanoPi {
  eNanoPiModel eModel; /**< Modèle */
  int iGpioRev; /**< Révision du GPIO */
  eNanoPiMcu eMcu; /**< Type de MCU SoC */
  int iMemMB; /**< Quantité de mémoire en MB */
  const char * sManufacturer; /**< Nom du fabricant */
} xNanoPi;

/* internal public functions ================================================ */
/**
  @brief Retourne le modèle de carte nanopi
  @return le modèle de carte nanopi
 */
eNanoPiModel eNanoPiBoard (void);

/**
 * @brief Lecture des informations sur le NanoPi
 * @return Pointeur sur les informations, NULL si erreur
 */
const xNanoPi * pxNanoPiInfo (void);

/**
 * @brief Chaîne de caractères correspondant à un modèle
 * 
 * @param eModel modèle
 * @return string correspondante
 */
const char * sNanoPiModelToStr (eNanoPiModel eModel);

/**
 * @brief Chaîne de caractères correspondant à un mcu
 * 
 * @param eMcu mcu
 * @return string correspondante
 */
const char * sNanoPiMcuToStr (eNanoPiMcu eMcu);


/* constants ================================================================ */
#define GPIO_A0     0
#define GPIO_A1     6
#define GPIO_A2     2
#define GPIO_A3     3
#define GPIO_A4     17
#define GPIO_A5     18
#define GPIO_A6     1
#define GPIO_A7     26
#define GPIO_A8     23
#define GPIO_A9     25
#define GPIO_A10    19
#define GPIO_A11    9
#define GPIO_A12    8
#define GPIO_A13    27
#define GPIO_A14    29
#define GPIO_A15    28
#define GPIO_A16    24
#define GPIO_A17    11
#define GPIO_A18    31
#define GPIO_A19    30
#define GPIO_A20    21
#define GPIO_A21    22

#define GPIO_C0     12
#define GPIO_C1     13
#define GPIO_C2     14
#define GPIO_C3     10

#define GPIO_G6     15
#define GPIO_G7     16
#define GPIO_G8     4
#define GPIO_G9     5

#define GPIO_G11    7

#define GPIO_L3     33

#define GPIO_L10    32
#define GPIO_L11    20

#define LED_STATUS  GPIO_A10
#define LED_POWER   GPIO_L10
#define IR_RX       GPIO_L11

#define PWM1        1

#define I2C0_SDA    8
#define I2C0_SCL    9

#define I2C1_SDA    30
#define I2C1_SCL    31

#define SPI0_CS     10
#define SPI0_MOSI   12
#define SPI0_MISO   13
#define SPI0_CLK    14

#define SPI1_CS     27
#define SPI1_MOSI   28
#define SPI1_MISO   24
#define SPI1_CLK    29

#define UART1_TX    15
#define UART1_RX    16
#define UART1_RTS   4
#define UART1_CTS   5

#define UART2_TX    0
#define UART2_RX    6
#define UART2_RTS   2
#define UART2_CTS   3

#define UART3_TX    27
#define UART3_RX    29
#define UART3_RTS   28
#define UART3_CTS   24

#define SPDIF_OUT   11

// Raspberry Pi Names
#define GPIO_GEN0  0
#define GPIO_GEN1  1
#define GPIO_GEN2  2
#define GPIO_GEN3  3
#define GPIO_GEN4  4
#define GPIO_GEN5  5
#define GPIO_GEN6  6
#define GPIO_GEN7  7

#define I2C_SDA   8
#define I2C_SCL   9

#define SPI_CE0_N 10
// --- incompatible #define SPI_CE1_N 11
#define SPI_MOSI  12
#define SPI_MISO  13
#define SPI_SCLK  14

#define UART_TXD  15
#define UART_RXD  16

/**
 *  @}
 */

#if ! defined(__DOXYGEN__)
// -----------------------------------------------------------------------------
/* Allwinner H3  internals ================================================== */
#define H3_IO1_BASE  (0x01C20000)
#define H3_IO2_BASE  (0x01F02000)
#define H3_PAGE_SIZE  (4*1024)
#define H3_BLOCK_SIZE (4*1024)
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
#ifdef __cplusplus
  }
#endif
#endif /*_SYSIO_NANOPI_H_ defined */
