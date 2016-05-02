/**
 * @file sysio/string.h
 * @brief Chaîne de caractères
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
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
 * @brief Convertion d'un string en entier long
 * 
 * @param str string commençant par la représentation d'un nombre entier.
 * @param n pointeur sur le nombre entier long résultat, si aucune conversion 
 * valide n'a pu être effectuée (-1), la valeur pointée n'est pas modifiée.
 * @param base base du nombre, si base vaut 0 ou  16, la chaîne peut inclure un 
 * préfixe « 0x » et le nombre sera interprété en base 16. Sinon, une base 
 * valant zéro est interprétée comme 10 (décimale) sauf si le caractère suivant 
 * est « 0 », auquel cas la base est 8 (octale).
 * @return 0, -1 si erreur
 */
int iStrToLong (const char * str, long * n, int base);

/** 
 * @brief Convertion d'un string en entier très long
 * 
 * @param str string commençant par la représentation d'un nombre entier.
 * @param n pointeur sur le nombre entier très long résultat, si aucune conversion 
 * valide n'a pu être effectuée (-1), la valeur pointée n'est pas modifiée.
 * @param base base du nombre, si base vaut 0 ou  16, la chaîne peut inclure un 
 * préfixe « 0x » et le nombre sera interprété en base 16. Sinon, une base 
 * valant zéro est interprétée comme 10 (décimale) sauf si le caractère suivant 
 * est « 0 », auquel cas la base est 8 (octale).
 * @return 0, -1 si erreur
 */
int iStrToLongLong (const char * str, long long * n, int base);

/** 
 * @brief Convertion d'un string en double
 * 
 * @param str string commençant par la représentation d'un décimal.
 * @param n pointeur sur le nombre décimal résultat, si aucune conversion 
 * valide n'a pu être effectuée (-1), la valeur pointée n'est pas modifiée.
 * @return 0, -1 si erreur
 */
int iStrToDouble (const char * str, double * n);

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
