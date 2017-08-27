/**
 * @file
 * @brief Test fonctions de base RFM69
 *
 * -----------------------------<<WARNING>>-------------------------------------
 * Un module RFM69 est alimenté en 3.3V et n'est donc pas compatible
 * avec ARDUINO sans adaptation de niveau... on utilisera plutôt une carte xplbee !
 * -----------------------------------------------------------------------------
 */
#include <sysio/delay.h>
#include <sysio/rf69.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

/* constants ================================================================ */
#define DIO0_IRQ  25
#define NODE_ID   2
#define NET_ID    1

/* main ===================================================================== */
int
main (void) {
  int t = 1, ret;

  printf ("\n\n*** RFM69 Common test ***\n\n");

  printf ("Test %d - xRf69New(0,0,%d)\n", t++, DIO0_IRQ);
  xRf69 * rf = xRf69New (0, 0, DIO0_IRQ);
  assert (rf);
  printf ("Success\n\n");

  printf ("Test %d - iRf69Open (rf,eRf69Band868Mhz,%d,%d)\n", t++,
          NODE_ID, NET_ID);
  ret = iRf69Open (rf, eRf69Band868Mhz, NODE_ID, NET_ID);
  assert (ret == 0);
  printf ("Success\n\n");

  // NodeId
  printf ("Test %d - iRf69NodeId()\n", t++);
  ret = iRf69NodeId (rf);
  printf ("Node ID: 0x%02X\n", ret);
  assert (ret == NODE_ID);
  printf ("Success\n\n");

  // NetId
  printf ("Test %d - iRf69NetworkId()\n", t++);
  ret = iRf69NetworkId (rf);
  printf ("Network ID: 0x%02X\n", ret);
  assert (ret == NET_ID);
  printf ("Success\n\n");

  // temperature
  printf ("Test %d - iRf69Temperature()\n", t++);
  ret = iRf69Temperature (rf, 0);
  printf ("Temperature: %d oC\n", ret);
  assert (ret != INT_MAX);
  printf ("Success\n\n");

  // frequency
  printf ("Test %d - lRf69Frequency()\n", t++);
  double f = (double) lRf69Frequency (rf);
  printf ("Frequency: %.0f Hz (%.3f MHz)\n", f, f / 1E6);
  assert (ret >= 0);
  printf ("Success\n\n");

  // powerlevel/highpower
  printf ("Test %d - iRf69PowerLevel() & bRf69isHighPower()\n", t++);
  int level = iRf69PowerLevel (rf);
  printf ("Power Level: %d (raw)\n", level);
  assert (level >= 0);
  ret = bRf69isHighPower (rf);
  printf ("High Power mode: %d\n", ret);
  assert (ret >= 0);
  if (ret) {
    level = level / 2 + 5;
  }
  else {
    level -= 18;
  }
  printf ("Power Level: %d dBm\n", level);
  printf ("Success\n\n");

  // rssi
  printf ("Test %d - iRf69Rssi()\n", t++);
  ret = iRf69Rssi (rf, false);
  printf ("RSSI: %i dBm\n", ret);
  assert (ret != INT_MAX);
  printf ("Success\n\n");

  // bRf69isPromiscuous
  printf ("Test %d - Promiscuous\n", t++);
  ret = bRf69isPromiscuous (rf);
  printf ("Promiscuous: %d\n", ret);
  assert (ret == true);
  ret = iRf69SetPromiscuous (rf, false);
  assert (ret == 0);
  ret = bRf69isPromiscuous (rf);
  assert (ret == false);
  ret = iRf69SetPromiscuous (rf, true);
  assert (ret == 0);
  ret = bRf69isPromiscuous (rf);
  assert (ret == true);
  printf ("Success\n\n");

  // iRf69SetEncryptKey
  printf ("Test %d - AES Encrypt\n", t++);
  ret = iRf69isEncrypted (rf);
  printf ("AES Encrypt: %d\n", ret);
  assert (ret == false);
  ret = iRf69SetEncryptKey (rf, "ABCDABCDABCDABCD");
  assert (ret == 0);
  ret = iRf69isEncrypted (rf);
  assert (ret == true);
  ret = iRf69SetEncryptKey (rf, 0);
  assert (ret == 0);
  ret = iRf69isEncrypted (rf);
  assert (ret == false);
  printf ("Success\n\n");

  // iRf69SetEncryptKey
  printf ("Test %d - iRf69RcCalibration()\n", t++);
  ret = iRf69RcCalibration (rf);
  assert (ret == 0);
  printf ("Success\n\n");

  // iRf69Sleep
  printf ("Test %d - iRf69Sleep()\n", t++);
  ret = iRf69Sleep (rf);
  assert (ret == 0);
  printf ("Success\n\n");

  ret = iRf69Close(rf);
  assert (ret == 0);
  vRf69Delete (rf);

  return 0;
}

/* ========================================================================== */
