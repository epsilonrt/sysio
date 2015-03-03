/*
 * sysio/serial.h
 * @brief Liaison série
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 * Revision History ---
 *    20140208 - Initial version
 */

#ifndef _SYSIO_SERIAL_H_
#define _SYSIO_SERIAL_H_
#include <sysio/defs.h>

__BEGIN_C_DECLS
/* ========================================================================== */

#define EBADBAUD (-2)

typedef enum {

  SERIAL_FLOW_NONE,
  SERIAL_FLOW_RTSCTS,
  SERIAL_FLOW_UNKNOWN
} eSerialFlow;

/* internal public functions ================================================ */

/**
 *  Open and initialise the serial port
 *
 * @param device device to operate on.
 * @param baud baudrate
 * @return file descriptor on the device, negative value on error
 */
int iSerialOpen (const char *device, const int baud);

/**
 *  Release the serial port
 *
 * @param file descriptor on the device.
 */
void vSerialClose (int fd);

/**
 *  Flush the serial buffers (both tx & rx)
 *
 * @param file descriptor on the device.
 */
void vSerialFlush (int fd);

eSerialFlow eSerialGetFlow (int fd);
eSerialFlow eSerialSetFlow (int fd, eSerialFlow eNewFlow);
const char * eSerialGetFlowStr (int fd);

/* ========================================================================== */
__END_C_DECLS
#endif /* _SYSIO_SERIAL_H_ */
