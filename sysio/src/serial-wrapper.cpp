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
#include <stdlib.h>
#include <errno.h>
#include <stdexcept>

//##############################################################################
//#                                                                            #
//#                            xSerial Class                                   #
//#                                                                            #
//##############################################################################

/* public  ================================================================== */

// -----------------------------------------------------------------------------
Serial::Serial() : fd (-1) {

}

// -----------------------------------------------------------------------------
Serial::~Serial () {

  close();
}

// -----------------------------------------------------------------------------
int
Serial::open (const char *device, const int baud) {

  fd = iSerialOpen (device, baud);
  return fd;
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

  return static_cast <Serial::FlowControl> (eSerialGetFlow (fd));
}

// -----------------------------------------------------------------------------
bool 
Serial::setFlowControl (Serial::FlowControl newFlowControl) {

  return static_cast <Serial::FlowControl> (eSerialSetFlow (fd, static_cast <eSerialFlow> (newFlowControl))) == newFlowControl;
}

// -----------------------------------------------------------------------------
const char * 
Serial::flowControlName() {

  return eSerialGetFlowStr (fd);
}

/* ========================================================================== */
