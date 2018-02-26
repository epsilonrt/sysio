/*
 * arch/arm/nanopi/gpiodevice_nanopi.cpp
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <iomanip>
#include <exception>
#include <sysio/gpiopin.h>
#include <sysio/delay.h>
#include "gpiodevice_nanopi.h"

/*
  H3/H5 PIO Registers:
    Module  Base Address  Port
    PIO1    0x01C20800    A,B,C,D,E,F,G (n=0 to n=6, n=1 no pin)
    PIO2    0x01F02C00    L (n=0)

    Register  Offset          Description
    Pn_CFG0   0x0000 + n*0x24 Port n Configure Register 0 (n from 0 to 6)
    Pn_CFG1   0x0004 + n*0x24 Port n Configure Register 1 (n from 0 to 6)
    Pn_CFG2   0x0008 + n*0x24 Port n Configure Register 2 (n from 0 to 6)
    Pn_CFG3   0x000C + n*0x24 Port n Configure Register 3 (n from 0 to 6)
    Pn_DAT    0x0010 + n*0x24 Port n Data Register (n from 0 to 6)
    Pn_DRV0   0x0014 + n*0x24 Port n Multi-Driving Register 0 (n from 0 to 6)
    Pn_DRV1   0x0018 + n*0x24 Port n Multi-Driving Register 1 (n from 0 to 6)
    Pn_PUL0   0x001C + n*0x24 Port n Pull Register 0 (n from 0 to 6)
    Pn_PUL1   0x0020 + n*0x24 Port n Pull Register 1 (n from 0 to 6)
 */
struct __attribute__ ( (__packed__)) PioBank  {
  uint32_t CFG[4];
  uint32_t DAT;
  uint32_t DRV[2];
  uint32_t PUL[2];
};
typedef struct PioBank PioBank;

// -----------------------------------------------------------------------------
//
//                        GpioDeviceNanoPi Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
GpioDeviceNanoPi::GpioDeviceNanoPi() : GpioDevice() {
  const xNanoPi * info = pxNanoPiInfo();

  if (!info) {

    throw std::system_error (ENOTSUP, std::generic_category(),
                             "It seems that this system is not a NanoPi board !");
  }
  _gpioDescriptor = &_gpioDescriptors.at (info->iGpioRev);
}

// -----------------------------------------------------------------------------
GpioDeviceNanoPi::~GpioDeviceNanoPi() {
}

// -----------------------------------------------------------------------------
const GpioDescriptor *
GpioDeviceNanoPi::descriptor() const {
  return _gpioDescriptor;
}

// -----------------------------------------------------------------------------
bool
GpioDeviceNanoPi::open() {

  if (!isOpen()) {

    iomap[0] = xIoMapOpen (Io1Base, MapBlockSize);

    if (iomap[0]) {

      iomap[1] = xIoMapOpen (Io2Base, MapBlockSize);
      if (iomap[1]) {


        setOpen (true);
        // debugPrintAllBanks ();
      }
      else {

        iIoMapClose (iomap[0]);
      }
    }
    if (!isOpen()) {

      throw std::system_error (EACCES, std::generic_category());
    }
  }
  return isOpen();
}

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::close() {

  if (isOpen()) {
    iIoMapClose (iomap[0]);
    iIoMapClose (iomap[1]);
    setOpen (false);
  }
}

