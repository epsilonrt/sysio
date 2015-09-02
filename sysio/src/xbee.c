/**
 * @file xbee.c
 * @brief XBee module interface functions
 * 
 * Copyright © 2006-2008 Tymm Twillman <tymm@booyaka.com>
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <string.h>
#include <stdlib.h>
#include <util/atomic.h>
#include "xbee_private.h"

/* private functions ======================================================== */

/* -----------------------------------------------------------------------------
 * Génère un numéro de trame différent de zéro
 */
uint8_t
ucXBeeNextFrameId (xXBee *xbee) {
  uint8_t frame_id;

  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {

    if (++xbee->out.frame_id == 0) {

      ++xbee->out.frame_id;
    }
    frame_id = xbee->out.frame_id;
  }
  return frame_id;
}

/* -----------------------------------------------------------------------------
 * Calcule le CRC d'un paquet
 */
uint8_t
ucXBeeCrc (const xXBeePkt *pkt) {
  uint8_t *pkt_data = ( (uint8_t *) pkt) + sizeof (xXBeePktHdr);
  uint16_t i;
  uint8_t crc = 0;

  for (i = 0; i < ntohs ( ( (xXBeePktHdr *) pkt)->len); i++) {

    crc += * (pkt_data++);
  }

  return ~crc;
}

#if 0
// -----------------------------------------------------------------------------
uint32_t
ulXBeePktAddress32 (xXBeePkt *pkt, uint8_t ucOffset) {
  static uint32_t s;
  uint8_t *p = pucXBeePktAddrSrc64 (pkt);

  if (p) {

    memcpy (&s, p + ucOffset, 4);
    vSwapBytes ( (uint8_t *) &s, 4);
  }
  else {

    s = XBEE_SH_UNKNOWN;
  }
  return s;
}
// -----------------------------------------------------------------------------
#endif


/* private functions ======================================================== */

/* -----------------------------------------------------------------------------
 * Queue a packet for transmission
 *
 * needs to queue packet to be sent to XBEE module; e.g. copy the packet to a
 * UART buffer.
 *  On error, -1 should be returned and the packet should NOT be freed.
 *  On success, 0 should be returned; if XBEE_ALLOC is set, this function or
 *   someone downstream is responsible for freeing it -- the packet has been
 *   handed off.  This is to minimize copying of data.
 */
int
iXBeeOut (xXBee *xbee, xXBeePkt *pkt, uint8_t len) {
  int ret = 0;

  while (len) {
    ret = fwrite (&pkt[ret], len, 1, xbee->io_stream);
    len -= ret;
  }
  vXBeeFreePkt (xbee, pkt);

  return ret;
}

/* -----------------------------------------------------------------------------
 * Accept data from an XBee module & build into valid XBEE
 *  packets
 */
void
vXBeeIn (xXBee *xbee, const void *buf, uint8_t len) {
  uint8_t *data = (uint8_t *) buf;

  while (len) {
    switch (xbee->in.bytes_rcvd) {

      case 0:
        while (*data != XBEE_PKT_START) {

          if (!--len) {

            return;
          }
          data++;
        }

        xbee->in.hdr_data[xbee->in.bytes_rcvd++] = *data++;
        if (!--len) {

          return;
        }

        /* Fall thru */

      case 1:
        xbee->in.hdr_data[xbee->in.bytes_rcvd++] = *data++;
        if (!--len) {

          return;
        }

        /* Fall thru */

      case 2:
        xbee->in.hdr_data[xbee->in.bytes_rcvd++] = *data++;

        /* Got enough to get packet length */

        xbee->in.bytes_left = ntohs ( ( (xXBeePktHdr *) xbee->in.hdr_data)->len);

        if (xbee->in.bytes_left > XBEE_MAX_DATA_LEN
            || ( (xbee->in.packet
                  = pvXBeeAllocPkt (xbee, XBEE_RECV, xbee->in.bytes_left + 4)) == NULL)
           ) {

          xbee->in.bytes_left = 0;
          INC_RX_ERROR (xbee);
          continue;
        }

        xbee->in.bytes_left++; /* Extra for crc (alloc_pkt already accounts for it) */

        memcpy (& (xbee->in.packet->hdr), & (xbee->in.hdr_data),
                sizeof (xbee->in.hdr_data));

        if (!--len) {

          return;
        }

        /* Fall thru */

      default
          :
        while (xbee->in.bytes_left--) {

          ( (uint8_t *) xbee->in.packet) [xbee->in.bytes_rcvd++] = *data++;
          if (!--len && xbee->in.bytes_left) {

            return;
          }
        }
    }

    if (ucXBeeCrc (xbee->in.packet)
        != ( (uint8_t *) xbee->in.packet) [xbee->in.bytes_rcvd - 1]) {

      xbee->in.bytes_rcvd = 0;
      INC_RX_CRC_ERROR (xbee);
      continue;
    }

    if (iXBeeRecvPktCB (xbee, xbee->in.packet, xbee->in.bytes_rcvd)) {

      vXBeeFreePkt (xbee, xbee->in.packet);
      INC_RX_DROPPED (xbee);
    }

    xbee->in.bytes_rcvd = 0;
  }
}


