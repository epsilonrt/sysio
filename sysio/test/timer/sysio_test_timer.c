/*
 * sysio_test_timer.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/timer.h>

/* constants ================================================================ */
#define TIMEOUT 1000

/* private variables ======================================================== */
static xTimer * tm;
static volatile int c;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  int ret = iTimerDelete (tm);
  if (ret != 0) {
    perror ("iTimerDelete()");
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
static int
iCallback (xTimer * t) {

  char * m = pvTimerUserData (t);
  printf ("%s %d\n", m, c);
  c++;
  return 0;
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int ret, i;
  long ms;
  bool b;
  char msg[] = "Hello !";

  tm = xTimerNew();
  assert (tm);

  vTimerSetInterval (tm, TIMEOUT);
  ms = lTimerInterval (tm);
  assert (ms == TIMEOUT);

  b = bTimerIsSingleShot (tm);
  assert (b == true);
  vTimerSetSingleShot (tm, false);
  b = bTimerIsSingleShot (tm);
  assert (b == false);
  vTimerSetSingleShot (tm, true);
  b = bTimerIsSingleShot (tm);
  assert (b == true);


  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");
  for (;;) {
    
    c = 0;
    vTimerSetSingleShot (tm, false);
    ret = iTimerSetCallback (tm, iCallback, msg);
    assert (ret == 0);
    ret = iTimerStart (tm);
    assert (ret == 0);
    while (c < 10) 
      ;
    ret = iTimerStop (tm);
    assert (ret == 0);
    ret = iTimerUnsetCallback(tm);

    i = 0;
    vTimerSetSingleShot (tm, true);
    while (i++ < 10) {
      ret = iTimerStart (tm);
      assert (ret == 0);
      while (!bTimerIsTimeout (tm))
        ;
      putchar ('s');
      fflush (stdout);
    }
    putchar ('\n');
    ret = iTimerStop (tm);
    assert (ret == 0);

    i = 0;
    vTimerSetSingleShot (tm, false);
    ret = iTimerStart (tm);
    assert (ret == 0);
    while (i++ < 10) {

      ret = iTimerWait (tm);
      printf ("%d", ret);
      fflush (stdout);
    }
    putchar ('\n');
    ret = iTimerStop (tm);
    assert (ret == 0);
  }

  return 0;
}
/* ========================================================================== */