// -----------------------------------------------------------------------------
GpioPinMode
GpioDeviceNanoPi::mode (const GpioPin * pin) const {
  GpioPinMode ret;
  PioBank * b;
  int i, r, m;
  int g = pin->mcuNumber();
  int f = g;

  b = pinBank (&f);
  r = f >> 3;
  i = (f - (r * 8)) * 4;
  m = b->CFG[r] >> i & 7;
  ret = _int2mode.at (m);

  // PA5: PWM0 FUNC3-> UART0 !,  PA6: PWM1 FUNC3, PL10: S_PWM FUNC2
  if ( ( (ret == ModeAlt3) && ( (g == 5) && (g == 6))) ||
       ( (ret == ModeAlt2) && (g == 104))) {
    ret = ModePwm;
  }
  return ret;
}

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::setMode (const GpioPin * pin, GpioPinMode m) {
  PioBank * b;
  int i, r, v;
  int g = pin->mcuNumber();
  int f = g;

  b = pinBank (&f);
  r = f >> 3;
  i = (f - (r * 8)) * 4;

  if (m == ModePwm) {
    // PA5: PWM0 FUNC3-> UART0 !,  PA6: PWM1 FUNC3, PL10: S_PWM FUNC2
    if ( (g == 5) && (g == 6)) {
      m = ModeAlt3;
    }
    else if (g == 104) {

      m = ModeAlt2;
    }
    else {
      throw std::invalid_argument ("ModePwm can only be set for PA5, PA6 or PL10");
    }
  }

  v = _mode2int.at (m);
  b->CFG[r] &= ~ (0b1111 << i);  // clear config
  b->CFG[r] |= (v << i);
  debugPrintBank (b);
}

// -----------------------------------------------------------------------------
GpioPinPull
GpioDeviceNanoPi::pull (const GpioPin * pin) const {
  PioBank * b;
  int i, r, v;
  int g = pin->mcuNumber();

  b = pinBank (&g);
  r = g >> 4;
  i = (g - (r * 16)) * 2;
  v = b->PUL[r] >> i & 3;
  /*
    00: Pull-up/down disable
    10: Pull-down
    01: Pull-up
    11: Reserved
  */
  switch (v) {
    case 0:
      return PullOff;
    case 2:
      return PullDown;
    case 1:
      return PullUp;
    default:
      break;
  }
  return PullUnknown;
}

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::setPull (const GpioPin * pin, GpioPinPull p) {
  PioBank * b;
  int v = -1;
  int g = pin->mcuNumber();
  int f = g;

  b = pinBank (&f);
  /*
    00: Pull-up/down disable
    10: Pull-down
    01: Pull-up
    11: Reserved
  */
  switch (p) {
    case PullOff:
      v = 0;
      break;
    case PullDown:
      v = 2;
      break;
    case PullUp:
      v = 1;
      break;
    default:
      break;
  }

  if (v >= 0) {
    int i, r;

    r = f >> 4;
    i = (f - (r * 16)) * 2;
    b->PUL[r] &= ~ (0b11 << i); // clear config -> disable
    delay_us (10);
    b->PUL[r] |= (v << i);
    delay_us (10);
    debugPrintBank (b);
  }
}

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::write (const GpioPin * pin, bool v) {
  PioBank * b;
  int g = pin->mcuNumber();

  b = pinBank (&g);
  if (v) {

    b->DAT |= 1 << g;;
  }
  else {

    b->DAT &= ~ (1 << g);
  }
  debugPrintBank (b);
}

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::toggle (const GpioPin * pin) {
  PioBank * b;
  int g = pin->mcuNumber();

  b = pinBank (&g);
  b->DAT ^= 1 << g;;
  debugPrintBank (b);
}

// -----------------------------------------------------------------------------
bool
GpioDeviceNanoPi::read (const GpioPin * pin) {
  PioBank * b;
  int g = pin->mcuNumber();

  b = pinBank (&g);
  return b->DAT & (1 << g) ? true : false;
}

// -----------------------------------------------------------------------------
const std::map<GpioPinMode, std::string> &
GpioDeviceNanoPi::modes() const {
  return _modes;
}


// -----------------------------------------------------------------------------
const std::map<GpioPinMode, std::string> GpioDeviceNanoPi::_modes = {
  {ModeInput, "in"},
  {ModeOutput, "out"},
  {ModeAlt2, "alt2"},
  {ModeAlt3, "alt3"},
  {ModeAlt4, "alt4"},
  {ModeAlt5, "alt5"},
  {ModeAlt6, "alt6"},
  {ModeDisabled, "off"},
  {ModePwm, "pwm"},
};

// -----------------------------------------------------------------------------
const std::map<int, GpioPinMode> GpioDeviceNanoPi::_int2mode = {
  {0, ModeInput},
  {1, ModeOutput},
  {2, ModeAlt2},
  {3, ModeAlt3},
  {4, ModeAlt4},
  {5, ModeAlt5},
  {6, ModeAlt6},
  {7, ModeDisabled}
};

