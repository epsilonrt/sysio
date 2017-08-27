/**
 * Copyright © 2017 epsilonRT, All rights reserved.
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * <http://www.cecill.info>.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 *
 * @file
 * @brief
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <assert.h>
#include <sysio/i2c.h>
#include <sysio/gp2.h>

/* structures =============================================================== */
struct xG2pSensor {
  int fd;
  xG2pSetting setting;
};

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xG2pSensor *
xGp2Open (const char * bus, int slave, const xG2pSetting * setting) {
  xG2pSensor * gp2;

  gp2 = calloc (sizeof (xG2pSensor), 1);
  assert (gp2);

  gp2->fd = iI2cOpen (bus, slave);
  if (gp2->fd < 0) {

    free (gp2);
    return NULL;
  }
  memcpy (&gp2->setting, setting, sizeof (xG2pSetting));
  return gp2;
}

// -----------------------------------------------------------------------------
int
iGp2Close (xG2pSensor * gp2) {
  assert (gp2);

  int fd = gp2->fd;
  free (gp2);
  return iI2cClose (fd);
}

// -----------------------------------------------------------------------------
void
vGp2SetSetting (xG2pSensor * gp2, const xG2pSetting * setting) {
  assert (gp2);

  memcpy (&gp2->setting, setting, sizeof (xG2pSetting));
}

// -----------------------------------------------------------------------------
int
iGp2ReadVoltage (xG2pSensor * gp2) {
  uint8_t buf[2];
  int ret;

  assert (gp2);
  ret = iI2cReadRegBlock (gp2->fd, 0, buf, 2);
  if (ret != 2) {
    
    return -1;
  }
  return (buf[0] + (buf[1] << 8));
}

// -----------------------------------------------------------------------------
int
iGp2Read (xG2pSensor * gp2) {
  int ret;

  ret = iGp2ReadVoltage (gp2);
  if (ret >= 0) {

    ret = iGp2VoltageToDensity (ret, &gp2->setting);
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iGp2VoltageToDensity (int v, const xG2pSetting * setting) {

  // y = (x-x1)*((y2-y1)/(x2-x1))+y1
  return round ((((double) v) - setting->dV1) *
         ( (setting->dD2 - setting->dD1) / (setting->dV2 - setting->dV1)) +
         setting->dD1);
}
/* ========================================================================== */
