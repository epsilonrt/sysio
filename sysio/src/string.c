/*
 * @file sysio/string.c
 * @brief Chaîne de caractères
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sysio/string.h>

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
char *
strlwr (char * str) {
  char * p = str;

  while (*p) {
    if (isupper (*p)) {
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
    if (islower (*p)) {
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
    if (islower (*src)) {
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
    if (isupper (*src)) {
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

// -----------------------------------------------------------------------------
int
iStrToDouble (const char * str, double * d) {
  char *endptr;
  double val;

  errno = 0;    /* Pour distinguer la réussite/échec après l'appel */
  val = strtod (str, &endptr);
  
  if (errno == ERANGE) {
         
    return -1;
  }

  if (endptr == str) {
    
    return -1;
  }

  /* Si nous sommes ici, strtol() a analysé un nombre avec succès */
  *d = val;
  return 0;
}

// -----------------------------------------------------------------------------
int
iStrToLong (const char * str, long * l, int base) {
  char *endptr;
  long val;

  errno = 0;    /* Pour distinguer la réussite/échec après l'appel */
  val = strtol (str, &endptr, base);
  
  if ( (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
       || (errno != 0 && val == 0)) {
         
    return -1;
  }

  if (endptr == str) {
    
    return -1;
  }

  /* Si nous sommes ici, strtol() a analysé un nombre avec succès */
  *l = val;
  return 0;
}

// -----------------------------------------------------------------------------
int
iStrToLongLong (const char * str, long long * ll, int base) {
  char *endptr;
  long long val;

  errno = 0;    /* Pour distinguer la réussite/échec après l'appel */
  val = strtoll (str, &endptr, base);
  
  if ( (errno == ERANGE && (val == LLONG_MAX || val == LLONG_MIN))
       || (errno != 0 && val == 0)) {
         
    return -1;
  }

  if (endptr == str) {
    
    return -1;
  }

  /* Si nous sommes ici, strtoll() a analysé un nombre avec succès */
  *ll = val;
  return 0;
}

/* ========================================================================== */