// -----------------------------------------------------------------------------
const std::map<GpioPinMode, int> GpioDeviceNanoPi::_mode2int = {
  { ModeInput, 0},
  { ModeOutput, 1},
  { ModeAlt2, 2},
  { ModeAlt3, 3},
  { ModeAlt4, 4},
  { ModeAlt5, 5},
  { ModeAlt6, 6},
  { ModeDisabled, 7}
};

/* -----------------------------------------------------------------------------
  H3/H5 Ports:
  Port A(PA): 22 input/output port
  Port C(PC): 17 input/output port
  Port D(PD): 18 input/output port
  Port E(PE) : 16 input/output port
  Port F(PF) : 7 input/output port
  Port G(PG) : 14 input/output port
  Port L(PL) : 12 input/output port
*/
const int GpioDeviceNanoPi::_portSize[] = {
  22, 0, 17, 18, 16, 7, 14, 12, -1
};

// -----------------------------------------------------------------------------
PioBank *
GpioDeviceNanoPi::bank (unsigned int bkindex) const {
  PioBank * bk = nullptr;

  if (isOpen()) {

    if (bkindex < 7) {

      bk = (PioBank *) pIo (iomap[0], (Pio1Offset + bkindex * 0x24) >> 2);
    }
    else if (bkindex == 7) {

      bk = (PioBank *) pIo (iomap[1], Pio2Offset >> 2);
    }
  }
  else {

    throw std::system_error (ENODEV, std::generic_category(), "Device closed");
  }

  return bk;
}

// -----------------------------------------------------------------------------
PioBank *
GpioDeviceNanoPi::pinBank (int * mcupin) const {
  PioBank * bk = nullptr;
  const int * p = _portSize;
  unsigned int bkindex = 0;
  int ng = *mcupin;

  while ( (*p >= 0) && (ng >= *p)) {

    ng -= *p;
    do {
      bkindex++;
      p++;
    }
    while (bkindex == 1);   // saute PortB
  }

  if (bkindex < 8) {

    *mcupin = ng;
    bk = bank (bkindex);
    if (isDebug()) {

      char c = (bkindex < 7 ? 'A' + bkindex : 'L');
      std::cout << "---Port " << c << "---" << std::endl;
      debugPrintBank (bk);
    }
  }
  else {

    throw std::out_of_range ("Unable to find Allwinner H3/H5 PIO registers bank");
  }

  return bk;
}

// -----------------------------------------------------------------------------

void
GpioDeviceNanoPi::debugPrintAllBanks () const {

  if (isDebug()) {

    for (int i = 0; i < 8; i++) {

      if (i != 1) {

        PioBank * b = bank (i);
        char c = (i < 7 ? 'A' + i : 'L');
        std::cout << "---Port " << c << "---" << std::endl;
        debugPrintBank (b);
      }
    }
    std::cout << "---------------------------------" << std::endl << std::endl;
  }
}

#define HEX(width, val) std::setfill('0') << std::setw(width) << std::hex << val

// -----------------------------------------------------------------------------
void
GpioDeviceNanoPi::debugPrintBank (const PioBank * b) const {

  if (isDebug()) {

    std::cout << "CFG0=0x" << HEX (8, b->CFG[0]) << " - CFG1=0x" << HEX (8, b->CFG[1]) << std::endl;
    std::cout << "CFG2=0x" << HEX (8, b->CFG[2]) << " - CFG3=0x" << HEX (8, b->CFG[3]) << std::endl;
    std::cout << "DAT =0x" << HEX (8, b->DAT) << std::endl;
    std::cout << "DRV0=0x" << HEX (8, b->DRV[0]) << " - DRV1=0x" << HEX (8, b->DRV[1]) << std::endl;
    std::cout << "PUL0=0x" << HEX (8, b->PUL[0]) << " - PUL1=0x" << HEX (8, b->PUL[1]) << std::endl;
  }
}

/* ========================================================================== */
