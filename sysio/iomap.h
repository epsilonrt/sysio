/**
 * @file sysio/iomap.h
 * @brief Projection mémoire
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_IOMAP_H_
#define _SYSIO_IOMAP_H_
#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_iomap Accès mémoire IO
 *
 *  Ce module fournit les fonctions pour accéder à la mémoire où sont implantés
 *  les coupleurs d'entrée-sortie
 *  @{
 */

/* structures =============================================================== */

/**
 * @brief Projection mémoire
 *
 * Cet objet permet d'accéder à des registres d'entrée-sortie
 * (gpio, port parallèle...) \n
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xIoMap xIoMap;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'une projection mémoire
 *
 * @param base adresse de base de la zone à projeter
 * @param size taille de la zone à projeter en octets
 * @return pointeur sur la projection, NULL si erreur
 */
xIoMap * xIoMapOpen (unsigned long base, unsigned int size);

/**
 * @brief Fermeture d'une projection mémoire
 *
 * @param p pointeur sur la projection
 */
int iIoMapClose (xIoMap *p);


/**
 * @brief Indique si une projection mémoire est ouverte
 *
 * @param p pointeur sur la projection
 * @return true si ouverte
 */
bool bIoMapIsOpen (const xIoMap *p);

/**
 * @brief Adresse de base de la zone projetée
 *
 * @param p pointeur sur la projection
 * @return la taille en octets
 */
unsigned long ulIoMapBase (const xIoMap *p);

/**
 * @brief Taille de la zone projetée
 *
 * @param p pointeur sur la projection
 * @return l'adresse de base
 */
unsigned int uIoMapSize (const xIoMap *p);

/**
 * @brief Pointeur d'accès aux registres
 *
 * @param p pointeur sur la projection
 * @param offset offset à l'intérieur de la zone en sizeof(int)
 * @return le pointeur sur le registre, NULL si erreur
 */
volatile unsigned int * pIo(const xIoMap *p, unsigned int offset);

/**
 * @}
 */

/* ========================================================================== */
__END_C_DECLS
#endif /*_SYSIO_IOMAP_H_ defined */
