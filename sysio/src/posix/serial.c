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
#include <stdio.h>
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

/* private variables ======================================================== */
static const char sUnknown[] = "Unknown" ;

/* private functions ======================================================== */

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
int
iSerialDataAvailable (int fd) {
  int iDataAvailable;

  if (ioctl (fd, FIONREAD, &iDataAvailable) == -1) {
    return -1;
  }
  return iDataAvailable;
}

// -----------------------------------------------------------------------------
int
iSerialGetBaudrate (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return iSerialTermiosBaudrate (&ts);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eSerialDataBits
eSerialGetDataBits (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return iSerialTermiosDataBits (&ts);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eSerialStopBits
eSerialGetStopBits (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return iSerialTermiosStopBits (&ts);
  }
  return -1;
}

// -----------------------------------------------------------------------------
eSerialParity
eSerialGetParity (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return iSerialTermiosParity (&ts);
  }
  return -1;
}


// -----------------------------------------------------------------------------
eSerialFlow
eSerialGetFlow (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return iSerialTermiosFlow (&ts);
  }
  return -1;
}

// -----------------------------------------------------------------------------
const char *
sSerialConfigStrShort (int fd) {
  struct termios ts;

  if (tcgetattr (fd, &ts) == 0) {

    return sSerialTermiosToStr (&ts);
  }
  return NULL;
}


// -----------------------------------------------------------------------------
int
iSerialSetBaudrate (int fd, int iBaudrate) {
  int i = -1;
  speed_t s = eSerialIntToSpeed (iBaudrate);

  if (s != -1) {
    struct termios ts;

    if ((i = tcgetattr (fd, &ts)) == 0) {

      cfsetispeed (&ts, s);
      cfsetospeed (&ts, s);
      tcflush (fd, TCIOFLUSH);
      return tcsetattr (fd, TCSANOW, &ts);
    }
  }

  return i;
}

