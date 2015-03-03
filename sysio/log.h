/**
 * @file sysio/log.h
 * @brief Gestion des logs
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_LOG_H_
#define _SYSIO_LOG_H_
#include <sysio/defs.h>
#include <syslog.h>
#include <stdarg.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/* public variables ========================================================= */
extern const char* __progname;

/* internal public functions ================================================ */
void vLogInit (const char * myname, int mask);
void vLogDaemonize (bool daemon);
void vLogSetMask (int mask);
int  iLogMask (void);

/**
 * LOG_EMERG      Le système est inutilisable.
 * LOG_ALERT      Des actions doivent être entreprises immédiatement.
 * LOG_CRIT       Les conditions sont critiques.
 * LOG_ERR        Des erreurs se produisent.
 * LOG_WARNING    Des avertissement se présentent.
 * LOG_NOTICE     Condition normale, mais message significatif.
 * LOG_INFO       Message d'information simple.
 * LOG_DEBUG      Message de débogage.
 */
void vLog (int priority, const char *format, ...);

/* ========================================================================== */
#if !defined(__DOXYGEN__)
#ifdef LOG_ASSERT
#ifdef  _ASSERT_H

# undef _ASSERT_H
# undef assert
# undef __ASSERT_VOID_CAST
# undef __ASSERT_FUNCTION

# ifdef __USE_GNU
#  undef assert_perror
# endif

#endif /* assert.h  */

#include <features.h>

/* This prints an "Assertion failed" message and aborts.  */
extern void __vLogAssertFail (__const char *__assertion, __const char *__file,
         unsigned int __line, __const char *__function)
     __THROW __attribute__ ((__noreturn__));

/* Likewise, but prints the error text for ERRNUM.  */
extern void __vLogAssertPerrorFail (int __errnum, __const char *__file,
          unsigned int __line,
          __const char *__function)
     __THROW __attribute__ ((__noreturn__));


#if defined __cplusplus && __GNUC_PREREQ (2,95)
# define __ASSERT_VOID_CAST static_cast<void>
#else
# define __ASSERT_VOID_CAST (void)
#endif

/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __ASSERT_FUNCTION  __PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNCTION  __func__
#  else
#   define __ASSERT_FUNCTION  ((__const char *) 0)
#  endif
# endif

# define assert(expr)             \
  ((expr)               \
   ? __ASSERT_VOID_CAST (0)           \
   : __vLogAssertFail (__STRING(expr), __FILE__, __LINE__, __ASSERT_FUNCTION))

# ifdef __USE_GNU
#  define assert_perror(errnum)           \
  (!(errnum)                \
   ? __ASSERT_VOID_CAST (0)           \
   : __vLogAssertPerrorFail ((errnum), __FILE__, __LINE__, __ASSERT_FUNCTION))
# endif

#else /* LOG_ASSERT defined */
#include <assert.h>
#endif /* LOG_ASSERT not defined */
// -----------------------------------------------------------------------------
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_LOG_H_ defined */
