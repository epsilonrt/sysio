/**
 * @file sysio/dlist.c
 * @brief Liste doublement chaînée (Implémentation)
 *
 * Inspiré par la lecture du livre "Maitrise des algorithmes en C" de K. Loudon
 * 
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysio/dlist.h>

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
void
vDListInit (xDList *list, vDListElmtDestroy destroy) {
  assert (list);
  
  //  Initialize the list.
  memset (list, 0, sizeof(xDList));
  list->destroy = destroy;
}

// -----------------------------------------------------------------------------
void 
vDListSetSearch (xDList *list, pvDListElmtKey fkey, iDListElmtMatch fmatch) {
  
  list->key = fkey;
  list->match = fmatch;
}

// -----------------------------------------------------------------------------
int 
iDListRemove (xDList *list, xDListElmt *element, void **data) {

  //  Do not allow a NULL element or removal from an empty list.
  if (element == NULL || iDListSize (list) == 0) {
    return -1;
  }

  //  Remove the element from the list.
  *data = element->data;

  if (element == list->head) {

    //  Handle removal from the head of the list.
    list->head = element->next;

    if (list->head == NULL) {
      list->tail = NULL;
    }
    else {
      element->next->prev = NULL;
    }
  }
  else {

    //  Handle removal from other than the head of the list.
    element->prev->next = element->next;

    if (element->next == NULL) {
      list->tail = element->prev;
    }
    else {
      element->next->prev = element->prev;
    }
  }

  //  Free the storage allocated by the abstract data type.
  free (element);

  //  Adjust the size of the list to account for the removed element.
  list->size--;
  return 0;
}

// -----------------------------------------------------------------------------
void 
vDListDestroy (xDList *list) {

  void * data;

  //  Remove each element.
  while (iDListSize (list) > 0) {

    if (iDListRemove (list, pxDListTail (list), (void **) &data) == 0 && list->
        destroy != NULL) {

      //  Call a user-defined function to free dynamically allocated data.
      list->destroy (data);
    }
  }

  //  No operations are allowed now, but clear the structure as a precaution.
  memset (list, 0, sizeof (xDList));
}

// -----------------------------------------------------------------------------
int 
iDListInsertAfter (xDList *list, xDListElmt *element, const void *data) {
  xDListElmt * new_element;

  //  Do not allow a NULL element unless the list is empty.
  if (element == NULL && iDListSize (list) != 0) {
    return -1;
  }

  //  Allocate storage for the element.
  if ( (new_element = (xDListElmt *) malloc (sizeof (xDListElmt))) == NULL) {
    return -1;
  }

  //  Insert the new element into the list.
  new_element->data = (void *) data;

  if (iDListSize (list) == 0) {

    //  Handle insertion when the list is empty.
    list->head = new_element;
    list->head->prev = NULL;
    list->head->next = NULL;
    list->tail = new_element;
  }
  else {

    //  Handle insertion when the list is not empty.
    new_element->next = element->next;
    new_element->prev = element;

    if (element->next == NULL) {
      list->tail = new_element;
    }
    else {
      element->next->prev = new_element;
    }

    element->next = new_element;
  }

  //  Adjust the size of the list to account for the inserted element.
  list->size++;

  return 0;
}

// -----------------------------------------------------------------------------
int 
iDListInsertBefore (xDList *list, xDListElmt *element, const void *data) {
  xDListElmt * new_element;

  //  Do not allow a NULL element unless the list is empty.
  if (element == NULL && iDListSize (list) != 0) {
    return -1;
  }

  //  Allocate storage to be managed by the abstract data type.
  if ( (new_element = (xDListElmt *) malloc (sizeof (xDListElmt))) == NULL) {
    return -1;
  }

  //  Insert the new element into the list.
  new_element->data = (void *) data;

  if (iDListSize (list) == 0) {

    //  Handle insertion when the list is empty.
    list->head = new_element;
    list->head->prev = NULL;
    list->head->next = NULL;
    list->tail = new_element;
  }
  else {

    //  Handle insertion when the list is not empty.
    new_element->next = element;
    new_element->prev = element->prev;

    if (element->prev == NULL) {
      list->head = new_element;
    }
    else {
      element->prev->next = new_element;
    }

    element->prev = new_element;
  }

  //  Adjust the size of the list to account for the new element.
  list->size++;

  return 0;
}

// -----------------------------------------------------------------------------
xDListElmt * 
pxDListFindFirst (xDList *list, const void * key) {
  
  if ((list->key) && (list->match) && (iDListSize(list))) {
    xDListElmt * element = pxDListHead(list);
    
    do {
      if (list->match (key, list->key(element)) == 0) {
        return element;
      }
      element = pxDListElmtNext(element);
    } while (element);
  }
  return NULL;
}

/* ========================================================================== */
