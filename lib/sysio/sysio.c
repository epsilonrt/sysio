/*
 * lib/sysio.c
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include <sysio/defs.h>

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
static int
iGetInt (const char * str) {
  char * endptr;
  int i = -1;

  long val = strtol (str, &endptr, 0);
  if (endptr > str) {
    i = val;
  }
  return i;
}

/* internal public functions ================================================ */
// "1.2-3-g06d9610"
// -----------------------------------------------------------------------------
const char * sSysIoVersion (void) {

  return VERSION;
}

// -----------------------------------------------------------------------------
const char * sSysIoGitCommit (void) {

  return strchr (VERSION, 'g');
}

// -----------------------------------------------------------------------------
int iSysIoMajor (void) {

  return iGetInt (VERSION);
}

// -----------------------------------------------------------------------------
int iSysIoMinor (void) {
  const char * p = strchr (VERSION, '.');
  if (p) {
    return iGetInt (++p);
  }
  return -1;
}

// -----------------------------------------------------------------------------
int iSysIoBuild (void) {
  const char * p = strchr (VERSION, '-');
  if (p) {
    return iGetInt (++p);
  }
  return 0;
}

/* ========================================================================== */
