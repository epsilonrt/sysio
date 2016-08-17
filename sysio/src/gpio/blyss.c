/**
 * @file
 * @brief
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdlib.h>
#include <sysio/blyss.h>
#include <sysio/doutput.h>
#include <sysio/dinput.h>
#include <sysio/delay.h>

/* macros =================================================================== */

/* constants ================================================================ */
#define BLYSS_FLAG 0xFE
#define BLYSS_NOF_BITS 52
#define BLYSS_IDX_FLAG  0
#define BLYSS_IDX_GCHAN 8
#define BLYSS_IDX_ADDR  12
#define BLYSS_IDX_CHAN  28
#define BLYSS_IDX_STATE 32
#define BLYSS_IDX_ROLL  36
#define BLYSS_IDX_TOKEN 44

/* timing setup ============================================================= */
#define HEADER_TIME 2400  // us
#define T1          400   // us
#define T2          800   // us
#define FOOTER_TIME 24    // ms

/* private constants ======================================================== */
static const char error[] = "Error";
static const char ok[] = "Ok";
static const uint8_t barrel[] = { 0x98, 0xDA, 0x1E, 0xE6, 0x67 };
static const uint8_t channels[] = { 0, 8, 4, 2, 1, 3, 5, 6, 7 };

/* structures =============================================================== */
struct xBlyss {
  xDoutPort * out;
  xDinPort  * in;
  uint8_t last_token;
  uint8_t roll_idx;
};

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static inline int
iSetOut (xBlyss * b) {

  return iDoutSet (0, b->out);
}

// -----------------------------------------------------------------------------
static inline int
iClearOut (xBlyss * b) {

  return iDoutClear (0, b->out);
}

