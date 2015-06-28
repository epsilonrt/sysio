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
#include <string>

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

    Serial(const char * portname, int baudrate);
    ~Serial();
    bool open();
    int baudrate() const;
    void setBaudrate (int baudrate);
    const char * port() const;
    void setPort (const char * portname);
    void close();
    int fileno() const;
    void flush();
    FlowControl flowControl() const;
    bool setFlowControl (FlowControl newFlowControl);
    const char * flowControlName() const;
    bool setFlowControlName (const char * newFlowControl);
    
    // Swig access functions (python interface)
    inline int getFileno() const { return fileno(); }
    inline int getBaudrate() const { return baudrate(); }
    inline const char * getPort() const { return port(); }
    inline int getFlowControl() const { return flowControl(); }
    inline const char * getFlowControlName() const { return flowControlName(); }

  private:
    int fd;
    int _baudrate;
    std::string _portname;
};

/* ========================================================================== */
#endif /* _SYSIO_SERIAL_HPP_ */
