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
#include <stdarg.h>
#include "version.h"

/* constants ================================================================ */
#define ERROR_BUFFER_SIZE 256

/* private variables ======================================================== */
static char sError[ERROR_BUFFER_SIZE];

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

// -----------------------------------------------------------------------------
const char *
sSysIoStrError (void) {
  return sError;
}

// -----------------------------------------------------------------------------
int
iSysIoError (const char *format, ...) {
  va_list va;
  int size;

  va_start (va, format);
  size = vsnprintf (sError, ERROR_BUFFER_SIZE - 1, format, va);
  va_end (va);
  return size;
}

// -----------------------------------------------------------------------------
bool
bSysIoLogAssert (void) {
#ifdef LOG_ASSERT
  return true;
#else
  return false;
#endif
}

/* ========================================================================== */
