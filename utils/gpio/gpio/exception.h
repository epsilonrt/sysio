/**
 * @file
 * @brief gpio utility exception
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_UTILS_GPIO_EXCEPTION_H_
#define _SYSIO_UTILS_GPIO_EXCEPTION_H_
#include <string>
#include <exception>

// -----------------------------------------------------------------------------
class Exception: public std::exception {
  public:
    enum Code {
      CommandExpected = 1,
      UnknownCommand,
      ArgumentExpected,
      IllegalMode,
      PinNumberExpected,
      NotBinaryValue,
      NotPwmValue,
      NotOutputPin,
      NotPwmPin
    };

    explicit Exception (Exception::Code code, int value = 0);
    explicit Exception (Exception::Code code, const std::string& param);
    explicit Exception (Exception::Code code, const char* param) : Exception (code, std::string (param)) {}
    virtual ~Exception() throw () {}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char * what() const throw () {

      return _msg.c_str();
    }


    virtual Exception::Code code() const throw () {
      return _code;
    }

  protected:
    Exception::Code _code;
    std::string _param;
    int _value;
    std::string _msg;
};
/* ========================================================================== */
#endif /*_SYSIO_UTILS_GPIO_EXCEPTION_H_ defined */
