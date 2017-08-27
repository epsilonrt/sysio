/**
 * @file
 * @brief Démo du module LedRgb avec un contrôleur TLC59116
 *
 * Ce programme allume successivement 4 leds RGB connectées à un TLC59116
 * avec les couleurs de l'arc-en-ciel, puis il modifie la luminosité des
 * leds pour chaque couleur et enfin effectue un test de la fonction
 * clignotement couleur par couleur.
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sysio/delay.h>
#include <sysio/ledrgb.h>

/* constants ================================================================ */
#define NOF_LEDS 4

/* private variables ======================================================== */
static xLedRgbDevice * dev;

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
// Fonction excutée lors d'un appui sur CTRL + C
static void
vSigIntHandler (int sig) {
  int ret;

  // Extinction de toutes les leds
  iLedRgbSetGrpMode (dev, LEDRGB_ALL_LEDS, eLedRgbModeOff);

  // Destruction de l'objet LedRgb
  ret = iLedRgbDeleteDevice (dev);
  if (ret != 0) {

    printf ("\niLedRgbDeleteDevice() failed with %d returned value\n", ret);
    exit (EXIT_FAILURE);
  }
  printf ("\neverything was closed.\nHave a nice day !\n");
  exit (EXIT_SUCCESS);
}

/* main ===================================================================== */
int
main (int argc, char **argv) {
  int ret;
  // liste des couleurs utilisées
  uint32_t color_code[7] = {RGB_PURPLE, RGB_INDIGO, RGB_BLUE, RGB_GREEN, RGB_YELLOW, RGB_ORANGE, RGB_RED};
  // noms des couleurs utilisées
  char * color_name[7] = { "violet", "indigo", "bleu", "vert", "jaune", "orange", "rouge" };

  /*
   * Liste des TLC59116 présents
   * Chaque circuit TLC59116 est défini par le bus I²C sur lequel il se trouve
   * (i2c_bus) et par son adresse I²C (c.f. § 10.1.2 du datasheet p. 23).
   * La liste des TLC59116 doit se terminer avec un élément dont le champs
   * i2c_bus vaut NULL.
   * Ici nous avons un seul contrôleur TLC59116 connecté sur le bus i2c-1 à
   * l'adresse TLC59116_ADDR (0, 0, 0, 0), c'est à dire, 110 0000 (0x60 donc)
   */
  xTlc59116Config tlc59116_list[] = {
    {.i2c_bus = "/dev/i2c-1", .i2c_addr = TLC59116_ADDR (0, 0, 0, 0) },
    /* Fin de la liste */ {.i2c_bus = NULL, 0}
  };
  /*
   * Liste des leds RGB
   * Chaque led est définie par 3 broches : red, green, blue.
   * Chaque broche est définie par son numéro de sortie (0 à 15 pour OUT0 à OUT15)
   * et par son numéro de contrôleur TLC59116.
   * Ici nous avons 4 leds, toutes connectées au seul contrôleur présent (0 donc).
   */
  xTlc59116Led setup[NOF_LEDS] = {
    {
      .red = { .out = 0, .ctrl = 0 },
      .green = { .out =  1, .ctrl = 0 },
      .blue = { .out =  2, .ctrl = 0 }
    },
    {
      .red = { .out = 9, .ctrl = 0 },
      .green = { .out = 10, .ctrl = 0 },
      .blue = { .out = 11, .ctrl = 0 }
    },
    {
      .red = { .out = 3, .ctrl = 0 },
      .green = { .out =  4, .ctrl = 0 },
      .blue = { .out =  5, .ctrl = 0 }
    },
    {
      .red = { .out = 6, .ctrl = 0 },
      .green = { .out =  7, .ctrl = 0 },
      .blue = { .out =  8, .ctrl = 0 }
    }
  };

  /*
   * Création et initialisation de l'objet permettant le contrôle des leds
   * Un pointeur sur l'objet créé est renvoyé, NULL si erreur.
   * La présence des contrôleurs TLC59116 est vérifiée et il sont initialisés
   */
  dev = xLedRgbNewDevice (eLedRgbDeviceTlc59116, tlc59116_list);
  if (dev == NULL) {
    printf ("Error: Unable to connect to TLC59116 controllers, check their configuration or hardware !\n");
    exit (EXIT_FAILURE);
  }

  /*
   * Ajout des leds
   * Les leds sont configurés en mode complet (couleur, luminosité, clignotement)
   */
  for (int i = 0; i < NOF_LEDS; i++) {

    ret = iLedRgbAddLed (dev, eLedRgbModeFull, &setup[i]);
    if (ret < 0) {

      printf ("Error:Unable to add led number %d !\n", i);
      exit (EXIT_FAILURE);
    }
  }

  // vSigIntHandler() intercepte le CTRL+C
  signal (SIGINT, vSigIntHandler);

  printf ("TLC59116 Led RGB Demo\nPress Ctrl+C to abort ...\n");
  for (;;) {

    // Allumage succéssif des leds avec les couleurs de l'arc-en-ciel
    for (int color = 0; color < 7; color++) {

      printf ("%s: ", color_name [color]);
      for (int led = 0; led < iLedRgbSize (dev); led++) {

        ret = iLedRgbSetColor (dev, led, color_code[color] );
        if (ret != 0) {

          printf ("Error:Unable to set color for led %d !\n", led);
          exit (EXIT_FAILURE);
        }
        putchar ('.');
        fflush (stdout);
        delay_ms (500);
      }
      putchar ('\n');
    }

    // Allumage graduel de toutes les leds couleur par couleur
    for (int color = 0; color < 7; color++) {

      // Extinction de toutes les leds
      ret = iLedRgbSetDimmer (dev, 0, 0);
      if (ret != 0) {

        printf ("Error:Unable to dimmer for all leds !\n");
        exit (EXIT_FAILURE);
      }
      delay_ms (1000);

      // Allumage de toutes les leds avec la même couleur
      printf ("%s\n", color_name [color]);
      ret = iLedRgbSetGrpColor (dev, LEDRGB_ALL_LEDS, color_code[color]);
      if (ret != 0) {

        printf ("Error:Unable to set color all leds !\n");
        exit (EXIT_FAILURE);
      }

      // Augmentation progressive de la luminosité (0 à 255 pour un TLC59116)
      for (int dim = 0; dim <= 255; dim++) {

        ret = iLedRgbSetDimmer (dev, 0, dim);
        if (ret != 0) {

          printf ("Error:Unable to dimmer for all leds !\n");
          exit (EXIT_FAILURE);
        }
        delay_ms (25);
      }
    }

    // Clignotement de toutes les leds couleur par couleur
    for (int color = 0; color < 7; color++) {

      // Allumage de toutes les leds avec la même couleur
      printf ("%s\n", color_name [color]);
      ret = iLedRgbSetGrpColor (dev, LEDRGB_ALL_LEDS, color_code[color]);
      if (ret != 0) {

        printf ("Error:Unable to set color all leds !\n");
        exit (EXIT_FAILURE);
      }

      // Mode clignotement désactivé, luminosité au max.
      ret = iLedRgbSetDimmer (dev, 0, 255);
      if (ret != 0) {

        printf ("Error:Unable to dimmer for all leds !\n");
        exit (EXIT_FAILURE);
      }

      /*
       * Clignotement avec une période de 100ms et un rapport cyclique de 64/256 (25%)
       * pendant 5 secondes
       */
      ret = iLedRgbSetBlinker (dev, 0, 100, 64);
      if (ret != 0) {

        printf ("Error:Unable to blinker for all leds !\n");
        exit (EXIT_FAILURE);
      }
      delay_ms (5000);

      // Extinction de toutes les leds, clignotement désactivé pendant 1 sec.
      ret = iLedRgbSetDimmer (dev, 0, 0);
      if (ret != 0) {

        printf ("Error:Unable to dimmer for all leds !\n");
        exit (EXIT_FAILURE);
      }
      delay_ms (1000);
    }

    // Dévalide le clignotement et règle luminosité au max
    ret = iLedRgbSetDimmer (dev, 0, 255);
    if (ret != 0) {

      printf ("Error:Unable to dimmer for all leds !\n");
      exit (EXIT_FAILURE);
    }
  }

  return 0;
}
/* ========================================================================== */
