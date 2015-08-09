/*
 * sysio/win32/delay.c
 * @brief fonctions de temporisation
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <windows.h>
#include <sysio/delay.h>

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
// Must be thread friendly
int
delay_ms (long lMs) {

  return delay_us (lMs * 1000UL);
}

// -----------------------------------------------------------------------------
// Must be thread friendly
int
delay_us (long lUs) {

  if (lUs) {

    if (lUs == -1) {

      Sleep (INFINITE);
    }
    else {
      struct timeval tv;
      tv.tv_sec = lUs / 1000000UL;
      tv.tv_usec = lUs % 1000000UL;
      return select (0, NULL, NULL, NULL, &tf);
    }
  }
  return 0;
}

/* ========================================================================== */
