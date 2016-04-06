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
#include <sysio/delay.h>
#include "i2c-dev.h"

/* private variables ======================================================== */
static bool bI2cDebug;

// -----------------------------------------------------------------------------
static void
vI2cPrintDebug (int iErr, const char * func) {

  if ((iErr < 0) && (bI2cDebug)) {
    PERROR ("%s:%s", func, strerror (errno));
  }
}

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
  PERROR ("%s:%s", __func__, strerror (errno));
  return -1;
}

// -----------------------------------------------------------------------------
int
iI2cClose (int fd) {

  if (close (fd) != 0) {

    PERROR ("%s:%s", __func__, strerror (errno));
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iI2cRead (int fd) {
  int i = i2c_smbus_read_byte (fd);

  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadReg8 (int fd, uint8_t reg) {
  int i = i2c_smbus_read_byte_data (fd, reg);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadReg16 (int fd, uint8_t reg) {
  int i = i2c_smbus_read_word_data (fd, reg);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cReadRegBlock (int fd, uint8_t reg, uint8_t *values, uint8_t len) {
  int i = i2c_smbus_read_i2c_block_data (fd, reg, len, values);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWrite (int fd, uint8_t data) {
  int i = i2c_smbus_write_byte (fd, data);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteReg8 (int fd, uint8_t reg, uint8_t value) {
  int i = i2c_smbus_write_byte_data (fd, reg, value);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteReg16 (int fd, uint8_t reg, uint16_t value) {
  int i = i2c_smbus_write_word_data (fd, reg, value);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int
iI2cWriteRegBlock (int fd, uint8_t reg, const uint8_t * values, uint8_t len) {
  int i = i2c_smbus_write_i2c_block_data (fd, reg, len, values);

  vI2cPrintDebug (i, __func__);
  return i;
}


// -----------------------------------------------------------------------------
int
iI2cReadBlock (int fd, uint8_t * block, int len) {
  int i = read (fd, block, len);

  vI2cPrintDebug (i, __func__);
  return i;
}

// -----------------------------------------------------------------------------
int iI2cWriteBlock (int fd, const uint8_t * block, int len) {
  int i = write (fd, block, len);

  vI2cPrintDebug (i, __func__);
  return i;
}

/* structures =============================================================== */
struct xI2cMem {
  int        i2caddr;    /* slave device address */
  uint32_t   mem_size;   /* size (sum of all addr) */
  uint16_t   page_size;  /* for writes */
  uint8_t    flags;
  int fd;
  uint8_t * offset;
  uint8_t * data;
};

/* constants ================================================================ */
#ifndef I2CMEM_TRY_DELAYMS
#define I2CMEM_TRY_DELAYMS 5
#endif

#ifndef I2CMEM_TRY_MAX
#define I2CMEM_TRY_MAX     10
#endif

/* private functions ======================================================== */
/** ----------------------------------------------------------------------------
 * @brief Calcul et écriture de l'offset en fonction des caractéristiques de la mémoire.
 *
 * Les mémoires I2c doivent transmettre un ou deux octets d'offset lors d'un
 * accès direct. Si la taille de la mémoire excède la taille accessible avec
 * l'offset, les bits de poids fort de l'offset sont stockés dans les bits de
 * poids faible de l'octet de l'adresse I2c (limités à 3 bits).
 * Ainsi avec un offset sur 8 bits, on pourra gérer l'accès à une mémoire de
 * 2^(8+3) = 2^11 = 2048 octets = 2ko (8kbits).
 * Ainsi avec un offset sur 16 bits, on pourra gérer l'accès à une mémoire de
 * 2^(16+3) = 2^19 = 524288 octets = 512 ko.
 * Actuellement (avril 2016), la plus grande EEPROM existante est une 2Mbits
 * soit 256 ko.
 *
 * @param m pointeur sur la mémoire
 * @param offset offset à atteindre
 * @return l'adresse i2c modifiée éventuellement avec les bits de poids fort
 * de l'offset, les bits de poids faible de l'offset sont stockés dans le
 * buffer d'écriture de m
 */
static int
prviSetOffset (xI2cMem * m, uint32_t offset) {
  uint32_t full_mask;
  uint16_t lsb_mask;
  uint8_t offset_size;
  uint8_t offset_msb;

  full_mask = (m->mem_size - 1);
  offset_size = m->flags & I2CMEM_FLAG_ADDR16 ? 16 : 8;
  lsb_mask = m->flags & I2CMEM_FLAG_ADDR16 ? 0xFFFF : 0xFF;

  // Isole les bits de rang < offset_size, max 8 bits !
  offset_msb = (offset & full_mask) >> offset_size;
  offset &= lsb_mask;

  if (m->flags & I2CMEM_FLAG_ADDR16) {

    m->offset[0] = offset >> 8;
    m->offset[1] = offset;
  }
  else {

    m->offset[0] = offset;
  }

  if (offset_msb) {

    full_mask >>= offset_size;
    return (m->i2caddr & ~full_mask) | offset_msb;
  }

  return m->i2caddr;
}

// -----------------------------------------------------------------------------
xI2cMem *
xI2cMemOpen (const char * device, int i2caddr,
             uint32_t mem_size, uint16_t page_size, uint8_t flags) {
  xI2cMem * m;
  unsigned long i2cfuncs;

  m = calloc (sizeof (xI2cMem), 1 );
  assert (m);

  m->fd = iI2cOpen (device, i2caddr);
  if (m->fd < 0) {

    goto xI2cMemOpenError;
  }


  if (ioctl (m->fd, I2C_FUNCS, &i2cfuncs) < 0) {

    goto xI2cMemOpenError;
  }

  if (! (i2cfuncs & I2C_FUNC_I2C) ) {

    PERROR ("Plain i2c-level commands unsupported");
    goto xI2cMemOpenError;
  }

  m->i2caddr = i2caddr;
  m->page_size = page_size;
  m->mem_size = mem_size;
  m->flags = flags;

  if (flags & I2CMEM_FLAG_READONLY)  {
    
    // read only, le buffer ne contient que l'offset
    page_size = 0;
  }
  
  // Ajoute la taille nécessaire pour l'offset
  page_size += (flags & I2CMEM_FLAG_ADDR16) ? 2 : 1;

  m->offset = malloc (page_size);
  assert (m->offset);

  // m->data pointe après l'offset
  if (!(flags & I2CMEM_FLAG_READONLY))  {

    m->data = &m->offset[flags & I2CMEM_FLAG_ADDR16 ? 2 : 1];
  }
  return m;

xI2cMemOpenError:
  free (m);
  return NULL;

}

// -----------------------------------------------------------------------------
int
iI2cMemClose (xI2cMem * m) {
  int ret;

  ret = iI2cClose (m->fd);
  if (! (m->flags & I2CMEM_FLAG_READONLY) ) {

    free (m->offset);
  }
  free (m);
  return ret;
}

// -----------------------------------------------------------------------------
int
iI2cMemWrite  (xI2cMem * m,
               uint32_t offset, const uint8_t * buffer, uint16_t size) {
  int ret;
  int io_try;
  struct i2c_rdwr_ioctl_data i2c_data;
  struct i2c_msg msg[1];
  uint16_t count, remaining_bytes;
  const uint8_t * p;

  remaining_bytes = size;
  msg[0].addr = prviSetOffset (m, offset);

  /*
   * write operation
   */
  i2c_data.msgs = msg;
  i2c_data.nmsgs = 1;             // use one message structure

  msg[0].buf = (char *) m->offset;
  msg[0].flags = 0;     // don't need flags
  p = buffer;

  while (remaining_bytes) {

    count = MIN (m->page_size, remaining_bytes);
    memcpy (m->data, p, count);
    msg[0].len = count + 2;

    io_try = 0;
    do {
      // Si eeprom no ready, ret = -1, errno = EIO
      ret = ioctl (m->fd, I2C_RDWR, &i2c_data);

      if ( (ret < 0) && (errno == EIO) ) {

        // Mémoire non prête, on incrémente le nombre d'essais ...
        if (++io_try > I2CMEM_TRY_MAX) {

          // Timeout, la mémoire est bloquée ou absente
          return ret;
        }

        // on temporise avant de recommencer
        delay_ms (I2CMEM_TRY_DELAYMS);
      }
    }
    while ( (ret < 0) && (errno == EIO) );

    if (ret < 0) {

      return ret;
    }

    p += count;
    remaining_bytes -= count;
  }

  return 0;
}


// -----------------------------------------------------------------------------
int
iI2cMemRead  (xI2cMem * m,
              uint32_t offset, uint8_t * buffer, uint16_t size) {
  int ret;
  struct i2c_rdwr_ioctl_data i2c_data;
  struct i2c_msg msg[2];
  int io_try = 0;

  /*
   * Two operation is necessary for read operation.
   * First determine offset address
   * , and then receive data.
   */
  i2c_data.msgs = msg;
  i2c_data.nmsgs = 2;     // two i2c_msg

  msg[0].addr = prviSetOffset (m, offset);
  msg[0].flags = 0;     // write
  msg[0].len = 2;       // offset
  msg[0].buf = (char *) m->offset;

  /*
   * Second, read data from the EEPROM
   */
  msg[1].addr = msg[0].addr;
  msg[1].flags = I2C_M_RD;      // read command
  msg[1].len = size;
  msg[1].buf = (char *) buffer;

  do {
    // Si eeprom no ready, ret = -1, errno = EIO
    ret = ioctl (m->fd, I2C_RDWR, &i2c_data);

    if ( (ret < 0) && (errno == EIO) ) {

      // Mémoire non prête, on incrémente le nombre d'essais ...
      if (++io_try > I2CMEM_TRY_MAX) {

        // Timeout, la mémoire est bloquée ou absente
        break;
      }

      // on temporise avant de recommencer
      delay_ms (I2CMEM_TRY_DELAYMS);
    }
  }
  while ( (ret < 0) && (errno == EIO) );

  if (ret < 0) {

    return ret;
  }

  return size;
}

// -----------------------------------------------------------------------------
int 
iI2cMemAddr (const xI2cMem * m) {
  return m->i2caddr;
}

// -----------------------------------------------------------------------------
uint32_t 
ulI2cMemSize (const xI2cMem * m) {
  return m->mem_size;
}

// -----------------------------------------------------------------------------
uint16_t 
usI2cMemPageSize (const xI2cMem * m) {
  return m->page_size;
}

// -----------------------------------------------------------------------------
uint8_t 
ucI2cMemFlags (const xI2cMem * m) {
  return m->flags;
}

/* ========================================================================== */
