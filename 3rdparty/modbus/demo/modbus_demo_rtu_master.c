/*
 * modbus_demo_rtu_master.c
 * @brief Test modbus RTU en maître
 *
 * Lecture des registres d'entrée 0 et 1 d'une carte Humidité SolarPi 2015.
 * -------------------------- Input registers ----------------------------------
 * Reg    Size   Description
 * -----------------------------------------------------------------------------
 * 1      16-bit Humidité mesurée en centièmes de %RH, entier non signé
 * 2      16-bit Valeur brute ADC correspondant à la mesure en LSB, entier non signé
 * -----------------------------------------------------------------------------
 *
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>

/* constants ================================================================ */
#define TIMEOUT 0.5
#define SLAVE_ADDR 32 // Carte Humidité projet SolarPi 2015
#define DEFAULT_BAUDRATE 38400

/* main ===================================================================== */
int
main (int argc, char **argv) {
  modbus_t * mb;
  uint16_t value[2]; 
  uint32_t  sec, usec;
  double timeout = TIMEOUT;
  const char * port;
  int ret;
  int slave = SLAVE_ADDR;
  int baud = DEFAULT_BAUDRATE;

  if (argc < 2) {

    printf ("Usage: %s serial_port [baud]\n", argv[0]);
    exit (EXIT_FAILURE);
  }
  port = argv[1];
  if (argc > 2) {

    baud = atoi (argv[2]);
  }

  mb = modbus_new_rtu (port, baud, 'E', 8, 1);
  if (mb == NULL) {

    perror ("Unable to create the libmodbus context");
  }

  // Permet d'avoir des messages détaillés
  modbus_set_debug (mb, TRUE);

  // Uniquement sur un système Linux équipé d'un port RS485 (SC16IS ...)
  //modbus_rtu_set_serial_mode (mb, MODBUS_RTU_RS485);

  // Connection au bus
  if (modbus_connect (mb) == -1) {

    printf ("Connection failed: %s\n", modbus_strerror (errno));
    modbus_free (mb);
  }

  // Réglage du timeout de réponse (temps laissé à l'esclave pour répondre)
  sec = (uint32_t) timeout;
  usec = (uint32_t) ( (timeout - sec) * 1E6); // calcul nombre de us restants
  modbus_set_response_timeout (mb, sec, usec);

  // Réglage de l'adresse de l'esclave
  modbus_set_slave (mb, slave);

  /* Lecture de 2 registres à partir de 0 (registres 0 et 1 donc ...)
   * Registre 0: Humidité relative % 
   * Registre 1: Humidité brute en LSB
   * le registre 1 modbus est à l'adresse 0 PDU, regsitre 2 adresse 1 ...
   */
  ret = modbus_read_input_registers (mb, 0, 2, value);
  if (ret != 2) {
    
    printf ("Read failed: %s\n", modbus_strerror (errno));
  }
  
  // Affichage des valeurs de registre
  for (int i = 0; i < 2; i++) {
    
    printf ("reg[%d]=%d\n", i, value[i]);
  }
  
  modbus_close (mb);
  modbus_free (mb);
  return 0;
}
/* ========================================================================== */
