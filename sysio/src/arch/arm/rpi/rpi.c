/*
 * rpi.c
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <string.h>
#include <sysio/rpi.h>

/* -----------------------------------------------------------------------------
  $ cat /proc/cpuinfo
  processor : 0
  model name  : ARMv6-compatible processor rev 7 (v6l)
  BogoMIPS  : 2.00
  Features  : half thumb fastmult vfp edsp java tls
  CPU implementer : 0x41
  CPU architecture: 7
  CPU variant : 0x0
  CPU part  : 0xb76
  CPU revision  : 7

  Hardware  : BCM2708
  Revision  : 000e
  Serial    : 0000000059ad5ead
 */
int
iRpiRev (void) {
  FILE* fp;
  char buffer[1024];
  size_t bytes_read;
  char* match;
  int revision;

  /* Read the entire contents of /proc/cpuinfo into the buffer.  */
  fp = fopen ("/proc/cpuinfo", "r");
  if (fp == NULL) {

    perror ("fopen");
    return -1;
  }
  bytes_read = fread (buffer, 1, sizeof (buffer), fp);
  fclose (fp);

  /* Bail if read failed or if buffer isn't big enough.  */
  if (bytes_read == 0 || bytes_read == sizeof (buffer)) {

    return 0;
  }

  /* NUL-terminate the text.  */
  buffer[bytes_read] = '\0';

  /* Locate BCM2708 for check if is it a Raspberry  */
  match = strstr (buffer, "BCM2708");
  if (match == NULL) {

    return 0;
  }

  /* Locate BCM2708 for checked if this host is a Raspberry  */
  match = strstr (buffer, "Revision");
  if (match == NULL) {

    return 0;
  }

  /* Parse the line to extract the revision.  */
  sscanf (match, "Revision  : %x", &revision);
#if 0
  printf ("Hardware Revision: 0x%04x\n", revision);
#endif
  return revision;
}

/* ========================================================================== */
