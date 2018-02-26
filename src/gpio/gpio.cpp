/**
 * @file
 * @brief GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include <sysio/gpiodevice.h>

// -----------------------------------------------------------------------------
//
//                           Gpio Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Gpio::Gpio (GpioDevice * dev) :
  _roc (true), _device (dev) {

  const std::vector<GpioConnectorDescriptor> & v = device()->descriptor()->connector;

  for (int i = 0; i < v.size(); i++) {

    _connector.push_back (std::make_shared<GpioConnector> (this, & v[i]));
  }
  setNumbering (NumberingLogical);
}

// -----------------------------------------------------------------------------
Gpio::~Gpio() {

  close();
  delete _device;
}

// -----------------------------------------------------------------------------
bool
Gpio::open() {

  return device()->open();
}

// -----------------------------------------------------------------------------
void
Gpio::close() {

  if (releaseOnClose()) {
    for (auto p = _pin.begin(); p != _pin.end(); ++p) {
      p->second->release();
    }
  }
  device()->close();
}

// -----------------------------------------------------------------------------
bool
Gpio::isOpen() const {

  return device()->isOpen();
}

// -----------------------------------------------------------------------------
GpioPinNumbering
Gpio::numbering() const {

  return _numbering;
}

// -----------------------------------------------------------------------------
void
Gpio::setNumbering (GpioPinNumbering nb) {

  if (nb != numbering()) {

    _pin.clear();

    for (int i = 0; i < connectors(); i++) {
      const std::map<int, std::shared_ptr<GpioPin>> & pinmap = connector (i)->pin();

      for (auto pair = pinmap.cbegin(); pair != pinmap.cend(); ++pair) {

        std::shared_ptr<GpioPin> p = pair->second;
        if (p->type() == TypeGpio) {

          int num = p->number (nb);
          _pin[num] = p;
        }
      }
    }
    _numbering = nb;
  }
}

// -----------------------------------------------------------------------------
const std::string &
Gpio::numberingName () const {

  return GpioPin::numberingName (numbering());
}

// -----------------------------------------------------------------------------
bool
Gpio::releaseOnClose() const {

  return _roc;
}

// -----------------------------------------------------------------------------
void
Gpio::setReleaseOnClose (bool enable) {

  _roc = enable;
}

// -----------------------------------------------------------------------------
int
Gpio::size() const {

  return _pin.size();
}

// -----------------------------------------------------------------------------
const std::map<int, std::shared_ptr<GpioPin>> &
Gpio::pin() {

  return _pin;
}

// -----------------------------------------------------------------------------
GpioPin *
Gpio::pin (int number) const {

  return _pin.at (number).get();
};

// -----------------------------------------------------------------------------
GpioPinMode
Gpio::mode (int number) const {

  return pin (number)->mode();
}

// -----------------------------------------------------------------------------
void
Gpio::setMode (int number, GpioPinMode mode) {

  pin (number)->setMode (mode);
}

// -----------------------------------------------------------------------------
GpioPinPull
Gpio::pull (int number) const {

  return pin (number)->pull();
}

// -----------------------------------------------------------------------------
void
Gpio::setPull (int number, GpioPinPull pull) {

  pin (number)->setPull (pull);
}

// -----------------------------------------------------------------------------
void
Gpio::write (int number, bool value) {

  pin (number)->write (value);
}

// -----------------------------------------------------------------------------
void
Gpio::toggle (int number) {

  pin (number)->toggle();
}

// -----------------------------------------------------------------------------
bool
Gpio::read (int number) {

  return pin (number)->read();
}

// -----------------------------------------------------------------------------
void
Gpio::release (int number) {

  pin (number)->release();
}

// -----------------------------------------------------------------------------
int
Gpio::connectors() const {

  return _connector.size();
}

// -----------------------------------------------------------------------------
GpioConnector *
Gpio::connector (int num) const {

  return _connector.at (num).get();
}

// -----------------------------------------------------------------------------
GpioDevice *
Gpio::device() const {

  return _device;
}

// -----------------------------------------------------------------------------
bool 
Gpio::isDebug() const {
  
  return device()->isDebug();
}

// -----------------------------------------------------------------------------
void 
Gpio::setDebug (bool enable) {
  
  device()->setDebug (enable);
}

/* ========================================================================== */
