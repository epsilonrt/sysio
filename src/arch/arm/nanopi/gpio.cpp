/*
 * arch/arm/nanopi/gpio.cpp
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include "gpiodevice_nanopi.h"

namespace Sysio {

// -----------------------------------------------------------------------------
//
//                           Gpio Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Gpio::Gpio (AccessLayer layer) : Gpio (new DeviceNanoPi(), layer) {

  }
}
/* ========================================================================== */
