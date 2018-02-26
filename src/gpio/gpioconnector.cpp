/**
 * @file
 * @brief Connecteurs GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include <sysio/gpiodevice.h>
#include <exception>

// -----------------------------------------------------------------------------
//
//                          GpioConnector Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
GpioConnector::GpioConnector (Gpio * parent, const GpioConnectorDescriptor * desc) :
  _parent (parent), _descriptor (desc) {
  int num;
  const std::vector<GpioPinDescriptor> & v = _descriptor->pin;

  for (int i = 0; i < v.size(); i++) {

    num = pinNumber (v[i].num.row, v[i].num.column);
    _pin[num] = std::make_shared<GpioPin> (this, & v[i]);
  }
}

// -----------------------------------------------------------------------------
GpioConnector::~GpioConnector() {

}

// -----------------------------------------------------------------------------
const std::string &
GpioConnector::name() const {

  return _descriptor->name;
}

// -----------------------------------------------------------------------------
int
GpioConnector::rows() const {

  return _descriptor->rows;
}

// -----------------------------------------------------------------------------
int
GpioConnector::columns() const {

  return _descriptor->columns;
}

// -----------------------------------------------------------------------------
int
GpioConnector::size() const {

  return _pin.size();
}

// -----------------------------------------------------------------------------
int
GpioConnector::pinNumber (int row, int column) const {

  return _descriptor->pinNumber (row, column, columns());
}

// -----------------------------------------------------------------------------
const std::map<int, std::shared_ptr<GpioPin>> &
GpioConnector::pin()  {

  return _pin;
}

// -----------------------------------------------------------------------------
std::shared_ptr<GpioPin>
GpioConnector::pin (int number) const {

  return _pin.at (number); // throw out_of_range if not exist
}

// -----------------------------------------------------------------------------
GpioPinMode
GpioConnector::mode (int number) const {

  return pin (number)->mode();
}

// -----------------------------------------------------------------------------
void
GpioConnector::setMode (int number, GpioPinMode mode) {

  pin (number)->setMode (mode);
}

// -----------------------------------------------------------------------------
GpioPinPull
GpioConnector::pull (int number) const {

  return pin (number)->pull();
}

// -----------------------------------------------------------------------------
void
GpioConnector::setPull (int number, GpioPinPull pull) {

  pin (number)->setPull (pull);
}

// -----------------------------------------------------------------------------
void
GpioConnector::write (int number, bool value) {

  pin (number)->write (value);
}

// -----------------------------------------------------------------------------
void
GpioConnector::toggle (int number) {

  pin (number)->toggle();
}

// -----------------------------------------------------------------------------
bool
GpioConnector::read (int number) {

  return pin (number)->read();
}

// -----------------------------------------------------------------------------
void
GpioConnector::release (int number) {

  pin (number)->release();
}

// -----------------------------------------------------------------------------
Gpio *
GpioConnector::gpio() const {

  return _parent;
}

// -----------------------------------------------------------------------------
GpioDevice *
GpioConnector::device() const {

  return gpio()->device();
}

// -----------------------------------------------------------------------------
bool 
GpioConnector::isDebug() const {
  
  return device()->isDebug();
}

// -----------------------------------------------------------------------------
void 
GpioConnector::setDebug (bool enable) {
  
  device()->setDebug (enable);
}

/* ========================================================================== */
