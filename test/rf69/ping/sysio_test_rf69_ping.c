/**
 * @file
 * @brief Test de transmission avec un module RFM69
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
  int src, target, level, net;
  bool bPrintStat = true;
  bool bTxOn = true;
  bool bPromiscuous = true;

  // Affiche le menu
  printf ("\n\n** RFM69 Ping Test **\n"
          "----------- Menu -----------\n"
          "[space]: start/stop Tx\n"
          "      p: toggle promiscous\n"
          "      t: change target node\n"
          "      w: change power level\n");

  rf = xRf69New (0, 0, DIO0_IRQ);
  assert (rf);

  ret = iRf69Open (rf, eRf69Band868Mhz, MYNODE_ID, NET_ID);
  assert (ret == 0);

  src = iRf69NodeId (rf);
  assert (src >= 0);

  net = iRf69NetworkId (rf);
  assert (net >= 0);

  target = TARGET_ID;

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
          bTxOn = ! bTxOn;
          printf ("Tx %s\n", bTxOn ? "On" : "Off");
          break;

        case 'p':
          bPromiscuous = ! bPromiscuous;
          ret = iRf69SetPromiscuous (rf, bPromiscuous);
          printf ("Promiscuous %s\n", bPromiscuous ? "On" : "Off");
          break;

        case 't':
          raw (false); // réactive l'echo et l'attente ENTER pour scanf
          do {
            printf ("\nTarget ID (%d for broadcast)? ", RF69_BROADCAST_ADDR);
            scanf ("%d", &ret);
          }
          while ( (ret < 1) || (ret > 255) );
          target = ret;
          bPrintStat = true;
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
              "Transmitting data on network %d to node [%d]...\n",
              frf / 1000, level, src, bPromiscuous, net, target);
      bPrintStat = false;
    }

    if (bTxOn) {
      // Transmission activée, on envoit...
      ret = iRf69CanSend (rf);
      assert (ret >= 0);
      if (ret) {
        static int count;
        char msg[20];
        snprintf (msg, sizeof (msg), "Hello %d", count++);
        printf ("T[%d]>[%d] '%s' > ", src, target, msg);

        ret = iRf69Send (rf, target, msg, strlen (msg), target != RF69_BROADCAST_ADDR);
        assert (ret == true);

        if (target != RF69_BROADCAST_ADDR) {
          int rssi;

          ret = iRf69WaitAckReceived (rf, target, 1000);
          rssi = iRf69Rssi (rf, false);
          assert (rssi != INT_MAX);
          switch (ret) {
            case false:
              printf ("Timeout");
              break;
            case true:
              printf ("Ack");
              break;
            default:
              printf ("Error %d", ret);
              break;
          }
          printf (" [RSSI = %d]\n", rssi);
        }
        else {
          
          putchar('\n');
        }
        delay_ms (1000);
      }
    }

    // Réception des paquets hors ACK
    ret = iRf69ReceiveDone (rf);
    assert (ret >= 0);
    if (ret) {

      if (iRf69DataLength (rf) ) {

        printf ("R>%s\n", sRf69Data (rf) );
      }

      ret = iRf69AckRequested (rf);
      assert (ret >= 0);
      if (ret) {

        ret = iRf69SendAck (rf, 0, 0);
        assert (ret == true);
      }
    }
  }
  return 0;
}

/* ========================================================================== */