// -----------------------------------------------------------------------------
int
iSerialSetDataBits (int fd, eSerialDataBits eDataBits) {
  struct termios ts;
  int i;

  if ((i = tcgetattr (fd, &ts)) == 0) {

    ts.c_cflag &= ~CSIZE;
    switch (eDataBits) {

      case SERIAL_DATABIT_5:
        ts.c_cflag |= CS5;
        break;
      case SERIAL_DATABIT_6:
        ts.c_cflag |= CS6;
        break;
      case SERIAL_DATABIT_7:
        ts.c_cflag |= CS7;
        break;
      case SERIAL_DATABIT_8:
        ts.c_cflag |= CS8;
        break;
      default:
        return -1;
    }
    tcflush (fd, TCIOFLUSH);
    return tcsetattr (fd, TCSANOW, &ts);
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iSerialSetStopBits (int fd, eSerialStopBits eStopBits) {
  struct termios ts;
  int i;

  if ((i = tcgetattr (fd, &ts)) == 0) {

    ts.c_cflag &= ~CSTOPB;
    switch (eStopBits) {

      case SERIAL_STOPBIT_ONE:
        break;
      case SERIAL_STOPBIT_TWO:
        ts.c_cflag |= CSTOPB;
        break;
      default:
        return -1;
    }
    tcflush (fd, TCIOFLUSH);
    return tcsetattr (fd, TCSANOW, &ts);
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iSerialSetParity (int fd, eSerialParity eParity) {
  struct termios ts;
  int i;

  if ((i = tcgetattr (fd, &ts)) == 0) {

    ts.c_cflag &= ~(PARENB | PARODD);
    switch (eParity) {

      case SERIAL_PARITY_NONE:
        break;
      case SERIAL_PARITY_EVEN:
        ts.c_cflag |= PARENB;
        break;
      case SERIAL_PARITY_ODD:
        ts.c_cflag |= PARENB | PARODD;
        break;
      default:
        return -1;
    }
    tcflush (fd, TCIOFLUSH);
    return tcsetattr (fd, TCSANOW, &ts);
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iSerialSetFlow (int fd, eSerialFlow eFlow) {
  struct termios ts;
  int i;

  if ((i = tcgetattr (fd, &ts)) == 0) {

    ts.c_cflag &= ~CRTSCTS;
    ts.c_iflag &= ~(IXON | IXOFF | IXANY);
    switch (eFlow) {

      case SERIAL_FLOW_NONE:
        break;
      case SERIAL_FLOW_RTSCTS:
        ts.c_cflag |= CRTSCTS;
        break;
      case SERIAL_FLOW_XONXOFF:
        ts.c_iflag |= IXON | IXOFF | IXANY;
        break;
      default:
        return -1;
    }
    tcflush (fd, TCIOFLUSH);
    return tcsetattr (fd, TCSANOW, &ts);
  }
  return i;
}

// -----------------------------------------------------------------------------
int
iSerialTermiosBaudrate (const struct termios * ts) {
  speed_t baud = cfgetospeed (ts);

  return iSerialSpeedToInt (baud);
}

// -----------------------------------------------------------------------------
int
iSerialTermiosDataBits (const struct termios * ts) {

  switch (ts->c_cflag & CSIZE) {
    case CS5:
      return SERIAL_DATABIT_5;
    case CS6:
      return SERIAL_DATABIT_6;
    case CS7:
      return SERIAL_DATABIT_7;
    case CS8:
      return SERIAL_DATABIT_8;
    default:
      break;
  }
  return SERIAL_DATABIT_UNKNOWN;
}

// -----------------------------------------------------------------------------
int
iSerialTermiosStopBits (const struct termios * ts) {

  if (ts->c_cflag & CSTOPB) {
    return SERIAL_STOPBIT_TWO;
  }
  return SERIAL_STOPBIT_ONE;
}

// -----------------------------------------------------------------------------
int
iSerialTermiosParity (const struct termios * ts) {

  switch (ts->c_cflag & (PARENB | PARODD)) {
    case PARENB:
      return SERIAL_PARITY_EVEN;
    case PARENB | PARODD:
      return SERIAL_PARITY_ODD;
    default:
      break;
  }
  return SERIAL_PARITY_NONE;
}

// -----------------------------------------------------------------------------
int
iSerialTermiosFlow (const struct termios * ts) {

  if (ts->c_cflag & CRTSCTS) {
    return SERIAL_FLOW_RTSCTS;
  }
  if (ts->c_iflag & (IXON | IXOFF | IXANY)) {
    return SERIAL_FLOW_XONXOFF;
  }
  return SERIAL_FLOW_NONE;
}

// -----------------------------------------------------------------------------
const char *
sSerialTermiosToStr  (const struct termios * ts) {
  // BBBBBBBB-DPSF
  static char str[14];
  snprintf (str, 14, "%d-%d%c%d%c",
    iSerialTermiosBaudrate (ts),
    iSerialTermiosDataBits (ts),
    iSerialTermiosParity (ts),
    iSerialTermiosStopBits (ts),
    iSerialTermiosFlow (ts));
  return str;
}

// -----------------------------------------------------------------------------
int
iSerialSpeedToInt (speed_t speed) {
  int baud;

  switch (speed) {
    case B0:
      baud = 0;
      break;
    case B50:
      baud = 50;
      break;
    case B75:
      baud = 75;
      break;
    case B110:
      baud = 110;
      break;
    case B134:
      baud = 134;
      break;
    case B150:
      baud = 150;
      break;
    case B200:
      baud = 200;
      break;
    case B300:
      baud = 300;
      break;
    case B600:
      baud = 600;
      break;
    case B1200:
      baud = 1200;
      break;
    case B1800:
      baud = 1800;
      break;
    case B2400:
      baud = 2400;
      break;
    case B4800:
      baud = 4800;
      break;
    case B9600:
      baud = 9600;
      break;
    case B19200:
      baud = 19200;
      break;
    case B38400:
      baud = 38400;
      break;
    case   B57600:
      baud = 57600;
      break;
    case  B115200:
      baud = 115200;
      break;
    case  B230400:
      baud = 230400;
      break;
    case  B460800:
      baud = 460800;
      break;
    case  B500000:
      baud = 500000;
      break;
    case  B576000:
      baud = 576000;
      break;
    case  B921600:
      baud = 921600;
      break;
    case B1000000:
      baud = 1000000;
      break;
    case B1152000:
      baud = 1152000;
      break;
    case B1500000:
      baud = 1500000;
      break;
    case B2000000:
      baud = 2000000;
      break;
    case B2500000:
      baud = 2500000;
      break;
    case B3000000:
      baud = 3000000;
      break;
    case B3500000:
      baud = 3500000;
      break;
    case B4000000:
      baud = 4000000;
      break;
    default:
      baud = -1;
      break;
  }
  return baud;
}

// -----------------------------------------------------------------------------
speed_t
eSerialIntToSpeed (int baud) {
  speed_t speed;

  switch (baud) {
    case 0:
      speed = B0;
      break;
    case 50:
      speed = B50;
      break;
    case 75:
      speed = B75;
      break;
    case 110:
      speed = B110;
      break;
    case 134:
      speed = B134;
      break;
    case 150:
      speed = B150;
      break;
    case 200:
      speed = B200;
      break;
    case 300:
      speed = B300;
      break;
    case 600:
      speed = B600;
      break;
    case 1200:
      speed = B1200;
      break;
    case 1800:
      speed = B1800;
      break;
    case 2400:
      speed = B2400;
      break;
    case 4800:
      speed = B4800;
      break;
    case 9600:
      speed = B9600;
      break;
    case 19200:
      speed = B19200;
      break;
    case 38400:
      speed = B38400;
      break;
    case 57600:
      speed = B57600;
      break;
    case 115200:
      speed = B115200;
      break;
    case 230400:
      speed = B230400;
      break;
    case 460800:
      speed = B460800;
      break;
    case 500000:
      speed = B500000;
      break;
    case 576000:
      speed = B576000;
      break;
    case 921600:
      speed = B921600;
      break;
    case 1000000:
      speed = B1000000;
      break;
    case 1152000:
      speed = B1152000;
      break;
    case 1500000:
      speed = B1500000;
      break;
    case 2000000:
      speed = B2000000;
      break;
    case 2500000:
      speed = B2500000;
      break;
    case 3000000:
      speed = B3000000;
      break;
    case 3500000:
      speed = B3500000;
      break;
    case 4000000:
      speed = B4000000;
      break;
    default:
      speed = -1;
      break;
  }
  return speed;
}

// -----------------------------------------------------------------------------
const char *
sSerialFlowToStr (eSerialFlow eFlow) {

  switch (eFlow) {

    case SERIAL_FLOW_NONE:
      return "None";
    case SERIAL_FLOW_RTSCTS:
      return "Hardware";
    case SERIAL_FLOW_XONXOFF:
      return "Software";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialParityToStr (eSerialParity eParity) {

  switch (eParity) {

    case SERIAL_PARITY_NONE:
      return "None";
    case SERIAL_PARITY_EVEN:
      return "Even";
    case SERIAL_PARITY_ODD:
      return "Odd";
    case SERIAL_PARITY_SPACE:
      return "Space";
    case SERIAL_PARITY_MARK:
      return "Mark";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialDataBitsToStr (eSerialDataBits eDataBits) {

  switch (eDataBits) {

    case SERIAL_DATABIT_5:
      return "5";
    case SERIAL_DATABIT_6:
      return "6";
    case SERIAL_DATABIT_7:
      return "7";
    case SERIAL_DATABIT_8:
      return "8";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialStopBitsToStr (eSerialStopBits eStopBits) {

  switch (eStopBits) {

    case SERIAL_STOPBIT_ONE:
      return "1";
    case SERIAL_STOPBIT_TWO:
      return "2";
    case SERIAL_STOPBIT_ONEHALF:
      return "1.5";
    default:
      break;
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sSerialGetFlowStr (int fd) {

  return sSerialFlowToStr (eSerialGetFlow (fd));
}

/* ========================================================================== */
