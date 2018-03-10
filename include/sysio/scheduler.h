/**
 * @file
 * @brief Scheduler Class
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_SCHEDULER_H_
#define _SYSIO_SCHEDULER_H_

/**
 *  @defgroup sysio_sceduler Scheduler
 */

/**
 *  @addtogroup sysio_sceduler
 *  @{
 */

namespace Sysio {

  /**
   * @class Scheduler
   * @author epsilonrt
   * @date 10/03/18
   * @brief
   */
  class Scheduler {

    public:
      /**
       * @brief Définie la priorité en temps réel du thread appelant
       *
       * L'algorithme choisie est le round-robin. Sur un système Linux, la
       * valeur normale de priorité est de 20, la valeur minimale est de 1 et
       * la valeur maximale est de 99. Donner une valeur de 99 est une très
       * mauvaise idée qui peut bloquer le système...
       *
       * @param priority valeur de la priorité
       */
      static void setRtPriority (int priority);
  };
}

/**
* @}
*/

/* ========================================================================== */
#endif /*_SYSIO_SCHEDULER_H_ defined */
