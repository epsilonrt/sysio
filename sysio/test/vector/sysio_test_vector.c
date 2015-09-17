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
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* public variables ========================================================= */

/* private functions ======================================================== */
static void destroy (void *data);
static int match (const void *key1, const void *key2);
static const void * key (const void * element);

#define NAME_SIZE 5
static const char * name[NAME_SIZE] = {
  "emil",
  "hannes",
  "lydia",
  "olle",
  "erik"
};

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int i,ret;
  xVector v1;

  ret = iVectorInit (&v1, NULL, NULL);
  assert (ret == 0);
  
  printf ("Test 1:\n");
  for (i = 0; i < NAME_SIZE; i++) {
    
    ret = iVectorAppend (&v1, (void *) name[i]);
    assert (ret == 0);
    ret = iVectorSize(&v1);
    assert (ret == i + 1);
  }

  for (i = 0; i < iVectorSize (&v1); i++) {
    
    char * p = (char *) pvVectorGet (&v1, i);
    assert (p);
    printf ("%s\n", p);
    assert (strcmp (p, name[i]) == 0);
  }

  printf ("\nTest 2:\n");
  for (i = 0; i < iVectorSize (&v1) / 2; i++) {
    
    int n = iVectorSize (&v1) - i - 1;
    char * p1 = (char *) pvVectorGet (&v1, i);
    assert (p1);
    char * p2 = (char *) pvVectorGet (&v1, n);
    assert (p2);
    ret = iVectorReplace (&v1, i, p2);
    assert (ret == 0);
    ret = iVectorReplace (&v1, n, p1);
    assert (ret == 0);
  }
  for (i = 0; i < iVectorSize (&v1); i++) {
    
    char * p = (char *) pvVectorGet (&v1, i);
    assert (p);
    printf ("%s\n", p);
  }

  printf ("\nTest 3:\n");
  iVectorRemove (&v1, 1);
  iVectorRemove (&v1, 3);

  for (i = 0; i < iVectorSize (&v1); i++) {
    char * p = (char *) pvVectorGet (&v1, i);
    printf ("%s\n", (char *) pvVectorGet (&v1, i));
  }

  vVectorDestroy (&v1);

  return 0;
}

static void destroy (void *data) {

}
static int match (const void *key1, const void *key2) {
  return 0;
}
static const void * key (const void * element) {
  return NULL;
}

/* ========================================================================== */
