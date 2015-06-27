/*
 * sysio/posix/serial.c
 * @brief Liaison série (Implémentation)
 * Copyright © 2014 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sysio/serial.h>
#include <sysio/delay.h>
#include <sysio/log.h>


#ifdef ARCH_ARM_RASPBERRYPI
// Le code ci-dessous est pour le Raspberry Pi
/* macros =================================================================== */
#define F_CPU 700000000UL // Raspberry Pi System Clock: 700 MHz
#define BAUDRATE(baudrate_reg) (unsigned long)(F_CPU/(8UL*(baudrate_reg+1UL)))

/* constants ================================================================ */
#define MAX_BAUDRATE BAUDRATE(0)
#define MIN_BAUDRATE BAUDRATE(0xFFFF)

// -----------------------------------------------------------------------------
static int
iCheckBaudrate (unsigned long ulBaud) {

  if ((ulBaud < MIN_BAUDRATE) || (ulBaud > MAX_BAUDRATE)) {

    vLog (LOG_WARNING, "Baudrate: %lu Bd out of range {%lu,%lu}",
                                ulBaud, MIN_BAUDRATE, MAX_BAUDRATE);
    return EBADBAUD;
  }
  return 0;
}
#else
#define iCheckBaudrate(b) (0)
#endif

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iSerialOpen (const char *device, const int baud) {
  int fd = iCheckBaudrate (baud);

  if (fd == 0) {
    struct termios options;
    speed_t myBaud;
    int     status;

    switch (baud) {
      case     50:
        myBaud =     B50;
        break;
      case     75:
        myBaud =     B75;
        break;
      case    110:
        myBaud =    B110;
        break;
      case    134:
        myBaud =    B134;
        break;
      case    150:
        myBaud =    B150;
        break;
      case    200:
        myBaud =    B200;
        break;
      case    300:
        myBaud =    B300;
        break;
      case    600:
        myBaud =    B600;
        break;
      case   1200:
        myBaud =   B1200;
        break;
      case   1800:
        myBaud =   B1800;
        break;
      case   2400:
        myBaud =   B2400;
        break;
      case   4800:
        myBaud =   B4800;
        break;
      case   9600:
        myBaud =   B9600;
        break;
      case  19200:
        myBaud =  B19200;
        break;
      case  38400:
        myBaud =  B38400;
        break;
      case  57600:
        myBaud =  B57600;
        break;
      case 115200:
        myBaud = B115200;
        break;
      case 230400:
        myBaud = B230400;
        break;

      default:
        return EBADBAUD;
    }

    if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
      return -1;

    fcntl (fd, F_SETFL, O_RDWR);

    // Get and modify current options:
    tcgetattr (fd, &options);

    cfmakeraw   (&options);
    cfsetispeed (&options, myBaud);
    cfsetospeed (&options, myBaud);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    // read() should never block
    options.c_cc [VMIN]  = 0;
    options.c_cc [VTIME] = 0;

    tcflush (fd, TCIOFLUSH);
    tcsetattr (fd, TCSANOW, &options);

    ioctl (fd, TIOCMGET, &status);
    status |= TIOCM_DTR;
    status |= TIOCM_RTS;
    ioctl (fd, TIOCMSET, &status);

    delay_ms (10);
  }

  return fd;
}

// -----------------------------------------------------------------------------
void
vSerialFlush (int fd) {

  tcflush (fd, TCIOFLUSH);
}

// -----------------------------------------------------------------------------
void
vSerialClose (int fd) {

  close (fd);
}

// -----------------------------------------------------------------------------
eSerialFlow
eSerialGetFlow (int fd) {
  struct termios xTermIos;
  eSerialFlow eCurrentFlow = SERIAL_FLOW_UNKNOWN;

  if (tcgetattr (fd, &xTermIos) == 0) {

    if (xTermIos.c_cflag & CRTSCTS) {

      eCurrentFlow = SERIAL_FLOW_RTSCTS;
    }
    else {

      eCurrentFlow = SERIAL_FLOW_NONE;
    }
  }
  return eCurrentFlow;
}

// -----------------------------------------------------------------------------
const char *
eSerialGetFlowStr (int fd) {
  static const char * sFlow[] = { "None", "Hardware", "Software" };
  static const char sUnknown[] = "Unknown" ;
  eSerialFlow eCurrentFlow;

  eCurrentFlow = eSerialGetFlow (fd);
  if (eCurrentFlow != SERIAL_FLOW_UNKNOWN) {
    
    return sFlow[eCurrentFlow];
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
eSerialFlow
eSerialSetFlow (int fd, eSerialFlow eNewFlow) {
  eSerialFlow eCurrentFlow;

  eCurrentFlow = eSerialGetFlow (fd);
  if (eNewFlow != eCurrentFlow) {
    struct termios xTermIos;

    switch (eNewFlow) {

      case SERIAL_FLOW_NONE:
        xTermIos.c_cflag &= ~CRTSCTS;
        break;
      case SERIAL_FLOW_RTSCTS:
        xTermIos.c_cflag |= CRTSCTS;
        break;
      default:
        return SERIAL_FLOW_UNKNOWN;
        break;
    }
    tcsetattr (fd, TCSANOW, &xTermIos);

    // Check if done
    tcgetattr (fd, &xTermIos);
    if (xTermIos.c_cflag & CRTSCTS) {

      eCurrentFlow = SERIAL_FLOW_RTSCTS;
    }
    else {

      eCurrentFlow = SERIAL_FLOW_NONE;
    }
  }

  return eCurrentFlow;
}

/* ========================================================================== */
