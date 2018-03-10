/**
 * @file
 * @brief Scheduler Class
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/scheduler.h>
#include <system_error>
#include <sched.h>

namespace Sysio {

// -----------------------------------------------------------------------------
//
//                        Scheduler Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void Scheduler::setRtPriority (int priority) {
    struct sched_param sparam;
    int min, max;

    sched_setscheduler (0, SCHED_RR, &sparam);
    min = sched_get_priority_min (SCHED_RR);
    max = sched_get_priority_max (SCHED_RR);

    if (priority < min) {

      sparam.sched_priority = min;
    }
    if (priority > max) {

      sparam.sched_priority = max;
    }
    else {

      sparam.sched_priority = priority;
    }

    if (sched_setscheduler (0, SCHED_RR, &sparam) < 0) {

      throw std::system_error (errno, std::system_category(), __FUNCTION__);
    }
  }
}
/* ========================================================================== */
