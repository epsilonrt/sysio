/*
 * @file sysio/string.c
 * @brief Chaîne de caractères
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <ctype.h>
#include <sysio/string.h>


/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
char *
strlwr (char * str) {
  char * p = str;
  
  while (*p) {
    if (isupper(*p)) {
      *p = tolower (*p);
    }
    p++;
  }
  return str;
}

// -----------------------------------------------------------------------------
char *
strupr (char * str) {
  char * p = str;
  
  while (*p) {
    if (islower(*p)) {
      *p = toupper (*p);
    }
    p++;
  }
  return str;
}

// -----------------------------------------------------------------------------
char *
strcpyupr (char * dst, const char * src) {
  char * p = dst;
  
  while (*src) {
    if (islower(*src)) {
      *p = toupper (*src);
    }
    else {
      *p = *src;
    }
    p++;
    src++;
  }
  return dst;
}

// -----------------------------------------------------------------------------
char *
strcpylwr (char * dst, const char * src) {
  char * p = dst;
  
  while (*src) {
    if (isupper(*src)) {
      *p = tolower (*src);
    }
    else {
      *p = *src;
    }
    p++;
    src++;
  }
  return dst;
}

/* ========================================================================== */
