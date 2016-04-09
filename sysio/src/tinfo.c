/**
 * @file tinfo.c
 * @brief Télé-information client des compteurs utilisés par ERDF (Implémentation)
 *
 * Copyright © 2016 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sysio/serial.h>
#include <sysio/string.h>
#include <sysio/tinfo.h>

/* constants ================================================================ */
#define TINFO_CB_SIZE (TINFO_CB_LAST - TINFO_CB_FIRST + 1)
#define TINFO_BUFFER_SIZE 1024

// Temps d'attente correspondant à l'espace de silence min. entre 2 trames
#define TINFO_TIMEOUT 17

// Caractères ASCII de contrôle
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define HT  0x09
#define LF  0x0A
#define CR  0x0D
#define SP  0x20

/* macros =================================================================== */
#ifdef DEBUG
#define TINFO_DEBUG(fmt,...) printf ("%s:%d: %s(): " fmt "\n", BASENAME(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define TINFO_DEBUG(fmt,...)
#endif

/* private variables ======================================================== */
static const char sUnknown[] = "----";

static const char * sPtecList[] = {
  "TH..", "HC..", "HP..", "HN..", "PM..",
  "HCJB",  "HCJW", "HCJR", "HPJB", "HPJW", "HPJR"
};

static const char * sTempoColorList[] = {
  "BLEU", "BLAN", "ROUG"
};

static const char * sOpTarifList[] = {
  "BASE", "HC", "EJP", "TEMPO"
};

/* structures =============================================================== */
struct xTinfo {

  // Variables de configuration
  iTinfoCb cb[TINFO_CB_SIZE];
  void * uctx;

  // Variables de travail
  int serial;
  char buf[TINFO_BUFFER_SIZE];
  int buflen;
  char ht;
  eTinfoTempoColor demain;
  eTinfoPtec ptec;
  unsigned motdetat;
};

/* private functions ======================================================== */
// -----------------------------------------------------------------------------
static void
prvvReadBlueShortItem (xTinfo * t, xTinfoFrame * f ,
                       const char * label, const char * data) {
  long n;

  if (strncmp (label, "ADIR", 4) == 0) {
    // 3 chiffres décimaux, A

    if (iStrToLong (data, &n, 10) == 0) {
      int phi = data[4] - '1';

      if ( (phi >= 0) && (phi <= 2)) {

        f->blue_short.adir[phi] = n;
        TINFO_DEBUG ("ADIR[%d]=%d A", phi + 1, f->blue_short.adir[phi]);
      }
      else {

        TINFO_DEBUG ("Warning ! Illegal phase number");
      }
    }
  }
  else if (strcmp (label, "ADCO") == 0) {
    // 12 chiffres décimaux

    if (iStrToLong (data, &n, 10) == 0) {

      f->blue_short.adco = n;
      TINFO_DEBUG ("ADCO=%012ld", f->blue_short.adco);
    }
  }
  else if (strncmp (label, "IINST", 5) == 0) {
    // 3 chiffres décimaux, A

    if (iStrToLong (data, &n, 10) == 0) {
      int phi = data[5] - '1';

      if ( (phi >= 0) && (phi <= 2)) {

        f->blue_short.iinst[phi] = n;
        TINFO_DEBUG ("IINST[%d]=%d A", phi + 1, f->blue_short.iinst[phi]);
      }
      else {

        TINFO_DEBUG ("Warning ! Illegal phase number");
      }
    }
  }
}

