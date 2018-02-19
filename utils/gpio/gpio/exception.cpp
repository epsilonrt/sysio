/**
 * @file
 * @brief gpio utility exeception impl.
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <sstream>
#include "exception.h"

// -----------------------------------------------------------------------------
Exception::Exception (Exception::Code code, int value) :
  _code (code), _value (value) {
  std::ostringstream s;

  switch (_code) {
    case CommandExpected:
      _msg.assign ("Command expected");
      break;
    case ArgumentExpected:
      _msg.assign ("Arguments expected");
      break;
    case PinNumberExpected:
      _msg.assign ("Pin number expected");
      break;
    case NotBinaryValue:
      s << _value << " is not a binary value";
      _msg = s.str();
      break;
    case NotPwmValue:
      s << _value << " is not a pwm value";
      _msg = s.str();
      break;
    case NotOutputPin:
      s << "Pin #" << value << " is not an output";
      _msg = s.str();
    case NotPwmPin:
      s << "Pin #" << value << " is not an pwm output";
      _msg = s.str();
      break;
    default:
      break;
  }
}


Exception::Exception (Exception::Code code, const std::string& param) :
  _code (code), _param (param) {

  switch (_code) {
      _msg.assign ("Unknown command: ");
      break;
    case IllegalMode:
      _msg.assign ("Illegal mode: ");
      break;
    default:
      break;
  }
  _msg.append (_param);
}
/* ========================================================================== */
