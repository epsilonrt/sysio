/**
 * @file
 * @brief Demo module blyss en émission
 *
 * Demo émission Modules Blyss™
 * 
 * Ce programme permet d'envoyer des commandes sur une liaison LiveEz™ 433.92 MHz
 * à l'aide d'un module d'émission connecté à une broche de GPIO. Par exemple,
 * on pourra utiliser le module SeeedStudio du kit :
 * https://www.seeedstudio.com/433Mhz-RF-link-kit-p-127.html#
 * Le module émetteur est alimenté entre GND et +5V, l'entrée DATA sera reliée
 * à une broche de GPIO (3.3 ou 5V). \n
 * Le programme émule par défaut une télécommande 8 canaux Ref. 582883
 * Le synopsis est le suivant :
 * 
 * @code
 * sysio_demo_blyss_tx channel {on,off,loop} [pin]
 * @endcode
 *
 * - `channel` numéro du canal à commander entre 1 et 8, 0 pour tous les canaux
 * - `{on,off,loop}` commande à effectuer : `on` pour marche, `off` pour arrêt et
 * `loop` pour entrer dans une boucle qui bascule l'état du canal toutes les 5
 * secondes (CTRL+C por arrêter)
 * - `[pin]` numéro de la broche à utiliser, optionnel, par défaut le programme
 * utilise la broche 0 (numérotation logique qui dépend de la carte utilisé, voir
 * les modules Raspberry Pi ou NanoPi pour la numérotation.
 * .
 * Voilà ce que ça donne : \n
 * @code
    sysio_demo_blyss_tx 4 on
    Press Ctrl+C to abort ...
    RF frame          : FE508241098E9
    RF footprint      : FE - Ok
    RF global channel : 5
    RF adress         : 0824
    RF channel        : 4
    Light status      : On
    Rolling code      : 98 - Ok
    Token             : E9

    everything was closed.
    Have a nice day !
 * @endcode
 * Si le programme n'est pas installé à l'aide d'un `sudo make install`, la
 * commande est `sudo ./sysio_demo_blyss_tx 4 on`, en effet l'accès au GPIO
 * est réservé à root.
 * 
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/blyss.h>
#include <sysio/string.h>

/* constants ================================================================ */
#define DEFAULT_OUT_PIN 0

/* private variables ======================================================== */
// pointeur sur l'objet blyss
static xBlyss * b;
// pointeur sur un objet trame
static xBlyssFrame * f;
/*
 * identification de notre émetteur (émulation)
 * le appareils commandés mémorise cet identifiant pendant la phase d'appairage
 * On peut modifier l'id par défaut...
 */
static const uint8_t my_id[] = {
  0x05, 0x08, 0x24 // Télécommande 8 canaux Ref. 582883
  //0x79, 0x5F, 0x78 // Micro émetteur @SkyWodd
  //0x39, 0x21, 0xA8 // Télécommande @SkyWodd
  //0x0E, 0xCB, 0xE8 // Détecteur IR @skywodd
};

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Fonction de fermeture "propre" appelée par CTRL+C ou à la fin du programme
static void
vSigIntHandler (int sig) {

  free (f); // la trame est libérée à l'aide de free()
  // par contre l'objet blyss nécessite un appel à iBlyssDelete
  if (iBlyssDelete (b) != 0) {

    printf ("\niBlyssDelete() failed !\n");
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------
static void
vExitFailure (void) {

  printf ("Usage: %s channel {on,off,loop} [pin]\n", __progname);
  exit (EXIT_FAILURE);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  uint8_t channel;
  long num;
  int out_pin = DEFAULT_OUT_PIN; // out_pin est affectée par défaut
  bool loop = false; // mode boucle -> true
  bool state = true;  // état à transmettre true = ON, par défaut c'est ON

  // Décodage des paramètres de la ligne de commande, le format est le suivant:
  // sysio_demo_blyss_tx channel {on,off,loop} [pin]
  if (argc < 3) { // le nom + 2 paramètres obligatoires

    vExitFailure();
  }
  
  // Décodage du premier paramètre: le numéro de canal
  if (iStrToLong (argv[1], &num, 0) < 0) {

    printf ("Error: channel must be a number !\n");
    vExitFailure();
  }
  
  // Vérification qu'il s'agit d'un numéro valide
  if (!bBlyssChannelIsValid (num)) {

    printf ("Error: illegal channel !\n");
    vExitFailure();
  }

  // canal dans channel
  channel = num;

  // Décodage du 2ième paramètre: la commande {on,off,loop}
  if (strcasecmp (argv[2], "on")) {

    if (strcasecmp (argv[2], "off")) {

      if (strcasecmp (argv[2], "loop")) {

        printf ("Error: illegal command !\n");
        vExitFailure();
      }
      else {

        // "loop"
        loop = true;
      }
    }
    else {

      // "off"
      state = false;
    }
  }

  // 3ième paramètre optionnel, c'est le numéro de broche
  if (argc > 3) {

    if (iStrToLong (argv[3], &num, 0) < 0) {

      printf ("Error: pin must be a number !\n");
      vExitFailure();
    }
    out_pin = num;
  }

  // Création de l'objet xBlyss
  b = xBlyssNew (out_pin, -1);
  assert (b); // vérifies que le pointeur sur l'objet n'est pas nul

  // Création d'une nouvelle trame avec l'identifiant de la télécommande 8 canaux
  f = xBlyssFrameNew (my_id);
  assert (b);

  // modifie la trame pour le canal demandé
  vBlyssFrameSetChannel (f, channel);

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);
  printf ("Press Ctrl+C to abort ...\n");

  do {

    // modifie la trame pour l'état demandé (state -> ON/OFF donc true/false)
    vBlyssFrameSetState (f, state);

    // Envoi de la trame avec 4 répétitions
    if (iBlyssSend (b, f, 4) != 0) {

      // Erreur d'envoi
      printf ("iBlyssSend() failed !\n");
    }
    else {

      // Succès, on affiche la trame envoyée
      vBlyssFramePrint (f);
    }
    
    // Si mode boucle, on attend 5 s. et on bascule l'état

    if (loop) {
      
      delay_ms (5000);
      state = !state;
    }
  }
  while (loop); // si loop=true, on boucle, sinon on quitte

  // on appelle la fonction qui va tout fermer proprement
  vSigIntHandler (SIGQUIT);
  return 0;
}
/* ========================================================================== */
