/**
 * @file xbee_private.h
 * Maxstream XBee module private Header
 * 
 * 
 * Copyright © 2006-2008 Tymm Twillman <tymm@booyaka.com>
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_XBEE_PROTOCOL_H_
#define _SYSIO_XBEE_PROTOCOL_H_

#include <sysio/defs.h>
__BEGIN_C_DECLS
/* ========================================================================== */
#include "xbee.h"

#ifdef CONFIG_XBEE_REENTRANT_TX
#error CONFIG_XBEE_REENTRANT_TX requires XBEE_ALLOC to be set!
#endif

#if (SYSIO_XBEE_SERIES != 1) && (SYSIO_XBEE_SERIES != 2)
#error SYSIO_XBEE_SERIES must be specified to 1 or 2
#endif

/* macros =================================================================== */

/* In case we need to serialize access for transmission;
 *  reception is made to always come from one XBee module so
 *  shouldn't need to serialize that.
 */
#ifndef CONFIG_XBEE_REENTRANT_TX
#define LOCK_FRAME_ID(xbee)      do {} while(0)
#define UNLOCK_FRAME_ID(xbee)    do {} while(0)
#endif

/* Error counters can be added later if desired */
#ifndef XBEE_DEBUG
#define INC_RX_CRC_ERROR(xbee) do {} while(0)
#define INC_RX_ERROR(xbee)     do {} while(0)
#define INC_RX_DROPPED(xbee) do {} while(0)
#define INC_TX_ERROR(xbee)     do {} while(0)
#define INC_TX_DROPPED(xbee) do {} while(0)
#else
#define INC_RX_CRC_ERROR(xbee) do { xbee->rx_crc_error++; } while(0)
#define INC_RX_ERROR(xbee)     do { xbee->rx_error++; } while(0)
#define INC_RX_DROPPED(xbee) do { xbee->rx_dropped++; } while(0)
#define INC_TX_ERROR(xbee)     do { xbee->tx_error++; } while(0)
#define INC_TX_DROPPED(xbee) do { xbee->tx_dropped++;} while(0)
#endif

/* constants ================================================================ */
#ifndef ENOMEM
# define ENOMEM 12
#endif

/* p2p  CE=0 (end devices) A1=0 (no end dev assoc) same ID/CH
 * coordinator: CE=1, A2=n (coordinator assoc)
 *   SP= sleep perd ST= time betw sleep (should be same on
 *   coord/noncoord)
 * assoc - coord'd only; comm between modules thru coord'r
 *  PAN's - need coordinator.  A1 allows totally dynamic assoc
 */

/* --- General XBee Definitions --- */

/* "Start of packet" byte; always sent as the first
 *  byte of each packet
 */
#define XBEE_PKT_START 0x7e


/* Maximum packet size; datasheet basically says 100 payload bytes max */
#define XBEE_MAX_DATA_LEN        128


/* --- Bits in packets --- */

/* Communication status bits */

#define XBEE_STATUS_HW_RESET      0x01
#define XBEE_STATUS_WD_RESET      0x02
#define XBEE_STATUS_ASSOC         0x04
#define XBEE_STATUS_DISASSOC      0x08
#define XBEE_STATUS_SYNC_LOST     0x10
#define XBEE_STATUS_COORD_REALIGN 0x20
#define XBEE_STATUS_COORD_RESET   0x40

/* Command status bits */

#define XBEE_CMDSTATUS_OK  0
#define XBEE_CMDSTATUS_ERR 1

/* Transmit options */

#define XBEE_TX_FLAG_NO_ACK 0x01
#define XBEE_TX_FLAG_SEND_BCAST_PAN_ID 0x04

/* Transmit status bits */

#define XBEE_TXSTATUS_SUCCESS  0x00
#define XBEE_TXSTATUS_NO_ACK   0x01
#define XBEE_TXSTATUS_CCA_FAIL 0x02
#define XBEE_TXSTATUS_PURGES   0x03

/* Received options */

#define XBEE_RX_FLAG_ADDR_BCAST 0x02
#define XBEE_RX_FLAG_PAN_BCAST  0x04


/* --- Definitions & macros for library use --- */

/* Initialize an XBee header */
#define XBEE_HDR_INIT(hdr, data_len) \
         ((hdr).start = 0x7e, (hdr).len = htons(data_len))