// -----------------------------------------------------------------------------
static void
prvvReadBlueItem (xTinfo * t, xTinfoFrame * f ,
                  const char * label, const char * data) {
  long n;

  if (strcmp (label, "OPTARIF") == 0) {
    // 4 lettres
    if (strcmp (data, "BASE") == 0) {

      f->blue.optarif = eTinfoOpTarifBase;
    }
    else if (strcmp (data, "HC..") == 0) {

      f->blue.optarif = eTinfoOpTarifHc;
    }
    else if (strcmp (data, "EJP.") == 0) {

      f->blue.optarif = eTinfoOpTarifEjp;
    }
    else if (strncmp (data, "BBR", 3) == 0) {

      f->blue.optarif = eTinfoOpTarifTempo;
      f->blue.tarif.tempo.pgm1 = ( (data[3] & 0x18) >> 3) + 0x40; // A, B, C
      f->blue.tarif.tempo.pgm2 = (data[3] & 0x7) + 0x30; // P0...P7
      TINFO_DEBUG ("TEMPO.PGM1=%c", f->blue.tarif.tempo.pgm1);
      TINFO_DEBUG ("TEMPO.PGM2=%c", f->blue.tarif.tempo.pgm2);
    }

    TINFO_DEBUG ("OPTARIF=%s", sTinfoOpTarifToStr (f->blue.optarif));
    return;
  }

  if (strcmp (label, "ISOUSC") == 0) {
    // 2 chiffres décimaux, A

    if (iStrToLong (data, &n, 10) == 0) {

      f->blue.isousc = n;
      TINFO_DEBUG ("ISOUSC=%d A", f->blue.isousc);
    }
    return;
  }
  else if (strcmp (label, "PTEC") == 0) {
    // 4 lettres

    for (eTinfoPtec ptec = eTinfoPtecTh; ptec <= eTinfoPtecHpJr; ptec++) {

      if (strcmp (sPtecList[ptec - 1], data) == 0) {

        f->blue.ptec = ptec;
        TINFO_DEBUG ("PTEC=%s", sTinfoPetcToStr (f->blue.ptec));
        break;
      }
    }
    return;
  }
  else if (strcmp (label, "PAPP") == 0) {
    // 5 chiffres décimaux, VA

    if (iStrToLong (data, &n, 10) == 0) {

      f->blue.papp = n;
      TINFO_DEBUG ("PAPP=%d VA", f->blue.papp);
    }
    return;
  }
  else if (strncmp (label, "IINST", 5) == 0) {
    // 3 chiffres décimaux, A

    if (iStrToLong (data, &n, 10) == 0) {

      if (strlen (data) > 5) {
        // Triphasé

        int phi = data[5] - '1';
        if ( (phi >= 0) && (phi <= 2)) {

          f->blue.flag |= eTinfoFlagTri;
          f->blue.iinst[phi] = n;
          TINFO_DEBUG ("IINST[%d]=%d A", phi + 1, f->blue.iinst[phi]);
        }
        else {

          TINFO_DEBUG ("Warning ! Illegal phase number");
        }
      }
      else {

        f->blue.iinst[0] = n;
        TINFO_DEBUG ("IINST=%d A", f->blue.iinst[0]);
      }
    }
    return;
  }
  else if (strncmp (label, "IMAX", 4) == 0) {
    // 3 chiffres décimaux, A

    if (iStrToLong (data, &n, 10) == 0) {

      if (strlen (data) > 4) {
        // Triphasé

        int phi = data[4] - '1';
        if ( (phi >= 0) && (phi <= 2)) {

          f->blue.flag |= eTinfoFlagTri;
          f->blue.imax[phi] = n;
          TINFO_DEBUG ("IMAX[%d]=%d A", phi + 1, f->blue.imax[phi]);
        }
        else {

          TINFO_DEBUG ("Warning ! Illegal phase number");
        }
      }
      else {

        f->blue.imax[0] = n;
        TINFO_DEBUG ("IMAX=%d A", f->blue.imax[0]);
      }
    }
    return;
  }
  else if (strcmp (label, "MOTDETAT") == 0) {
    // 6 chiffres hexadécimaux

    if (iStrToLong (data, &n, 16) == 0) {

      f->blue.motdetat = n;
      TINFO_DEBUG ("MOTDETAT=0x%06X", f->blue.motdetat);
    }
    return;
  }

  // Données spécifiques à un type d'alimentation mono/triphasé
  if (f->blue.flag & eTinfoFlagTri) {

    // Triphasé
    if (strcmp (label, "PMAX") == 0) {
      // 5 chiffres décimaux, W

      if (iStrToLong (data, &n, 10) == 0) {

        f->blue.pmax = n;
        TINFO_DEBUG ("PMAX=%d W", f->blue.pmax);
      }
      return;
    }
    else if (strcmp (label, "PPOT") == 0) {
      // 2 chiffres hexadécimaux

      if (iStrToLong (data, &n, 16) == 0) {

        f->blue.ppot = n;
        TINFO_DEBUG ("PPOT=0x%02X", f->blue.ppot);
      }
      return;
    }
  }
  else {

    // Monophasé
    if (strcmp (label, "ADPS") == 0) {
      // 3 chiffres décimaux, A

      if (iStrToLong (data, &n, 10) == 0) {

        f->blue.adps = n;
        f->blue.flag |= eTinfoFlagAdps;
        TINFO_DEBUG ("ADPS=%d A", f->blue.adps);
      }
      return;
    }
  }

  // Données spécifiques à une option tarifaire
  switch (f->blue.optarif) {

    case eTinfoOpTarifBase:
      // Option Base -----------------------------------------------------------

      if (strcmp (label, "BASE") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.base.index = n;
          TINFO_DEBUG ("BASE=%ld Wh", f->blue.tarif.base.index);
        }
      }
      break;

    case eTinfoOpTarifHc:
      // Option Heures creuse --------------------------------------------------

      if (strcmp (label, "HCHC") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.hc.index_hc = n;
          TINFO_DEBUG ("HCHC=%ld Wh", f->blue.tarif.hc.index_hc);
        }
      }
      else if (strcmp (label, "HCHP") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.hc.index_hp = n;
          TINFO_DEBUG ("HCHP=%ld Wh", f->blue.tarif.hc.index_hp);
        }
      }
      else if (strcmp (label, "HHPHC") == 0) {
        // 1 lettre

        f->blue.tarif.hc.horaire = data[0];
        TINFO_DEBUG ("HHPHC=%c", f->blue.tarif.hc.horaire);
      }
      break;

    case eTinfoOpTarifEjp:
      // Option EJP ------------------------------------------------------------

      if (strcmp (label, "EJPHN") == 0) {

        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.ejp.index_hn = n;
          TINFO_DEBUG ("EJPHN=%ld Wh", f->blue.tarif.ejp.index_hn);
        }
      }
      else if (strcmp (label, "EJPHPM") == 0) {

        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.ejp.index_hpm = n;
          TINFO_DEBUG ("EJPHPM=%ld Wh", f->blue.tarif.ejp.index_hpm);
        }
      }
      else if (strcmp (label, "PEJP") == 0) {

        // 2 chiffres décimaux, min

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.ejp.pejp = n;
          TINFO_DEBUG ("PEJP=%d", f->blue.tarif.ejp.pejp);
        }
      }
      break;

    case eTinfoOpTarifTempo:
      // Option Tempo ----------------------------------------------------------

      if (strcmp (label, "BBRHCJB") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hcjb = n;
          TINFO_DEBUG ("BBRHCJB=%ld Wh", f->blue.tarif.tempo.index_hcjb);
        }
      }
      else if (strcmp (label, "BBRHPJB") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hpjb = n;
          TINFO_DEBUG ("BBRHPJB=%ld Wh", f->blue.tarif.tempo.index_hpjb);
        }
      }
      else if (strcmp (label, "BBRHCJW") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hcjw = n;
          TINFO_DEBUG ("BBRHCJW=%ld Wh", f->blue.tarif.tempo.index_hcjw);
        }
      }
      else if (strcmp (label, "BBRHPJW") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hpjw = n;
          TINFO_DEBUG ("BBRHPJW=%ld Wh", f->blue.tarif.tempo.index_hpjw);
        }
      }
      else if (strcmp (label, "BBRHCJR") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hcjr = n;
          TINFO_DEBUG ("BBRHCJR=%ld Wh", f->blue.tarif.tempo.index_hcjr);
        }
      }
      else if (strcmp (label, "BBRHPJR") == 0) {
        // 9 chiffres décimaux, Wh

        if (iStrToLong (data, &n, 10) == 0) {

          f->blue.tarif.tempo.index_hpjr = n;
          TINFO_DEBUG ("BBRHPJR=%ld Wh", f->blue.tarif.tempo.index_hpjr);
        }
      }
      else if (strcmp (label, "DEMAIN") == 0) {
        // 4 lettres

        for (eTinfoTempoColor color = eTinfoColorBlue;
             color <= eTinfoColorRed; color++) {

          if (strcmp (sTempoColorList[color - 1], data) == 0) {

            f->blue.tarif.tempo.demain = color;
            TINFO_DEBUG ("DEMAIN=%s", sTinfoTempoColorToStr (color));
            break;
          }
        }
      }
      break;

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
static void
prvvReadItem (xTinfo * t, xTinfoFrame * f , char * label) {
  char * sep, * data;
  long n;

  // Remplace le spérateur entre étiquette et données par \0
  sep = strchr (label, t->ht);
  *sep = '\0';

  // data pointe le champ données terminé par un \0
  data = sep + 1;

  if (f->raw.frame == 0) {
    // Premier groupe de données

    if (strcmp (label, "ADCO") == 0) {
      // 12 chiffres décimaux

      if (iStrToLong (data, &n, 10) == 0) {

        f->raw.frame = eTinfoFrameBlue;
        f->blue.adco = n;
        TINFO_DEBUG ("<< Blue Frame >>");
        TINFO_DEBUG ("ADCO=%012ld", f->blue.adco);
      }
    }
    else if (strncmp (label, "ADIR", 4) == 0) {

      f->raw.frame = eTinfoFrameBlue;
      f->blue_short.flag |= (eTinfoFlagShort | eTinfoFlagTri | eTinfoFlagAdps);
      TINFO_DEBUG ("<< Blue Frame (short)>>");
      prvvReadBlueShortItem (t, f, label, data);
    }
    return;
  }

  if (f->raw.frame == eTinfoFrameBlue) {

    if (f->blue.flag & eTinfoFlagShort) {

      prvvReadBlueShortItem (t, f, label, data);
    }
    else {

      prvvReadBlueItem (t, f, label, data);
    }
  }
}

