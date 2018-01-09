/*
 * template.cpp
 * @brief Description of your program
 */
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <csignal>
#include <sysio/delay.h>

using namespace std;

/* constants ================================================================ */
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* public variables ========================================================= */

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  cout << endl << "everything was closed." << endl << "Have a nice day !" << endl;
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {

  // Replace the lines below with your code.
  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  cout << "Press Ctrl+C to abort ..." << endl;
  for (;;) {
    cout << '.' << flush;
    delay_ms (1000);
  }

  return 0;
}
/* ========================================================================== */
