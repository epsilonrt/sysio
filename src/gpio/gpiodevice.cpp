/**
 * @file
 * @brief Accès matériel GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpiodevice.h>

namespace Sysio {

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

// -----------------------------------------------------------------------------
  void
  Device::toggle (const Pin * pin) {
  }

// -----------------------------------------------------------------------------
  Pin::Pull
  Device::pull (const Pin * pin) const {
    return Pin::PullUnknown;
  }

// -----------------------------------------------------------------------------
  unsigned int
  Device::flags() const {
    return 0;
  }
// -----------------------------------------------------------------------------
}
/* ========================================================================== */
