/**
 * @file sysio/chipio/chipio.h
 * @brief Circuit d'entrées-sorties universel
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/i2c.h>
#include <sysio/chipio.h>
#include "interface.h"

/* structures =============================================================== */
typedef struct xChipIo {
  int fd; /*< Descripteur de fichier */
  pthread_mutex_t mutex; /*< Gestion de l'accès concurent au bus i2c */
  uint8_t ucRev;
  uint8_t ucOptAv;
} xChipIo;

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xChipIo *
xChipIoOpen (const char * sI2cBus, int iSlaveAddr) {
  xChipIo * chip = malloc (sizeof (xChipIo));

  if (chip) {
    int iReg;

    memset (chip, 0, sizeof (xChipIo));
    chip->fd = iI2cOpen (sI2cBus, iSlaveAddr);
    if (chip->fd < 0) {

      goto open_error_exit;
    }
    pthread_mutex_init (&chip->mutex, NULL);

    iReg = iChipIoReadReg8 (chip, eRegRev);
    if (iReg >= 0) {
      chip->ucRev = iReg;
    }
    iReg = iChipIoReadReg8 (chip, eRegOptAv);
    if (iReg >= 0) {
      chip->ucOptAv = iReg;
    }
  }
  return chip;

open_error_exit:
  free (chip);
  return NULL;
}

// -----------------------------------------------------------------------------
int
iChipIoRevisionMajor (xChipIo * chip) {

  return (unsigned)chip->ucRev >> 4;
}

// -----------------------------------------------------------------------------
int
iChipIoRevisionMinor (xChipIo * chip) {

  return (unsigned)chip->ucRev & 0x0F;
}

// -----------------------------------------------------------------------------
int
iChipIoAvailableOptions (xChipIo * chip) {

  return (unsigned) chip->ucOptAv;
}

// -----------------------------------------------------------------------------
int
iChipIoClose (xChipIo * chip) {

  int iRet = iI2cClose (chip->fd);
  free (chip);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoReadReg8 (xChipIo * chip, uint8_t reg) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cReadReg8 (chip->fd, reg);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoReadReg16 (xChipIo * chip, uint8_t reg) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cReadReg16 (chip->fd, reg);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoReadRegBlock (xChipIo * chip, uint8_t reg, uint8_t * buffer, uint8_t size) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cReadRegBlock (chip->fd, reg, buffer, size);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoWriteReg8 (xChipIo * chip, uint8_t reg, uint8_t data) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cWriteReg8 (chip->fd, reg, data);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoWriteReg16 (xChipIo * chip, uint8_t reg, uint16_t data) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cWriteReg16 (chip->fd, reg, data);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

// -----------------------------------------------------------------------------
int
iChipIoWriteRegBlock (xChipIo * chip, uint8_t reg, const uint8_t * buffer, uint8_t size) {
  int iRet;

  pthread_mutex_lock (&chip->mutex);
  iRet = iI2cWriteRegBlock (chip->fd, reg, buffer, size);
  pthread_mutex_unlock (&chip->mutex);
  return iRet;
}

/* ========================================================================== */
