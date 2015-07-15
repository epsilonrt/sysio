/**
 * @file test_ptty.c
 * @brief ptty read/write test
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


static pid_t pid;

// -----------------------------------------------------------------------------
// Read characters from the ptty and echo them to stdout
void
read_from_ptty (int fd) {
  FILE *stream;
  char str[128];
  const char * who;

  if (pid == (pid_t) 0) {
    who = "slave";
  }
  else {
    who = "master";
  }
  stream = fdopen (fd, "r");

  while ( fgets (str, 128, stream) != NULL) {

    printf ("%s: %s", who, str);
  }
  fclose (stream);
}

// -----------------------------------------------------------------------------
// Write some random text to the ptty
void
write_to_ptty (int fd) {
  FILE *stream;
  const char * who;
  const char hello[] = "hello, world !\n";
  const char bye[] = "goodbye, world !\n";

  if (pid == (pid_t) 0) {
    who = "slave";
  }
  else {
    who = "master";
  }

  stream = fdopen (fd, "w");
  printf ("%s: %s", who, hello);
  fprintf (stream, hello);
  printf ("%s: %s", who, bye);
  fprintf (stream, bye);
  fclose (stream);
}

// ------------------------------------------------------------------------------
int
main (int argc, char **argv) {
  int fdm;
  int mwrite = 0;

  if (argc > 1) {
    if (strcasecmp (argv[1], "master") == 0) {
      mwrite = 1;
    }
  }

  /* Create the ptty. */
  if ( (fdm = getpt ()) < 0) {

    perror ("getpt failed");
    return EXIT_FAILURE;
  }
  printf ("ptm has been successfully created\n");
  fflush (stdout);

  /* Create the child process. */
  pid = fork ();
  if (pid == (pid_t) 0) {
    int fds;
    char * pts;

    /* This is the child process */
    if ( (grantpt (fdm)) != 0) {

      perror ("grantpt failed");
      return EXIT_FAILURE;
    }
    if ( (unlockpt (fdm)) != 0) {

      perror ("grantpt failed");
      return EXIT_FAILURE;
    }
    ;
    if ( (pts = ptsname (fdm)) == NULL) {

      perror ("ptsname failed");
      return EXIT_FAILURE;
    }
    printf ("pts has been successfully created: %s\n", pts);
    fflush (stdout);
    if ( (fds = open (pts, O_RDWR))  < 0) {

      perror ("open failed");
      return EXIT_FAILURE;
    }
    printf ("pts has been successfully opened\n");
    fflush (stdout);

    if (mwrite) {
      read_from_ptty (fds);
    }
    else {
      write_to_ptty (fds);
    }
    return EXIT_SUCCESS;
  }
  else if (pid < (pid_t) 0) {

    /* The fork failed. */
    fprintf (stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  }
  else {

    /* This is the parent process */
    if (mwrite) {
      write_to_ptty (fdm);
    }
    else {
      read_from_ptty (fdm);
    }
    return EXIT_SUCCESS;
  }
}