// -----------------------------------------------------------------------------
// La trame est correcte, on va la découper et la transmettre à l'utilisateur
static int
prviSplitFrame (xTinfo * t) {
  xTinfoFrame f;
  int ret = 0;
  char * group = t->buf;
  bool bFrameHasBeenProcessed = false;

  memset (&f, 0, sizeof (f));

  while (group) {

    // Cherche le LF de début de ligne
    group = strchr (group, LF);

    if (group) {
      char * end;

      // Si trouvé, passe sur le premier caractère de l'étiquette
      group++;

      // Cherche le CR de fin de ligne, end pointe sur le CR de fin de groupe
      end = strchr (group, CR);

      // Ecrase le séparateur de champ final avec \0
      * (end - 2) = '\0';
      // Récupère les données de la ligne
      prvvReadItem (t, &f, group);
      // Ligne suivante
      group = end + 1;
    }
  }

  // On passe la trame aux différents gestionnaires
  if (f.raw.frame == eTinfoFrameBlue) {

    // Avertissement de Dépassement de Puissance Souscrite
    if (f.blue.flag & eTinfoFlagAdps) {

      TINFO_DEBUG ("<< Avertissement de Dépassement de Puissance Souscrite >>");
      if (t->cb[eTinfoCbAdps]) {

        ret = t->cb[eTinfoCbAdps] (t, &f);
        bFrameHasBeenProcessed = true;
      }
    }

    // Changement Période Tarifaire En Cours
    if (t->ptec != f.blue.ptec) {

      t->ptec = f.blue.ptec;
      f.blue.flag |= eTinfoFlagNewPetc;
      TINFO_DEBUG ("<< Changement Période Tarifaire En Cours: %s >>",
                   sTinfoPetcToStr (t->ptec));
      if (t->cb[eTinfoCbPetc]) {

        ret = t->cb[eTinfoCbPetc] (t, &f);
        bFrameHasBeenProcessed = true;
      }
    }

    // Changement de mot d'état
    if (t->motdetat != f.blue.motdetat) {

      t->motdetat = f.blue.motdetat;
      f.blue.flag |= eTinfoFlagNewMotEtat;
      TINFO_DEBUG ("<< Nouveau Mot d'état: 0x%06X >>",  t->motdetat);
      if (t->cb[eTinfoCbMotEtat]) {

        ret = t->cb[eTinfoCbMotEtat] (t, &f);
        bFrameHasBeenProcessed = true;
      }
    }

    if (f.blue.optarif == eTinfoOpTarifTempo) {

      if (t->demain != f.blue.tarif.tempo.demain) {

        t->demain =  f.blue.tarif.tempo.demain;
        f.blue.flag |= eTinfoFlagTempoNewColor;
        TINFO_DEBUG ("<< Changement couleur demain: %s >>",
                     sTinfoTempoColorToStr (t->demain));
        if (t->cb[eTinfoCbTempo]) {

          ret = t->cb[eTinfoCbTempo] (t, &f);
          bFrameHasBeenProcessed = true;
        }
      }
    }
  }

  if ( (bFrameHasBeenProcessed == false) && (t->cb[eTinfoCbFrame])) {

    ret = t->cb[eTinfoCbFrame] (t, &f);
  }

  t->buflen = 0; // trame traitée, on flushe
  return ret;
}

