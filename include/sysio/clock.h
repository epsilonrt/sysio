/**
 * @file
 * @brief Clock
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_CLOCK_H_
#define _SYSIO_CLOCK_H_

#include <cstdint>

/**
 *  @defgroup sysio_clock Clock
 *  Provides the functionality of a clock for time measurement and delay.
 */

namespace Sysio {

  /**
  *  @addtogroup sysio_clock
  *  @{
  */

  /**
   * @class Clock
   * @author epsilonrt
   * @date 03/05/18
   * @brief Horloge
   */
  class Clock {

    public:
      /**
       * @brief Constructor
       *
       * His call is the origin of the times provided by the millis() and micros()
       * functions.
       */
      Clock();

      /**
       * @brief Destructor
       */
      virtual ~Clock();

      /**
       * @brief Pauses for the amount of milliseconds
       *
       * Pauses the thread for the amount of time (in milliseconds) specified
       * as parameter.
       *
       * @param ms the number of milliseconds to pause, -1 deaden the thread
       * until it is woken up by a signal
       */
      void delay (unsigned long ms);

      /**
       * @brief Pauses for the amount of microseconds
       *
       * Pauses the thread for the amount of time (in microseconds) specified
       * as parameter. \n
       * Depending on the value provided, this function can wait for a loop
       * or alarm (for short values) or put the thread into the corresponding
       * number of us.
       *
       * @param us the number of milliseconds to pause, -1 deaden the thread
       * until it is woken up by a signal
       */
      void delayMicroseconds (unsigned long us);

      /**
       * @brief Number of milliseconds
       *
       * Returns the number of milliseconds since began running the current
       * program.
       */
      unsigned long millis();

      /**
       * @brief Number of microseconds
       *
       * Returns the number of microseconds since began running the current
       * program.
       */
      unsigned long micros();

    private:
      uint64_t _us;
      uint64_t _ms;
  };
  /**
   * @}
   */
}

/* ========================================================================== */
#endif /*_SYSIO_CLOCK_H_ defined */
