/**
 * @file
 * @brief Accès matériel GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpiodevice.h>

// -----------------------------------------------------------------------------
//
//                        GpioDevice Class
//
// -----------------------------------------------------------------------------
GpioDevice::GpioDevice() : _isopen (false), _isdebug (false) {

}

// -----------------------------------------------------------------------------
GpioDevice::~GpioDevice() {

}

// -----------------------------------------------------------------------------
bool
GpioDevice::isOpen() const {

  return _isopen;
}

// -----------------------------------------------------------------------------
void
GpioDevice::setOpen (bool open) {
  _isopen = open;
}

// -----------------------------------------------------------------------------
bool
GpioDevice::isDebug() const {
  return _isdebug;
}

// -----------------------------------------------------------------------------
void
GpioDevice::setDebug (bool enable) {
  _isdebug = enable;
}

/* ========================================================================== */