// -----------------------------------------------------------------------------
// La trame est complète (STX...ETX), on va vérifier les checksums, si pas
// d'erreur, on la passera à prviSplitFrame()
static int
prviProbeFrame (xTinfo * t) {
  char checksum;

  // Vérification des checksums de chaque groupe
  for (int i = 1; i < (t->buflen - 1); i++) {

    if (t->buf[i] == LF) {

      checksum = 0;
    }
    else {
      /* Calcul checksum suivant le mode de calcul 1:
       * En cas de prise en compte future du compteur SAPHIR, il faudra
       * gérer le calcul en mode 2... */

      if (t->buf[i + 2] != CR) {

        checksum += t->buf[i];
      }
      else {
        checksum = (checksum & 0x3F) + 0x20;

        if (t->buf[i + 1] != checksum) {

          // Erreur de checksum
          TINFO_DEBUG ("Checksum error");
          t->buflen = 0;
          errno = EIO;
          return -1;
        }

        if (t->ht == 0) {
          // On récupère le caractère séparateur de champ utilisé
          // Depuis 2013, ce n'est plus l'espace !
          t->ht = t->buf[i];
        }

        i += 2; // on se place sur le CR
      }
    }
  }
  TINFO_DEBUG ("Frame properly formatted");

  // Trame correcte, on va la découper et la transmettre à l'utilisateur
  return prviSplitFrame (t);;
}

