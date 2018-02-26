/**
 * arch/arm/nanopi/gpiodevice_nanopi.h
 *
 * Accès matériel GPIO pour nanopi
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_ARCH_ARM_NANOPI_GPIODEVICE_H_
#define _SYSIO_ARCH_ARM_NANOPI_GPIODEVICE_H_

#include <sysio/gpiodevice.h>
#include <sysio/iomap.h>
#include <sysio/nanopi.h>

struct PioBank;
// -----------------------------------------------------------------------------
//
//                          GpioDeviceNanoPi Class
//
// -----------------------------------------------------------------------------
class GpioDeviceNanoPi : public GpioDevice {

  public:
    GpioDeviceNanoPi();
    virtual ~GpioDeviceNanoPi();

    const GpioDescriptor * descriptor() const;

    bool open();
    void close();
    
    void setMode (const GpioPin * pin, GpioPinMode m);
    void setPull (const GpioPin * pin, GpioPinPull p);
    void write (const GpioPin * pin, bool v);
    void toggle (const GpioPin * pin);
    bool read (const GpioPin * pin);

    GpioPinMode mode (const GpioPin * pin) const;
    GpioPinPull pull (const GpioPin * pin) const;
    
    const std::map<GpioPinMode, std::string> & modes() const;

  private:
    xIoMap * iomap[2];
    const GpioDescriptor * _gpioDescriptor;

    static const int _portSize[];
    static const std::map<int, GpioPinMode> _int2mode;
    static const std::map<GpioPinMode, int> _mode2int;
    static const std::map<int, GpioDescriptor> _gpioDescriptors;
    static const std::map<GpioPinMode, std::string> _modes;

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

/* ========================================================================== */
#endif /*_SYSIO_ARCH_ARM_NANOPI_GPIODEVICE_H_ defined */
