/*
 * rpi.c
 * @brief
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sysio/rpi.h>

/* constants ================================================================ */
static const char sManUnknown[] = "Unknown";
static const char sManSony[] = "Sony";
static const char sManQisda[] = "Qisda";
static const char sManEgoman[] = "Egoman";
static const char sManEmbest[] = "Embest";

static const xRpi xRpiDb[] = {
  /*
    | Revision | Release Date | Model          | PCB Revision | Memory | Notes                     |
    | :------- | :----------- | :------------- | :----------- | :----- | :------------------------ |
    | 0002     | Q1 2012      | B              | 1.0          | 256MB  |                           |
    | 0003     | Q3 2012      | B (ECN0001)    | 1.0          | 256MB  | Fuses mod and D14 removed |
  */
  {.iRev = 0x00000002, .eModel = eRpiModelB, .iGpioRev = 1, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 1, .iPcbMinor = 0, .sManufacturer = sManUnknown},
  {.iRev = 0x00000003, .eModel = eRpiModelB, .iGpioRev = 1, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 1, .iPcbMinor = 0, .sManufacturer = sManUnknown},
  /*
    | 0004     | Q3 2012      | B              | 2.0          | 256MB  | Mfg by Sony               |
    | 0005     | Q4 2012      | B              | 2.0          | 256MB  | Mfg by Qisda              |
    | 0006     | Q4 2012      | B              | 2.0          | 256MB  | Mfg by Egoman             |
  */
  {.iRev = 0x00000004, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x00000005, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManQisda},
  {.iRev = 0x00000006, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManEgoman},
  /*
    | 0007     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Egoman             |
    | 0008     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Sony               |
    | 0009     | Q1 2013      | A              | 2.0          | 256MB  | Mfg by Qisda              |
  */
  {.iRev = 0x00000007, .eModel = eRpiModelA, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManEgoman},
  {.iRev = 0x00000008, .eModel = eRpiModelA, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x00000009, .eModel = eRpiModelA, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManQisda},
  /*

    | 000d     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Egoman             |
    | 000e     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Sony               |
    | 000f     | Q4 2012      | B              | 2.0          | 512MB  | Mfg by Qisda              |
  */
  {.iRev = 0x0000000d, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManEgoman},
  {.iRev = 0x0000000e, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x0000000f, .eModel = eRpiModelB, .iGpioRev = 2, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 2, .iPcbMinor = 0, .sManufacturer = sManQisda},
  /*
    | 0010     | Q3 2014      | B+             | 1.0          | 512MB  | Mfg by Sony               |
    | 0011     | Q2 2014      | Compute Module | 1.0          | 512MB  | Mfg by Sony               |
    | 0012     | Q4 2014      | A+             | 1.0          | 256MB  | Mfg by Sony               |
    | 0013     | Q1 2015      | B+             | 1.2          | 512MB  | ?                         |
  */
  {.iRev = 0x00000010, .eModel = eRpiModelBPlus,         .iGpioRev = 3, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 1, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x00000011, .eModel = eRpiModelComputeModule, .iGpioRev = 3, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 1, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x00000012, .eModel = eRpiModelAPlus,         .iGpioRev = 3, .eMcu = eRpiMcuBcm2708, .iMemMB = 256, .iPcbMajor = 1, .iPcbMinor = 0, .sManufacturer = sManSony},
  {.iRev = 0x00000013, .eModel = eRpiModelBPlus,         .iGpioRev = 3, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 1, .iPcbMinor = 2, .sManufacturer = sManUnknown},
  /*
    | a01041   | Q1 2015      | 2 Model B      | 1.1          | 1GB    | Mfg by Sony               |
    | a21041   | Q1 2015      | 2 Model B      | 1.1          | 1GB    | Mfg by Embest, China      |
  */
  {.iRev = 0x00a01041, .eModel = eRpiModel2B,            .iGpioRev = 3, .eMcu = eRpiMcuBcm2709, .iMemMB = 1024, .iPcbMajor = 1, .iPcbMinor = 1, .sManufacturer = sManSony},
  {.iRev = 0x00a21041, .eModel = eRpiModel2B,            .iGpioRev = 3, .eMcu = eRpiMcuBcm2709, .iMemMB = 1024, .iPcbMajor = 1, .iPcbMinor = 1, .sManufacturer = sManEmbest},
  /*

    | 900092   | Q4 2015      | Zero           | 1.2          | 512MB  | Mfg by Sony               |
   */
  {.iRev = 0x00900092, .eModel = eRpiModelZero,          .iGpioRev = 3, .eMcu = eRpiMcuBcm2708, .iMemMB = 512, .iPcbMajor = 1, .iPcbMinor = 2, .sManufacturer = sManSony},
  /* Last element */
  {.iRev = -1,         .eModel = eRpiModelUnknown,       .iGpioRev = -1, .eMcu = eRpiMcuUnknown, .iMemMB = -1, .iPcbMajor = -1, .iPcbMinor = -1, .sManufacturer = sManUnknown},
};

/* private variables ======================================================== */
static int iRev = -1;
static const xRpi * pxRpi;

/* private functions ======================================================== */
static int
iStartWith (const char * text, const char * with) {
  while (*text == ' ') {
    text++;
  }
  return strstr (text, with) == text;
}

/* public variables ========================================================= */
/* internal public functions ================================================ */

/* -----------------------------------------------------------------------------
  $ cat /proc/cpuinfo
  processor : 0
  model name  : ARMv6-compatible processor rev 7 (v6l)
  BogoMIPS  : 2.00
  Features  : half thumb fastmult vfp edsp java tls
  CPU implementer : 0x41
  CPU architecture: 7
  CPU variant : 0x0
  CPU part  : 0xb76
  CPU revision  : 7

  Hardware  : BCM2708
  Revision  : 000e
  Serial    : 0000000059ad5ead
 */
int
iRpiRev (void) {

  if (iRev == -1) {
    /* Read the entire contents of /proc/cpuinfo into the buffer.  */
    FILE * fp = fopen ("/proc/cpuinfo", "r");

    if (fp) {
      char *p;
      int revision = 0;
      char buffer[1024];
      bool bisBcm = false;

      while ( (p = fgets (buffer, sizeof (buffer), fp) ) ) {

        /* Locate Hardware for check if is it a Raspberry  */
        if (iStartWith (p, "Hardware") ) {

          if (strstr (p, "BCM2709") ) {

            if (strstr (p, "BCM2708") ) {

              return 0;
            }
          }
          bisBcm = true;
        }
        else if ( (iStartWith (p, "Revision") ) && (bisBcm) ) {

          p = strstr (p, "Revision");
          /* Parse the line to extract the revision.  */
          if (sscanf (p, "Revision  : %x", &revision) > 0) {

            iRev = revision;
          }
        }
      }
      if (fclose (fp) < 0) {

        perror ("fclose");
      }
      return revision;
    }

    perror ("fopen");
    return -1;
  }
  return iRev;
}

// -----------------------------------------------------------------------------
const xRpi *
pxRpiInfo (void) {

  if (pxRpi == NULL) {

    int r = iRpiRev();

    if (r > 0) {
      const xRpi * p = xRpiDb;

      // Recherche dans la base de données de la révision
      while ( (p->iRev != -1) && (p->iRev != r) ) {
        p++;
      }
      pxRpi = p;
    }
  }
  return pxRpi;
}

/* ========================================================================== */
