/*
 * arch/arm/nanopi/gpio.cpp
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include "gpiodevice_nanopi.h"

namespace Gpio {

// -----------------------------------------------------------------------------
//
//                           Board Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  Board::Board (AccessLayer layer) : Board (new DeviceNanoPi(), layer) {

  }
}
/* ========================================================================== */
