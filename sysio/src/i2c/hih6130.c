/**
 * @file sysio/hih6130.h
 * @brief Capteur I2c HIH6130 de Honeywell (Implémentation)
 *
 * Copyright © 2016 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sysio/hih6130.h>
#include <sysio/i2c.h>
#include <sysio/delay.h>
#include "i2c-dev.h"

/* constants ================================================================ */
/*
 * Valeurs des bits d'état retournés par le capteur
 */
typedef enum {
  HIH6130_DONE  = 0x00, /*< Données valides renvoyées */
  HIH6130_STALE = 0x40, /*< Donnée déjà lue */
  HIH6130_CMD   = 0x80, /*< Capteur en mode commande */
  HIH6130_DIAG  = 0xC0, /*< Diagnostic en cours */
  HIH6130_STATUS = HIH6130_DIAG /**< Masque des bits d'état */
} eHih6130Status;

#ifdef DEBUG
#define HIH_DEBUG(fmt,...)
//#define HIH_DEBUG(fmt,...) printf ("%s:%d: %s(): " fmt "\n", BASENAME(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define HIH_DEBUG(fmt,...)
#endif

/* private variables ======================================================== */
struct xHih6130 {
  int     fd;
  xHih6130Data data;
  uint8_t status;
  /* Bits de status */
#define MR_PROGRESS 0x01 /* Demande de mesure en cours */
#define DATA_AVAILABLE 0x02  /* Données disponibles */
};

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xHih6130 * 
xHih6130Open (const char * device, int i2caddr) {
  xHih6130 * s;
  unsigned long i2cfuncs;

  s = calloc (sizeof (xHih6130), 1);
  assert (s);


  s->fd = iI2cOpen (device, i2caddr);
  if (s->fd < 0) {

    goto hih6130OE;
  }

  if (ioctl (s->fd, I2C_FUNCS, &i2cfuncs) < 0) {

    goto hih6130OC;
  }

  if (! (i2cfuncs & I2C_FUNC_I2C)) {

    errno = EIO;
    PERROR ("Plain i2c-level commands unsupported");
    goto hih6130OC;
  }

  delay_ms (100); // Startup-time 60 ms max.

  // Démarrage d'une mesure pour vérification de présence du capteur
  if (iHih6130Start (s) != 0) {

    /* Erreur sur le bus I2C */
    errno = EIO;
    goto hih6130OC;
  }
  else {
    int ret;
    xHih6130Data dummy;

    // Lecture du résultat de la mesure pour vérification
    do {
      ret = iHih6130Read (s, &dummy);
      if (ret < 0) {

        goto hih6130OC;
      }
      delay_ms (10);
    }
    while (ret == HIH6130_BUSY);
  }

  return s;

hih6130OC:
  close (s->fd);
hih6130OE:
  free (s);
  return NULL;
}

// -----------------------------------------------------------------------------
int
iHih6130Close (xHih6130 * s) {
  int ret;

  ret = iI2cClose (s->fd);
  free (s);
  return ret;
}

// -----------------------------------------------------------------------------
int
iHih6130Start (xHih6130 * s) {

  /* Ecriture d'une trame vide = Measurement Request */
  if (iI2cWriteBlock (s->fd, NULL, 0) < 0) {

    /* Erreur sur le bus I2C */
    s->status = 0;
    return -1;
  }

  s->status = MR_PROGRESS; /* Demande de mesure en cours ... */
  return 0;
}

// -----------------------------------------------------------------------------
int
iHih6130Read (xHih6130 * s, xHih6130Data * data) {

  if (s->status == MR_PROGRESS) {
    uint8_t buf[4];
    int ret;

    /* Mesure en cours */
    ret = iI2cReadBlock (s->fd, buf, sizeof (buf));
    if (ret < 2) {

      return -1;
    }

    switch (buf[0] & HIH6130_STATUS) {

      case HIH6130_DONE: {
        int32_t lHum, lTemp;

        /* Nouvelles données disponibles */
        s->status = DATA_AVAILABLE;
        
        // Calcul de l'humidité relative en dixième de %
        lHum = ( ((int32_t)buf[0] & ~HIH6130_STATUS) << 8) + buf[1];
        s->data.dHum = ((double)lHum * 100.0) / 16382.0;

        if (ret == 4) {

          // Calcul de la température en dixième de oC
          lTemp = (((int32_t)buf[2] << 8) + (int32_t)buf[3]) >> 2;
          s->data.dTemp = (((double)lTemp * 165.0) / 16382.0) - 40.0;
        }
        else {

          s->data.dTemp = NAN;
        }
        HIH_DEBUG ("Raw temp.: %d - Raw hum.: %d", lTemp, lHum);
      }
      break;

      case HIH6130_STALE:
        return HIH6130_BUSY;

      case HIH6130_CMD:
      case HIH6130_DIAG:
        break;
    }
  }

  if (s->status == DATA_AVAILABLE) {

    /* Recopie dernières données disponibles */
    data->dHum = s->data.dHum;
    data->dTemp = s->data.dTemp;
  }
  else {

    /* Aucune mesure n'a encore été démarrée... */
    return -1;
  }

  return 0;
}

/* ========================================================================== */
