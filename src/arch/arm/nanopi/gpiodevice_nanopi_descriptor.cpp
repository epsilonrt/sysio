/*
 * arch/arm/nanopi/gpiodevice_nanopi.cpp
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include "gpiodevice_nanopi.h"

// -----------------------------------------------------------------------------
//
//                     GpioDeviceNanoPi Gpio Descriptor
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dilConnector (int row, int column, int columns) {

  return (row - 1) * columns + column;
}

// -----------------------------------------------------------------------------
static int silConnector (int row, int column, int columns) {

  return row;
}

// -----------------------------------------------------------------------------
const std::map<int, GpioDescriptor>
GpioDeviceNanoPi::_gpioDescriptors = {
  //----------------------------------------------------------------------------
  // GPIO Rev. 1
  // NanoPi Neo, Neo2, Neo Plus2, Neo Air --->
  {
    1,
    {
      "nanopineo",
      {
        // Connecteurs
        {
          "con1", 1, 12, 2, dilConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "3.3V"}}},
            {TypePower, { -1, -1, -1, 1, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIOA12"}, {ModeOutput, "GPIOA12"}, {ModeAlt2, "I2C0SDA"}, {ModeAlt3, "DIRX"}, {ModeAlt6, "PAEINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIOA11"}, {ModeOutput, "GPIOA11"}, {ModeAlt2, "I2C0SCK"}, {ModeAlt3, "DITX"}, {ModeAlt6, "PAEINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIOG11"}, {ModeOutput, "GPIOG11"}, {ModeAlt2, "PCM1CLK"}, {ModeAlt6, "PGEINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIOG6"}, {ModeOutput, "GPIOG6"}, {ModeAlt2, "UART1TX"}, {ModeAlt6, "PGEINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIOG7"}, {ModeOutput, "GPIOG7"}, {ModeAlt2, "UART1RX"}, {ModeAlt6, "PGEINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIOA0"}, {ModeOutput, "GPIOA0"}, {ModeAlt2, "UART2TX"}, {ModeAlt3, "JTAGMS"}, {ModeAlt6, "PAEINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIOA6"}, {ModeOutput, "GPIOA6"}, {ModeAlt2, "SIMPWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PAEINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIOA2"}, {ModeOutput, "GPIOA2"}, {ModeAlt2, "UART2RTS"}, {ModeAlt3, "JTAGDO"}, {ModeAlt6, "PAEINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIOA3"}, {ModeOutput, "GPIOA3"}, {ModeAlt2, "UART2CTS"}, {ModeAlt3, "JTAGDI"}, {ModeAlt6, "PAEINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIOG8"}, {ModeOutput, "GPIOG8"}, {ModeAlt2, "UART1RTS"}, {ModeAlt6, "PGEINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIOG9"}, {ModeOutput, "GPIOG9"}, {ModeAlt2, "UART1CTS"}, {ModeAlt6, "PGEINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIOC0"}, {ModeOutput, "GPIOC0"}, {ModeAlt2, "NANDWE"}, {ModeAlt3, "SPI0MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIOC1"}, {ModeOutput, "GPIOC1"}, {ModeAlt2, "NANDALE"}, {ModeAlt3, "SPI0MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIOA1"}, {ModeOutput, "GPIOA1"}, {ModeAlt2, "UART2RX"}, {ModeAlt3, "JTAGCK"}, {ModeAlt6, "PAEINT1"}}},
            {TypeGpio, {14, 24, 66, 12, 1}, {{ModeInput, "GPIOC2"}, {ModeOutput, "GPIOC2"}, {ModeAlt2, "NANDCLE"}, {ModeAlt3, "SPI0CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIOC3"}, {ModeOutput, "GPIOC3"}, {ModeAlt2, "NANDCE1"}, {ModeAlt3, "SPI0CS"}}},
          }
        },
        {
          "dbg_uart", 2, 4, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "GND"}}},
            {TypePower, { -1, -1, -1, 2, 1}, {{ModeInput, "5V"}}},
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIOA4"}, {ModeOutput, "GPIOA4"}, {ModeAlt2, "UART0TX"}, {ModeAlt6, "PAEINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIOA5"}, {ModeOutput, "GPIOA5"}, {ModeAlt2, "UART0RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIOA10"}, {ModeOutput, "STAT_LED"}, {ModeAlt2, "SIMDET"}, {ModeAlt6, "PAEINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIOL10"}, {ModeOutput, "PWR_LED"}, {ModeAlt2, "SPWM"}, {ModeAlt6, "PLEINT10"}}},
          }
        },
        {
          "con2", 4, 12, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "5V"}}},
            {TypeUsb, { -1, -1, -1, 2, 1}, {{ModeInput, "USB-DP1"}}},
            {TypeUsb, { -1, -1, -1, 3, 1}, {{ModeInput, "USB-DM1"}}},
            {TypeUsb, { -1, -1, -1, 4, 1}, {{ModeInput, "USB-DP2"}}},
            {TypeUsb, { -1, -1, -1, 5, 1}, {{ModeInput, "USB-DM2"}}},
            {TypeGpio, {20, 105, 363, 6, 1}, {{ModeInput, "GPIOL11"}, {ModeOutput, "GPIOL11"}, {ModeAlt2, "SCIRRX"}, {ModeAlt6, "PLEINT11"}}},
            {TypeGpio, {11, 17, 17, 7, 1}, {{ModeInput, "GPIOA17"}, {ModeOutput, "GPIOA17"}, {ModeAlt2, "OWAOUT"}, {ModeAlt6, "PAEINT17"}}},
            {TypeGpio, {31, 18, 18, 8, 1}, {{ModeInput, "GPIOA18"}, {ModeOutput, "GPIOA18"}, {ModeAlt2, "PCM0SYNC"}, {ModeAlt3, "I2C1SCK"}, {ModeAlt6, "PAEINT18"}}},
            {TypeGpio, {30, 19, 19, 9, 1}, {{ModeInput, "GPIOA19"}, {ModeOutput, "GPIOA19"}, {ModeAlt2, "PCM0CLK"}, {ModeAlt3, "I2C1SDA"}, {ModeAlt6, "PAEINT19"}}},
            {TypeGpio, {21, 20, 20, 10, 1}, {{ModeInput, "GPIOA20"}, {ModeOutput, "GPIOA20"}, {ModeAlt2, "PCM0DOUT"}, {ModeAlt3, "SIMVPPEN"}, {ModeAlt6, "PAEINT20"}}},
            {TypeGpio, {22, 21, 21, 11, 1}, {{ModeInput, "GPIOA21"}, {ModeOutput, "GPIOA21"}, {ModeAlt2, "PCM0DIN"}, {ModeAlt3, "SIMVPPPP"}, {ModeAlt6, "PAEINT21"}}},
            {TypePower, { -1, -1, -1, 12, 1}, {{ModeInput, "GND"}}},
          }
        },
      }
    }
  },
  // <--- NanoPi Neo, Neo2, Neo Plus2, Neo Air
  //----------------------------------------------------------------------------
  // GPIO Rev. 2
  // NanoPi M1 --->
  {
    2,
    {
      "nanopim1",
      {
        // Connecteurs
        {
          "con1", 1, 20, 2, dilConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "3.3V"}}},
            {TypePower, { -1, -1, -1, 1, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIOA12"}, {ModeOutput, "GPIOA12"}, {ModeAlt2, "I2C0SDA"}, {ModeAlt3, "DIRX"}, {ModeAlt6, "PAEINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIOA11"}, {ModeOutput, "GPIOA11"}, {ModeAlt2, "I2C0SCK"}, {ModeAlt3, "DITX"}, {ModeAlt6, "PAEINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIOG11"}, {ModeOutput, "GPIOG11"}, {ModeAlt2, "PCM1CLK"}, {ModeAlt6, "PGEINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIOG6"}, {ModeOutput, "GPIOG6"}, {ModeAlt2, "UART1TX"}, {ModeAlt6, "PGEINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIOG7"}, {ModeOutput, "GPIOG7"}, {ModeAlt2, "UART1RX"}, {ModeAlt6, "PGEINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIOA0"}, {ModeOutput, "GPIOA0"}, {ModeAlt2, "UART2TX"}, {ModeAlt3, "JTAGMS"}, {ModeAlt6, "PAEINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIOA6"}, {ModeOutput, "GPIOA6"}, {ModeAlt2, "SIMPWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PAEINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIOA2"}, {ModeOutput, "GPIOA2"}, {ModeAlt2, "UART2RTS"}, {ModeAlt3, "JTAGDO"}, {ModeAlt6, "PAEINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIOA3"}, {ModeOutput, "GPIOA3"}, {ModeAlt2, "UART2CTS"}, {ModeAlt3, "JTAGDI"}, {ModeAlt6, "PAEINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIOG8"}, {ModeOutput, "GPIOG8"}, {ModeAlt2, "UART1RTS"}, {ModeAlt6, "PGEINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIOG9"}, {ModeOutput, "GPIOG9"}, {ModeAlt2, "UART1CTS"}, {ModeAlt6, "PGEINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIOC0"}, {ModeOutput, "GPIOC0"}, {ModeAlt2, "NANDWE"}, {ModeAlt3, "SPI0MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIOC1"}, {ModeOutput, "GPIOC1"}, {ModeAlt2, "NANDALE"}, {ModeAlt3, "SPI0MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIOA1"}, {ModeOutput, "GPIOA1"}, {ModeAlt2, "UART2RX"}, {ModeAlt3, "JTAGCK"}, {ModeAlt6, "PAEINT1"}}},
            {TypeGpio, {14, 24, 66, 12, 1}, {{ModeInput, "GPIOC2"}, {ModeOutput, "GPIOC2"}, {ModeAlt2, "NANDCLE"}, {ModeAlt3, "SPI0CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIOC3"}, {ModeOutput, "GPIOC3"}, {ModeAlt2, "NANDCE1"}, {ModeAlt3, "SPI0CS"}}},
            {TypePower, { -1, -1, -1, 13, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {11, 17, 17, 13, 2}, {{ModeInput, "GPIOA17"}, {ModeOutput, "GPIOA17"}, {ModeAlt2, "OWAOUT"}, {ModeAlt6, "PAEINT17"}}},
            {TypeGpio, {30, 19, 19, 14, 1}, {{ModeInput, "GPIOA19"}, {ModeOutput, "GPIOA19"}, {ModeAlt2, "PCM0CLK"}, {ModeAlt3, "I2C1SDA"}, {ModeAlt6, "PAEINT19"}}},
            {TypeGpio, {31, 18, 18, 14, 2}, {{ModeInput, "GPIOA18"}, {ModeOutput, "GPIOA18"}, {ModeAlt2, "PCM0SYNC"}, {ModeAlt3, "I2C1SCK"}, {ModeAlt6, "PAEINT18"}}},
            {TypeGpio, {21, 20, 20, 15, 1}, {{ModeInput, "GPIOA20"}, {ModeOutput, "GPIOA20"}, {ModeAlt2, "PCM0DOUT"}, {ModeAlt3, "SIMVPPEN"}, {ModeAlt6, "PAEINT20"}}},
            {TypePower, { -1, -1, -1, 15, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {22, 21, 21, 16, 1}, {{ModeInput, "GPIOA21"}, {ModeOutput, "GPIOA21"}, {ModeAlt2, "PCM0DIN"}, {ModeAlt3, "SIMVPPPP"}, {ModeAlt6, "PAEINT21"}}},
            {TypeGpio, {26, 7, 7, 16, 2}, {{ModeInput, "GPIOA7"}, {ModeOutput, "GPIOA7"}, {ModeAlt2, "SIMCLK"}, {ModeAlt6, "PAEINT7"}}},
            {TypeGpio, {23, 8, 8, 17, 1}, {{ModeInput, "GPIOA8"}, {ModeOutput, "GPIOA8"}, {ModeAlt2, "SIMDATA"}, {ModeAlt6, "PAEINT8"}}},
            {TypePower, { -1, -1, -1, 17, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {24, 16, 16, 18, 1}, {{ModeInput, "GPIOA16"}, {ModeOutput, "GPIOA16"}, {ModeAlt2, "SPI1MISO"}, {ModeAlt3, "UART3CTS"}, {ModeAlt6, "PAEINT16"}}},
            {TypeGpio, {27, 13, 13, 18, 2}, {{ModeInput, "GPIOA13"}, {ModeOutput, "GPIOA13"}, {ModeAlt2, "SPI1CS"}, {ModeAlt3, "UART3TX"}, {ModeAlt6, "PAEINT13"}}},
            {TypeGpio, {25, 9, 9, 19, 1}, {{ModeInput, "GPIOA9"}, {ModeOutput, "GPIOA9"}, {ModeAlt2, "SIMRST"}, {ModeAlt6, "PAEINT9"}}},
            {TypeGpio, {28, 15, 15, 19, 2}, {{ModeInput, "GPIOA15"}, {ModeOutput, "GPIOA15"}, {ModeAlt2, "SPI1MOSI"}, {ModeAlt3, "UART3RTS"}, {ModeAlt6, "PAEINT15"}}},
            {TypePower, { -1, -1, -1, 20, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {29, 14, 14, 20, 2}, {{ModeInput, "GPIOA14"}, {ModeOutput, "GPIOA14"}, {ModeAlt2, "SPI1CLK"}, {ModeAlt3, "UART3RX"}, {ModeAlt6, "PAEINT14"}}},
          }
        },
        {
          "dbg_uart", 2, 4, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "GND"}}},
            {TypePower, { -1, -1, -1, 2, 1}, {{ModeInput, "5V"}}},
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIOA4"}, {ModeOutput, "GPIOA4"}, {ModeAlt2, "UART0TX"}, {ModeAlt6, "PAEINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIOA5"}, {ModeOutput, "GPIOA5"}, {ModeAlt2, "UART0RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIOA10"}, {ModeOutput, "STAT_LED"}, {ModeAlt2, "SIMDET"}, {ModeAlt6, "PAEINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIOL10"}, {ModeOutput, "PWR_LED"}, {ModeAlt2, "SPWM"}, {ModeAlt6, "PLEINT10"}}},
            {TypeGpio, {33, 97, 355, 3, 1}, {{ModeInput, "BUT_K1"}, {ModeOutput, "GPIOL3"}, {ModeAlt2, "SUARTRX"}, {ModeAlt6, "PLEINT3"}}},
            {TypeGpio, {20, 105, 363, 4, 1}, {{ModeInput, "IR_RX"}, {ModeOutput, "GPIOL11"}, {ModeAlt2, "SCIRRX"}, {ModeAlt6, "PLEINT11"}}},
          }
        },
      }
    }
  },
  // <--- NanoPi M1
  //----------------------------------------------------------------------------
  // GPIO Rev. 3
  // NanoPi M1 Plus --->
  {
    3,
    {
      "nanopim1plus",
      {
        // Connecteurs
        {
          "con1", 1, 20, 2, dilConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "3.3V"}}},
            {TypePower, { -1, -1, -1, 1, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIOA12"}, {ModeOutput, "GPIOA12"}, {ModeAlt2, "I2C0SDA"}, {ModeAlt3, "DIRX"}, {ModeAlt6, "PAEINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIOA11"}, {ModeOutput, "GPIOA11"}, {ModeAlt2, "I2C0SCK"}, {ModeAlt3, "DITX"}, {ModeAlt6, "PAEINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIOG11"}, {ModeOutput, "GPIOG11"}, {ModeAlt2, "PCM1CLK"}, {ModeAlt6, "PGEINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIOG6"}, {ModeOutput, "GPIOG6"}, {ModeAlt2, "UART1TX"}, {ModeAlt6, "PGEINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIOG7"}, {ModeOutput, "GPIOG7"}, {ModeAlt2, "UART1RX"}, {ModeAlt6, "PGEINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIOA0"}, {ModeOutput, "GPIOA0"}, {ModeAlt2, "UART2TX"}, {ModeAlt3, "JTAGMS"}, {ModeAlt6, "PAEINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIOA6"}, {ModeOutput, "GPIOA6"}, {ModeAlt2, "SIMPWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PAEINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIOA2"}, {ModeOutput, "GPIOA2"}, {ModeAlt2, "UART2RTS"}, {ModeAlt3, "JTAGDO"}, {ModeAlt6, "PAEINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIOA3"}, {ModeOutput, "GPIOA3"}, {ModeAlt2, "UART2CTS"}, {ModeAlt3, "JTAGDI"}, {ModeAlt6, "PAEINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIOG8"}, {ModeOutput, "GPIOG8"}, {ModeAlt2, "UART1RTS"}, {ModeAlt6, "PGEINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIOG9"}, {ModeOutput, "GPIOG9"}, {ModeAlt2, "UART1CTS"}, {ModeAlt6, "PGEINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIOC0"}, {ModeOutput, "GPIOC0"}, {ModeAlt2, "NANDWE"}, {ModeAlt3, "SPI0MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIOC1"}, {ModeOutput, "GPIOC1"}, {ModeAlt2, "NANDALE"}, {ModeAlt3, "SPI0MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIOA1"}, {ModeOutput, "GPIOA1"}, {ModeAlt2, "UART2RX"}, {ModeAlt3, "JTAGCK"}, {ModeAlt6, "PAEINT1"}}},
            {TypeGpio, {14, 24, 66, 12, 1}, {{ModeInput, "GPIOC2"}, {ModeOutput, "GPIOC2"}, {ModeAlt2, "NANDCLE"}, {ModeAlt3, "SPI0CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIOC3"}, {ModeOutput, "GPIOC3"}, {ModeAlt2, "NANDCE1"}, {ModeAlt3, "SPI0CS"}}},
            {TypePower, { -1, -1, -1, 13, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {11, 17, 17, 13, 2}, {{ModeInput, "GPIOA17"}, {ModeOutput, "GPIOA17"}, {ModeAlt2, "OWAOUT"}, {ModeAlt6, "PAEINT17"}}},
            {TypeGpio, {30, 19, 19, 14, 1}, {{ModeInput, "GPIOA19"}, {ModeOutput, "GPIOA19"}, {ModeAlt2, "PCM0CLK"}, {ModeAlt3, "I2C1SDA"}, {ModeAlt6, "PAEINT19"}}},
            {TypeGpio, {31, 18, 18, 14, 2}, {{ModeInput, "GPIOA18"}, {ModeOutput, "GPIOA18"}, {ModeAlt2, "PCM0SYNC"}, {ModeAlt3, "I2C1SCK"}, {ModeAlt6, "PAEINT18"}}},
            {TypeGpio, {21, 20, 20, 15, 1}, {{ModeInput, "GPIOA20"}, {ModeOutput, "GPIOA20"}, {ModeAlt2, "PCM0DOUT"}, {ModeAlt3, "SIMVPPEN"}, {ModeAlt6, "PAEINT20"}}},
            {TypePower, { -1, -1, -1, 15, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {22, 21, 21, 16, 1}, {{ModeInput, "GPIOA21"}, {ModeOutput, "GPIOA21"}, {ModeAlt2, "PCM0DIN"}, {ModeAlt3, "SIMVPPPP"}, {ModeAlt6, "PAEINT21"}}},
            {TypeNotConnected, { -1, -1, -1, 16, 2}, {{ModeInput, "NC"}}},
            {TypeNotConnected, { -1, -1, -1, 17, 1}, {{ModeInput, "NC"}}},
            {TypePower, { -1, -1, -1, 17, 2}, {{ModeInput, "GND"}}},
            {TypeNotConnected, { -1, -1, -1, 18, 1}, {{ModeInput, "NC"}}},
            {TypeNotConnected, { -1, -1, -1, 18, 2}, {{ModeInput, "NC"}}},
            {TypeGpio, {25, 9, 9, 19, 1}, {{ModeInput, "GPIOA9"}, {ModeOutput, "GPIOA9"}, {ModeAlt2, "SIMRST"}, {ModeAlt6, "PAEINT9"}}},
            {TypeNotConnected, { -1, -1, -1, 19, 2}, {{ModeInput, "NC"}}},
            {TypePower, { -1, -1, -1, 20, 1}, {{ModeInput, "GND"}}},
            {TypeNotConnected, { -1, -1, -1, 20, 2}, {{ModeInput, "NC"}}},
          }
        },
        {
          "dbg_uart", 2, 4, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "GND"}}},
            {TypePower, { -1, -1, -1, 2, 1}, {{ModeInput, "5V"}}},
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIOA4"}, {ModeOutput, "GPIOA4"}, {ModeAlt2, "UART0TX"}, {ModeAlt6, "PAEINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIOA5"}, {ModeOutput, "GPIOA5"}, {ModeAlt2, "UART0RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PAEINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIOA10"}, {ModeOutput, "STAT_LED"}, {ModeAlt2, "SIMDET"}, {ModeAlt6, "PAEINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIOL10"}, {ModeOutput, "PWR_LED"}, {ModeAlt2, "SPWM"}, {ModeAlt6, "PLEINT10"}}},
            {TypeGpio, {33, 97, 355, 3, 1}, {{ModeInput, "BUT_K1"}, {ModeOutput, "GPIOL3"}, {ModeAlt2, "SUARTRX"}, {ModeAlt6, "PLEINT3"}}},
            {TypeGpio, {20, 105, 363, 4, 1}, {{ModeInput, "IR_RX"}, {ModeOutput, "GPIOL11"}, {ModeAlt2, "SCIRRX"}, {ModeAlt6, "PLEINT11"}}},
          }
        },
      }
    }
  },
  // <--- NanoPi M1 Plus
  //----------------------------------------------------------------------------
};

/* ========================================================================== */
