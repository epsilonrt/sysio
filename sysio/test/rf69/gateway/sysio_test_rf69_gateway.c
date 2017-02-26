/**
 * @file
 * @brief Test de réception avec un module RFM69
 *
 * -----------------------------<<WARNING>>-------------------------------------
 * Un module RFM69 est alimenté en 3.3V et n'est donc pas compatible
 * avec ARDUINO sans adaptation de niveau... on utilisera plutôt une carte xplbee !
 * -----------------------------------------------------------------------------
 */
#include <sysio/delay.h>
#include <sysio/rf69.h>
#include <sysio/term.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* constants ================================================================ */
#define DIO0_IRQ  6 // BCM25 on Raspberry Pi
#define MYNODE_ID 1
#define TARGET_ID 10
#define NET_ID    1

/* private variables ======================================================== */
static xRf69 * rf;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
vSigIntHandler (int sig) {

  raw (false);
  vRf69Delete (rf);
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (void) {
  int ret, c;
  int src, level, net;
  bool bPrintStat = true;
  bool bRxOn = true;
  bool bPromiscuous = true;

  // Affiche le menu
  printf ("\n\n** RFM69 Gateway Test **\n"
          "----------- Menu -----------\n"
          "[space]: start/stop Tx\n"
          "      p: toggle promiscous\n"
          "      w: change power level\n");

  rf = xRf69New (0, 0, DIO0_IRQ);
  assert (rf);

  ret = iRf69Open (rf, eRf69Band868Mhz, MYNODE_ID, NET_ID);
  assert (ret == 0);

  src = iRf69NodeId (rf);
  assert (src >= 0);

  net = iRf69NetworkId (rf);
  assert (net >= 0);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("Press Ctrl+C to abort ...\n");

  for (;;) {

    raw (true); // désactive l'echo et l'attente du ENTER
    if (kbhit() ) { // Touche clavier appuyée ?

      c = getchar(); // lecture touche

      // Commandes utilisateur
      switch (c) {
        case ' ':
          bRxOn = ! bRxOn;
          printf ("Rx %s\n", bRxOn ? "On" : "Off");
          break;

        case 'p':
          bPromiscuous = ! bPromiscuous;
          ret = iRf69SetPromiscuous (rf, bPromiscuous);
          printf ("Promiscuous %s\n", bPromiscuous ? "On" : "Off");
          break;

        case 'w':
          raw (false); // réactive l'echo et l'attente ENTER pour scanf
          do {
            printf ("\nPower level [-18,13]? ");
            scanf ("%d", &ret);
          }
          while ( (ret < -18) || (ret > 13) );
          level = ret + 18;
          ret = iRf69SetPowerLevel (rf, level);
          assert (ret == 0);
          bPrintStat = true;
          break;
        default:
          break;
      }
    }

    if (bPrintStat) {
      // Affiche les infos sur la liaison
      long frf = lRf69Frequency (rf);
      assert (frf >= 0);

      level = iRf69PowerLevel (rf);
      assert (level >= 0);
      ret = bRf69isHighPower (rf);
      assert (ret >= 0);
      if (ret) {

        level = level / 2 + 5;
      }
      else {
        level -= 18;
      }

      bPromiscuous = bRf69isPromiscuous (rf);

      printf ("Frf: %lu kHz - Power Level: %d dBm - Promiscuous: %d\n"
              "Own address: [%d]\n"
              "Listening data on network %d...\n",
              frf / 1000, level, src, bPromiscuous, net);
      bPrintStat = false;
    }

    if (bRxOn) {
      // Réception des paquets

      ret = iRf69ReceiveDone (rf);
      assert (ret >= 0);

      if (ret) {
        int rssi = iRf69Rssi (rf, false);
        assert (rssi != INT_MAX);

        printf ("R[%d]<[%d] ", iRf69TargetId (rf), iRf69SenderId (rf) );
        if (iRf69DataLength (rf) ) {

          printf ("'%s'", sRf69Data (rf) );
        }

        ret = iRf69AckRequested (rf);
        assert (ret >= 0);

        if (ret) {

          ret = iRf69SendAck (rf, 0, 0);
          assert (ret == true);
          printf (" > Ack");
        }
        printf (" [RSSI = %d]\n", rssi);
      }
    }
  }
  return 0;
}

/* ========================================================================== */
