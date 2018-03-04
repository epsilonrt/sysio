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
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace Gpio {

// -----------------------------------------------------------------------------
//
//                          Connector Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Connector::Connector (Board * parent, const Descriptor * desc) :
    _isopen (false), _parent (parent), _descriptor (desc) {
    int num;
    const std::vector<Pin::Descriptor> & v = _descriptor->pin;

    for (int i = 0; i < v.size(); i++) {

      num = pinNumber (v[i].num.row, v[i].num.column);
      _pin[num] = std::make_shared<Pin> (this, & v[i]);
    }
  }

// -----------------------------------------------------------------------------
  Connector::~Connector() {

    close();
  }

// -----------------------------------------------------------------------------
  bool Connector::open() {

    if (!isOpen()) {

      for (auto p = _pin.begin(); p != _pin.end(); ++p) {

        if (!p->second->open()) {

          return false;
        }
      }
      _isopen = true;
    }
    return isOpen();
  }

// -----------------------------------------------------------------------------
  void Connector::close() {

    if (isOpen()) {

      for (auto p = _pin.begin(); p != _pin.end(); ++p) {

        p->second->close();
      }
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  bool Connector::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  const std::string &
  Connector::name() const {

    return _descriptor->name;
  }
// -----------------------------------------------------------------------------
  int
  Connector::number() const {

    return _descriptor->number;
  }

// -----------------------------------------------------------------------------
  int
  Connector::rows() const {

    return _descriptor->rows;
  }

// -----------------------------------------------------------------------------
  int
  Connector::columns() const {

    return _descriptor->columns;
  }

// -----------------------------------------------------------------------------
  int
  Connector::size() const {

    return _pin.size();
  }

// -----------------------------------------------------------------------------
  int
  Connector::pinNumber (int row, int column) const {

    return _descriptor->pinNumber (row, column, columns());
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Pin>> &
  Connector::pin()  {

    return _pin;
  }

// -----------------------------------------------------------------------------
  Pin *
  Connector::pin (int number) const {

    return _pin.at (number).get(); // throw out_of_range if not exist
  }

// -----------------------------------------------------------------------------
  Board *
  Connector::board() const {

    return _parent;
  }

// -----------------------------------------------------------------------------
  Device *
  Connector::device() const {

    return board()->device();
  }

// -----------------------------------------------------------------------------
  bool
  Connector::isDebug() const {

    return device()->isDebug();
  }

// -----------------------------------------------------------------------------
  void
  Connector::setDebug (bool enable) {

    device()->setDebug (enable);
  }

// -----------------------------------------------------------------------------
//                      Opérateur << vers ostream
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  enum Alignment {
    Left,
    Right,
    Center
  };

// -----------------------------------------------------------------------------
  class Field {
    public:
      std::string::size_type size;
      std::string name;
  };

// -----------------------------------------------------------------------------
  const std::array<Field, 7> _field = {{
      {5, "sOc"},
      {5, "sIo"},
      {10, "Name"},
      {6, "Mode"},
      {6, "Pull"},
      {3, "V"},
      {4, "Ph"}
    }
  };

// -----------------------------------------------------------------------------
  std::string
  toUpper (const std::string & s) {
    std::string out (s);

    std::transform (out.begin(), out.end(), out.begin(),
                    std::ptr_fun<int, int> (std::toupper));
    return out;
  }

// -----------------------------------------------------------------------------
  std::string
  format (const std::string & s, std::string::size_type w, Alignment a) {
    std::string in (s);
    std::string out (w, ' ');
    std::string::size_type pos;

    if (w < 3) {
      w = 3;
    }
    if (in.size() > (w - 2)) {

      in.resize (w - 2);
    }

    switch (a) {
      case Left:
        pos = 1;
        break;
      case Right:
        pos = out.size() - in.size() - 1;
        break;
      case Center:
        pos = (out.size() - in.size()) / 2;
        break;
    }

    out.replace (pos, in.size(), s);
    return out;
  }

// -----------------------------------------------------------------------------
  void
  printHline (int columns, std::ostream & os) {

    os << '+';
    for (unsigned int i = 0; i < _field.size(); i++) {

      os << std::string (_field[i].size, '-') << '+';
    }
    if (columns > 1) {

      os << '+';
      for (int i = _field.size() - 1; i >= 0 ; --i) {
        size_t s = _field[i].size;
        os << std::string (s, '-') << '+';
      }
    }
    os << std::endl;
  }

// -----------------------------------------------------------------------------
  void
  printTitle (int columns, std::ostream & os) {
    printHline (columns, os);

    os << '|';
    for (unsigned int i = 0; i < _field.size(); i++) {

      os << format (_field[i].name, _field[i].size, Center) << '|';
    }
    if (columns > 1) {

      os << '|';
      for (int i = _field.size() - 1; i >= 0 ; --i) {

        os << format (_field[i].name, _field[i].size, Center) << '|';
      }
    }
    os << std::endl;

    printHline (columns, os);
  }


// -----------------------------------------------------------------------------
  void
  printRow (const Connector * connector, int number, std::ostream & os) {
    std::array<std::string, 5> s;
    unsigned int i = 0;

    Pin * pin = connector->pin (number++);
    os << '|';
    if (pin->type() == Pin::TypeGpio) {
      s[0] = std::to_string (pin->mcuNumber());
      s[1] = std::to_string (pin->logicalNumber());
      s[2] = toUpper (pin->modeName());
      s[3] = toUpper (pin->pullName());
      if ( (pin->mode() == Pin::ModeInput) || (pin->mode() == Pin::ModeOutput)) {
        s[4] = std::to_string (pin->read());
      }
    }
    os << format (s[0], _field[i++].size, Right) << '|';
    os << format (s[1], _field[i++].size, Right) << '|';
    os << format (pin->name(), _field[i++].size, Right) << '|';
    os << format (s[2], _field[i++].size, Right) << '|';
    os << format (s[3], _field[i++].size, Right) << '|';
    os << format (s[4], _field[i++].size, Right) << '|';
    os << format (std::to_string (pin->physicalNumber()), _field[i++].size, Right) << '|';

    if (connector->columns() > 1) {

      pin = connector->pin (number);

      os << '|';
      if (pin->type() == Pin::TypeGpio) {
        s[0] = std::to_string (pin->mcuNumber());
        s[1] = std::to_string (pin->logicalNumber());
        s[2] = toUpper (pin->modeName());
        s[3] = toUpper (pin->pullName());
        if ( (pin->mode() == Pin::ModeInput) || (pin->mode() == Pin::ModeOutput)) {
          s[4] = std::to_string (pin->read());
        }
        else {
          s[4].clear();
        }
      }
      else {

        for (auto & n : s) {
          n.clear();
        }
      }
      os << format (std::to_string (pin->physicalNumber()), _field[--i].size, Left) << '|';
      os << format (s[4], _field[--i].size, Left) << '|';
      os << format (s[3], _field[--i].size, Left) << '|';
      os << format (s[2], _field[--i].size, Left) << '|';
      os << format (pin->name(), _field[--i].size, Left) << '|';
      os << format (s[1], _field[--i].size, Left) << '|';
      os << format (s[0], _field[--i].size, Left) << '|';
    }
    os << std::endl;
  }

// -----------------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& os, const Connector * c)  {
    std::string::size_type width = 0;
    std::ostringstream buf;

    if (c->columns() > 2) {

      throw std::out_of_range ("the number of columns must not exceed 2.");
    }

    for (auto & n : _field) {
      width += n.size;
    }
    width = (width + _field.size()) * c->columns();

    // entête
    os.width (width);

    buf << c->name() << " (#" << c->number() << ")";
    os << std::setw ( (width + buf.str().size()) / 2 + 1)  << toUpper (buf.str()) << std::endl;
    printTitle (c->columns(), os);
    // broches
    for (int i = 1; i <= c->size(); i += c->columns()) {

      printRow (c, i, os);
    }
    // pied de page
    if (c->rows() > 6) {

      printTitle (c->columns(), os);
    }
    else {

      printHline (c->columns(), os);
    }
    return os;
  }
}
/* ========================================================================== */
