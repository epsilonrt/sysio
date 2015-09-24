/**
 * @file sysio/string.h
 * @brief Chaîne de caractères
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_STRING_H_
#define _SYSIO_STRING_H_

#include <string.h>
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_string String
 *
 *  Ce module fournit des functions de manipulations de chaînes de caractères.
 *  @{
 */

/* internal public functions ================================================ */
/**
 * @brief Convertit une chaîne de caractères en minuscules
 * @param str pointeur sur la chaîne de caractères
 * @return pointeur sur la chaîne de caractères
 */
char * strlwr (char * str);

/**
 * @brief Convertit une chaîne de caractères en majuscules
 * @param str pointeur sur la chaîne de caractères
 * @return pointeur sur la chaîne de caractères
 */
char * strupr (char * str);

/**
 * @brief Copie une chaîne de caractères en minuscules
 * @param dst pointeur sur la chaîne de caractères destination
 * @param src pointeur sur la chaîne de caractères source
 * @return pointeur sur la chaîne de caractères
 */
char * strcpylwr (char * dst, const char * src);

/**
 * @brief Copie une chaîne de caractères en majuscules
 * @param dst pointeur sur la chaîne de caractères destination
 * @param src pointeur sur la chaîne de caractères source
 * @return pointeur sur la chaîne de caractères
 */
char * strcpyupr (char * dst, const char * src);

/**
 *  @}
 * @}
 */

#if !defined(__DOXYGEN__)
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#endif /* !defined(__DOXYGEN__) */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_STRING_H_ defined */