/* To get the length of the data portion of a received packet */

#define XBEE_RECV_A64_DATA_LEN(pkt) (ntohs(pkt->hdr.len) - 11)
#define XBEE_RECV_A16_DATA_LEN(pkt) (ntohs(pkt->hdr.len) - 5)
#define XBEE_CMD_RESP_PARAM_LEN(pkt) (ntohs(pkt->hdr.len) - 5)

#ifdef XBEE_ALLOC
# define XBEE_ALLOC_PKT(dir, data_len) \
   (xXBeePkt *)XBEE_ALLOC_BUF((dir), (data_len) + sizeof(xXBeePktHdr) + 1)
#endif

/* structures =============================================================== */

/* --- Packet layouts --- */
/* XBEE_PKT_TYPE_ATCMD 0x08: S1 & S2 Series -- */
/* XBEE_PKT_TYPE_QATCMD 0x09: S1 & S2 Series -- */
typedef struct xXBeeAtCmdPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t command[2];
  uint8_t param[0];
} __attribute__ ( (__packed__)) xXBeeAtCmdPkt;

/* XBEE_PKT_TYPE_ATCMD_RESP 0x88: S1 & S2 Series -- */
typedef struct xXBeeAtCmdRespPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t command[2];
  uint8_t status;
  uint8_t param[0];
} __attribute__ ( (__packed__)) xXBeeAtCmdRespPkt;

/* XBEE_PKT_TYPE_REMOTE_ATCMD 0x17: S1 & S2 Series -- */
typedef struct xXBeeRemoteAtCmdPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t dest64[8];
  uint8_t dest16[2];
  uint8_t apply;
  uint8_t command[2];
  uint8_t param[0];
} __attribute__ ( (__packed__)) xXBeeRemoteAtCmdPkt;

/* XBEE_PKT_TYPE_REMOTE_ATCMD_RESP 0x97: S1 & S2 Series -- */
typedef struct xXBeeRemoteAtCmdRespPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t src64[8];
  uint8_t src16[2];
  uint8_t command[2];
  uint8_t status;
  uint8_t param[0];
} __attribute__ ( (__packed__)) xXBeeRemoteAtCmdRespPkt;

/* XBEE_PKT_TYPE_MODEM_STATUS 0x8A: S1 & S2 Series -- */
typedef struct xXBeeModemStatusPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t status;
} __attribute__ ( (__packed__)) xXBeeModemStatusPkt;

/* ========================= S2 Series Only ================================= */

/* XBEE_PKT_TYPE_ZB_TX_REQ 0x10: S2 Series */
typedef struct xXBeeZbTxReqPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t dest64[8];
  uint8_t dest16[2];
  uint8_t radius;
  uint8_t opt;      /* Multicast = bit 3 */
  uint8_t data[0];  /* Up to 72 bytes/pkt */
} __attribute__ ( (__packed__)) xXBeeZbTxReqPkt;

/* XBEE_PKT_TYPE_ZB_RX 0x90: S2 Series */
typedef struct xXBeeZbRxPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src64[8];
  uint8_t src16[2];
  uint8_t opt;
  uint8_t data[0];  /* Up to 72 bytes/pkt */
} __attribute__ ( (__packed__)) xXBeeZbRxPkt;

/* XBEE_PKT_TYPE_ZB_RX_IO 0x92: S2 Series */
typedef struct xXBeeZbRxIoPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src64[8];
  uint8_t src16[2];
  uint8_t opt;
  uint8_t num_samples; /* always 1 */
  uint16_t dch_mask;
  uint8_t ach_mask;
  uint16_t data[0]; /* First sample digital if any digital chan active
                          rest are 16-bit analog rdgs                  */
} __attribute__ ( (__packed__)) xXBeeZbRxIoPkt;

/* XBEE_PKT_TYPE_ZB_RX_SENSOR 0x94: S2 Series */
typedef struct xXBeeZbRxSensorPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src64[8];
  uint8_t src16[2];
  uint8_t opt;
  uint8_t sensor;
  uint16_t values[4];
  uint16_t temp;
} __attribute__ ( (__packed__)) xXBeeZbRxSensorPkt;

/* XBEE_PKT_TYPE_ZB_TX_STATUS 0x8B: S2 Series */
typedef struct xXBeeZbTxStatusPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t dst16[2];
  uint8_t retry;
  uint8_t status; /* 0: Success */
  uint8_t discovery;
} __attribute__ ( (__packed__)) xXBeeZbTxStatusPkt;


