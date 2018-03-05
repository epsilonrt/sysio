/**
 * @file
 * @brief Horloge
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/clock.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

namespace Sysio {

// -----------------------------------------------------------------------------
//
//                        Clock Class
//
// -----------------------------------------------------------------------------
  Clock::Clock() {
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    _ms = (uint64_t) t.tv_sec * (uint64_t) 1000ULL    + (uint64_t) (t.tv_nsec / 1000000ULL);
    _us = (uint64_t) t.tv_sec * (uint64_t) 1000000ULL + (uint64_t) (t.tv_nsec / 1000ULL);
  }

// -----------------------------------------------------------------------------
  Clock::~Clock() {

  }

// -----------------------------------------------------------------------------
  unsigned long
  Clock::millis () {
    uint64_t now;
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    now = (uint64_t) t.tv_sec * (uint64_t) 1000ULL    + (uint64_t) (t.tv_nsec / 1000000ULL);

    return (unsigned long) (now - _ms);
  }

// -----------------------------------------------------------------------------
  unsigned long
  Clock::micros () {
    uint64_t now;
    struct timespec t;

    clock_gettime (CLOCK_MONOTONIC_RAW, &t);
    now = (uint64_t) t.tv_sec * (uint64_t) 1000000ULL + (uint64_t) (t.tv_nsec / 1000ULL);

    return (unsigned long) (now - _us);
  }

// -----------------------------------------------------------------------------
  void
  Clock::delay (unsigned long d) {

    if (d) {

      if (d == -1) {

        sleep (-1);
      }
      else {
        struct timespec dt;

        dt.tv_nsec = (d % 1000UL) * 1000000UL;
        dt.tv_sec  = d / 1000UL;
        nanosleep (&dt, NULL);
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Clock::delayMicroseconds (unsigned long d) {

    if (d) {

      if (d == -1) {

        sleep (-1);
      }
      else {
        unsigned long s, us;
        struct timeval t1;

        gettimeofday (&t1, NULL);
        s  = d / 1000000UL;
        us = d % 1000000UL;

        if (d < 100) {
          struct timeval dt, t2;

          dt.tv_sec = s;
          dt.tv_usec = us;
          timeradd (&t1, &dt, &t2) ; // TODO: NO POSIX COMPLIANT
          while (timercmp (&t1, &t2, <)) {  // TODO: NO POSIX COMPLIANT

            gettimeofday (&t1, NULL);
          }
        }
        else {
          struct timespec dt;

          dt.tv_nsec = us * 1000UL;
          dt.tv_sec = s;
          nanosleep (&dt, NULL);
        }
      }
    }
  }
}
/* ========================================================================== */
