/**
 * @file
 * @brief Module de transmission UHF RFM69 (Implémentation générique)
 *
 * http://www.hoperf.com/rf_transceiver/modules/RFM69W.html
 *
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <string.h>
#include "rf69_private.h"

/* private variables ======================================================== */
static const struct xRf69Config  xInitConfigPgm[] PROGMEM = {
  {
    /*
     * 0x01
     */
    REG_OPMODE,
    RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY
  },
  {
    /*
     * 0x02
     * no shaping
     */
    REG_DATAMODUL,
    RF_DATAMODUL_DATAMODE_PACKET |
    RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00
  },
  {
    /*
     * 0x03
     * default: 4.8 KBPS
     */
    REG_BITRATEMSB,
    RF_BITRATEMSB_55555
  },
  {
    /*
     * 0x04
     */
    REG_BITRATELSB,
    RF_BITRATELSB_55555
  },
  {
    /*
     * 0x05
     * default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
     */
    REG_FDEVMSB,
    RF_FDEVMSB_50000
  },
  {
    /*
     * 0x06
     */
    REG_FDEVLSB,
    RF_FDEVLSB_50000
  },
  /*
   * 0x11
   * looks like PA1 and PA2 are not implemented on RFM69W, hence the max
   * output power is 13dBm:
   * +17dBm and +20dBm are possible on RFM69HW
   * +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1)**
   * +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
   * +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high
   * power PA settings (** section 3.3.7 in datasheet)
   */
//  {
//    REG_PALEVEL,
//    RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF |
//    RF_PALEVEL_OUTPUTPOWER_11111
//  },
  /*
   * 0x13
   * over current protection (default is 95mA)
   */
//  {
//    REG_OCP,
//    RF_OCP_ON | RF_OCP_TRIM_95
//  },
  {
    /*
     * 0x19
     * (BitRate < 2 * RxBw)
     * RXBW defaults are (RxBw: 10.4KHz) :
     * { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5}
     */
    REG_RXBW,
    RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2
    // for BR-19200: RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3
  },
  {
    /*
     * 0x25
     * DIO0 is the only IRQ we're using
     */
    REG_DIOMAPPING1,
    RF_DIOMAPPING1_DIO0_01
  },
  {
    /*
     * 0x26
     * DIO5 ClkOut disable for power saving
     */
    REG_DIOMAPPING2,
    RF_DIOMAPPING2_CLKOUT_OFF
  },
  {
    /*
     * 0x28
     * writing to this bit ensures that the FIFO & status flags are reset
     */
    REG_IRQFLAGS2,
    RF_IRQFLAGS2_FIFOOVERRUN
  },
  {
    /*
     * 0x29
     * must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
     */
    REG_RSSITHRESH,
    220
  },
  /*
   * 0x2D
   * default 3 preamble bytes 0xAAAAAA
   */
//  {
//    REG_PREAMBLELSB,
//    RF_PREAMBLESIZE_LSB_VALUE
//  },
  {
    /*
     * 0x2E
     */
    REG_SYNCCONFIG,
    RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0
  },
  {
    /*
     * 0x2F
     * attempt to make this compatible with sync1 byte of RFM12B lib
     */
    REG_SYNCVALUE1,
    0x2D
  },
  {
    /*
     * 0x37
     */
    REG_PACKETCONFIG1,
    RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_ON |
    RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF
  },
  {
    /*
     * 0x38
     * in variable length mode: the max frame size, not used in TX
     */
    REG_PAYLOADLENGTH,
    RF_FIFO_SIZE
  },
  {
    /*
     * 0x3A
     */
    REG_BROADCASTADRS,
    RF69_BROADCAST_ADDR
  },
  {
    /*
     * 0x3C
     * TX on FIFO not empty, header size = 4
     */
    REG_FIFOTHRESH,
    RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE
    // RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | (RF69_HEADER_SIZE - 1)
  },
  {
    /*
     * 0x3D
     * RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
     */
    REG_PACKETCONFIG2,
    RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF
    // for BR-19200: RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF
  },
  {
    /*
     * 0x6F
     * run DAGC continuously in RX mode for Fading Margin Improvement,
     * recommended default for AfcLowBetaOn=0
     */
    REG_TESTDAGC,
    RF_DAGC_IMPROVED_LOWBETA0
  },
  {255, 0} // the end
};

