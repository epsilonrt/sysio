/**
 * @file
 * @brief Accès matériel GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_DEVICE_H_
#define _SYSIO_GPIO_DEVICE_H_

#include <sysio/gpio.h>

#ifndef DOXYGEN
/*
 * @class GpioDevice
 * @author epsilonrt
 * @date 02/22/18
 * @brief Accès à la couche matérielle du GPIO
 *
 * Classe abstraite devant être implémentée sur la plateforme cible.
 */
class GpioDevice {

  public:
    GpioDevice();
    virtual ~GpioDevice();
    virtual bool isOpen() const;
    virtual bool isDebug() const;
    virtual void setDebug (bool enable);
    
    // Méthodes purement virtuelles
    virtual const GpioDescriptor * descriptor() const = 0;
    
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual GpioAccessLayer preferedAccessLayer() const = 0;

    virtual void setMode (const GpioPin * pin, GpioPinMode m) = 0;
    virtual void setPull (const GpioPin * pin, GpioPinPull p) = 0;
    virtual void write (const GpioPin * pin, bool v) = 0;
    virtual void toggle (const GpioPin * pin) = 0;
    
    virtual bool read (const GpioPin * pin) const = 0;
    virtual GpioPinMode mode (const GpioPin * pin) const = 0;
    virtual GpioPinPull pull (const GpioPin * pin) const = 0;
    
    virtual const std::map<GpioPinMode, std::string> & modes() const = 0;

  protected:
    virtual void setOpen (bool open);

  private:
    bool _isopen;
    bool _isdebug;
};
#endif /* DOXYGEN not defined */
/* ========================================================================== */
#endif /*_SYSIO_GPIO_DEVICE_H_ defined */
