/*
 * template.c
 * @brief Description de votre programme
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/spi.h>

/* constants ================================================================ */
#define TEST_SPEED  250000
#define TEST_MODE
#define TEST_LSB
#define TEST_BITS
/* macros =================================================================== */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */

static const char *   regs[] = {
  "RegFifo", "RegOpMode", "RegDataModul", "RegBitrateMsb", "RegBitrateLsb",
  "RegFdevMsb", "RegFdevLsb", "RegFrfMsb", "RegFrfMid", "RegFrfLsb",
  "RegOsc1", "RegAfcCtrl", "Reserved0C", "RegListen1", "RegListen2",
  "RegListen3", "RegVersion", "RegPaLevel", "RegPaRamp", "RegOcp",
  "Reserved14", "Reserved15", "Reserved16", "Reserved17", "RegLna",
  "RegRxBw", "RegAfcBw", "RegOokPeak", "RegOokAvg", "RegOokFix",
  "RegAfcFei", "RegAfcMsb", "RegAfcLsb", "RegFeiMsb", "RegFeiLsb",
  "RegRssiConfig", "RegRssiValue", "RegDioMapping1", "RegDioMapping2", "RegIrqFlags1",
  "RegIrqFlags2", "RegRssiThresh", "RegRxTimeout1", "RegRxTimeout2", "RegPreambleMsb",
  "RegPreambleLsb", "RegSyncConfig", "RegSyncValue1", "RegSyncValue2", "RegSyncValue3",
  "RegSyncValue4", "RegSyncValue5", "RegSyncValue6", "RegSyncValue7", "RegSyncValue8",
  "RegPacketConfig1", "RegPayloadLength", "RegNodeAdrs", "RegBroadcastAdrs", "RegAutoModes",
  "RegFifoThresh", "RegPacketConfig2", "RegAesKey1", "RegAesKey2", "RegAesKey3",
  "RegAesKey4", "RegAesKey5", "RegAesKey6", "RegAesKey7", "RegAesKey8",
  "RegAesKey9", "RegAesKey10", "RegAesKey11", "RegAesKey12", "RegAesKey13",
  "RegAesKey14", "RegAesKey15", "RegAesKey16", "RegTemp1", "RegTemp2",
  "RegTestLna", "RegTestPa1", "RegTestPa2", "RegTestDagc", "RegTestAfc",
  NULL // must be last
};

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vPrintConfig (const xSpiIos * c) {

  printf ("\tspeed:\t%u\n\tmode:\t%d\n\tbits per word:\t%d\n\tnumbering:\t%s\n",
          c->speed, c->mode, c->bits, (c->lsb ? "LSB" : "MSB"));
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int t = 1, ret;
  const char * cDev;
  int addr = 0;
  xSpiIos config, old_config, new_config;
  int fd;
  static uint8_t rx_buffer[16];
  static uint8_t tx_buffer[16];

  printf ("SPI test\n");
  if (argc < 2) {

    printf ("Usage: %s spi_device (eg: /dev/spidev0.0)\n", argv[0]);
    exit (EXIT_FAILURE);
  }
  cDev = argv[1];

  printf ("Test %d> ", t++);
  fd = iSpiOpen (cDev);
  assert (fd >= 0);
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  ret = iSpiGetConfig (fd, &old_config);
  assert (ret == 0);
  printf ("Initial config:\n");
  vPrintConfig (&old_config);
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  new_config.speed = old_config.speed / 2;
  new_config.mode = eSpiMode0;
  new_config.bits = eSpiBits8;
  new_config.lsb = eSpiNumberingMsb;
  printf ("New config:\n");
  vPrintConfig (&new_config);
  ret = iSpiSetConfig (fd, &new_config);
  assert (ret == 0);

  ret = iSpiGetConfig (fd, &config);
  assert (ret == 0);
  printf ("Current config:\n");
  vPrintConfig (&config);
  assert ( (config.speed == new_config.speed) && (config.mode == new_config.mode)
           && (config.bits == new_config.bits) && (config.lsb == new_config.lsb));
  printf ("Success\n\n");

  printf ("Test %d>\n#,register,value\n", t++);
  while (regs[addr] != NULL) {

    tx_buffer[0] = addr;
    ret = iSpiXfer (fd, tx_buffer, sizeof (tx_buffer), rx_buffer, sizeof (rx_buffer));
    assert (ret >= 0);
    printf ("0x%02X,%s,0x%02X\n", addr, regs[addr], rx_buffer[1]);
    addr++;
  }
  printf ("Success\n\n");

  printf ("Test %d>\n#,register,value\n", t++);
  addr = 0;
  while (regs[addr] != NULL) {

    ret = iSpiReadReg8 (fd, addr);
    assert (ret >= 0);
    printf ("0x%02X,%s,0x%02X\n", addr, regs[addr], ret);
    addr++;
  }
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  addr = 0;
  while ( (strcmp (regs[addr], "RegBitrateMsb") != 0) && regs[addr] != NULL) {

    addr++;
  }
  if (regs[addr]) {

    ret = iSpiReadReg16 (fd, addr);
    assert (ret >= 0);
    printf ("0x%02X,%s,0x%04X\n", addr, regs[addr], ret);
    assert (ret == 0x1A0B);
  }
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  addr = 0;
  while ( (strcmp (regs[addr], "RegAesKey1") != 0) && regs[addr] != NULL) {

    addr++;
  }
  if (regs[addr]) {
    uint8_t buffer[16];

    ret = iSpiReadRegBlock (fd, addr, buffer, 16);
    assert (ret >= 0);

    printf ("0x%02X,%s,%d,0x", addr, regs[addr], ret);
    for (int i = 0; i < ret; i++) {
      printf ("%02X", buffer[i]);
    }
    putchar ('\n');
  }
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  if (regs[addr]) {

    ret = iSpiWriteReg8 (fd, addr, 0x5A);
    assert (ret == 1);
    ret = iSpiReadReg8 (fd, addr);
    assert (ret >= 0);
    printf ("0x%02X,%s,0x%02X\n", addr, regs[addr], ret);
    assert (ret == 0x5A);
  }
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  if (regs[addr]) {

    ret = iSpiWriteReg16 (fd, addr, 0x5AA5);
    assert (ret == 2);
    ret = iSpiReadReg16 (fd, addr);
    assert (ret >= 0);
    printf ("0x%02X,%s,0x%04X\n", addr, regs[addr], ret);
    assert (ret == 0x5AA5);
  }
  printf ("Success\n\n");

  printf ("Test %d>\n", t++);
  if (regs[addr]) {

    for (int i = 0; i < 16; i++) {
      tx_buffer[i] = i;
    }
    ret = iSpiWriteRegBlock (fd, addr, tx_buffer, 16);
    assert (ret == 16);
    ret = iSpiReadRegBlock (fd, addr, rx_buffer, 16);
    assert (ret == 16);
    printf ("0x%02X,%s,%d,0x", addr, regs[addr], ret);
    for (int i = 0; i < 16; i++) {
      printf ("%02X", rx_buffer[i]);
    }
    putchar ('\n');
    assert (memcmp (tx_buffer, rx_buffer, 16) == 0);
  }
  printf ("Success\n\n");

  ret = iSpiClose (fd);
  assert (ret == 0);
  printf ("\neverything was closed.\nHave a nice day !\n");

  return 0;
}
/* ========================================================================== */
