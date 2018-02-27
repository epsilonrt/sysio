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
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIO_A12"}, {ModeOutput, "GPIO_A12"}, {ModeAlt2, "TWI0_SDA"}, {ModeAlt3, "DI_RX"}, {ModeAlt6, "PA_EINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIO_A11"}, {ModeOutput, "GPIO_A11"}, {ModeAlt2, "TWI0_SCK"}, {ModeAlt3, "DI_TX"}, {ModeAlt6, "PA_EINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIO_G11"}, {ModeOutput, "GPIO_G11"}, {ModeAlt2, "PCM1_CLK"}, {ModeAlt6, "PG_EINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIO_G6"}, {ModeOutput, "GPIO_G6"}, {ModeAlt2, "UART1_TX"}, {ModeAlt6, "PG_EINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIO_G7"}, {ModeOutput, "GPIO_G7"}, {ModeAlt2, "UART1_RX"}, {ModeAlt6, "PG_EINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIO_A0"}, {ModeOutput, "GPIO_A0"}, {ModeAlt2, "UART2_TX"}, {ModeAlt3, "JTAG_MS"}, {ModeAlt6, "PA_EINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIO_A6"}, {ModeOutput, "GPIO_A6"}, {ModeAlt2, "SIM_PWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PA_EINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIO_A2"}, {ModeOutput, "GPIO_A2"}, {ModeAlt2, "UART2_RTS"}, {ModeAlt3, "JTAG_DO"}, {ModeAlt6, "PA_EINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIO_A3"}, {ModeOutput, "GPIO_A3"}, {ModeAlt2, "UART2_CTS"}, {ModeAlt3, "JTAG_DI"}, {ModeAlt6, "PA_EINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIO_G8"}, {ModeOutput, "GPIO_G8"}, {ModeAlt2, "UART1_RTS"}, {ModeAlt6, "PG_EINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIO_G9"}, {ModeOutput, "GPIO_G9"}, {ModeAlt2, "UART1_CTS"}, {ModeAlt6, "PG_EINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIO_C0"}, {ModeOutput, "GPIO_C0"}, {ModeAlt2, "NAND_WE"}, {ModeAlt3, "SPI0_MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIO_C1"}, {ModeOutput, "GPIO_C1"}, {ModeAlt2, "NAND_ALE"}, {ModeAlt3, "SPI0_MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIO_A1"}, {ModeOutput, "GPIO_A1"}, {ModeAlt2, "UART2_RX"}, {ModeAlt3, "JTAG_CK"}, {ModeAlt6, "PA_EINT1"}}},
            {TypeGpio, {14, 24, 93, 12, 1}, {{ModeInput, "GPIO_C2"}, {ModeOutput, "GPIO_C2"}, {ModeAlt2, "NAND_CLE"}, {ModeAlt3, "SPI0_CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIO_C3"}, {ModeOutput, "GPIO_C3"}, {ModeAlt2, "NAND_CE1"}, {ModeAlt3, "SPI0_CS"}}},
          }
        },
        {
          "dbg_uart", 2, 4, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "GND"}}},
            {TypePower, { -1, -1, -1, 2, 1}, {{ModeInput, "5V"}}},
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIO_A4"}, {ModeOutput, "GPIO_A4"}, {ModeAlt2, "UART0_TX"}, {ModeAlt6, "PA_EINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIO_A5"}, {ModeOutput, "GPIO_A5"}, {ModeAlt2, "UART0_RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PA_EINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIO_A10"}, {ModeOutput, "GPIO_A10"}, {ModeAlt2, "SIM_DET"}, {ModeAlt6, "PA_EINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIO_L10"}, {ModeOutput, "GPIO_L10"}, {ModeAlt2, "S_PWM"}, {ModeAlt6, "S_PL_EINT10"}}},
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
            {TypeGpio, {20, 105, 363, 6, 1}, {{ModeInput, "GPIO_L11"}, {ModeOutput, "GPIO_L11"}, {ModeAlt2, "S_CIR_RX"}, {ModeAlt6, "S_PL_EINT11"}}},
            {TypeGpio, {11, 17, 17, 7, 1}, {{ModeInput, "GPIO_A17"}, {ModeOutput, "GPIO_A17"}, {ModeAlt2, "OWA_OUT"}, {ModeAlt6, "PA_EINT17"}}},
            {TypeGpio, {31, 18, 18, 8, 1}, {{ModeInput, "GPIO_A18"}, {ModeOutput, "GPIO_A18"}, {ModeAlt2, "PCM0_SYNC"}, {ModeAlt3, "TWI1_SCK"}, {ModeAlt6, "PA_EINT18"}}},
            {TypeGpio, {30, 19, 19, 9, 1}, {{ModeInput, "GPIO_A19"}, {ModeOutput, "GPIO_A19"}, {ModeAlt2, "PCM0_CLK"}, {ModeAlt3, "TWI1_SDA"}, {ModeAlt6, "PA_EINT19"}}},
            {TypeGpio, {21, 20, 20, 10, 1}, {{ModeInput, "GPIO_A20"}, {ModeOutput, "GPIO_A20"}, {ModeAlt2, "PCM0_DOUT"}, {ModeAlt3, "SIM_VPPEN"}, {ModeAlt6, "PA_EINT20"}}},
            {TypeGpio, {22, 21, 21, 11, 1}, {{ModeInput, "GPIO_A21"}, {ModeOutput, "GPIO_A21"}, {ModeAlt2, "PCM0_DIN"}, {ModeAlt3, "SIM_VPPPP"}, {ModeAlt6, "PA_EINT21"}}},
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
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIO_A12"}, {ModeOutput, "GPIO_A12"}, {ModeAlt2, "TWI0_SDA"}, {ModeAlt3, "DI_RX"}, {ModeAlt6, "PA_EINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIO_A11"}, {ModeOutput, "GPIO_A11"}, {ModeAlt2, "TWI0_SCK"}, {ModeAlt3, "DI_TX"}, {ModeAlt6, "PA_EINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIO_G11"}, {ModeOutput, "GPIO_G11"}, {ModeAlt2, "PCM1_CLK"}, {ModeAlt6, "PG_EINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIO_G6"}, {ModeOutput, "GPIO_G6"}, {ModeAlt2, "UART1_TX"}, {ModeAlt6, "PG_EINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIO_G7"}, {ModeOutput, "GPIO_G7"}, {ModeAlt2, "UART1_RX"}, {ModeAlt6, "PG_EINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIO_A0"}, {ModeOutput, "GPIO_A0"}, {ModeAlt2, "UART2_TX"}, {ModeAlt3, "JTAG_MS"}, {ModeAlt6, "PA_EINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIO_A6"}, {ModeOutput, "GPIO_A6"}, {ModeAlt2, "SIM_PWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PA_EINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIO_A2"}, {ModeOutput, "GPIO_A2"}, {ModeAlt2, "UART2_RTS"}, {ModeAlt3, "JTAG_DO"}, {ModeAlt6, "PA_EINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIO_A3"}, {ModeOutput, "GPIO_A3"}, {ModeAlt2, "UART2_CTS"}, {ModeAlt3, "JTAG_DI"}, {ModeAlt6, "PA_EINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIO_G8"}, {ModeOutput, "GPIO_G8"}, {ModeAlt2, "UART1_RTS"}, {ModeAlt6, "PG_EINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIO_G9"}, {ModeOutput, "GPIO_G9"}, {ModeAlt2, "UART1_CTS"}, {ModeAlt6, "PG_EINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIO_C0"}, {ModeOutput, "GPIO_C0"}, {ModeAlt2, "NAND_WE"}, {ModeAlt3, "SPI0_MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIO_C1"}, {ModeOutput, "GPIO_C1"}, {ModeAlt2, "NAND_ALE"}, {ModeAlt3, "SPI0_MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIO_A1"}, {ModeOutput, "GPIO_A1"}, {ModeAlt2, "UART2_RX"}, {ModeAlt3, "JTAG_CK"}, {ModeAlt6, "PA_EINT1"}}},
            {TypeGpio, {14, 24, 93, 12, 1}, {{ModeInput, "GPIO_C2"}, {ModeOutput, "GPIO_C2"}, {ModeAlt2, "NAND_CLE"}, {ModeAlt3, "SPI0_CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIO_C3"}, {ModeOutput, "GPIO_C3"}, {ModeAlt2, "NAND_CE1"}, {ModeAlt3, "SPI0_CS"}}},
            {TypePower, { -1, -1, -1, 13, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {11, 17, 17, 13, 2}, {{ModeInput, "GPIO_A17"}, {ModeOutput, "GPIO_A17"}, {ModeAlt2, "OWA_OUT"}, {ModeAlt6, "PA_EINT17"}}},
            {TypeGpio, {30, 19, 19, 14, 1}, {{ModeInput, "GPIO_A19"}, {ModeOutput, "GPIO_A19"}, {ModeAlt2, "PCM0_CLK"}, {ModeAlt3, "TWI1_SDA"}, {ModeAlt6, "PA_EINT19"}}},
            {TypeGpio, {31, 18, 18, 14, 2}, {{ModeInput, "GPIO_A18"}, {ModeOutput, "GPIO_A18"}, {ModeAlt2, "PCM0_SYNC"}, {ModeAlt3, "TWI1_SCK"}, {ModeAlt6, "PA_EINT18"}}},
            {TypeGpio, {21, 20, 20, 15, 1}, {{ModeInput, "GPIO_A20"}, {ModeOutput, "GPIO_A20"}, {ModeAlt2, "PCM0_DOUT"}, {ModeAlt3, "SIM_VPPEN"}, {ModeAlt6, "PA_EINT20"}}},
            {TypePower, { -1, -1, -1, 15, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {22, 21, 21, 16, 1}, {{ModeInput, "GPIO_A21"}, {ModeOutput, "GPIO_A21"}, {ModeAlt2, "PCM0_DIN"}, {ModeAlt3, "SIM_VPPPP"}, {ModeAlt6, "PA_EINT21"}}},
            {TypeGpio, {26, 7, 7, 16, 2}, {{ModeInput, "GPIO_A7"}, {ModeOutput, "GPIO_A7"}, {ModeAlt2, "SIM_CLK"}, {ModeAlt6, "PA_EINT7"}}},
            {TypeGpio, {23, 8, 8, 17, 1}, {{ModeInput, "GPIO_A8"}, {ModeOutput, "GPIO_A8"}, {ModeAlt2, "SIM_DATA"}, {ModeAlt6, "PA_EINT8"}}},
            {TypePower, { -1, -1, -1, 17, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {24, 16, 16, 18, 1}, {{ModeInput, "GPIO_A16"}, {ModeOutput, "GPIO_A16"}, {ModeAlt2, "SPI1_MISO"}, {ModeAlt3, "UART3_CTS"}, {ModeAlt6, "PA_EINT16"}}},
            {TypeGpio, {27, 13, 13, 18, 2}, {{ModeInput, "GPIO_A13"}, {ModeOutput, "GPIO_A13"}, {ModeAlt2, "SPI1_CS"}, {ModeAlt3, "UART3_TX"}, {ModeAlt6, "PA_EINT13"}}},
            {TypeGpio, {25, 9, 9, 19, 1}, {{ModeInput, "GPIO_A9"}, {ModeOutput, "GPIO_A9"}, {ModeAlt2, "SIM_RST"}, {ModeAlt6, "PA_EINT9"}}},
            {TypeGpio, {28, 15, 15, 19, 2}, {{ModeInput, "GPIO_A15"}, {ModeOutput, "GPIO_A15"}, {ModeAlt2, "SPI1_MOSI"}, {ModeAlt3, "UART3_RTS"}, {ModeAlt6, "PA_EINT15"}}},
            {TypePower, { -1, -1, -1, 20, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {29, 14, 14, 20, 2}, {{ModeInput, "GPIO_A14"}, {ModeOutput, "GPIO_A14"}, {ModeAlt2, "SPI1_CLK"}, {ModeAlt3, "UART3_RX"}, {ModeAlt6, "PA_EINT14"}}},
          }
        },
        {
          "dbg_uart", 2, 4, 1, silConnector,
          {
            {TypePower, { -1, -1, -1, 1, 1}, {{ModeInput, "GND"}}},
            {TypePower, { -1, -1, -1, 2, 1}, {{ModeInput, "5V"}}},
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIO_A4"}, {ModeOutput, "GPIO_A4"}, {ModeAlt2, "UART0_TX"}, {ModeAlt6, "PA_EINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIO_A5"}, {ModeOutput, "GPIO_A5"}, {ModeAlt2, "UART0_RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PA_EINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIO_A10"}, {ModeOutput, "GPIO_A10"}, {ModeAlt2, "SIM_DET"}, {ModeAlt6, "PA_EINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIO_L10"}, {ModeOutput, "GPIO_L10"}, {ModeAlt2, "S_PWM"}, {ModeAlt6, "S_PL_EINT10"}}},
            {TypeGpio, {33, 97, 355, 3, 1}, {{ModeInput, "GPIO_L3"}, {ModeOutput, "GPIO_L3"}, {ModeAlt2, "S_UART_RX"}, {ModeAlt6, "S_PL_EINT3"}}},
            {TypeGpio, {20, 105, 363, 4, 1}, {{ModeInput, "GPIO_L11"}, {ModeOutput, "GPIO_L11"}, {ModeAlt2, "S_CIR_RX"}, {ModeAlt6, "S_PL_EINT11"}}},
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
            {TypeGpio, {8, 12, 12, 2, 1}, {{ModeInput, "GPIO_A12"}, {ModeOutput, "GPIO_A12"}, {ModeAlt2, "TWI0_SDA"}, {ModeAlt3, "DI_RX"}, {ModeAlt6, "PA_EINT12"}}},
            {TypePower, { -1, -1, -1, 2, 2}, {{ModeInput, "5V"}}},
            {TypeGpio, {9, 11, 11, 3, 1}, {{ModeInput, "GPIO_A11"}, {ModeOutput, "GPIO_A11"}, {ModeAlt2, "TWI0_SCK"}, {ModeAlt3, "DI_TX"}, {ModeAlt6, "PA_EINT11"}}},
            {TypePower, { -1, -1, -1, 3, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {7, 91, 203, 4, 1}, {{ModeInput, "GPIO_G11"}, {ModeOutput, "GPIO_G11"}, {ModeAlt2, "PCM1_CLK"}, {ModeAlt6, "PG_EINT11"}}},
            {TypeGpio, {15, 86, 198, 4, 2}, {{ModeInput, "GPIO_G6"}, {ModeOutput, "GPIO_G6"}, {ModeAlt2, "UART1_TX"}, {ModeAlt6, "PG_EINT6"}}},
            {TypePower, { -1, -1, -1, 5, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {16, 87, 199, 5, 2}, {{ModeInput, "GPIO_G7"}, {ModeOutput, "GPIO_G7"}, {ModeAlt2, "UART1_RX"}, {ModeAlt6, "PG_EINT7"}}},
            {TypeGpio, {0, 0, 0, 6, 1}, {{ModeInput, "GPIO_A0"}, {ModeOutput, "GPIO_A0"}, {ModeAlt2, "UART2_TX"}, {ModeAlt3, "JTAG_MS"}, {ModeAlt6, "PA_EINT0"}}},
            {TypeGpio, {1, 6, 6, 6, 2}, {{ModeInput, "GPIO_A6"}, {ModeOutput, "GPIO_A6"}, {ModeAlt2, "SIM_PWREN"}, {ModeAlt3, "PWM1"}, {ModeAlt6, "PA_EINT6"}}},
            {TypeGpio, {2, 2, 2, 7, 1}, {{ModeInput, "GPIO_A2"}, {ModeOutput, "GPIO_A2"}, {ModeAlt2, "UART2_RTS"}, {ModeAlt3, "JTAG_DO"}, {ModeAlt6, "PA_EINT2"}}},
            {TypePower, { -1, -1, -1, 7, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {3, 3, 3, 8, 1}, {{ModeInput, "GPIO_A3"}, {ModeOutput, "GPIO_A3"}, {ModeAlt2, "UART2_CTS"}, {ModeAlt3, "JTAG_DI"}, {ModeAlt6, "PA_EINT3"}}},
            {TypeGpio, {4, 88, 200, 8, 2}, {{ModeInput, "GPIO_G8"}, {ModeOutput, "GPIO_G8"}, {ModeAlt2, "UART1_RTS"}, {ModeAlt6, "PG_EINT8"}}},
            {TypePower, { -1, -1, -1, 9, 1}, {{ModeInput, "3.3V"}}},
            {TypeGpio, {5, 89, 201, 9, 2}, {{ModeInput, "GPIO_G9"}, {ModeOutput, "GPIO_G9"}, {ModeAlt2, "UART1_CTS"}, {ModeAlt6, "PG_EINT9"}}},
            {TypeGpio, {12, 22, 64, 10, 1}, {{ModeInput, "GPIO_C0"}, {ModeOutput, "GPIO_C0"}, {ModeAlt2, "NAND_WE"}, {ModeAlt3, "SPI0_MOSI"}}},
            {TypePower, { -1, -1, -1, 10, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {13, 23, 65, 11, 1}, {{ModeInput, "GPIO_C1"}, {ModeOutput, "GPIO_C1"}, {ModeAlt2, "NAND_ALE"}, {ModeAlt3, "SPI0_MISO"}}},
            {TypeGpio, {6, 1, 1, 11, 2}, {{ModeInput, "GPIO_A1"}, {ModeOutput, "GPIO_A1"}, {ModeAlt2, "UART2_RX"}, {ModeAlt3, "JTAG_CK"}, {ModeAlt6, "PA_EINT1"}}},
            {TypeGpio, {14, 24, 93, 12, 1}, {{ModeInput, "GPIO_C2"}, {ModeOutput, "GPIO_C2"}, {ModeAlt2, "NAND_CLE"}, {ModeAlt3, "SPI0_CLK"}}},
            {TypeGpio, {10, 25, 67, 12, 2}, {{ModeInput, "GPIO_C3"}, {ModeOutput, "GPIO_C3"}, {ModeAlt2, "NAND_CE1"}, {ModeAlt3, "SPI0_CS"}}},
            {TypePower, { -1, -1, -1, 13, 1}, {{ModeInput, "GND"}}},
            {TypeGpio, {11, 17, 17, 13, 2}, {{ModeInput, "GPIO_A17"}, {ModeOutput, "GPIO_A17"}, {ModeAlt2, "OWA_OUT"}, {ModeAlt6, "PA_EINT17"}}},
            {TypeGpio, {30, 19, 19, 14, 1}, {{ModeInput, "GPIO_A19"}, {ModeOutput, "GPIO_A19"}, {ModeAlt2, "PCM0_CLK"}, {ModeAlt3, "TWI1_SDA"}, {ModeAlt6, "PA_EINT19"}}},
            {TypeGpio, {31, 18, 18, 14, 2}, {{ModeInput, "GPIO_A18"}, {ModeOutput, "GPIO_A18"}, {ModeAlt2, "PCM0_SYNC"}, {ModeAlt3, "TWI1_SCK"}, {ModeAlt6, "PA_EINT18"}}},
            {TypeGpio, {21, 20, 20, 15, 1}, {{ModeInput, "GPIO_A20"}, {ModeOutput, "GPIO_A20"}, {ModeAlt2, "PCM0_DOUT"}, {ModeAlt3, "SIM_VPPEN"}, {ModeAlt6, "PA_EINT20"}}},
            {TypePower, { -1, -1, -1, 15, 2}, {{ModeInput, "GND"}}},
            {TypeGpio, {22, 21, 21, 16, 1}, {{ModeInput, "GPIO_A21"}, {ModeOutput, "GPIO_A21"}, {ModeAlt2, "PCM0_DIN"}, {ModeAlt3, "SIM_VPPPP"}, {ModeAlt6, "PA_EINT21"}}},
            {TypeNotConnected, { -1, -1, -1, 16, 2}, {{ModeInput, "NC"}}},
            {TypeNotConnected, { -1, -1, -1, 17, 1}, {{ModeInput, "NC"}}},
            {TypePower, { -1, -1, -1, 17, 2}, {{ModeInput, "GND"}}},
            {TypeNotConnected, { -1, -1, -1, 18, 1}, {{ModeInput, "NC"}}},
            {TypeNotConnected, { -1, -1, -1, 18, 2}, {{ModeInput, "NC"}}},
            {TypeGpio, {25, 9, 9, 19, 1}, {{ModeInput, "GPIO_A9"}, {ModeOutput, "GPIO_A9"}, {ModeAlt2, "SIM_RST"}, {ModeAlt6, "PA_EINT9"}}},
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
            {TypeGpio, {17, 4, 4, 3, 1}, {{ModeInput, "GPIO_A4"}, {ModeOutput, "GPIO_A4"}, {ModeAlt2, "UART0_TX"}, {ModeAlt6, "PA_EINT4"}}},
            {TypeGpio, {18, 5, 5, 4, 1}, {{ModeInput, "GPIO_A5"}, {ModeOutput, "GPIO_A5"}, {ModeAlt2, "UART0_RX"}, {ModeAlt3, "PWM0"}, {ModeAlt6, "PA_EINT5"}}},
          }
        },
        {
          "inner", 3, 2, 1, silConnector,
          {
            {TypeGpio, {19, 10, 10, 1, 1}, {{ModeInput, "GPIO_A10"}, {ModeOutput, "GPIO_A10"}, {ModeAlt2, "SIM_DET"}, {ModeAlt6, "PA_EINT10"}}},
            {TypeGpio, {32, 104, 362, 2, 1}, {{ModeInput, "GPIO_L10"}, {ModeOutput, "GPIO_L10"}, {ModeAlt2, "S_PWM"}, {ModeAlt6, "S_PL_EINT10"}}},
            {TypeGpio, {33, 97, 355, 3, 1}, {{ModeInput, "GPIO_L3"}, {ModeOutput, "GPIO_L3"}, {ModeAlt2, "S_UART_RX"}, {ModeAlt6, "S_PL_EINT3"}}},
            {TypeGpio, {20, 105, 363, 4, 1}, {{ModeInput, "GPIO_L11"}, {ModeOutput, "GPIO_L11"}, {ModeAlt2, "S_CIR_RX"}, {ModeAlt6, "S_PL_EINT11"}}},
          }
        },
      }
    }
  },
  // <--- NanoPi M1 Plus
  //----------------------------------------------------------------------------
};

/* ========================================================================== */
