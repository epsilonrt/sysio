/**
 * @file sysio/iaq.h
 * @brief Capteur I2c IAQ de amS (Implémentation)
 *
 * Copyright © 2016 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sysio/iaq.h>
#include <sysio/i2c.h>
#include <sysio/delay.h>
#include "i2c-dev.h"

/* constants ================================================================ */
/*
 * Valeurs des bits d'état retournés par le capteur
 */
#define IAQ_ERROR 0x80

#ifdef DEBUG
#define IAQ_DEBUG(fmt,...) printf ("%s:%d: %s(): " fmt "\n", BASENAME(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define IAQ_DEBUG(fmt,...)
#endif

/* private variables ======================================================== */
struct xIaq {
  int     fd;
  uint8_t status;
  uint16_t co2;
  uint32_t resistance;
  uint16_t tvoc;
};

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xIaq *
xIaqOpen (const char * device, int i2caddr) {
  xIaq * s;
  unsigned long i2cfuncs;

  s = calloc (sizeof (xIaq), 1);
  assert (s);


  s->fd = iI2cOpen (device, i2caddr);
  if (s->fd < 0) {

    goto iaqOE;
  }

  if (ioctl (s->fd, I2C_FUNCS, &i2cfuncs) < 0) {

    goto iaqOC;
  }

  if (! (i2cfuncs & I2C_FUNC_I2C)) {

    errno = EIO;
    PERROR ("Plain i2c-level commands unsupported");
    goto iaqOC;
  }

  return s;

iaqOC:
  close (s->fd);
iaqOE:
  free (s);
  return NULL;
}

// -----------------------------------------------------------------------------
int
iIaqClose (xIaq * s) {
  int ret;

  ret = iI2cClose (s->fd);
  free (s);
  return ret;
}

// -----------------------------------------------------------------------------
int
iIaqRead (xIaq * s, xIaqData * data) {
  uint8_t buf[9];
  int ret;

  /* Mesure en cours */
  ret = iI2cReadBlock (s->fd, buf, sizeof (buf));
  if (ret != 9) {

    return -1;
  }

  // Extraction des données du bloc
  s->co2 = (buf[0] << 8) + buf[1];
  s->status = buf[2];
  s->resistance = (buf[4] << 16) + (buf[5] << 8) + buf[6];
  s->tvoc = (buf[7] << 8) + buf[8];
  
  if (s->status == 0) {
    
    data->usCo2 = s->co2;
    data->usTvoc = s->tvoc;
  }

  return s->status != IAQ_ERROR ? s->status : -1;
}

/* ========================================================================== */
