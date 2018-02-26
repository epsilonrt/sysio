/**
 * @file
 * @brief Broche et connecteurs GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpiopin.h>
#include <sysio/gpiodevice.h>
#include <exception>

// -----------------------------------------------------------------------------
//
//                              GpioPin Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
std::map<GpioPinType, std::string> GpioPin::_types = {
  {TypeGpio, "Gpio"},
  {TypePower, "Power"},
  {TypeUsb, "Usb"},
  {TypeSound, "Sound"},
  {TypeVideo, "Video"},
  {TypeNotConnected, "Not Connected"}
};

// -----------------------------------------------------------------------------
std::map<GpioPinNumbering, std::string> GpioPin::_numberings = {
  {NumberingLogical, "Logical"},
  {NumberingMcu, "Mcu"},
  {NumberingSystem, "System"}
};

// -----------------------------------------------------------------------------
std::map<GpioPinPull, std::string> GpioPin::_pulls = {
  {PullOff, "off"},
  {PullDown, "down"},
  {PullUp, "up"}
};

// -----------------------------------------------------------------------------
GpioPin::GpioPin (GpioConnector * parent, const GpioPinDescriptor * desc) :
  _parent (parent), _descriptor (desc), _holdMode (ModeUnknown),
  _holdPull (PullUnknown), _holdState (false) {

}

// -----------------------------------------------------------------------------
GpioPin::~GpioPin() {
}

// -----------------------------------------------------------------------------
GpioPinType
GpioPin::type() const {

  return _descriptor->type;
}

// -----------------------------------------------------------------------------
int
GpioPin::logicalNumber () const {

  return _descriptor->num.logical;
}

// -----------------------------------------------------------------------------
int
GpioPin::mcuNumber () const {

  return _descriptor->num.mcu;
}

// -----------------------------------------------------------------------------
int
GpioPin::systemNumber () const {

  return _descriptor->num.system;
}

// -----------------------------------------------------------------------------
int
GpioPin::number (GpioPinNumbering n) const {

  switch (n) {
    case NumberingLogical:
      return logicalNumber();
    case NumberingMcu:
      return mcuNumber();
    case NumberingSystem:
      return systemNumber();
    default:
      break;
  }

  throw std::invalid_argument ("Bad pin numbering requested");
}

// -----------------------------------------------------------------------------
int
GpioPin::row() const {

  return _descriptor->num.row;
}

// -----------------------------------------------------------------------------
int
GpioPin::column() const {

  return _descriptor->num.column;
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::name (GpioPinMode m) const {

  return _descriptor->name.at (m);
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::name() const {

  return name (mode());
}

// -----------------------------------------------------------------------------
GpioConnector *
GpioPin::connector() const {

  return _parent;
}

// -----------------------------------------------------------------------------
Gpio *
GpioPin::gpio() const {

  return connector()->gpio();
}

// -----------------------------------------------------------------------------
GpioDevice *
GpioPin::device() const {

  return connector()->device();
}

// -----------------------------------------------------------------------------
GpioPinMode
GpioPin::mode () const {

  if (type() != TypeGpio) {

    throw std::domain_error ("mode() only for GPIO pins");
  }
  return device()->mode (this);
}

// -----------------------------------------------------------------------------
void
GpioPin::setMode (GpioPinMode m) {

  if (type() != TypeGpio) {

    throw std::domain_error ("setMode() only for GPIO pins");
  }

  if (_holdMode == ModeUnknown) {

    _holdMode = mode();
    if (_holdMode == ModeOutput) {

      _holdState = read();
    }
  }
  device()->setMode (this, m);
}

// -----------------------------------------------------------------------------
GpioPinPull
GpioPin::pull () const {

  if (type() != TypeGpio) {

    throw std::domain_error ("pull() only for GPIO pins");
  }

  return device()->pull (this);
}

// -----------------------------------------------------------------------------
void
GpioPin::setPull (GpioPinPull p) {

  if (type() != TypeGpio) {

    throw std::domain_error ("setPull() only for GPIO pins");
  }

  if (_holdPull == PullUnknown) {

    _holdPull = pull();
  }

  device()->setPull (this, p);
}

// -----------------------------------------------------------------------------
void
GpioPin::write (bool value) {

  if (type() != TypeGpio) {

    throw std::domain_error ("write() only for GPIO pins");
  }
  device()->write (this, value);
}

// -----------------------------------------------------------------------------
void
GpioPin::toggle () {

  if (type() != TypeGpio) {

    throw std::domain_error ("toggle() only for GPIO pins");
  }
  device()->toggle (this);
}

// -----------------------------------------------------------------------------
bool
GpioPin::read () {

  if (type() != TypeGpio) {

    throw std::domain_error ("read() only for GPIO pins");
  }
  return device()->read (this);
}

// -----------------------------------------------------------------------------
void
GpioPin::release () {

  if (type() != TypeGpio) {

    throw std::domain_error ("release() only for GPIO pins");
  }

  if (_holdMode != ModeUnknown) {

    setMode (_holdMode);
    if (_holdMode == ModeOutput) {

      write (_holdState);
    }
    _holdMode = ModeUnknown;
  }

  if (_holdPull != PullUnknown) {

    setPull (_holdPull);
    _holdPull = PullUnknown;
  }
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::modeName() const {

  return modeName (mode ());
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::typeName() const {

  return typeName (type());
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::numberingName (GpioPinNumbering n)  {

  return _numberings.at (n);
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::modeName (GpioPinMode m) const {

  return device()->modes().at (m);
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::typeName (GpioPinType t) {

  return _types.at (t);
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::pullName () const {
  
  return pullName(pull());
}

// -----------------------------------------------------------------------------
const std::string &
GpioPin::pullName (GpioPinPull p) {

  return _pulls.at (p);
}

// -----------------------------------------------------------------------------
const std::map<GpioPinType, std::string> &
GpioPin::types () {

  return _types;
}

// -----------------------------------------------------------------------------
const std::map<GpioPinMode, std::string> &
GpioPin::modes () const {

  return device()->modes();
}

// -----------------------------------------------------------------------------
const std::map<GpioPinNumbering, std::string> &
GpioPin::numberings () {

  return _numberings;
}

// -----------------------------------------------------------------------------
const std::map<GpioPinPull, std::string> & 
GpioPin::pulls () {
  
  return _pulls;
}

// -----------------------------------------------------------------------------
bool
GpioPin::isDebug() const {

  return device()->isDebug();
}

// -----------------------------------------------------------------------------
void
GpioPin::setDebug (bool enable) {

  device()->setDebug (enable);
}

/* ========================================================================== */
