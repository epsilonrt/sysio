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
#define DIO0_IRQ  6     // BCM25 on Raspberry Pi
#define MYNODE_ID 1     // Adresse de notre module
#define TARGET_ID 10    // Adresse du module destinataire de nos messages
#define NET_ID    1     // Numéro de notre réseau

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
  printf ("\n\n** RFM69 Ping Demo **\n"
                  "----------- Menu -----------\n"
                  "[space]: start/stop Tx\n"
                  "      p: toggle promiscous\n"
                  "      t: change target node\n"
                  "      w: change power level\n");

  // Création de l'objet xRf69
  // Les 2 premiers paramètres ne sont pas utilisés sur AVR
  xRf69 * rf = xRf69New (0, 0, DIO0_IRQ);
  assert (rf); // vérifie que le pointeur n'est pas nul

  // Ouverture de la connexion vers le module RFM69
  ret = iRf69Open (rf, eRf69Band868Mhz, MYNODE_ID, NET_ID);
  assert (ret == 0);

  // Lecture de notre identifiant (pour vérification et affichage)
  src = iRf69NodeId (rf);
  assert (src == MYNODE_ID);

  // Lecture de notre  réseau (pour vérification et affichage)
  net = iRf69NetworkId (rf);
  assert (net == NET_ID);

  target = TARGET_ID; // L'adresse de la cible est stockée et pourra être modifiée

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n");

  for (;;) {

    raw (true); // désactive l'echo et l'attente du ENTER
    if (kbhit() ) { // Touche clavier appuyée ?

      c = getchar();
      // Commandes utilisateur
      switch (c) {
        case ' ': // Arrêt / Marche transmission
          bTxOn = ! bTxOn;
          printf ("\nTx %s\n", bTxOn ? "On" : "Off");
          break;

        case 'p': // Basculement mode promiscuité
          bPromiscuous = ! bPromiscuous;
          ret = iRf69SetPromiscuous (rf, bPromiscuous);
          printf ("\nPromiscuous %s\n", bPromiscuous ? "On" : "Off");
          break;

        case 't': // Modification de l'adresse de la cible
          raw (false); // réactive l'echo et l'attente ENTER pour scanf
          do { // demande de saisie
            printf ("\nTarget ID (%d for broadcast)? ", RF69_BROADCAST_ADDR);
            scanf ("%d", &ret);
          }
          while ( (ret < 1) || (ret > 255)); // tant que l'adresse n'est pas correcte
          target = ret; // mémorisation adresse cible
          bPrintStat = true; // demande affichage des caractèristiques de la liaison
          break;

        case 'w': // Modification de la puissance d'émission
          raw (false); // réactive l'echo et l'attente ENTER pour scanf
          do { // demande de saisie
            printf ("\nPower level [-18,13]? ");
            scanf ("%d", &ret);
          }
          while ( (ret < -18) || (ret > 13));// tant que la puissance n'est pas correcte
          level = ret + 18; // puissance relative 0 à 31
          ret = iRf69SetPowerLevel (rf, level); // modification puissance
          assert (ret == 0); // vérification absence d'erreur
          bPrintStat = true; // demande affichage des caractèristiques de la liaison
          break;
        
        default: // tous les autres caractères sont ignorés
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
      printf ("\nFrf: %lu kHz - Power Level: %d dBm - Promiscuous: %d\n"
                      "Own address: [%d]\n"
                      "Transmitting data on network %d to node [%d]...\n",
                frf / 1000, level, bPromiscuous, src, net, target);
      bPrintStat = false; // on désactive l'affichage
    }

    if (bTxOn) {
      // Transmission activée, on envoie...

      // Peut-on émettre ?
      ret = iRf69CanSend (rf);
      assert (ret >= 0);

      if (ret == true) { // oui
        static int count; // compteur de message
        char msg[20]; // buffer de caractères pour cpnstruire le message

        // Construction du message 'Hello XXX' avec XXX = count
        snprintf (msg, sizeof (msg), "Hello %d", count++);
        printf ("\nT[%d]>[%d] '%s' > ", src, target, msg);

        // Envoi du message, on demande un Ack uniquement si la cible n'est pas le broadcast
        ret = iRf69Send (rf, target, msg, strlen (msg), target != RF69_BROADCAST_ADDR);
        assert (ret == true); // vérification trame envoyée

        if (target != RF69_BROADCAST_ADDR) { // si pas broadcast
          int rssi = 0;

          // On attend l'ack de la cible
          ret = iRf69WaitAckReceived (rf, target, 1000);
          if (ret == true) {
            // on récupère le niveau de réception
            rssi = iRf69Rssi (rf, false);
          }
          assert (rssi != INT_MAX);
          // On affiche le résultat
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
          printf (" [RSSI = %d]", rssi);
        }
        delay_ms (1000); // attente nécessaire pour éviter un débordement réseau
      }
    }

    // Réception des paquets hors ACK

    ret = iRf69ReceiveDone (rf); // a-t-on reçu ?
    assert (ret >= 0);
    
    if (ret == true) { // Oui

      // Lecture niveau de réception
      int rssi = iRf69Rssi (rf, false);
      assert (rssi != INT_MAX);

      // Affichage message
      printf ("R[%d]<[%d] ", iRf69TargetId (rf), iRf69SenderId (rf));
      if (iRf69DataLength (rf)) {

        printf ("'%s'", sRf69Data (rf));
      }

      ret = iRf69AckRequested (rf); // Ack demandé ?
      assert (ret >= 0);

      if (ret == true) { // Oui

        // Envoi Ack (message vide, uniquement Ack)
        ret = iRf69SendAck (rf, 0, 0);
        assert (ret == true);
        printf (" > Ack"); //  > Ack [RSSI = -28]
      }
      printf (" [RSSI = %d]\n", rssi);
    }
  }
  return 0;
}


/* ========================================================================== */