static const long iFrfBounds[4][2] = {
  {RF69_FRF_315_MIN, RF69_FRF_315_MAX},
  {RF69_FRF_433_MIN, RF69_FRF_433_MAX},
  {RF69_FRF_868_MIN, RF69_FRF_868_MAX},
  {RF69_FRF_915_MIN, RF69_FRF_915_MAX}
};

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Vérifies que la fréquence demandée est légale
static bool
bIsLicensedFreq (long f) {

  for (int i = 0; i < 4; i++) {

    if ( (f >= iFrfBounds[i][0]) && (f <= iFrfBounds[i][1])) {

      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
static int
iSetFrequencyRegs (xRf69 * rf, long freq) {

  if (bIsLicensedFreq (freq)) {
    TRY_INT_INIT();
    uint8_t b, sr = 16;

    freq /= RF_FSTEP; // divide down by FSTEP to get FRF
    for (uint8_t reg = REG_FRFMSB; reg <= REG_FRFLSB; reg++) {

      b = freq >> sr;
      TRY_INT (iRf69WriteReg (rf, reg, b));
      sr -= 8;
    }
    return 0;
  }
  return -1;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iRf69Open (xRf69 * rf, eRf69Band eBand, uint8_t ucNodeId, uint8_t ucNetId) {

  if (!rf->is_open) {
    TRY_INT_INIT();
    long frf;
    int i = 0, ret;

    TRY_INT (iRf69WriteRegWithCheck (rf, REG_SYNCVALUE1, 0xAA, 50));
    TRY_INT (iRf69WriteRegWithCheck (rf, REG_SYNCVALUE1, 0x55, 50));

    do {

      TRY_INT (ret = iRf69WriteConstElmt (rf, &xInitConfigPgm[i++]));
    }
    while (ret == true);

    frf = (eBand == eRf69Band315Mhz ? RF69_FRF_315_DEF :
           (eBand == eRf69Band433Mhz ? RF69_FRF_433_DEF :
            (eBand == eRf69Band868Mhz ? RF69_FRF_868_DEF :
             RF69_FRF_915_DEF)));

    TRY_INT (iSetFrequencyRegs (rf, frf));

    /* 0x30 NETWORK ID */
    TRY_INT (iRf69WriteReg (rf, REG_SYNCVALUE2, ucNetId));
    /* 0x39 NODE ID */
    TRY_INT (iRf69WriteReg (rf, REG_NODEADRS, ucNodeId));

    TRY_INT (iRf69SetEncryptKey (rf, 0)); /*Ok*/
    TRY_INT (iRf69SetHighPower (rf, false)); /*Ok*/
    TRY_INT (iRf69SetPowerLevel (rf, RF69_DEFAULT_POWER)); /*Ok*/
    TRY_INT (iRf69SetMode (rf, eRf69ModeStandby)); /*Ok*/
    TRY_INT (iRf69SetPromiscuous (rf, true)); /*Ok*/
    TRY_INT (ret = iRf69WaitForReady (rf, 50)); /*Ok*/
    if (ret == true) {

      rf->node_id = ucNodeId;
      rf->is_open = true;
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
int
iRf69Close (xRf69 * rf) {
  // TODO
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69CanSend (xRf69 * rf) {
  TRY_INT_INIT();
  int rssi;

  rssi = iRf69Rssi (rf, false);
  if (rssi == INT_MAX) {

    return -1;
  }

  if ( (rf->mode == eRf69ModeRx) &&
       (rf->hdr.payload_len == 0) &&
       (rssi < RF69_CSMA_MIN_LEVEL_DBM)) {

    /* Nous sommes en mode réception, aucun paquet reçu, pas de réception en cours
     * (if signal stronger than -100dBm is detected assume channel activity
     * on peut passer en mode attente... */
    TRY_INT (iRf69SetMode (rf, eRf69ModeStandby));
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
int
iRf69Send (xRf69 * rf, uint8_t toAddress,
           const void * tx_buffer, uint8_t tx_len, bool bRequestAck) {
  int ret;

  ret = iRf69WaitToSend (rf, RF69_CSMA_TIMEOUT_MS);
  if (ret == true) {

    ret = iRf69SendFrame (rf, toAddress, tx_buffer, tx_len, bRequestAck, false);
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iRf69SendWithRetry (xRf69 * rf, uint8_t toAddress,
                    const void * tx_buffer, uint8_t tx_len,
                    uint8_t retries, int retryWaitTime) {
  int iAckReceived = false;
  xRf69Timer t;

  t = xRf69TimerNew ();

  while ( (retries) && (iAckReceived == false)) {

    iAckReceived = iRf69Send (rf, toAddress, tx_buffer, tx_len, true);
    if (iAckReceived < 0) {

      goto SendWithRetryExit;
    }
    vRf69TimerStart (t, retryWaitTime);
    while (!bRf69TimerTimeout (t) && (iAckReceived == false)) {

      iAckReceived = iRf69AckReceived (rf, toAddress);
      if (iAckReceived < 0) {

        goto SendWithRetryExit;
      }
    }
    retries--;
  }

SendWithRetryExit:
  vRf69TimerDelete (t);
  return iAckReceived;
}

// -----------------------------------------------------------------------------
int
iRf69SendAck (xRf69 * rf, const void * tx_buffer, uint8_t tx_len) {
  uint8_t sender;
  int rssi, ret;

  rf->hdr.ctl &= ~RF69_ACKREQ;
  sender = rf->hdr.sender;
  rssi = rf->rssi;

  ret = iRf69WaitToSend (rf, RF69_CSMA_TIMEOUT_MS);
  if (ret == true) {

    rf->hdr.sender = sender;
    ret = iRf69SendFrame (rf, sender, tx_buffer, tx_len, false, true);
  }
  rf->rssi = rssi;
  return ret;
}

// -----------------------------------------------------------------------------
int
iRf69WaitAckReceived (xRf69 * rf, uint8_t fromNodeId, int timeout) {
  int iAckReceived;
  xRf69Timer t;

  t = xRf69TimerNew ();
  vRf69TimerStart (t, timeout);

  do {

    iAckReceived = iRf69AckReceived (rf, fromNodeId);
  }
  while (!bRf69TimerTimeout (t) && (iAckReceived == false));
  vRf69TimerDelete (t);
  return iAckReceived;
}

// -----------------------------------------------------------------------------
int iRf69AckReceived (xRf69 * rf, uint8_t fromNodeId) {
  int ret;

  ret = iRf69ReceiveDone (rf);
  if (ret == true) {

    return (rf->hdr.sender == fromNodeId || fromNodeId == RF69_BROADCAST_ADDR)
           && (rf->hdr.ctl & RF69_ACK);
  }
  return ret;
}

// -----------------------------------------------------------------------------
int iRf69AckRequested (xRf69 * rf) {

  return (rf->hdr.ctl & RF69_ACKREQ) && (rf->hdr.dest != RF69_BROADCAST_ADDR);
}

// -----------------------------------------------------------------------------
int
iRf69DataLength (xRf69 * rf) {

  return rf->data_len;
}

// -----------------------------------------------------------------------------
const char *
sRf69Data (xRf69 * rf) {

  return (const char *) rf->data;
}

// -----------------------------------------------------------------------------
int
iRf69isEncrypted (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (b = iRf69ReadReg (rf, REG_PACKETCONFIG2));
  return (b & RF_PACKET2_AES_ON) != 0;
}

// -----------------------------------------------------------------------------
int
iRf69SetEncryptKey (xRf69 * rf, const char* key) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (iRf69SetMode (rf, eRf69ModeStandby));
  if (key != 0) {

    if (strlen (key) != 16) {

      return -1;
    }
    TRY_INT (iRf69WriteBlock (rf, REG_AESKEY1, (const uint8_t *) key, 16));
  }

  TRY_INT (b = iRf69ReadReg (rf, REG_PACKETCONFIG2));
  b = (b & 0xFE) | (key ? RF_PACKET2_AES_ON : 0);

  TRY_INT (iRf69WriteReg (rf, REG_PACKETCONFIG2, b));
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69SetNodeId (xRf69 * rf, uint8_t ucNodeId) {
  TRY_INT_INIT();

  TRY_INT (iRf69WriteReg (rf, REG_NODEADRS, ucNodeId));
  rf->node_id = ucNodeId;
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69NodeId (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (b = iRf69ReadReg (rf, REG_NODEADRS));
  return b;
}

// -----------------------------------------------------------------------------
int
iRf69SetNetworkId (xRf69 * rf, uint8_t ucNetId) {
  TRY_INT_INIT();

  TRY_INT (iRf69WriteReg (rf, REG_SYNCVALUE2, ucNetId));
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69NetworkId (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (b = iRf69ReadReg (rf, REG_SYNCVALUE2));
  return b;
}

// -----------------------------------------------------------------------------
long
lRf69Frequency (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;
  long f = 0;
  uint8_t ls = 16;

  for (uint8_t reg = REG_FRFMSB; reg <= REG_FRFLSB; reg++) {

    TRY_INT (b = iRf69ReadReg (rf, reg));
    f += ( (long) b) << ls;
    ls -= 8;
  }

  return RF_FSTEP * f;
}

// -----------------------------------------------------------------------------
int
iRf69SetFrequency (xRf69 * rf, long freq) {
  TRY_INT_INIT();
  uint8_t oldMode = rf->mode;

  if (oldMode == eRf69ModeTx) {

    TRY_INT (iRf69SetMode (rf, eRf69ModeRx));
  }

  TRY_INT (iSetFrequencyRegs (rf, freq));

  if (oldMode == eRf69ModeRx) {

    TRY_INT (iRf69SetMode (rf, eRf69ModeSynth));
  }
  return iRf69SetMode (rf, oldMode);
}

// -----------------------------------------------------------------------------
int
iRf69SetHighPower (xRf69 * rf, bool bOn) {
  TRY_INT_INIT();
  uint8_t palevel;

  TRY_INT (iRf69WriteReg (rf, REG_OCP, bOn ? RF_OCP_OFF : RF_OCP_ON));

  if (bOn) {
    uint8_t b;

    TRY_INT (b = iRf69ReadReg (rf, REG_PALEVEL));
    palevel = (b & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON;
  }
  else {

    palevel = RF_PALEVEL_PA0_ON | rf->power_level;
  }
  TRY_INT (iRf69WriteReg (rf, REG_PALEVEL, palevel));

  rf->is_rfm69hw = bOn;
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69SetPowerLevel (xRf69 * rf, int level) {
  TRY_INT_INIT();
  uint8_t b;

  level = MIN (level, 31);
  if (rf->is_rfm69hw) {

    level /= 2;
  }

  TRY_INT (b = iRf69ReadReg (rf, REG_PALEVEL));
  TRY_INT (iRf69WriteReg (rf, REG_PALEVEL, (b & 0xE0) | level));
  rf->power_level = level;
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69PowerLevel (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (b = iRf69ReadReg (rf, REG_PALEVEL));
  b &= 0x1F;
  if (rf->is_rfm69hw) {

    b *= 2;
  }
  return b;
}

// -----------------------------------------------------------------------------
int
iRf69Temperature (xRf69 * rf, int iCalFactor) {
  TRY_INT_INIT();
  int x;

  TRY_INT_VAL (iRf69SetMode (rf, eRf69ModeStandby), INT_MAX);
  TRY_INT_VAL (iRf69WriteReg (rf, REG_TEMP1, RF_TEMP1_MEAS_START), INT_MAX);

  do {
    TRY_INT_VAL (x = iRf69ReadReg (rf, REG_TEMP1), INT_MAX);
  }
  while ( (x & RF_TEMP1_MEAS_RUNNING));

  TRY_INT_VAL (x = iRf69ReadReg (rf, REG_TEMP2), INT_MAX);

  return -x + 20 + RF69_TEMP_CAL + iCalFactor;
}

// -----------------------------------------------------------------------------
// Utilisé sous interruption
int
iRf69Rssi (xRf69 * rf, bool bForceTrigger) {
  TRY_INT_INIT();
  uint8_t b;

  if (bForceTrigger) {

    TRY_INT_VAL (iRf69WriteReg (rf, REG_RSSICONFIG, RF_RSSI_START), INT_MAX);
    do {

      TRY_INT_VAL (b = iRf69ReadReg (rf, REG_RSSICONFIG), INT_MAX);
    }
    while ( (b & RF_RSSI_DONE) == 0);
  }

  TRY_INT_VAL (b = iRf69ReadReg (rf, REG_RSSIVALUE), INT_MAX);
  return - (b / 2);
}

// -----------------------------------------------------------------------------
int iRf69SetPromiscuous (xRf69 * rf, bool bOn) {

  if (bOn != rf->promiscuous) {
    TRY_INT_INIT();
    uint8_t b;

    TRY_INT (b = iRf69ReadReg (rf, REG_PACKETCONFIG1));
    b = (b & 0xF9) | (bOn ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST);

    TRY_INT (iRf69WriteReg (rf, REG_PACKETCONFIG1, b));
    rf->promiscuous = bOn;
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69SetDioMapping (xRf69 * rf, uint8_t dio, uint8_t map) {

  if ( (rf->is_open) && (dio >= 1) && (dio <= 5) && (dio != 4) &&
       (map >= 0) && (map <= 3)) {
    TRY_INT_INIT();
    uint8_t b;

    if (dio < 5) {
      /*
       * DIO1 5-4
       * DIO2 3-2
       * DIO3 1-0
       */
      uint8_t i = (3 - dio) * 2;

      TRY_INT (b = iRf69ReadReg (rf, REG_DIOMAPPING1));
      b &= ~ (3 << i);
      b |= map << i;
      TRY_INT (iRf69WriteReg (rf, REG_DIOMAPPING1, b));
    }
    else {
      /*
       * DIO5 5-4
       */

      TRY_INT (b = iRf69ReadReg (rf, REG_DIOMAPPING2));
      b &= ~ (3 << 4);
      b |= map << 4;
      TRY_INT (iRf69WriteReg (rf, REG_DIOMAPPING2, b));
    }
    return 0;
  }
  return -1;
}


// -----------------------------------------------------------------------------
bool
bRf69isPromiscuous (xRf69 * rf) {

  return rf->promiscuous;
}

// -----------------------------------------------------------------------------
bool
bRf69isHighPower (xRf69 * rf) {

  return rf->is_rfm69hw;
}

// -----------------------------------------------------------------------------
int
iRf69SenderId (xRf69 * rf) {

  return rf->hdr.sender;
}

// -----------------------------------------------------------------------------
int
iRf69TargetId (xRf69 * rf) {

  return rf->hdr.dest;
}

// -----------------------------------------------------------------------------
int
iRf69Sleep (xRf69 * rf) {

  return iRf69SetMode (rf, eRf69ModeSleep);
}

// -----------------------------------------------------------------------------
int
iRf69RcCalibration (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (iRf69WriteReg (rf, REG_OSC1, RF_OSC1_RCCAL_START));
  do {

    TRY_INT (b = iRf69ReadReg (rf, REG_OSC1));
  }
  while ( (b & RF_OSC1_RCCAL_DONE) == 0);
  return 0;
}

/* protected functions ====================================================== */
// -----------------------------------------------------------------------------
// Utilisé sous interruption
int
iRf69AvoidRxDeadLocks (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  TRY_INT (b = iRf69ReadReg (rf, REG_PACKETCONFIG2));
  b = (b & 0xFB) | RF_PACKET2_RXRESTART;
  TRY_INT (iRf69WriteReg (rf, REG_PACKETCONFIG2, b));
  return 0;
}

// -----------------------------------------------------------------------------
// Utilisé sous interruption
int
iRf69StartReceiving (xRf69 * rf) {
  TRY_INT_INIT();
  uint8_t b;

  rf->data_len = 0;
  rf->hdr.sender = 0;
  rf->hdr.dest = 0;
  rf->hdr.payload_len = 0;
  rf->hdr.ctl = 0;
  rf->rssi = 0;

  TRY_INT (iRf69AvoidRxDeadLocks (rf)); /*Ok*/

  TRY_INT (b = iRf69ReadReg (rf, REG_DIOMAPPING1));
  b |= RF_DIOMAPPING1_DIO0_01;
  TRY_INT (iRf69WriteReg (rf, REG_DIOMAPPING1, b));
  return iRf69SetMode (rf, eRf69ModeRx);
}

// -----------------------------------------------------------------------------
int
iRf69WaitIrq (xRf69 * rf, int timeout) {
  TRY_INT_INIT();
  bool bIrqHigh;
  xRf69Timer t;

  t = xRf69TimerNew ();
  vRf69TimerStart (t, timeout);

  do {

    TRY_INT (bIrqHigh = iRf69ReadIrqPin (rf));
  }
  while ( (bIrqHigh == false) && ! bRf69TimerTimeout (t));

  vRf69TimerDelete (t);
  return bIrqHigh;
}

// -----------------------------------------------------------------------------
int
iRf69WaitForReady (xRf69 * rf, int timeout) {
  TRY_INT_INIT();
  uint8_t b;
  xRf69Timer t;

  t = xRf69TimerNew ();
  vRf69TimerStart (t, timeout);

  do {

    TRY_INT (b = iRf69ReadReg (rf, REG_IRQFLAGS1));
  }
  while ( ( (b & RF_IRQFLAGS1_MODEREADY) == 0) && ! bRf69TimerTimeout (t));

  vRf69TimerDelete (t);
  return (b & RF_IRQFLAGS1_MODEREADY) != 0;
}

// -----------------------------------------------------------------------------
int
iRf69WriteRegWithCheck (xRf69 * rf, uint8_t reg, uint8_t data, int timeout) {
  TRY_INT_INIT();
  uint8_t b;
  xRf69Timer t;

  t = xRf69TimerNew ();
  vRf69TimerStart (t, timeout);

  do {
    TRY_INT (iRf69WriteReg (rf, reg, data));
    TRY_INT (b = iRf69ReadReg (rf, reg));
  }
  while ( (b != data) && ! bRf69TimerTimeout (t));

  vRf69TimerDelete (t);
  return (b == data);
}

// -----------------------------------------------------------------------------
// Utilisé sous interruption
int
iRf69SetHighPowerRegs (xRf69 * rf, bool bOn) {
  TRY_INT_INIT();

  TRY_INT (iRf69WriteReg (rf, REG_TESTPA1, bOn ? 0x5D : 0x55));
  TRY_INT (iRf69WriteReg (rf, REG_TESTPA2, bOn ? 0x7C : 0x70));
  return 0;
}

// -----------------------------------------------------------------------------
// Utilisé sous interruption
int
iRf69SetMode (xRf69 * rf, eRf69Mode eNewMode) {

  if (eNewMode != rf->mode) {
    TRY_INT_INIT();
    uint8_t b;

    TRY_INT (b = iRf69ReadReg (rf, REG_OPMODE));
    b &= 0xE3;

    switch (eNewMode) {

      case eRf69ModeTx:
        TRY_INT (iRf69WriteReg (rf, REG_OPMODE, b | RF_OPMODE_TRANSMITTER));
        if (rf->is_rfm69hw) {

          TRY_INT (iRf69SetHighPowerRegs (rf, true)); /*Ok*/
        }
        break;

      case eRf69ModeRx:
        TRY_INT (iRf69WriteReg (rf, REG_OPMODE, b | RF_OPMODE_RECEIVER));
        if (rf->is_rfm69hw) {

          TRY_INT (iRf69SetHighPowerRegs (rf, false)); /*Ok*/
        }
        break;

      case eRf69ModeSynth:
        TRY_INT (iRf69WriteReg (rf, REG_OPMODE, b | RF_OPMODE_SYNTHESIZER));
        break;

      case eRf69ModeStandby:
        TRY_INT (iRf69WriteReg (rf, REG_OPMODE, b | RF_OPMODE_STANDBY));
        break;

      case eRf69ModeSleep:
        TRY_INT (iRf69WriteReg (rf, REG_OPMODE, b | RF_OPMODE_SLEEP));
        break;

      default:
        return -1;
    }

    if (rf->mode == eRf69ModeSleep) {

      iRf69WaitForReady (rf, 50);
    }

    rf->mode = eNewMode;
  }
  return 0;
}

// -----------------------------------------------------------------------------
int
iRf69SendFrame (xRf69 * rf, uint8_t toAddress,
                const void * tx_buffer, uint8_t tx_len,
                bool bRequestACK, bool bSendAck) {
  TRY_INT_INIT();
  bool bIsOk;

  TRY_INT (iRf69SetMode (rf, eRf69ModeStandby)); /*Ok*/

  TRY_INT (bIsOk = iRf69WaitForReady (rf, 50)); /*Ok*/

  if (bIsOk) {
    uint8_t b;

    TRY_INT (b = iRf69ReadReg (rf, REG_DIOMAPPING1));
    b &= ~RF_DIOMAPPING1_DIO0_01; // RF_DIOMAPPING1_DIO0_00
    TRY_INT (iRf69WriteReg (rf, REG_DIOMAPPING1, b));

    tx_len = MIN (tx_len, RF69_MAX_DATA_LEN);
    rf->data[0] = tx_len + RF69_HEADER_SIZE - 1;
    rf->data[1] = toAddress;
    rf->data[2] = rf->node_id;
    rf->data[3] = bSendAck ? RF69_ACK : RF69_ACKREQ;
    memcpy (&rf->data[4], tx_buffer, tx_len);

    TRY_INT (iRf69WriteBlock (rf, REG_FIFO, rf->data, RF69_HEADER_SIZE + tx_len));
    TRY_INT (iRf69SetMode (rf, eRf69ModeTx)); /*Ok*/
    TRY_INT (bIsOk = iRf69WaitIrq (rf, RF69_TX_TIMEOUT_MS)); /*Ok*/
    TRY_INT (iRf69SetMode (rf, eRf69ModeStandby)); /*Ok*/
  }
  return bIsOk;
}

// -----------------------------------------------------------------------------
int
iRf69WaitToSend (xRf69 * rf, int timeout) {
  TRY_INT_INIT();
  bool bCanSend;
  xRf69Timer t;

  t = xRf69TimerNew ();
  vRf69TimerStart (t, timeout);

  TRY_INT (iRf69AvoidRxDeadLocks (rf)); /*Ok*/

  do {

    TRY_INT (bCanSend = iRf69CanSend (rf)); /*Ok*/
    if (! bCanSend) {

      TRY_INT (iRf69ReceiveDone (rf)); /*Ok*/
    }
  }
  while ( (bCanSend == false) && ! bRf69TimerTimeout (t));

  vRf69TimerDelete (t);
  return bCanSend;
}

// -----------------------------------------------------------------------------
void
vRf69Isr (xRf69 * rf) {

  if (rf->mode == eRf69ModeRx)  {
    TRY_INT_INIT();
    uint8_t irqflags;

    TRY_INT_VOID (irqflags = iRf69ReadReg (rf, REG_IRQFLAGS2));

    if (irqflags & RF_IRQFLAGS2_PAYLOADREADY) {
      int ret;
      TRY_INT_VOID (iRf69SetMode (rf, eRf69ModeStandby)); /*Ok*/

      ret = iRf69ReadReg (rf, REG_FIFO); // payload length
      if (ret >= (RF69_HEADER_SIZE - 1)) {
        rf->hdr.payload_len = ret;

        TRY_INT_VOID (iRf69ReadBlock (rf, REG_FIFO,
                                      (uint8_t *) &rf->hdr.dest,
                                      RF69_HEADER_SIZE - 1));

        if (rf->promiscuous || rf->hdr.dest == rf->node_id ||
            rf->hdr.dest == RF69_BROADCAST_ADDR) {
          int rssi;

          rf->data_len = MIN (rf->hdr.payload_len, RF_FIFO_SIZE) - 3;
          if (rf->data_len > 0) {
            TRY_INT_VOID (iRf69ReadBlock (rf, REG_FIFO, rf->data, rf->data_len));
            if (rf->data_len < RF69_MAX_DATA_LEN) {
              rf->data[rf->data_len] = 0;  // add null at end of string
            }
          }

          TRY_INT_VOID (iRf69SetMode (rf, eRf69ModeRx)); /*Ok*/
          rssi = iRf69Rssi (rf, false);
          if (rssi != INT_MAX) {

            rf->rssi = rssi;
          }
          return;
        }
      }
      rf->hdr.payload_len = 0;
      iRf69StartReceiving (rf); // fifo flush
    }
  }
}

/* ========================================================================== */
