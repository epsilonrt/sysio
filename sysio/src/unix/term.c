/**
 * @file
 * @brief Utilitaires fichiers
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <sysio/term.h>
#include <errno.h>

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
int
kbhit (void) {
  struct timeval tv;
  fd_set fds;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO (&fds);
  FD_SET (STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select (STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET (STDIN_FILENO, &fds);
}

// -----------------------------------------------------------------------------
int 
noecho (bool state) {
  int ret;
  struct termios ttystate;

  //get the terminal state
  ret = tcgetattr (STDIN_FILENO, &ttystate);

  if (ret == 0) {
    if (state) {
      //turn off canonical mode
      ttystate.c_lflag &= ~ ECHO;
      //minimum of number input read.
      ttystate.c_cc[VMIN] = 1;
    }
    else {
      //turn on canonical mode
      ttystate.c_lflag |= ECHO;
    }

    //set the terminal attributes.
    ret = tcsetattr (STDIN_FILENO, TCSANOW, &ttystate);

  }
  return ret;
}

// -----------------------------------------------------------------------------
int 
raw (bool state) {
  int ret;
  struct termios ttystate;

  //get the terminal state
  ret = tcgetattr (STDIN_FILENO, &ttystate);

  if (ret == 0) {
    if (state) {
      //turn off canonical mode
      ttystate.c_lflag &= ~ (ICANON | ECHO);
      //minimum of number input read.
      ttystate.c_cc[VMIN] = 1;
    }
    else {
      //turn on canonical mode
      ttystate.c_lflag |= ICANON | ECHO;
    }

    //set the terminal attributes.
    ret = tcsetattr (STDIN_FILENO, TCSANOW, &ttystate);

  }
  return ret;
}
/* ========================================================================== */
