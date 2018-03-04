/**
 * @file
 * @brief GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include <sysio/gpiodevice.h>

namespace Gpio {

// -----------------------------------------------------------------------------
//
//                           Board Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Board::Board (Device * dev, AccessLayer layer) :
    _roc (true), _isopen (false), _accesslayer (layer), _device (dev) {

    const std::vector<Connector::Descriptor> & v = device()->descriptor()->connector;

    if (layer == AccessLayerAuto) {

      _accesslayer = dev->preferedAccessLayer();
    }

    // Création des connecteurs à partir des descripteurs
    for (int i = 0; i < v.size(); i++) {

      _connector[v[i].number] = std::make_shared<Connector> (this, & v[i]);
    }
    setNumbering (Pin::NumberingLogical);
  }

// -----------------------------------------------------------------------------
  Board::~Board() {

    close();
    delete _device;
  }

// -----------------------------------------------------------------------------
  AccessLayer
  Board::accessLayer() const {

    return _accesslayer;
  }

// -----------------------------------------------------------------------------
  bool
  Board::open () {

    if (!isOpen()) {

      if (_accesslayer & AccessLayerIoMap) {

        if (! device()->open()) {

          return false;
        }
      }

      for (auto c = _connector.cbegin(); c != _connector.cend(); ++c) {

        if (!c->second->open()) {

          return false;
        }
      }

      _isopen = true;
    }

    return isOpen();
  }

// -----------------------------------------------------------------------------
  void
  Board::close() {

    if (isOpen()) {

      for (auto c = _connector.cbegin(); c != _connector.cend(); ++c) {

        c->second->close();
      }
      device()->close();
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  bool
  Board::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  Pin::Numbering
  Board::numbering() const {

    return _numbering;
  }

// -----------------------------------------------------------------------------
  void
  Board::setNumbering (Pin::Numbering nb) {

    if (nb != numbering()) {

      _pin.clear();

      for (auto cpair = _connector.cbegin(); cpair != _connector.cend(); ++cpair) {
        const std::shared_ptr<Connector> & conn = cpair->second;
        const std::map<int, std::shared_ptr<Pin>> & pinmap = conn->pin();

        for (auto pair = pinmap.cbegin(); pair != pinmap.cend(); ++pair) {

          std::shared_ptr<Pin> p = pair->second;
          if (p->type() == Pin::TypeGpio) {

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
  Board::numberingName () const {

    return Pin::numberingName (numbering());
  }

// -----------------------------------------------------------------------------
  bool
  Board::releaseOnClose() const {

    return _roc;
  }

// -----------------------------------------------------------------------------
  void
  Board::setReleaseOnClose (bool enable) {

    _roc = enable;
  }

// -----------------------------------------------------------------------------
  const std::string & 
  Board::name() const {
    
    return device()->descriptor()->name;
  }

// -----------------------------------------------------------------------------
  int
  Board::size() const {

    return _pin.size();
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Pin>> &
  Board::pin() {

    return _pin;
  }

// -----------------------------------------------------------------------------
  Pin *
  Board::pin (int number) const {

    return _pin.at (number).get();
  };

// -----------------------------------------------------------------------------
  int
  Board::connectors() const {

    return _connector.size();
  }

// -----------------------------------------------------------------------------
  Connector *
  Board::connector (int num) const {

    return _connector.at (num).get();
  }

// -----------------------------------------------------------------------------
  const std::map<int, std::shared_ptr<Connector>> &
  Board::connector() {
    return _connector;
  }

// -----------------------------------------------------------------------------
  Device *
  Board::device() const {

    return _device;
  }

// -----------------------------------------------------------------------------
  bool
  Board::isDebug() const {

    return device()->isDebug();
  }

// -----------------------------------------------------------------------------
  void
  Board::setDebug (bool enable) {

    device()->setDebug (enable);
  }
}
/* ========================================================================== */
