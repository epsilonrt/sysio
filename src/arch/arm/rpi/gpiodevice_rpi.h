/**
 * arch/arm/rpi/gpiodevice_rpi.h
 *
 * Accès matériel GPIO pour rpi
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_
#define _SYSIO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_

#include <sysio/gpiodevice.h>
#include <sysio/iomap.h>
#include <sysio/rpi.h>
namespace Sysio {

  struct PioBank;
// -----------------------------------------------------------------------------
//
//                        DeviceRaspberryPi Class
//
// -----------------------------------------------------------------------------
  class DeviceRaspberryPi : public Device {

    public:
      DeviceRaspberryPi();
      virtual ~DeviceRaspberryPi();

      const Gpio::Descriptor * descriptor() const;

      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;

      void setMode (const Pin * pin, Pin::Mode m);
      void setPull (const Pin * pin, Pin::Pull p);
      void write (const Pin * pin, bool v);
      void toggle (const Pin * pin);

      bool read (const Pin * pin) const;
      Pin::Mode mode (const Pin * pin) const;
      Pin::Pull pull (const Pin * pin) const;

      const std::map<Pin::Mode, std::string> & modes() const;

    private:
      xIoMap * iomap[2];
      const Gpio::Descriptor * _gpioDescriptor;

      static const int _portSize[];
      static const std::map<int, Pin::Mode> _int2mode;
      static const std::map<Pin::Mode, int> _mode2int;
      static const std::map<int, Gpio::Descriptor> _gpioDescriptors;
      static const std::map<Pin::Mode, std::string> _modes;

      const unsigned long Io1Base = H3_IO1_BASE;
      const unsigned int Pio1Offset = 0x800;
      const unsigned long Io2Base = H3_IO2_BASE;
      const unsigned int Pio2Offset = 0xC00;
      const unsigned int GpioSize = 106;
      const unsigned int MapBlockSize = H3_BLOCK_SIZE;

      void debugPrintBank (const PioBank * b) const;
      void debugPrintAllBanks () const;
      struct PioBank * pinBank (int * mcupin) const;
      struct PioBank * bank (unsigned int bkindex) const;
  };
}
/* ========================================================================== */
#endif /*_SYSIO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_ defined */