// -----------------------------------------------------------------------------
static int
iSendBit (xBlyss * b, bool bit) {

  if (iClearOut (b) == 0) {

    delay_us (bit ? T1 : T2);

    if (iSetOut (b) == 0) {

      delay_us (bit ? T2 : T1);
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
static inline int
iSendHeader (xBlyss * b) {

  if (iSetOut (b) == 0) {

    delay_us (HEADER_TIME);
    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
static inline int
iSendFooter (xBlyss * b) {

  if (iClearOut (b) == 0) {

    delay_ms (FOOTER_TIME);
    return 0;
  }
  return -1;
}

// -----------------------------------------------------------------------------
static inline int
iSendFrame (xBlyss * b, xBlyssFrame * f) {
  uint8_t byte;
  uint8_t mask = 0;

  if (iSendHeader (b)) {

    return -1;
  }

  for (uint8_t i = 0; i < BLYSS_NOF_BITS; i++) {

    if (mask == 0) {

      mask = 0x80;
      byte = f->raw[i / 8];
    }

    if (iSendBit (b, byte & mask)) {

      return -1;
    }
    mask >>= 1;
  }

  return iSendFooter (b);
}


// -----------------------------------------------------------------------------
static void
vSetBits (xBlyssFrame * f, uint8_t index, uint8_t len, uint16_t value) {
  uint8_t i = index / 8;
  uint8_t mask, ls;

  for (uint8_t c = 0; c < len; c += 4, index += 4, i++) {

    if ( (index % 8) == 0) {

      // index multiple de 8 (MSB)
      if ( (len - c) >= 8) {

        // copie un octet
        f->raw[i] = value >> c;
        c += 4;
        index += 4;
        continue;
      }

      // quartet MSB
      mask = 0x0F;
      ls = 4;
    }
    else {

      // quartet LSB
      mask = 0xF0;
      ls = 0;
    }

    // copie quartet
    f->raw[i] = (f->raw[i] & mask) | ( (value >> ( (len - c) - 4)) << ls);
  }
}

// -----------------------------------------------------------------------------
static uint16_t
usGetBits (const xBlyssFrame * f, uint8_t index, uint8_t len) {
  uint16_t value = 0;
  uint8_t i = index / 8;
  uint8_t nibble, rest;

  for (uint8_t c = 0; c < len; c += 4, index += 4, i++) {

    rest = (len - c);

    if ( (index != 4) && ( (index % 8) == 0)) {


      // index multiple de 8 (MSB)
      if (rest >= 8) {

        // copie un octet
        value |=  f->raw[i] << (rest - 8);
        c += 4;
        index += 4;
        continue;
      }

      // quartet MSB
      nibble = f->raw[i] >> 4;
    }
    else {

      // quartet LSB
      nibble = f->raw[i] & 0x0F;
    }

    // copie quartet
    value |= nibble << (rest - 4);
  }

  return value;
}


// -----------------------------------------------------------------------------
static inline void
vSetFlag (xBlyssFrame * f) {

  vSetBits (f, BLYSS_IDX_FLAG, 8, BLYSS_FLAG);
}

// -----------------------------------------------------------------------------
static inline uint8_t
ucFlag (const xBlyssFrame * f) {

  return usGetBits (f, BLYSS_IDX_FLAG, 8);
}

// -----------------------------------------------------------------------------
static inline uint8_t
ucToken (const xBlyssFrame * f) {

  return usGetBits (f, BLYSS_IDX_TOKEN, 8);
}

// -----------------------------------------------------------------------------
static inline void
vSetToken (xBlyss * b, xBlyssFrame * f) {

  vSetBits (f, BLYSS_IDX_TOKEN, 8, b->last_token);
  b->last_token += 10;
}

// -----------------------------------------------------------------------------
static inline uint8_t
ucRollingCode (const xBlyssFrame * f) {

  return usGetBits (f, BLYSS_IDX_ROLL, 8);
}

// -----------------------------------------------------------------------------
static inline void
vSetRollingCode (xBlyss * b, xBlyssFrame * f) {

  vSetBits (f, BLYSS_IDX_ROLL, 8, barrel[b->roll_idx]);
  if (++b->roll_idx >= sizeof (barrel)) {

    b->roll_idx = 0;
  }
}

// -----------------------------------------------------------------------------
static bool
bIsValidRollingCode (uint8_t code) {

  for (uint8_t i = 0; i < sizeof (barrel); i++) {

    if (barrel[i] == code) {

      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
static uint8_t
ucDecodeChannel (uint8_t field) {
  uint8_t c;

  for (c = 0; c < sizeof (channels); c++) {

    if (field == channels[c]) {

      return c;
    }
  }
  return -1;
}


/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
xBlyss *
xBlyssNew (int iOutPin, int iInPin) {
  xBlyss * b = calloc (1, sizeof (xBlyss));
  assert (b);

  if (iOutPin >= 0) {
    xDout pin = { .num = iOutPin, .act = true };

    b->out = xDoutOpen (&pin, 1);
    if (b->out == NULL) {

      goto xBlyssNewError;
    }
  }

  if (iInPin >= 0) {
    xDin pin = { .num = iInPin, .act = true, .pull = false };

    b->in = xDinOpen (&pin, 1);
    if (b->in == NULL) {

      goto xBlyssNewError;
    }
  }

  b->last_token = 0x7D;

  return b;

xBlyssNewError:
  free (b);
  return NULL;
}

// -----------------------------------------------------------------------------
int
iBlyssDelete (xBlyss * b) {
  int ret = 0;

  if (b->out) {

    ret = iDoutClose (b->out);
  }
  if (b->in) {

    ret = iDinClose (b->in);
  }
  free (b);
  return ret;
}

// -----------------------------------------------------------------------------
int
iBlyssSend (xBlyss * b, xBlyssFrame * f, uint8_t repeat) {

  if (b->out) {
    vSetRollingCode (b, f);
    vSetToken (b, f);

    while (repeat--) {

      if (iSendFrame (b, f)) {

        return -1;
      }
    }
  }

  return 0;
}


// -----------------------------------------------------------------------------
xBlyssFrame *
xBlyssNewFrame (const uint8_t * id) {

  xBlyssFrame * f = calloc (1, sizeof (xBlyssFrame));
  assert (f);

  vSetFlag (f);
  vBlyssSetTxId (f, id);
  return f;
}

// -----------------------------------------------------------------------------
void
vBlyssSetTxId (xBlyssFrame * f, const uint8_t * id) {

  vBlyssSetGlobalChannel (f, id[0]);
  vBlyssSetAddress (f, (id[1] << 8) |  id[2]);
}

// -----------------------------------------------------------------------------
void
vBlyssSetGlobalChannel (xBlyssFrame * f, uint8_t global_channel) {

  vSetBits (f, BLYSS_IDX_GCHAN, 4, global_channel);
}

// -----------------------------------------------------------------------------
void
vBlyssSetAddress (xBlyssFrame * f, uint16_t addr) {

  vSetBits (f, BLYSS_IDX_ADDR, 16, addr);
}

// -----------------------------------------------------------------------------
void
vBlyssSetChannel (xBlyssFrame * f, uint8_t channel) {

  if (channel < (sizeof (channels) - 1)) {

    vSetBits (f, BLYSS_IDX_CHAN, 4, channels[channel]);
  }
}

// -----------------------------------------------------------------------------
void
vBlyssSetState (xBlyssFrame * f, bool state) {

  vSetBits (f, BLYSS_IDX_STATE, 4, ! state);
}

// -----------------------------------------------------------------------------
uint8_t
ucBlyssGlobalChannel (const xBlyssFrame * f) {

  return usGetBits (f, BLYSS_IDX_GCHAN, 4);
}

// -----------------------------------------------------------------------------
uint8_t
ucBlyssChannel (const xBlyssFrame * f) {

  return ucDecodeChannel (usGetBits (f, BLYSS_IDX_CHAN, 4));
}

// -----------------------------------------------------------------------------
uint16_t
usBlyssAddress (const xBlyssFrame * f) {

  return usGetBits (f, BLYSS_IDX_ADDR, 16);
}

// -----------------------------------------------------------------------------
bool
bBlyssState (const xBlyssFrame * f) {

  return ! usGetBits (f, BLYSS_IDX_STATE, 4);
}

// -----------------------------------------------------------------------------
bool
bBlyssIsValidFrame (const void * buffer) {
  const xBlyssFrame * f = buffer;

  if (f->raw[0] == BLYSS_FLAG) {

    return bIsValidRollingCode (ucRollingCode (f));
  }
  return false;
}

// -----------------------------------------------------------------------------
bool 
bBlyssIsValidChannel (uint8_t channel) {
  
  return ucDecodeChannel (channel) != (uint8_t) -1;
}

// -----------------------------------------------------------------------------
void
vBlyssPrintFrameToFile (const xBlyssFrame * f, FILE * out) {
  uint16_t i;

  printf ("RF frame          : ");
  for (i = 0; i < BLYSS_NOF_BITS; i += 4) {

    printf ("%X", usGetBits (f, i, 4));
  }

  i = ucFlag (f);
  printf ("\nRF footprint      : %02X - %s\n", i, i == BLYSS_FLAG ? ok : error);
  printf ("RF global channel : %X\n", ucBlyssGlobalChannel (f));
  printf ("RF adress         : %04X\n", usBlyssAddress (f));

  printf ("RF channel        : %u\n", ucBlyssChannel (f));
  printf ("Light status      : %s\n", bBlyssState (f) ? "On" : "Off");

  i = ucRollingCode (f);
  printf ("Rolling code      : %02X - %s\n", i, bIsValidRollingCode (i) ? ok : error);
  printf ("Token             : %02X\n", ucToken (f));
}

/* ========================================================================== */