// -----------------------------------------------------------------------------
static int
prviProbeBuffer (xTinfo * t, char * buffer, int len) {
  int i;

  if (t->buflen == 0) {
    bool bStxFound = false;

    for (i = 0; i < len; i++) {

      if (buffer[i] == STX) {

        bStxFound = true;
        TINFO_DEBUG ("STX found");
      }
      if (bStxFound) {

        t->buf[t->buflen++] = buffer[i];
      }
    }
  }
  else {

    for (i = 0; i < len; i++) {

      t->buf[t->buflen++] = buffer[i];

      if (buffer[i] == ETX) {

        // Trame complète
        TINFO_DEBUG ("ETX found");
        return prviProbeFrame (t);
      }
      else if (buffer[i] == EOT) {

        // Trame interrompue, on flushe
        TINFO_DEBUG ("EOT found");
        t->buflen = 0;
      }
    }
  }

  return 0;
}

/* internal public functions ================================================ */
// -----------------------------------------------------------------------------
xTinfo *
xTinfoOpen (const char * port, unsigned long baudrate) {
  xTinfo * t;
  xSerialIos ios = {
    .dbits = SERIAL_DATABIT_7,
    .parity = SERIAL_PARITY_EVEN,
    .sbits = SERIAL_STOPBIT_ONE,
    .flow = SERIAL_FLOW_NONE
  };

  t = calloc (1, sizeof (xTinfo));
  assert (t);

  ios.baud = baudrate;
  t->serial = iSerialOpen (port, &ios);

  if (t->serial < 0) {
    free (t);
    return NULL;
  }

  vSerialFlush (t->serial);
  return t;
}

// -----------------------------------------------------------------------------
int
iTinfoPoll (xTinfo * t) {
  int ret;

  ret = iSerialPoll (t->serial, TINFO_TIMEOUT);
  if (ret < 0) {

    return -1;
  }
  else if (ret > 0) {
    char buffer[TINFO_BUFFER_SIZE];
    ret = read (t->serial, buffer, ret);
    if (ret < 0) {

      return -1;
    }
    return prviProbeBuffer (t, buffer, ret);
  }

  return 0;
}

// -----------------------------------------------------------------------------
int
iTinfoClose (xTinfo * t) {
  int ret = close (t->serial);
  free (t);
  return ret;
}

// -----------------------------------------------------------------------------
void
vTinfoSetCB (xTinfo * t, eTinfoCbType type, iTinfoCb cb) {

  t->cb[type] = cb;
}

// -----------------------------------------------------------------------------
void *
pvTinfoGetUserContext (const xTinfo * t) {

  return t->uctx;
}

// -----------------------------------------------------------------------------
void
vTinfoSetUserContext (xTinfo * t, void * uctx) {

  t->uctx = uctx;
}


// -----------------------------------------------------------------------------
const char *
sTinfoPetcToStr (eTinfoPtec p) {

  if ( (p >= eTinfoPtecTh) && (p <= eTinfoPtecHpJr)) {

    return sPtecList[p - 1];
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sTinfoTempoColorToStr (eTinfoTempoColor c) {

  if ( (c >= eTinfoColorBlue) && (c <= eTinfoColorRed)) {

    return sTempoColorList[c - 1];
  }
  return sUnknown;
}

// -----------------------------------------------------------------------------
const char *
sTinfoOpTarifToStr (eTinfoOpTarif t) {

  if ( (t >= eTinfoOpTarifBase) && (t <= eTinfoOpTarifTempo)) {

    return sOpTarifList[t - 1];
  }
  return sUnknown;
}

/* ========================================================================== */
