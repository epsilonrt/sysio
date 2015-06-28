/**
 * @file sysio++/serial.hpp
 * @brief Liaison série C++
 * 
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_SERIAL_HPP_
#define _SYSIO_SERIAL_HPP_

#include <sysio/defs.h>

//##############################################################################
//#                                                                            #
//#                             Serial Class                                   #
//#                                                                            #
//##############################################################################
/**
 * Serial Port Class
 */
class Serial {

  public:
    typedef enum {

      NoFlowControl,
      HardwareControl,
      SoftwareControl,
      UnknownFlowControl = -1
    } FlowControl;

    Serial();
    ~Serial();
    int open (const char *device, const int baud);
    void close();
    int fileno() const;
    void flush();
    FlowControl flowControl() const;
    bool setFlowControl (FlowControl newFlowControl);
    const char * flowControlName();

  private:
    int fd;
};

/* ========================================================================== */
#endif /* _SYSIO_SERIAL_HPP_ */
