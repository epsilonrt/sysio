/**
 * arch/arm/rpi/gpiodevice_bcm2835.h
 *
 * Accès matériel GPIO pour BCM2835
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

// -----------------------------------------------------------------------------
//
//                        DeviceBcm2835 Class
//
// -----------------------------------------------------------------------------
  class DeviceBcm2835 : public Device {

    public:
      DeviceBcm2835();
      virtual ~DeviceBcm2835();

      const Gpio::Descriptor * descriptor() const;

      bool open();
      void close();
      AccessLayer preferedAccessLayer() const;
      unsigned int flags() const;

      void setMode (const Pin * pin, Pin::Mode m);
      void setPull (const Pin * pin, Pin::Pull p);
      void write (const Pin * pin, bool v);

      bool read (const Pin * pin) const;
      Pin::Mode mode (const Pin * pin) const;

      const std::map<Pin::Mode, std::string> & modes() const;

    private:
      unsigned long _piobase;
      xIoMap * _iomap;
      const Gpio::Descriptor * _gpioDescriptor;

      inline unsigned int readReg (unsigned int offset) const {
        return *pIo (_iomap, offset);
      }
      
      inline void writeReg (unsigned int offset, unsigned int value) {
        *pIo (_iomap, offset) = value;
      }

      static const std::map<eRpiMcu, unsigned long> _iobase;
      static const std::map<unsigned int, Pin::Mode> _int2mode;
      static const std::map<Pin::Mode, unsigned int> _mode2int;
      static const std::map<int, Gpio::Descriptor> _gpioDescriptors;
      static const std::map<Pin::Mode, std::string> _modes;

      static const unsigned int  GpioSize     = 54;
      static const unsigned long Bcm2708Base  = BCM2708_IO_BASE;
      static const unsigned long Bcm2709Base  = BCM2709_IO_BASE;
      static const unsigned long Bcm2710Base  = BCM2709_IO_BASE;
      static const unsigned int  MapBlockSize = BCM270X_BLOCK_SIZE;
      static const unsigned int  PioOffset    = 0x200000;

// Register offsets
      static const unsigned int GFPSEL0 = 0;
      static const unsigned int GFPSEL1 = 1;
      static const unsigned int GFPSEL2 = 2;
      static const unsigned int GFPSEL3 = 3;
      static const unsigned int GFPSEL4 = 4;
      static const unsigned int GFPSEL5 = 5;
// Reserved: 6
      static const unsigned int GPSET0  = 7;
      static const unsigned int GPSET1  = 8;
// Reserved: 9
      static const unsigned int GPCLR0  = 10;
      static const unsigned int GPCLR1  = 11;
// Reserved: 12
      static const unsigned int GPLEV0  = 13;
      static const unsigned int GPLEV1  = 14;
      static const unsigned int GPPUD   =  37;
      static const unsigned int GPPUDCLK0 =   38;
  };
}
/* ========================================================================== */
#endif /*_SYSIO_ARCH_ARM_RASPBERRYPI_GPIODEVICE_H_ defined */