/* -----------------------------------------------------------------------------
 * Handle an incoming packet
 *
 * the packet will be fully formed and verified
 * for proper construction before being passed off to this function.  This
 * function should dig into the packet & process based on its contents.
 */
int
iXBeeRecvPktCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len) {
  uint8_t ucCbIndex = XBEE_CB_UNKNOWN;

  switch (ucXBeePktType (pkt)) {

    case XBEE_PKT_TYPE_ATCMD_RESP:
      ucCbIndex = XBEE_CB_AT_LOCAL;
      break;

    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      ucCbIndex = XBEE_CB_AT_REMOTE;
      break;

    case XBEE_PKT_TYPE_MODEM_STATUS:
      ucCbIndex = XBEE_CB_MODEM_STATUS;
      break;

#if SYSIO_XBEE_SERIES == 1
      //--------------------------------------------------------------------------
    case XBEE_PKT_TYPE_TX_STATUS:
      ucCbIndex = XBEE_CB_TX_STATUS;
      break;

    case XBEE_PKT_TYPE_RX16:
    case XBEE_PKT_TYPE_RX64:
      ucCbIndex = XBEE_CB_DATA;
      break;

    case XBEE_PKT_TYPE_RX16_IO:
    case XBEE_PKT_TYPE_RX64_IO:
      ucCbIndex = XBEE_CB_IO;
      break;
      //--------------------------------------------------------------------------

#elif SYSIO_XBEE_SERIES == 2
      //--------------------------------------------------------------------------
    case XBEE_PKT_TYPE_ZB_TX_STATUS:
      ucCbIndex = XBEE_CB_TX_STATUS;
      break;

    case XBEE_PKT_TYPE_ZB_RX:
      ucCbIndex = XBEE_CB_DATA;
      break;

    case XBEE_PKT_TYPE_ZB_RX_IO:
      ucCbIndex = XBEE_CB_IO;
      break;

    case XBEE_PKT_TYPE_ZB_RX_SENSOR:
      ucCbIndex = XBEE_CB_SENSOR;
      break;
      //--------------------------------------------------------------------------
#endif

    default:
      return -1;
  }

  if (xbee->in.user_cb[ucCbIndex] != 0) {

    return xbee->in.user_cb[ucCbIndex] (xbee, pkt, len);
  }
  return -1; /* pkt was dropped */
}

/* internal public functions ================================================ */

/* -----------------------------------------------------------------------------
 * Initialize this package
 */
void vXBeeInit (xXBee * xbee, FILE * io_stream) {

  memset (xbee, 0, sizeof (xXBee));
  xbee->io_stream = io_stream;
}

/* -----------------------------------------------------------------------------
 */
void
vXBeeSetCB (xXBee * xbee, eXBeeCbType cb_type, iXBeeRxCB cb) {

  if ( (cb_type >= XBEE_CB_AT_LOCAL) && (cb_type <= XBEE_CB_SENSOR)) {

    xbee->in.user_cb[cb_type] = cb;
  }
}

/* -----------------------------------------------------------------------------
 * Send a command to an XBee module
 */
