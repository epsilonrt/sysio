/**
 * @file
 * @brief Accès matériel GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpiodevice.h>

namespace Gpio {

// -----------------------------------------------------------------------------
//
//                        Device Class
//
// -----------------------------------------------------------------------------
  Device::Device() : _isopen (false), _isdebug (false) {

  }

// -----------------------------------------------------------------------------
  Device::~Device() {

  }

// -----------------------------------------------------------------------------
  bool
  Device::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  void
  Device::setOpen (bool open) {
    _isopen = open;
  }

// -----------------------------------------------------------------------------
  bool
  Device::isDebug() const {
    return _isdebug;
  }

// -----------------------------------------------------------------------------
  void
  Device::setDebug (bool enable) {
    _isdebug = enable;
  }
}
/* ========================================================================== */
