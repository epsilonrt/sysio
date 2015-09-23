/*
 * sysio_test_vector.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sysio/vector.h>

/* constants ================================================================ */
#define NAME_SIZE 5
/* macros =================================================================== */

/* structures =============================================================== */
typedef struct item {
  char name[10];
  int value;
} item;
/* private variables ======================================================== */
static const char * name[NAME_SIZE] = {
  "emil",
  "hannes",
  "lydia",
  "olle",
  "erik"
};

/* private functions ======================================================== */
static void * fnew (void);
static int fmatch (const void *fkey1, const void *fkey2);
static const void * fkey (const void * element);
static void vPrintStrVector (const xVector * v);
static void vPrintItemVector (const xVector * v);


/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i, ret;
  xVector v;

  ret = iVectorInit (&v, 3, NULL, NULL);
  assert (ret == 0);

  printf ("Test 1:\n");
  for (i = 0; i < NAME_SIZE; i++) {

    ret = iVectorAppend (&v, (void *) name[i]);
    assert (ret == 0);
    ret = iVectorSize (&v);
    assert (ret == i + 1);
  }

  for (i = 0; i < iVectorSize (&v); i++) {

    char * p = (char *) pvVectorGet (&v, i);
    assert (p);
    printf ("%s\n", p);
    assert (strcmp (p, name[i]) == 0);
  }
  printf ("Ok\n\n");

  printf ("Test 2:\n");
  for (i = 0; i < iVectorSize (&v) / 2; i++) {

    int n = iVectorSize (&v) - i - 1;
    char * p1 = (char *) pvVectorGet (&v, i);
    assert (p1);
    char * p2 = (char *) pvVectorGet (&v, n);
    assert (p2);
    ret = iVectorReplace (&v, i, p2);
    assert (ret == 0);
    ret = iVectorReplace (&v, n, p1);
    assert (ret == 0);
  }
  vPrintStrVector (&v);
  printf ("Ok\n\n");

  printf ("Test 3:\n");
  iVectorRemove (&v, 1);
  vPrintStrVector (&v);
  iVectorDestroy (&v);
  ret = iVectorSize (&v);
  assert (ret == 0);
  printf ("Ok\n\n");

  printf ("Test 4:\n");
  ret = iVectorInit (&v, 2, fnew, free);
  iVectorInitSearch (&v, fkey, fmatch);

  ret = iVectorResize (&v, NAME_SIZE);
  assert (ret == 0);
  ret = iVectorSize (&v);
  assert (ret == NAME_SIZE);
  vPrintItemVector (&v);
  printf ("Ok\n\n");

  printf ("Test 5:\n");
  for (i = 0; i < iVectorSize (&v); i++) {
    item * p = pvVectorGet (&v, i);
    assert (p);
    strcpy (p->name, name[i]);
    p->value = i;
  }
  vPrintItemVector (&v);
  printf ("Ok\n\n");

  printf ("Test 6:\n");
  for (i = 0; i < iVectorSize (&v); i++) {

    item * p = pvVectorFindFirst (&v, name[i]);
    assert (p);
    assert (strcmp (p->name, name[i]) == 0);
    assert (p->value == i);
  }
  printf ("Ok\n\n");

  printf ("Test 7:\n");
  ret = iVectorResize (&v, 3);
  assert (ret == 0);
  vPrintItemVector (&v);
  
  ret = iVectorSize (&v);
  assert (ret == 3);
  printf ("Ok\n\n");

  printf ("Test 8:\n");
  ret = iVectorRemove (&v, 1);
  assert (ret == 0);
  vPrintItemVector (&v);
  ret = iVectorSize (&v);
  assert (ret == 2);
  printf ("Ok\n\n");

  printf ("Test 9:\n");
  item * p = pvVectorGet (&v, 0);
  assert (p);
  ret = iVectorReplace (&v , 1, p);
  assert (ret == 0);
  vPrintItemVector (&v);
  iVectorDestroy (&v);
  ret = iVectorSize (&v);
  assert (ret == 0);
  printf ("Ok\n\n");

  return 0;
}

// -----------------------------------------------------------------------------
static void *
fnew (void) {

  item * p = calloc (1, sizeof (item));
  return p;
}


// -----------------------------------------------------------------------------
static int
fmatch (const void *fkey1, const void *fkey2) {

  return strcmp ( (const char *) fkey1, (const char *) fkey2);
}

// -----------------------------------------------------------------------------
static const void *
fkey (const void * e) {

  return ( (const item *) e)->name;
}

// -----------------------------------------------------------------------------
static void
vPrintStrVector (const xVector * v) {

  for (int i = 0; i < iVectorSize (v); i++) {

    char * p = (char *) pvVectorGet (v, i);
    assert (p);
    printf ("%s\n", p);
  }
}

// -----------------------------------------------------------------------------
static void
vPrintItemVector (const xVector * v) {

  for (int i = 0; i < iVectorSize (v); i++) {

    item * p = (item *) pvVectorGet (v, i);
    assert (p);
    printf ("%s=%d\n", p->name, p->value);
  }
}

/* ========================================================================== */
