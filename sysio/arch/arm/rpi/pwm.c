/*
 * pwm.c
 * @brief
 * based on code from http://www.frank-buss.de/raspberrypi/pwm.c
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sysio/delay.h>
#include <sysio/iomap.h>
#include <sysio/pwm.h>
#include <sysio/rpi.h>

/* constants ================================================================ */
#define CLK_DIVISOR_DEFAULT 32
#define CLK_DIVISOR_MIN     2
#define CLK_DIVISOR_MAX     4095

#define PASSWD  (0x5A000000)

// Bits
#define BUSY    (1 << 7)
#define KILL    (1 << 5)
#define ENAB    (1 << 4)
#define SRC_OSC (1)
#define MODE1   (1 << 1)
#define PWEN1   (1 << 0)
#define MSEN1   (1 << 7)

// Register offsets
#define PWM_CTL     0
#define PWM_RNG1    4
#define PWM_DAT1    5

#define PWMCLK_CNTL 40
#define PWMCLK_DIV  41

/* structures =============================================================== */
typedef struct xPwm {
  xIoMap * pmap;  // PWM map
  xIoMap * cmap;  // CLOCK map
  xGpio * gpio;
  int pin;
} xPwm;

/* macros =================================================================== */
#define preg(__reg) (*((volatile unsigned int *)(pIo(pwm.pmap, (__reg)))))
#define creg(__reg) (*((volatile unsigned int *)(pIo(pwm.cmap, (__reg)))))

/* private variables ========================================================= */
static xPwm pwm;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static bool
bIsOpen (void) {

  return bIoMapIsOpen(pwm.pmap) && bIoMapIsOpen(pwm.cmap);
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xPwm *
xPwmOpen (int pin) {

  if (bIsOpen()) {
    return &pwm;
  }

  pwm.gpio = xGpioOpen (NULL);
  if (!pwm.gpio) {

    return 0;
  }

  if (iGpioMode (pin, eModePwm, pwm.gpio) != 0) {

    PERROR ("This pin can not operate in PWM mode");
    (void) iGpioClose (pwm.gpio);
    return 0;
  }

  pwm.pin  = pin;
  if ((pwm.pmap = xIoMapOpen (BCM2708_PWM_BASE, BCM2708_BLOCK_SIZE))) {

    if ((pwm.cmap = xIoMapOpen (BCM2708_CLK_BASE, BCM2708_BLOCK_SIZE))) {

      return &pwm; // Success
    }
    (void) iIoMapClose (pwm.pmap);
  }

  // Failed
  (void) iGpioRelease (pin, pwm.gpio);
  (void) iGpioClose (pwm.gpio);
  memset (&pwm, 0, sizeof (xPwm));
  return 0;
}

// -----------------------------------------------------------------------------
int
iPwmClose (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return 0;
  }

  (void) iPwmStop (&pwm);
  int i = iGpioRelease (pwm.pin, pwm.gpio);
  i = (iIoMapClose (pwm.pmap) == 0) ? i : -1;
  i = (iIoMapClose (pwm.cmap) == 0) ? i : -1;
  i = (iGpioClose (pwm.gpio) == 0) ? i : -1;
  memset (&pwm, 0, sizeof (xPwm));
  return i;
}

// -----------------------------------------------------------------------------
int
iPwmRun (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }
  if (iPwmClockDiv() < 2) {
    // the clock is stopped, is started with the default dvisor
    if (iPwmSetClockDiv (CLK_DIVISOR_DEFAULT) < 0) {
      return -1;
    }
  }
  preg(PWM_CTL) = MSEN1 | PWEN1;
  return 0;
}

// -----------------------------------------------------------------------------
int
iPwmStop (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }

  // disable PWM
  preg(PWM_CTL) = 0;
  delay_us (10);
  return 0;
}

// -----------------------------------------------------------------------------
bool
bPwmIsRunning (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return false;
  }

  return (preg(PWM_CTL) != 0);
}

// -----------------------------------------------------------------------------
bool
bPwmIsOpen (UNUSED_VAR(xPwm *, unused)) {

  return bIsOpen();
}

// -----------------------------------------------------------------------------
int
iPwmSetRange (int range, UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }

  preg(PWM_RNG1) = range;
  return 0;
}

// -----------------------------------------------------------------------------
int
iPwmSetValue (int value, UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }

  preg(PWM_DAT1) = value;
  return 0;
}

// -----------------------------------------------------------------------------
int
iPwmRange (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }

  return preg(PWM_RNG1);
}

// -----------------------------------------------------------------------------
int
iPwmValue (UNUSED_VAR(xPwm *, unused)) {

  if (!bIsOpen()) {
    return -1;
  }

  return preg(PWM_DAT1);
}

// -----------------------------------------------------------------------------
int
iPwmDivisor (double dFreq) {

  return BCM2708_PWM_FCLK / dFreq;
}

// -----------------------------------------------------------------------------
double
dPwmFreq (int iDiv) {

  return BCM2708_PWM_FCLK / iDiv;
}

// -----------------------------------------------------------------------------
int
iPwmSetClockFreq (double dFreq) {

  return iPwmSetClockDiv (iPwmDivisor (dFreq));
}

// -----------------------------------------------------------------------------
double
dPwmClockFreq (void) {
  int div = iPwmClockDiv();

  if (div > 0) {
    return BCM2708_PWM_FCLK / div;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iPwmSetClockDiv (int div) {

  if (!bIsOpen()) {
    return -1;
  }

  if (div < CLK_DIVISOR_MIN) {
    div = CLK_DIVISOR_MIN;
    PERROR ("The clock divider is out of bounds, it was increased to %d", CLK_DIVISOR_MIN);
  }
  else {
    if (div > CLK_DIVISOR_MAX) {
      div = CLK_DIVISOR_MAX;
      PERROR ("The clock divider is out of bounds, it was reduced to %d", CLK_DIVISOR_MAX);
    }
  }

  int backupctl = preg(PWM_CTL);

  preg(PWM_CTL) = 0; // Stop PWM
  delay_us (10);

  creg(PWMCLK_CNTL) = PASSWD | KILL;
  delay_us (10);

  // set frequency
  creg(PWMCLK_DIV)  = PASSWD | (div << 12);

  // enable clock
  creg(PWMCLK_CNTL) = PASSWD | ENAB | SRC_OSC;

  preg(PWM_CTL) = backupctl;

  return div;
}

// -----------------------------------------------------------------------------
int
iPwmClockDiv (void) {

  if (!bIsOpen()) {
    return -1;
  }

  int div = (creg(PWMCLK_DIV) >> 12);
  return div;
}

/* ========================================================================== */
