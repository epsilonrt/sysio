/**
 * @file sysio++/src/serial-wrapper.cpp
 * @brief Liaison série (Wrapper C++)
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/serial.hpp>
#include <sysio/serial.h>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <cstring>

//##############################################################################
//#                                                                            #
//#                            xSerial Class                                   #
//#                                                                            #
//##############################################################################

/* public  ================================================================== */

// -----------------------------------------------------------------------------
Serial::Serial (const char * portname, int baudrate) : fd (-1) {

  _baudrate = baudrate;
  _portname = portname;
}

// -----------------------------------------------------------------------------
Serial::~Serial () {

  close();
}

// -----------------------------------------------------------------------------
bool
Serial::open () {

  fd = iSerialOpen (_portname.c_str(), _baudrate);
  return fd >= 0;
}

// -----------------------------------------------------------------------------
void Serial::close() {

  if (fd >= 0) {

    vSerialClose (fd);
    fd = -1;
  }
}

// -----------------------------------------------------------------------------
int Serial::fileno() const {

  return fd;
}

// -----------------------------------------------------------------------------
void
Serial::flush() {

  if (fd >= 0) {

    vSerialFlush (fd);
  }
}

// -----------------------------------------------------------------------------
Serial::FlowControl
Serial::flowControl() const {

  if (fd >= 0) {

    return static_cast <Serial::FlowControl> (eSerialGetFlow (fd));
  }
  return UnknownFlowControl;
}

// -----------------------------------------------------------------------------
bool
Serial::setFlowControl (Serial::FlowControl newFlowControl) {

  if (fd >= 0) {

    return static_cast <Serial::FlowControl> (eSerialSetFlow (fd, static_cast <eSerialFlow> (newFlowControl))) == newFlowControl;
  }
  return false;
}

// -----------------------------------------------------------------------------
const char *
Serial::flowControlName() const {

  return eSerialGetFlowStr (fd);
}

// -----------------------------------------------------------------------------
int Serial::baudrate() const {

  return _baudrate;
}

// -----------------------------------------------------------------------------
const char * Serial::port() const {

  return _portname.c_str();
}

// -----------------------------------------------------------------------------
bool
Serial::setFlowControlName (const char * flowStr) {
  static const char * name[] = { "none", "hard", "soft", 0 };
  int flow = NoFlowControl;
  const char * p = name[flow];

  while (*p) {

    if (strncasecmp (p, flowStr, MIN (strlen (flowStr), strlen (p))) == 0) {

      return setFlowControl (static_cast <Serial::FlowControl> (flow));
    }
    flow++;
    p++;
  }
  return false;
}

// -----------------------------------------------------------------------------
void
Serial::setBaudrate (int baudrate) {

  if (baudrate != _baudrate) {
    int old = _baudrate;
    _baudrate = baudrate;

    if (fd >= 0) {
      close();
      if (open() == false) {
        _baudrate = old;
        open();
      }
    }
  }
}

// -----------------------------------------------------------------------------
void
Serial::setPort (const char * portname) {

  if (_portname.compare(portname) != 0) {
    std::string old = _portname;
    _portname = portname;

    if (fd >= 0) {
      close();
      if (open() == false) {
        _portname = old;
        open();
      }
    }
  }
}


/* ========================================================================== */
