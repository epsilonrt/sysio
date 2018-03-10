/*
 * arch/arm/rpi/gpiodevice_rpi.cpp
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <iomanip>
#include <exception>
#include <sysio/gpio.h>
#include <sysio/clock.h>
#include "gpiodevice_bcm2835.h"

namespace Sysio {

  /* constants ================================================================ */
// -----------------------------------------------------------------------------
//
//                        DeviceBcm2835 Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  const std::map<eRpiMcu, unsigned long> DeviceBcm2835::_iobase {
    {eRpiMcuBcm2708, Bcm2708Base},
    {eRpiMcuBcm2709, Bcm2709Base},
    {eRpiMcuBcm2710, Bcm2710Base},
  };

// -----------------------------------------------------------------------------
  DeviceBcm2835::DeviceBcm2835() : Device () {
    const xRpi * info = pxRpiInfo();

    if (!info) {

      throw std::system_error (ENOTSUP, std::system_category(),
                               "It seems that this system is not a raspberry pi !");
    }
    _piobase = _iobase.at (info->eMcu) + PioOffset;
    _gpioDescriptor = &_gpioDescriptors.at (info->iGpioRev);
  }

// -----------------------------------------------------------------------------
  DeviceBcm2835::~DeviceBcm2835() {
  }

// -----------------------------------------------------------------------------
  AccessLayer
  DeviceBcm2835::preferedAccessLayer() const {
    return AccessLayerAll;
  }

// -----------------------------------------------------------------------------
  bool
  DeviceBcm2835::open() {

    if (!isOpen()) {

      _iomap = xIoMapOpen (_piobase, MapBlockSize);
      if (_iomap) {

        setOpen (true);
        // debugPrintAllBanks ();
      }
      else {

        throw std::system_error (errno, std::system_category());
      }
    }
    return isOpen();
  }

// -----------------------------------------------------------------------------
  void
  DeviceBcm2835::close() {

    if (isOpen()) {
      iIoMapClose (_iomap);
      setOpen (false);
    }
  }

// -----------------------------------------------------------------------------
  Pin::Mode
  DeviceBcm2835::mode (const Pin * pin) const {
    Pin::Mode m;
    int g;
    unsigned int r;

    g = pin->mcuNumber();
    r = readReg (GFPSEL0 + g / 10) >> ( (g % 10) * 3) & 7;
    m = _int2mode.at (r);
    /*
     *  - BCM12 ALT0 -> PWM0
     *  - BCM13 ALT0 -> PWM1
     *  - BCM18 ALT5 -> PWM0
     *  - BCM19 ALT5 -> PWM1
     */

    if ( ( (m == Pin::ModeAlt0) && ( (g == 12) || (g == 13))) ||
         ( (m == Pin::ModeAlt5) && ( (g == 18) || (g == 19)))) {
      m = Pin::ModePwm;
    }
    return m;
  }

// -----------------------------------------------------------------------------
  void
  DeviceBcm2835::setMode (const Pin * pin, Pin::Mode m) {
    int g;
    unsigned int offset, rval, lsr, mval;

    g = pin->mcuNumber();

    if (m == Pin::ModePwm) {

      if ( (g == 12) || (g == 13)) {

        m = Pin::ModeAlt0;
      }
      else if ( (g == 18) || (g == 19)) {

        m = Pin::ModeAlt5;
      }
      else {

        throw std::invalid_argument ("ModePwm can only be set for PA5, PA6 or PL10");
      }
    }

    offset = GFPSEL0 + g / 10;
    lsr = (g % 10) * 3;

    rval = readReg (offset);
    rval &= ~ (7 << lsr); // clear
    mval = _mode2int.at (m);
    rval |= mval << lsr;
    writeReg (offset, rval);
  }

// -----------------------------------------------------------------------------
  void
  DeviceBcm2835::setPull (const Pin * pin, Pin::Pull p) {
    unsigned int pval, pclk = GPPUDCLK0;
    int g = pin->mcuNumber();

    if (g > 31) {

      pclk++;
      g -= 32;
    }

    /*
      PUD - GPIO Pin Pull-up/down
      00 = Off – disable pull-up/down
      01 = Enable Pull Down control
      10 = Enable Pull Up control
      11 = Reserved
    */
    switch (p) {
      case Pin::PullOff:
        pval = 0;
        break;
      case Pin::PullDown:
        pval = 1;
        break;
      case Pin::PullUp:
        pval = 2;
        break;
      default:
        return;
    }
    /*
      required:
      1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
      to remove the current Pull-up/down)
      2. Wait 150 cycles – this provides the required set-up time for the control signal
      3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
      modify – NOTE only the pads which receive a clock will be modified, all others will
      retain their previous state.
      4. Wait 150 cycles – this provides the required hold time for the control signal
      5. Write to GPPUD to remove the control signal
      6. Write to GPPUDCLK0/1 to remove the clock
     */
    writeReg (GPPUD, pval);
    Clock::delayMicroseconds (10);
    writeReg (pclk, 1 << g);
    Clock::delayMicroseconds (10);
    writeReg (GPPUD, 0);
    writeReg (pclk, 0);
  }

// -----------------------------------------------------------------------------
  void
  DeviceBcm2835::write (const Pin * pin, bool v) {
    unsigned int offset = v ? GPSET0 : GPCLR0;
    int g = pin->mcuNumber();

    if (g > 31) {

      offset++;
      g -= 32;
    }
    writeReg (offset, 1 << g);
  }

// -----------------------------------------------------------------------------
  bool
  DeviceBcm2835::read (const Pin * pin) const {
    unsigned int offset = GPLEV0;
    int g = pin->mcuNumber();

    if (g > 31) {

      offset++;
      g -= 32;
    }

    return (readReg (offset) & (1 << g)) != 0;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  DeviceBcm2835::modes() const {
    return _modes;
  }


// -----------------------------------------------------------------------------
  const Gpio::Descriptor *
  DeviceBcm2835::descriptor() const {
    return _gpioDescriptor;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> DeviceBcm2835::_modes = {
    {Pin::ModeInput, "in"},
    {Pin::ModeOutput, "out"},
    {Pin::ModeAlt0, "alt0"},
    {Pin::ModeAlt1, "alt1"},
    {Pin::ModeAlt2, "alt2"},
    {Pin::ModeAlt3, "alt3"},
    {Pin::ModeAlt4, "alt4"},
    {Pin::ModeAlt5, "alt5"},
    {Pin::ModePwm, "pwm"},
  };

// -----------------------------------------------------------------------------
  const std::map<unsigned int, Pin::Mode> DeviceBcm2835::_int2mode = {
    {0, Pin::ModeInput},
    {1, Pin::ModeOutput},
    {4, Pin::ModeAlt0},
    {5, Pin::ModeAlt1},
    {6, Pin::ModeAlt2},
    {7, Pin::ModeAlt3},
    {3, Pin::ModeAlt4},
    {2, Pin::ModeAlt5},
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, unsigned int> DeviceBcm2835::_mode2int = {
    {Pin::ModeInput, 0},
    {Pin::ModeOutput, 1},
    {Pin::ModeAlt0, 4},
    {Pin::ModeAlt1, 5},
    {Pin::ModeAlt2, 6},
    {Pin::ModeAlt3, 7},
    {Pin::ModeAlt4, 3},
    {Pin::ModeAlt5, 2},
  };
}
/* ========================================================================== */
