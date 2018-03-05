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

#ifndef __DOXYGEN__

namespace Sysio {

  /*
   * @class Device
   * @author epsilonrt
   * @date 02/22/18
   * @brief Accès à la couche matérielle du GPIO
   *
   * Classe abstraite devant être implémentée sur la plateforme cible.
   */
  class Device {

    public:
      Device();
      virtual ~Device();
      virtual bool isOpen() const;
      virtual bool isDebug() const;
      virtual void setDebug (bool enable);

      // Méthodes purement virtuelles
      virtual const Gpio::Descriptor * descriptor() const = 0;

      virtual bool open() = 0;
      virtual void close() = 0;
      virtual AccessLayer preferedAccessLayer() const = 0;

      virtual void setMode (const Pin * pin, Pin::Mode m) = 0;
      virtual void setPull (const Pin * pin, Pin::Pull p) = 0;
      virtual void write (const Pin * pin, bool v) = 0;
      virtual void toggle (const Pin * pin) = 0;

      virtual bool read (const Pin * pin) const = 0;
      virtual Pin::Mode mode (const Pin * pin) const = 0;
      virtual Pin::Pull pull (const Pin * pin) const = 0;

      virtual const std::map<Pin::Mode, std::string> & modes() const = 0;

    protected:
      virtual void setOpen (bool open);

    private:
      bool _isopen;
      bool _isdebug;
  };
}
#endif /* DOXYGEN not defined */
/* ========================================================================== */
#endif /*_SYSIO_GPIO_DEVICE_H_ defined */
