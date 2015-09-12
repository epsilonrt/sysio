
/*
 * sysio_test_dlist.c
 * @brief Illustrates using a doubly-linked list
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <sysio/dlist.h>

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
static void print_list (const xDList *list) {
  xDListElmt          *element;
  int                *data, i;

  fprintf (stdout, "List size is %d\n", iDListSize (list));

  i = 0;
  element = pxDListHead (list);

  while (1) {

    data = pvDListElmtData (element);
    fprintf (stdout, "list[%03d]=%03d\n", i, *data);
    i++;

    if (bDListElmtIsTail (element)) {
      break;
    }
    else {
      element = pxDListElmtNext (element);
    }
  }
}

/* main ===================================================================== */

int main (int argc, char **argv) {

  xDList              list;
  xDListElmt          *element;
  int                *data, i;

  //  Initialize the doubly-linked list.
  vDListInit (&list, free);
  
  //  Perform some doubly-linked list operations.
  element = pxDListHead (&list);

  for (i = 10; i > 0; i--) {

    if ( (data = (int *) malloc (sizeof (int))) == NULL) {
      return 1;
    }

    *data = i;
    if (iDListInsertBefore (&list, pxDListHead (&list), data) != 0) {
      return 1;
    }

  }

  print_list (&list);
  element = pxDListHead (&list);

  for (i = 0; i < 8; i++) {
    element = pxDListElmtNext (element);
  }

  data = pvDListElmtData (element);
  fprintf (stdout, "Removing an element after the one containing %03d\n", *data);

  if (iDListRemove (&list, element, (void **) &data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Inserting 011 at the tail of the list\n");

  *data = 11;
  if (iDListInsertAfter (&list, pxDListTail (&list), data) != 0) {
    return 1;
  }

  print_list (&list);
  fprintf (stdout, "Removing an element at the tail of the list\n");

  element = pxDListTail (&list);
  if (iDListRemove (&list, element, (void **) &data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Inserting 012 just before the tail of the list\n");

  *data = 12;
  if (iDListInsertBefore (&list, pxDListTail (&list), data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Iterating and removing the fourth element\n");

  element = pxDListHead (&list);
  element = pxDListElmtNext (element);
  element = pxDListElmtPrev (element);
  element = pxDListElmtNext (element);
  element = pxDListElmtPrev (element);
  element = pxDListElmtNext (element);
  element = pxDListElmtNext (element);
  element = pxDListElmtNext (element);

  if (iDListRemove (&list, element, (void **) &data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Inserting 013 before the first element\n");

  *data = 13;
  if (iDListInsertBefore (&list, pxDListHead (&list), data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Removing an element at the head of the list\n");

  if (iDListRemove (&list, pxDListHead (&list), (void **) &data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Inserting 014 just after the head of the list\n");
  *data = 14;

  if (iDListInsertAfter (&list, pxDListHead (&list), data) != 0) {
    return 1;
  }

  print_list (&list);

  fprintf (stdout, "Inserting 015 two elements after the head of the list\n");

  if ( (data = (int *) malloc (sizeof (int))) == NULL) {
    return 1;
  }

  *data = 15;
  element = pxDListHead (&list);
  element = pxDListElmtNext (element);

  if (iDListInsertAfter (&list, element, data) != 0) {
    return 1;
  }

  print_list (&list);

  i = bDListElmtIsHead (pxDListHead (&list));
  fprintf (stdout, "Testing bDListElmtIsHead...Value=%d (1=OK)\n", i);
  i = bDListElmtIsHead (pxDListTail (&list));
  fprintf (stdout, "Testing bDListElmtIsHead...Value=%d (0=OK)\n", i);
  i = bDListElmtIsTail (pxDListTail (&list));
  fprintf (stdout, "Testing bDListElmtIsTail...Value=%d (1=OK)\n", i);
  i = bDListElmtIsTail (pxDListHead (&list));
  fprintf (stdout, "Testing bDListElmtIsTail...Value=%d (0=OK)\n", i);

//  Destroy the doubly-linked list.                                           *
  fprintf (stdout, "Destroying the list\n");
  vDListDestroy (&list);

  return 0;

}

/* ========================================================================== */