int
iXBeeSendAt (xXBee * xbee,
             const char cmd[],
             const uint8_t * params,
             uint8_t param_len) {
  xXBeeAtCmdPkt *pkt;
  uint8_t frame_id;
  int ret;


  pkt = (xXBeeAtCmdPkt *) pvXBeeAllocPkt (xbee, XBEE_XMIT, param_len + 8);
  if (pkt == NULL) {

    INC_TX_ERROR (xbee);
    return -ENOMEM;
  }

  XBEE_HDR_INIT (pkt->hdr, param_len + 4);

  pkt->type = XBEE_PKT_TYPE_ATCMD;

  frame_id = ucXBeeNextFrameId (xbee);

  pkt->frame_id = frame_id;

  pkt->command[0] = cmd[0];
  pkt->command[1] = cmd[1];

  if ( (params) && (param_len)) {

    memcpy (pkt->param, params, param_len);
  }
  pkt->param[param_len] = ucXBeeCrc ( (xXBeePkt *) pkt);

  ret = iXBeeOut (xbee, (xXBeePkt *) pkt,
                  sizeof (xXBeeAtCmdPkt) + param_len + 1);

  if (ret >= 0) {

    return frame_id;
  }

  vXBeeFreePkt (xbee, (xXBeePkt *) pkt);

  INC_TX_ERROR (xbee);

  return ret;
}


/* -----------------------------------------------------------------------------
 * Send a command to a remote XBee module
 */
int
iXBeeSendRemoteAt (xXBee * xbee,
                   const char cmd[],
                   const uint8_t params[],
                   uint8_t param_len,
                   const uint8_t addr64[8],
                   const uint8_t addr16[2],
                   uint8_t apply) {
  xXBeeRemoteAtCmdPkt *pkt;
  uint8_t frame_id;
  int ret;


  pkt = (xXBeeRemoteAtCmdPkt *) pvXBeeAllocPkt (xbee, XBEE_XMIT, param_len + 19);
  if (pkt == NULL) {

    INC_TX_ERROR (xbee);
    return -ENOMEM;
  }

  XBEE_HDR_INIT (pkt->hdr, param_len + 15);

  pkt->type = XBEE_PKT_TYPE_REMOTE_ATCMD;

  frame_id = ucXBeeNextFrameId (xbee);
  pkt->frame_id = frame_id;

  memcpy (pkt->dest64, addr64, 8);
  memcpy (pkt->dest16, addr16, 2);

  pkt->apply = apply ? 2 : 0;

  pkt->command[0] = cmd[0];
  pkt->command[1] = cmd[1];

  memcpy (pkt->param, params, param_len);
  pkt->param[param_len] = ucXBeeCrc ( (xXBeePkt *) pkt);

  ret = iXBeeOut (xbee, (xXBeePkt *) pkt,
                  sizeof (xXBeeRemoteAtCmdPkt) + param_len + 1);

  if (ret == 0) {

    return frame_id;
  }

  vXBeeFreePkt (xbee, (xXBeePkt *) pkt);
  INC_TX_ERROR (xbee);
  return ret;
}

/* -----------------------------------------------------------------------------
 * Poll the inputstream to read incoming bytes
 */
int
iXBeePoll (xXBee * xbee) {
  uint8_t buf[256];
  int len;

  clearerr (xbee->io_stream);
  len = fread (buf, 256, 1, xbee->io_stream);
  if (ferror (xbee->io_stream)) {

    return -1;
  }
  if (len) {

    vXBeeIn (xbee, buf, len);
  }
  return 0;
}

// -----------------------------------------------------------------------------
bool
bXBeePktAddressIsEqual (const uint8_t *a1, const uint8_t *a2, uint8_t len) {

  return memcmp (a1, a2, len) == 0;
}

// -----------------------------------------------------------------------------
const uint8_t *
pucXBeeAddr16Unknown (void) {
  static const uint8_t ucAddr16Unknown[] = { 0xFF, 0xFE };

  return ucAddr16Unknown;
}

