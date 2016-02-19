/**
 * @file sysio/test/pwm/pwm.c
 * @brief Test sortie PWM
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sysio/pwm.h>
#include <sysio/delay.h>

/* constants ================================================================ */
#define PIN 1
#define RANGE 64
#define VALUE 16
#define FREQ  10000.0

/* private variables ======================================================== */
static xPwm * pwm;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  assert (iPwmClose (pwm) == 0);
  printf("\neverything was closed.\nHave a nice day !\n");
  exit(EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int pin = PIN;
  int div;
  int range = RANGE;
  int value = VALUE;
  int freq = FREQ;

  char * endptr;
  const char * str;
  long val;

  if (argc > 1) {
    str = argv[1];

    val = strtol(str, &endptr, 0);
    if (endptr > str) {
      value = val;
    }
  }

  if (argc > 2) {
    str = argv[2];

    val = strtol(str, &endptr, 0);
    if (endptr > str) {
      range = val;
    }
  }

  if (argc > 3) {
    str = argv[3];

    val = strtol(str, &endptr, 0);
    if (endptr > str) {
      freq = val;
    }
  }

  div = iPwmDivisor(freq * range);

  printf ("Open gpio pwm pin %d... ", pin);
  pwm = xPwmOpen (pin);
  assert (pwm != 0);
  printf ("Success !\n");

  printf ("Setting range to %d.\n", range);
  assert (iPwmSetRange (range, pwm) == 0);
  printf ("\tRange sets to %d.\n", iPwmRange(pwm));

  printf ("Setting clock divisor to %d.\n", div);
  assert (iPwmSetClockDiv (div) > 0);
  printf ("\tClock divisor: %d\n", iPwmClockDiv());
  printf ("\tClock frequency: %g Hz\n", dPwmClockFreq());


  printf ("Setting value to %d.\n", value);
  assert (iPwmSetValue (value, pwm) == 0);
  printf ("\tValue sets to %d.\n", iPwmValue(pwm));

  printf ("\nRunning Pwm... ");
  assert (iPwmRun (pwm) == 0);
  printf ("%s !\n", bPwmIsRunning(pwm) ? "Success" : "Failed");
  double f = dPwmClockFreq() / iPwmRange(pwm);
  printf ("\tPwm frequency: %g Hz\n", f);
  double d = (double) iPwmValue(pwm) / (double) iPwmRange(pwm) * 100.0;
  printf ("\tThe duty cycle is %g %%\n", d);

  // vSigIntHandler() intercepte le CTRL+C
  signal(SIGINT, vSigIntHandler);

  printf("Press Ctrl+C to abort ...\n");
  sleep (-1); // Sleep and wait CTRL+C

  return 0;
}

/* ========================================================================== */
