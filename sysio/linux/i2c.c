/*
 * i2c.h
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sysio/i2c.h>
#include <sysio/log.h>
#include "i2c-dev.h"

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iI2cOpen (const char * device, int i2caddr) {
  int fd;

  if ( (fd = open (device, O_RDWR)) >= 0) {

    if (ioctl (fd, I2C_SLAVE, i2caddr) == 0) {

      return fd;
    }
  }
  PERROR ("%s", strerror (errno));
  return -1;
}

// -----------------------------------------------------------------------------
int
iI2cClose (int fd) {

  if (close (fd) != 0) {

    PERROR ("%s", strerror (errno));
    return -1;
  }
  return 0;
}
// -----------------------------------------------------------------------------
int
iI2cRead (int fd) {
  int i = i2c_smbus_read_byte (fd);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadReg8 (int fd, uint8_t reg) {
  int i = i2c_smbus_read_byte_data (fd, reg);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadReg16 (int fd, uint8_t reg) {
  int i = i2c_smbus_read_word_data (fd, reg);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadRegBlock (int fd, uint8_t reg, uint8_t *values, uint8_t len) {
  int i = i2c_smbus_read_i2c_block_data (fd, reg, len, values);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWrite (int fd, uint8_t data) {
  int i = i2c_smbus_write_byte (fd, data);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteReg8 (int fd, uint8_t reg, uint8_t value) {
  int i = i2c_smbus_write_byte_data (fd, reg, value);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteReg16 (int fd, uint8_t reg, uint16_t value) {
  int i = i2c_smbus_write_word_data (fd, reg, value);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteRegBlock (int fd, uint8_t reg, const uint8_t * values, uint8_t len) {
  int i = i2c_smbus_write_i2c_block_data (fd, reg, len, values);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}


// -----------------------------------------------------------------------------
int
iI2cReadBlock (int fd, uint8_t * block, int len) {
  int i = read (fd, block, len);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

// -----------------------------------------------------------------------------
int iI2cWriteBlock (int fd, const uint8_t * block, int len) {
  int i = write (fd, block, len);

  if (i < 0) {
    PERROR ("%s", strerror (errno));
  }
  return i;
}

/* ========================================================================== */
