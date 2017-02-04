/**
 * @file spi.c
 * @brief Bus SPI (implémentation)
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <sysio/spi.h>

/* macros =================================================================== */
/* constants ================================================================ */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */
/* public variables ========================================================= */
/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int iSpiOpen (const char *device) {
  int fd;

  fd = open (device, O_RDWR);

  return fd;
}


// -----------------------------------------------------------------------------
int iSpiClose (int fd) {

  return close (fd);
}

// -----------------------------------------------------------------------------
int
iSpiGetConfig (int fd, xSpiIos * config) {
  uint8_t       byte;
  uint32_t      word;
  int ret;

  // Read the current configuration.
  if ( (ret = ioctl (fd, SPI_IOC_RD_MODE, &byte)) < 0) {

    perror ("SPI_IOC_RD_MODE");
    return ret;
  }
  config->mode = byte;

  if ( (ret = ioctl (fd, SPI_IOC_RD_LSB_FIRST, &byte)) < 0) {

    perror ("SPI_IOC_RD_LSB_FIRST");
    return ret;
  }
  config->lsb = (byte == SPI_LSB_FIRST ? eSpiNumberingLsb : eSpiNumberingMsb);

  if ( (ret = ioctl (fd, SPI_IOC_RD_BITS_PER_WORD, &byte)) < 0) {

    perror ("SPI_IOC_RD_BITS_PER_WORD");
    return ret;
  }
  config->bits = (byte == 0 ? eSpiBits8 : byte);

  if ( (ret = ioctl (fd, SPI_IOC_RD_MAX_SPEED_HZ, &word)) < 0) {

    perror ("SPI_IOC_RD_MAX_SPEED_HZ");
    return ret;
  }
  config->speed = word;
  return 0;
}

// -----------------------------------------------------------------------------
int
iSpiSetConfig (int fd, const xSpiIos * new_config) {
  xSpiIos config;
  uint8_t       byte;
  uint32_t      word;
  int ret;

  if ( (ret = iSpiGetConfig (fd, &config)) < 0) {

    return ret;
  }

  // Set the new configuration.
  if ( (config.mode != new_config->mode) && (new_config->mode != eSpiModeNotSet)) {

    byte = new_config->mode;
    if ( (ret = ioctl (fd, SPI_IOC_WR_MODE, &byte)) < 0) {

      perror ("SPI_IOC_WR_MODE");
      return ret;
    }
  }

  if ( (config.lsb != new_config->lsb) && (new_config->lsb != eSpiNumberingNotSet)) {

    byte = (new_config->lsb == eSpiNumberingLsb ? SPI_LSB_FIRST : 0);
    if ( (ret = ioctl (fd, SPI_IOC_WR_LSB_FIRST, &byte)) < 0) {

      perror ("SPI_IOC_WR_LSB_FIRST");
      return ret;
    }
  }

  if ( (config.bits != new_config->bits) && (new_config->bits != eSpiBitsNotSet)) {

    byte = (new_config->bits == eSpiBits8 ? 0 : new_config->bits);
    if ( (ret = ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &byte)) < 0) {

      perror ("SPI_IOC_WR_BITS_PER_WORD");
      fprintf (stderr, "Unable to set bits to %d\n", byte);
      return ret;
    }
  }

  if ( (config.speed != new_config->speed) && (new_config->speed != 0)) {

    word = new_config->speed;
    if ( (ret = ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &word)) < 0) {

      perror ("SPI_IOC_WR_MAX_SPEED_HZ");
      fprintf (stderr, "Failed to set speed to %d\n", word);
      return ret;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
int iSpiXfer (int fd, uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t rx_len) {
  struct spi_ioc_transfer spi_message[1];
  memset (spi_message, 0, sizeof (spi_message));

  spi_message[0].rx_buf = (unsigned long) rx_buffer;
  spi_message[0].tx_buf = (unsigned long) tx_buffer;
  spi_message[0].len = tx_len;

  return ioctl (fd, SPI_IOC_MESSAGE (1), spi_message);
}

// -----------------------------------------------------------------------------
int iSpiRead (int fd, uint8_t *rx_buffer, uint8_t rx_len) {
  struct spi_ioc_transfer spi_message[1];
  memset (spi_message, 0, sizeof (spi_message));

  spi_message[0].rx_buf = (unsigned long) rx_buffer;
  spi_message[0].len = rx_len;


  return ioctl (fd, SPI_IOC_MESSAGE (1), spi_message);
}

// -----------------------------------------------------------------------------
int iSpiWrite (int fd, uint8_t *tx_buffer, uint8_t tx_len) {
  struct spi_ioc_transfer spi_message[1];
  memset (spi_message, 0, sizeof (spi_message));

  spi_message[0].tx_buf = (unsigned long) tx_buffer;
  spi_message[0].len = tx_len;

  return ioctl (fd, SPI_IOC_MESSAGE (1), spi_message);
}

// -----------------------------------------------------------------------------
int
iSpiReadReg8 (int fd, uint8_t reg) {
  int ret;
  uint8_t buffer[2] = { reg & ~_BV (SPI_REG_RW_BIT), 0 };

  ret = iSpiXfer (fd, buffer, sizeof (buffer), buffer, sizeof (buffer));
  if (ret >= 0) {
    ret = (unsigned) buffer[1];
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iSpiReadReg16 (int fd, uint8_t reg) {
  int ret;
  uint8_t buffer[3] = { reg & ~_BV (SPI_REG_RW_BIT), 0, 0 };

  ret = iSpiXfer (fd, buffer, sizeof (buffer), buffer, sizeof (buffer));
  if (ret >= 0) {

    ret = ( ( (unsigned) buffer[1]) << 8) + ( (unsigned) buffer[2]);
  }
  return ret;

}

// -----------------------------------------------------------------------------
int
iSpiReadRegBlock (int fd, uint8_t reg, uint8_t * buf, uint8_t len) {
  int ret;
  uint8_t * buffer;

  len++;
  buffer = calloc (1, len);
  assert (buffer);

  buffer[0] = reg & ~_BV (SPI_REG_RW_BIT);
  ret = iSpiXfer (fd, buffer, len, buffer, len);
  if (ret >= 0) {

    memcpy (buf, &buffer[1], --ret);
  }
  free (buffer);
  return ret;
}

// -----------------------------------------------------------------------------
int
iSpiWriteReg8 (int fd, uint8_t reg, uint8_t data) {
  uint8_t buffer[2] = { reg | _BV (SPI_REG_RW_BIT), data };

  return iSpiXfer (fd, buffer, sizeof (buffer), buffer, sizeof (buffer)) - 1;
}

// -----------------------------------------------------------------------------
int
iSpiWriteReg16 (int fd, uint8_t reg, uint16_t data) {
  uint8_t buffer[3] = { reg | _BV (SPI_REG_RW_BIT), data >> 8, data & 0xFF };

  return iSpiXfer (fd, buffer, sizeof (buffer), buffer, sizeof (buffer)) - 1;
}

// -----------------------------------------------------------------------------
int
iSpiWriteRegBlock (int fd, uint8_t reg, const uint8_t * buf, uint8_t len) {
  int ret;
  uint8_t * buffer;

  len++;
  buffer = calloc (1, len);
  assert (buffer);

  buffer[0] = reg | _BV (SPI_REG_RW_BIT);
  memcpy (&buffer[1], buf, len - 1);
  ret = iSpiXfer (fd, buffer, len, buffer, len);
  if (ret > 0) {

    ret--;
  }
  free (buffer);
  return ret;
}


/* ========================================================================== */
