/**
 * @file sysio/dlist.h
 * @brief Liste doublement chaînée
 *
 * Code issue de la lecture du livre "Maitrise des algorithmes en C" de K. Loudon
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_DLIST_H_
#define _SYSIO_DLIST_H_
#include <stdlib.h>
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */

/**
 *  @defgroup sysio_dlist Liste doublement chaînée
 *
 *  Ce module fournit les fonctions permettant de gérer des listes doublement chaînées.
 *  @{
 */

/* structures =============================================================== */

/**
 * @brief Define a structure for doubly-linked list elements
 */
typedef struct xDListElmt {

  void               *data;
  struct xDListElmt  *prev;
  struct xDListElmt  *next;

} xDListElmt;

/**
 * @brief  Define a structure for doubly-linked lists.
 */
typedef struct xDList {

  int                size;
  int (*match) (const void *key1, const void *key2);
  void (*destroy) (void *data);
  xDListElmt          *head;
  xDListElmt          *tail;
} xDList;

/* internal public functions ================================================ */
/**
 * @brief
 * @param list
 * @param data
 */
void vDListInit (xDList *list, void (*destroy) (void *data));

/**
 * @brief
 * @param list
 */
void vDListDestroy (xDList *list);

/**
 * @brief
 * @param list
 * @param element
 * @param data
 * @return
 */
int iDListInsertAfter (xDList *list, xDListElmt *element, const void *data);

/**
 * @brief
 * @param list
 * @param element
 * @param data
 * @return
 */
int iDListInsertBefore (xDList *list, xDListElmt *element, const void *data);

/**
 * @brief
 * @param list
 * @param element
 * @param data
 * @return
 */
int iDListRemove (xDList *list, xDListElmt *element, void **data);

# ifdef __DOXYGEN__
/**
 * @brief
 * @param list
 * @return
 */
static inline int iDListSize (const xDList *list);

/**
 * @brief
 * @param list
 * @return
 */
static inline xDListElmt * pxDListHead (const xDList *list);

/**
 * @brief
 * @param list
 * @return
 */
static inline xDListElmt * pxDListTail (const xDList *list);

/**
 * @brief
 * @param element
 * @return
 */
static inline bool bDListElmtIsHead (const xDListElmt *element);

/**
 * @brief
 * @param element
 * @return
 */
static inline bool bDListElmtIsTail (const xDListElmt *element);

/**
 * @brief
 * @param element
 * @return
 */
static inline void * pvDListElmtData (const xDListElmt *element);

/**
 * @brief
 * @param element
 * @return
 */
static inline xDListElmt * pxDListElmtNext (const xDListElmt *element);

/**
 * @brief
 * @param element
 * @return
 */
static inline xDListElmt * pxDListElmtPrev (const xDListElmt *element);

/**
 * @}
 */

# else /* __DOXYGEN__ not defined */

// -----------------------------------------------------------------------------
INLINE int
iDListSize (const xDList *list) {
  return list->size;
}

// -----------------------------------------------------------------------------
INLINE xDListElmt *
pxDListHead (const xDList *list) {
  return list->head;
}

// -----------------------------------------------------------------------------
INLINE xDListElmt *
pxDListTail (const xDList *list) {
  return list->tail;
}

// -----------------------------------------------------------------------------
INLINE bool
bDListElmtIsHead (const xDListElmt *element) {
  return element->prev == NULL ? 1 : 0;
}

// -----------------------------------------------------------------------------
INLINE bool
bDListElmtIsTail (const xDListElmt *element) {
  return element->next == NULL ? 1 : 0;
}

// -----------------------------------------------------------------------------
INLINE void *
pvDListElmtData (const xDListElmt *element) {
  return element->data;
}

// -----------------------------------------------------------------------------
INLINE xDListElmt *
pxDListElmtNext (const xDListElmt *element) {
  return element->next;
}

// -----------------------------------------------------------------------------
INLINE xDListElmt *
pxDListElmtPrev (const xDListElmt *element) {
  return element->prev;
}

# endif /* __DOXYGEN__ not defined */

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_DLIST_H_ */