/* ========================= S1 Series Only ================================= */

/* XBEE_PKT_TYPE_TX64 0x00: S1 Series */
typedef struct xXBeeTxReq64Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t dest[8];
  uint8_t opt;
  uint8_t data[0];
} __attribute__ ( (__packed__)) xXBeeTxReq64Pkt;

/* XBEE_PKT_TYPE_RX64 0x80: S1 Series */
typedef struct xXBeeRx64Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src[8];
  uint8_t rssi; /* signal strength */
  uint8_t opt;
  uint8_t data[0];
} __attribute__ ( (__packed__)) xXBeeRx64Pkt;

/* XBEE_PKT_TYPE_TX16 0x01: S1 Series */
typedef struct xXBeeTxReq16Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t dest[2];
  uint8_t opt;
  uint8_t data[0];
} __attribute__ ( (__packed__)) xXBeeTxReq16Pkt;

/* XBEE_PKT_TYPE_RX16 0x81: S1 Series */
typedef struct xXBeeRx16Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src[2];
  uint8_t rssi;
  uint8_t opt;
  uint8_t data[0];
} __attribute__ ( (__packed__)) xXBeeRx16Pkt;

/* XBEE_PKT_TYPE_RX64_IO 0x82: S1 Series */
typedef struct xXBeeRxIo64Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src[8];
  uint8_t rssi; /* signal strength */
  uint8_t opt;
  uint8_t num_samples;
  uint16_t ch_mask; /* bits 14-9: a5-a0 bits 8-0: d8-d0 active */
  uint16_t data[0]; /* First sample digital if any digital chan active
                          rest are 16-bit analog rdgs                  */
} __attribute__ ( (__packed__)) xXBeeRxIo64Pkt;

/* XBEE_PKT_TYPE_RX16_IO 0x83: S1 Series */
typedef struct xXBeeRxIo16Pkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t src[2];
  uint8_t rssi;
  uint8_t opt;
  uint8_t num_samples;
  uint16_t ch_mask; /* bits 14-9: a5-a0 bits 8-0: d8-d0 active */
  uint16_t data[0]; /* First sample digital if any digital chan active
                          rest are 16-bit analog rdgs                  */
} __attribute__ ( (__packed__)) xXBeeRxIo16Pkt;

/* XBEE_PKT_TYPE_TX_STATUS 0x89: S1 Series */
typedef struct xXBeeTxStatusPkt {
  xXBeePktHdr  hdr;
  uint8_t type;
  uint8_t frame_id;
  uint8_t status;
} __attribute__ ( (__packed__)) xXBeeTxStatusPkt;

/* private functions ======================================================== */

/*
 * Receive data
 *
 * calling iXBeeRecvPktCB on each packet when it's done assembling; this should
 * be called with raw data from UART, etc.
 * as it comes in.  *** YOU NEED TO CALL THIS ***
 */
void vXBeeIn (xXBee *xbee, const void *data, uint8_t len);


/*
 * Queue a packet for transmission
 *
 * needs to queue packet to be sent to XBEE module; e.g. copy the packet to a
 * UART buffer.
 *  On error, -1 should be returned and the packet should NOT be freed.
 *  On success, 0 should be returned; if XBEE_ALLOC is set, this function or
 *   someone downstream is responsible for freeing it -- the packet has been
 *   handed off.  This is to minimize copying of data.
 */
int iXBeeOut (xXBee *xbee, xXBeePkt *pkt, uint8_t len);

/*
 * Handle an incoming packet
 *
 * the packet will be fully formed and verified
 * for proper construction before being passed off to this function.  This
 * function should dig into the packet & process based on its contents.
 */
int iXBeeRecvPktCB (xXBee *xbee, xXBeePkt *pkt, uint8_t len);

/*
 * Generate & return next 8-bit frame ID
 */
uint8_t ucXBeeNextFrameId (xXBee *xbee);

/*
 * Generate CRC for an XBee packet
 */
uint8_t ucXBeeCrc (const xXBeePkt *pkt);

/* ========================================================================== */
__END_C_DECLS
/* *INDENT-ON* */
#endif /* #ifndef _SYSIO_XBEE_PROTOCOL_H_ ... */