// -----------------------------------------------------------------------------
const uint8_t *
pucXBeeAddr64Unknown (void) {
  static const uint8_t ucAddr64Unknown[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  return ucAddr64Unknown;
}

// -----------------------------------------------------------------------------
const uint8_t *
pucXBeeAddr64Coordinator (void) {
  static const uint8_t ucAddr64Coordinator[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  return ucAddr64Coordinator;
}

// -----------------------------------------------------------------------------
const uint8_t *
pucXBeeAddr64Broadcast (void) {
  static const uint8_t ucAddr64Broadcast[] = { 0, 0, 0, 0, 0, 0, 0xFF, 0xFF };

  return ucAddr64Broadcast;
}

// -----------------------------------------------------------------------------
uint8_t *
pucXBeePktAddrSrc64 (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {
#if SYSIO_XBEE_SERIES == 1
    case XBEE_PKT_TYPE_RX64:
      return ( (xXBeeRx64Pkt *) pkt)->src;
    case XBEE_PKT_TYPE_RX64_IO:
      return ( (xXBeeRxIo64Pkt *) pkt)->src;
#elif SYSIO_XBEE_SERIES == 2
    case XBEE_PKT_TYPE_ZB_RX:
      return ( (xXBeeZbRxPkt *) pkt)->src64;
    case XBEE_PKT_TYPE_ZB_RX_IO:
      return ( (xXBeeZbRxIoPkt *) pkt)->src64;
    case XBEE_PKT_TYPE_ZB_RX_SENSOR:
      return ( (xXBeeZbRxSensorPkt *) pkt)->src64;
#endif
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return ( (xXBeeRemoteAtCmdRespPkt *) pkt)->src64;
    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
uint8_t *
pucXBeePktAddrSrc16 (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {
#if SYSIO_XBEE_SERIES == 1
    case XBEE_PKT_TYPE_RX16:
      return ( (xXBeeRx16Pkt *) pkt)->src;
    case XBEE_PKT_TYPE_RX16_IO:
      return ( (xXBeeRxIo16Pkt *) pkt)->src;
#elif SYSIO_XBEE_SERIES == 2
    case XBEE_PKT_TYPE_ZB_RX:
      return ( (xXBeeZbRxPkt *) pkt)->src16;
    case XBEE_PKT_TYPE_ZB_RX_IO:
      return ( (xXBeeZbRxIoPkt *) pkt)->src16;
    case XBEE_PKT_TYPE_ZB_RX_SENSOR:
      return ( (xXBeeZbRxSensorPkt *) pkt)->src16;
#endif
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return ( (xXBeeRemoteAtCmdRespPkt *) pkt)->src16;
    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
uint8_t *
pucXBeePktData (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {
#if SYSIO_XBEE_SERIES == 1
    case XBEE_PKT_TYPE_RX64:
      return ( (xXBeeRx64Pkt *) pkt)->data;
    case XBEE_PKT_TYPE_RX64_IO:
      return ( (xXBeeRxIo64Pkt *) pkt)->data;
    case XBEE_PKT_TYPE_RX16:
      return ( (xXBeeRx16Pkt *) pkt)->data;
    case XBEE_PKT_TYPE_RX16_IO:
      return ( (xXBeeRxIo16Pkt *) pkt)->data;
#elif SYSIO_XBEE_SERIES == 2
    case XBEE_PKT_TYPE_ZB_RX:
      return ( (xXBeeZbRxPkt *) pkt)->data;
    case XBEE_PKT_TYPE_ZB_RX_IO:
      return (uint8_t *) ( (xXBeeZbRxIoPkt *) pkt)->data;
#endif
    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iXBeePktDataLen (xXBeePkt *pkt) {
  int len = usXBeePktLength (pkt) + 3;

  switch (ucXBeePktType (pkt)) {
#if SYSIO_XBEE_SERIES == 1
    case XBEE_PKT_TYPE_RX64:
      len -= sizeof (xXBeeRx64Pkt);
      break;
    case XBEE_PKT_TYPE_RX64_IO:
      len -= sizeof (xXBeeRxIo64Pkt);
      break;
    case XBEE_PKT_TYPE_RX16:
      len -= sizeof (xXBeeRx16Pkt);
      break;
    case XBEE_PKT_TYPE_RX16_IO:
      len -= sizeof (xXBeeRxIo16Pkt);
      break;
#elif SYSIO_XBEE_SERIES == 2
    case XBEE_PKT_TYPE_ZB_RX:
      len -= sizeof (xXBeeZbRxPkt);
      break;
    case XBEE_PKT_TYPE_ZB_RX_IO:
      len -= sizeof (xXBeeZbRxIoPkt);
      break;
#endif
    default:
      return -1;
  }

  return  len;
}

// -----------------------------------------------------------------------------
uint8_t
ucXBeePktType (xXBeePkt *pkt) {

  return pkt->type;
}

// -----------------------------------------------------------------------------
uint16_t
usXBeePktLength (xXBeePkt *pkt) {

  return ntohs (pkt->hdr.len);
}

// -----------------------------------------------------------------------------
int
iXBeePktFrameId (xXBeePkt *pkt) {


  switch (ucXBeePktType (pkt)) {

      /* XBEE_PKT_TYPE_ATCMD 0x08: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD:
      return ( (xXBeeAtCmdPkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD_RESP:
      return ( (xXBeeAtCmdRespPkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD:
      return ( (xXBeeRemoteAtCmdPkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return ( (xXBeeRemoteAtCmdRespPkt *) pkt)->frame_id;

#if SYSIO_XBEE_SERIES == 1
      /* XBEE_PKT_TYPE_TX64 0x00: S1 Series */
    case XBEE_PKT_TYPE_TX64:
      return ( (xXBeeTxReq64Pkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_TX16 0x01: S1 Series */
    case XBEE_PKT_TYPE_TX16:
      return ( (xXBeeTxReq16Pkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_TX_STATUS 0x89: S1 Series */
    case XBEE_PKT_TYPE_TX_STATUS:
      return ( (xXBeeTxStatusPkt *) pkt)->frame_id;

#elif SYSIO_XBEE_SERIES == 2
      /* XBEE_PKT_TYPE_ZB_TX_REQ 0x10: S2 Series */
    case XBEE_PKT_TYPE_ZB_TX_REQ:
      return ( (xXBeeZbTxReqPkt *) pkt)->frame_id;

      /* XBEE_PKT_TYPE_ZB_TX_STATUS 0x8B: S2 Series */
    case XBEE_PKT_TYPE_ZB_TX_STATUS:
      return ( (xXBeeZbTxStatusPkt *) pkt)->frame_id;
#endif

    default:
      break;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iXBeePktStatus (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {

      /* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD_RESP:
      return ( (xXBeeAtCmdRespPkt *) pkt)->status;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return ( (xXBeeRemoteAtCmdRespPkt *) pkt)->status;

      /* XBEE_PKT_TYPE_MODEM_STATUS 0x8A: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_MODEM_STATUS:
      return ( (xXBeeModemStatusPkt *) pkt)->status;

#if SYSIO_XBEE_SERIES == 1
      /* XBEE_PKT_TYPE_TX_STATUS 0x89: S1 Series */
    case XBEE_PKT_TYPE_TX_STATUS:
      return ( (xXBeeTxStatusPkt *) pkt)->status;
#elif SYSIO_XBEE_SERIES == 2
      /* XBEE_PKT_TYPE_ZB_TX_STATUS 0x8B: S2 Series */
    case XBEE_PKT_TYPE_ZB_TX_STATUS:
      return ( (xXBeeZbTxStatusPkt *) pkt)->status;
#endif
    default:
      break;
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iXBeePktDiscovery (xXBeePkt *pkt) {

#if SYSIO_XBEE_SERIES == 2
  if (ucXBeePktType (pkt) == XBEE_PKT_TYPE_ZB_TX_STATUS) {

    return ( (xXBeeZbTxStatusPkt *) pkt)->discovery;
  }
#endif
  return -1;
}

// -----------------------------------------------------------------------------
int
iXBeePktRetry (xXBeePkt *pkt) {

#if SYSIO_XBEE_SERIES == 2
  if (ucXBeePktType (pkt) == XBEE_PKT_TYPE_ZB_TX_STATUS) {

    return ( (xXBeeZbTxStatusPkt *) pkt)->retry;
  }
#endif
  return -1;
}

// -----------------------------------------------------------------------------
uint8_t *
iXBeePktDst16 (xXBeePkt *pkt) {

#if SYSIO_XBEE_SERIES == 2
  if (ucXBeePktType (pkt) == XBEE_PKT_TYPE_ZB_TX_STATUS) {

    return ( (xXBeeZbTxStatusPkt *) pkt)->dst16;
  }
#endif
  return 0;
}

// -----------------------------------------------------------------------------
// TODO: non testé
char *
pcXBeePktCommand (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {

      /* XBEE_PKT_TYPE_ATCMD 0x08: S1 & S2 Series -- */
      /* XBEE_PKT_TYPE_QATCMD 0x09: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD:
    case XBEE_PKT_TYPE_QATCMD:
      return (char *) ( (xXBeeAtCmdPkt *) pkt)->command;

      /* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD_RESP:
      return (char *) ( (xXBeeAtCmdRespPkt *) pkt)->command;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD:
      return (char *) ( (xXBeeRemoteAtCmdPkt *) pkt)->command;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return (char *) ( (xXBeeRemoteAtCmdRespPkt *) pkt)->command;

    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// TODO: non testé
int
iXBeePktParamLen (xXBeePkt *pkt) {
  int len = usXBeePktLength (pkt) + 3;

  switch (ucXBeePktType (pkt)) {

      /* XBEE_PKT_TYPE_ATCMD 0x08: S1 & S2 Series -- */
      /* XBEE_PKT_TYPE_QATCMD 0x09: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD:
    case XBEE_PKT_TYPE_QATCMD:
      len -= sizeof (xXBeeAtCmdPkt);
      break;

      /* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD_RESP:
      len -= sizeof (xXBeeAtCmdRespPkt);
      break;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD:
      len -= sizeof (xXBeeRemoteAtCmdPkt);
      break;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      len -= sizeof (xXBeeRemoteAtCmdRespPkt);
      break;

    default:
      return -1;
  }
  return len;
}

// -----------------------------------------------------------------------------
// TODO: non testé
uint8_t *
pucXBeePktParam (xXBeePkt *pkt) {

  switch (ucXBeePktType (pkt)) {

      /* XBEE_PKT_TYPE_ATCMD 0x08: S1 & S2 Series -- */
      /* XBEE_PKT_TYPE_QATCMD 0x09: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD:
    case XBEE_PKT_TYPE_QATCMD:
      return ( (xXBeeAtCmdPkt *) pkt)->param;

      /* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_ATCMD_RESP:
      return ( (xXBeeAtCmdRespPkt *) pkt)->param;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD:
      return ( (xXBeeRemoteAtCmdPkt *) pkt)->param;

      /* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
    case XBEE_PKT_TYPE_REMOTE_ATCMD_RESP:
      return ( (xXBeeRemoteAtCmdRespPkt *) pkt)->param;

    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------
// TODO: non testé
int
iXBeePktRadius (xXBeePkt *pkt) {

#if SYSIO_XBEE_SERIES == 2
  /* XBEE_PKT_TYPE_ZB_TX_REQ 0x10: S2 Series */
  if (ucXBeePktType (pkt) == XBEE_PKT_TYPE_ZB_TX_REQ) {

    return ( (xXBeeZbTxReqPkt *) pkt)->radius;
  }
#endif
  return -1;
}

// -----------------------------------------------------------------------------
// TODO: non testé
int
iXBeePktApply (xXBeePkt * pkt) {

  /* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
  if (ucXBeePktType (pkt) == XBEE_PKT_TYPE_REMOTE_ATCMD) {

    return ( (xXBeeRemoteAtCmdPkt *) pkt)->apply;
  }
  return -1;
}

// -----------------------------------------------------------------------------
// TODO: non testé
int iXBeePktRssi (xXBeePkt *pkt) {

#if SYSIO_XBEE_SERIES == 1
  switch (ucXBeePktType (pkt)) {

      /*  XBEE_PKT_TYPE_RX64 0x80: S1 Series */
    case XBEE_PKT_TYPE_RX64:
      return ( (xXBeeRx64Pkt *) pkt)->rssi;

      /* XBEE_PKT_TYPE_RX16 0x81: S1 Series */
    case XBEE_PKT_TYPE_RX16:
      return ( (xXBeeRx16Pkt *) pkt)->rssi;

      /* XBEE_PKT_TYPE_RX64_IO 0x82: S1 Series */
    case XBEE_PKT_TYPE_RX64_IO:
      return ( (xXBeeRxIo64Pkt *) pkt)->rssi;

      /* XBEE_PKT_TYPE_RX16_IO 0x83: S1 Series */
    case XBEE_PKT_TYPE_RX16_IO:
      return ( (xXBeeRxIo16Pkt *) pkt)->rssi;

    default:
      break;
  }
#endif
  return -1;
}

// -----------------------------------------------------------------------------
// TODO: non testé
int
iXBeePktOptions (xXBeePkt * pkt) {

  switch (ucXBeePktType (pkt)) {
#if SYSIO_XBEE_SERIES == 1
      /* XBEE_PKT_TYPE_TX64 0x00: S1 Series */
    case XBEE_PKT_TYPE_TX64:
      return ( (xXBeeTxReq64Pkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_RX64 0x80: S1 Series */
    case XBEE_PKT_TYPE_RX64:
      return ( (xXBeeRx64Pkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_TX16 0x01: S1 Series */
    case XBEE_PKT_TYPE_TX16:
      return ( (xXBeeTxReq16Pkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_RX16 0x81: S1 Series */
    case XBEE_PKT_TYPE_RX16:
      return ( (xXBeeRx16Pkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_RX64_IO 0x82: S1 Series */
    case XBEE_PKT_TYPE_RX64_IO:
      return ( (xXBeeRxIo64Pkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_RX16_IO 0x83: S1 Series */
    case XBEE_PKT_TYPE_RX16_IO:
      return ( (xXBeeRxIo16Pkt *) pkt)->opt;

#elif SYSIO_XBEE_SERIES == 2
      /* XBEE_PKT_TYPE_ZB_TX_REQ 0x10: S2 Series */
    case XBEE_PKT_TYPE_ZB_TX_REQ:
      return ( (xXBeeZbTxReqPkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_ZB_RX 0x90: S2 Series */
    case XBEE_PKT_TYPE_ZB_RX:
      return ( (xXBeeZbRxPkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_ZB_RX_IO 0x92: S2 Series */
    case XBEE_PKT_TYPE_ZB_RX_IO:
      return ( (xXBeeZbRxIoPkt *) pkt)->opt;
      /* XBEE_PKT_TYPE_ZB_RX_SENSOR 0x94: S2 Series */
    case XBEE_PKT_TYPE_ZB_RX_SENSOR:
      return ( (xXBeeZbRxSensorPkt *) pkt)->opt;
#endif
  }
  return -1;
}

#if 1
// TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO -
// -----------------------------------------------------------------------------
static inline uint16_t sample_len_from_mask (uint16_t mask) {
  uint8_t len = 0;

  // Digital; all fit in 1 sample
  if (mask & 0x1ff) {
    len++;
  }

  // Analog lines 0-4
  if (len & 0x0200) {
    len++;
  }
  if (len & 0x0400) {
    len++;
  }
  if (len & 0x0800) {
    len++;
  }
  if (len & 0x1000) {
    len++;
  }
  if (len & 0x2000) {
    len++;
  }

  return len;
}

/* -----------------------------------------------------------------------------
 * Note: no checking for case where packet is corrupt (says has more samples
 * than really has) if request for higher sample than really contains.
 */
int
iXBeePktDigital (xXBeePkt * pkt, int array[9], unsigned int index) {
  uint16_t mask = 0;
  uint16_t pin_vals = 0;
  uint8_t i;
  uint8_t samp_len;
  uint8_t npins = 0;

  for (i = 0; i < 9; i++) {

    array[i] = -1;
  }

  if (pkt->type == XBEE_PKT_TYPE_RX16_IO) {
    xXBeeRxIo16Pkt *io16pkt = (xXBeeRxIo16Pkt *) pkt;
    mask = ntohs (io16pkt->ch_mask);
    samp_len = sample_len_from_mask (mask);

    if ( (mask & 0x1ff) != 0 && (index < io16pkt->num_samples)) {
      pin_vals = ntohs (io16pkt->data[index * samp_len]);
    }
  }
  else if (pkt->type == XBEE_PKT_TYPE_RX64_IO) {
    xXBeeRxIo64Pkt *io64pkt = (xXBeeRxIo64Pkt *) pkt;
    mask = ntohs (io64pkt->ch_mask);
    samp_len = sample_len_from_mask (mask);

    if ( (mask & 0x1ff) != 0 && (index < io64pkt->num_samples)) {
      pin_vals = ntohs (io64pkt->data[index * samp_len]);
    }
  }
  else {
    // Unusable packet type
    return -1;
  }

  for (i = 0; i < 9; i++) {
    uint16_t b = (1 << i);
    if (mask & b) {
      if (pin_vals & b) {
        array[i] = 1;
      }
      else {
        array[i] = 0;
      }
      npins++;
    }
  }

  return npins;
}


/* -----------------------------------------------------------------------------
 * Note: no checking for case where packet is corrupt (says has more samples
 * than really has) if request for higher sample than really contains.
 */
int
iXBeePktAnalog (xXBeePkt * pkt, int array[6], unsigned int index) {
  uint16_t mask = 0;
  uint8_t pos = 0;
  uint8_t npins = 0;
  uint8_t i;
  uint8_t samp_len;


  for (i = 0; i < 6; i++) {
    array[i] = -1;
  }

  if (pkt->type == XBEE_PKT_TYPE_RX16_IO) {
    xXBeeRxIo16Pkt *io16pkt = (xXBeeRxIo16Pkt *) pkt;
    mask = ntohs (io16pkt->ch_mask);
    samp_len = sample_len_from_mask (mask);

    if (mask & 0x1ff) {
      // Skip digital readings
      pos++;
    }

    for (i = 0; i < 6; i++) {
      if (mask & (0x200 << i)) {
        array[i] = ntohs (io16pkt->data[index * samp_len + pos]);
        pos++;
        npins++;
      }
    }
  }
  else if (pkt->type == XBEE_PKT_TYPE_RX64_IO) {
    xXBeeRxIo64Pkt *io64pkt = (xXBeeRxIo64Pkt *) pkt;
    mask = ntohs (io64pkt->ch_mask);
    samp_len = sample_len_from_mask (mask);

    if (mask & 0x1ff) {
      // Skip digital readings
      pos++;
    }

    for (i = 0; i < 6; i++) {
      if (mask & (0x200 << i)) {
        array[i] = ntohs (io64pkt->data[index * samp_len + pos]);
        pos++;
        npins++;
      }
    }
  }
  else {
    return -1;
  }

  return npins;
}


/* -----------------------------------------------------------------------------
 */
int
iXBeePktSamples (xXBeePkt * pkt) {

  if (pkt->type == XBEE_PKT_TYPE_RX16_IO) {
    return ( (xXBeeRxIo16Pkt *) pkt)->num_samples;
  }
  else if (pkt->type == XBEE_PKT_TYPE_RX64_IO) {
    return ( (xXBeeRxIo16Pkt *) pkt)->num_samples;
  }

  return -1;
}

// -----------------------------------------------------------------------------
// TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO -
#endif

/* default public functions ================================================= */

/* -----------------------------------------------------------------------------
 * @brief Alloue la mémoire pour un paquet
 *
 * Cette fonction est utilisée par la bibliothèque pour allouer de la mémoire. \n
 * Le système d'allocation mémoire peut être adapté en fonction des besoins. \n
 * Par défaut, AvrIO utilise malloc() pour l'allocation et ne tiens pas compte de
 * direction. Si l'utilisateur ne souhaite pas utiliser ce mécanisme, il devra
 * réimplémenter cette fonction ainsi que \ref vXBeeFreePkt(). \n
 * Il pourra alors utiliser le paramètre direction pour différencier le
 * mécansime d'allocation.
 *
 * @param xbee pointeur sur le contexte
 * @param direction permet de diférencier le mécanisme d'allocation (inutilisé par défaut)
 * @param len taille en octet du buffer demandé
 * @return pointeur sur le buffer alloué
 */
void *
pvXBeeAllocPkt (xXBee * xbee, uint8_t direction, uint8_t len) {

  return malloc (len);
}

/* -----------------------------------------------------------------------------
 * @brief Libère un paquet alloué avec pvXBeeAllocPkt()
 *
 * Par défaut, AvrIO utilise free(). Si l'utilisateur ne souhaite pas utiliser
 * ce mécanisme, il devra réimplémenter cette fonction ainsi
 * que \ref pvXBeeAllocPkt().
 */
void
vXBeeFreePkt (xXBee * xbee, xXBeePkt * pkt) {

  free (pkt);
}

/* ========================================================================== */
