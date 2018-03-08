/**
 * @file
 * @author epsilonrt
 * @date 03/08/18
 * @brief Arduino emulation
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_ARDUINO_H_
#define _SYSIO_ARDUINO_H_

#include <sysio/gpio.h>
#include <sysio/clock.h>
#ifndef __DOXYGEN__
enum ArduinoPinMode {
  INPUT = Sysio::Pin::ModeInput,
  OUTPUT = Sysio::Pin::ModeOutput,
  INPUT_PULLUP,
  INPUT_PULLDOWN // Not supported by Arduino !
};
enum ArduinoIntEdge {
  // LOW & HIGH unsupported !
  CHANGE = Sysio::Pin::EdgeBoth,
  RISING = Sysio::Pin::EdgeRising,
  FALLING = Sysio::Pin::EdgeFalling
};
typedef Sysio::Pin::Isr Isr;
#define digitalPinToInterrupt(p) (p)
#else

/**
 *  @defgroup sysio_arduino Emulation Arduino
 *
 *  Ce module permet de programmer avec Sysio en utilisant une syntaxe proche d'Arduino.
 */

/**
 *  @addtogroup sysio_arduino
 *  @{
 */

/**
 * @enum ArduinoPinMode
 * @brief Mode d'utilisation d'une broche digitale
 */
enum ArduinoPinMode {
  INPUT,  ///< entrée sans résistance de tirage
  OUTPUT, ///< sortie
  INPUT_PULLUP,  ///< entrée avec résistance de tirage à l'état haut
  INPUT_PULLDOWN ///< entrée avec résistance de tirage à l'état bas, mode supplémentaire non disponible dans Arduino
};

/**
 * @enum ArduinoIntEdge
 * @brief Type de front déclenchant une interrption
 *
 * @warning le déclenchement sur niveau logique bas et haut (LOW/HIGH)
 * ne sont pas pris en charge
 */
enum ArduinoIntEdge {
  // LOW & HIGH unsupported !
  CHANGE, ///< front montant et descendant
  RISING, ///< front montant
  FALLING ///< front descendant
};

/**
 * @brief Transforme un numéro de broche en numéro d'interruption
 *
 * Cette macro est présente uniquement pour la compatibilté Arduino car elle
 * ne fait que recopier le numéro de broche.
 */
#define digitalPinToInterrupt(p)

/**
 * @brief Routine d'interruption
 *
 * Une routine d'interruption ne prend et ne renvoie aucun paramètre.
 */
typedef void (* Isr) (void);

#endif /* __DOXYGEN__ */

/**
 * @enum ArduinoBool
 * @brief Valeur binaire
 */
enum ArduinoBool {
  HIGH = true,  ///< état haut
  LOW = false   ///< état bas
};

// Digital pins ----------------------------------------------------------------
/**
 * @brief Modification du mode d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
void pinMode (int pin, ArduinoPinMode mode);

/**
 * @brief Modification d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
void digitalWrite (int pin, int value);

/**
 * @brief Lecture d'une broche digitale
 *
 *  https://www.arduino.cc/en/Tutorial/DigitalPins
 */
int digitalRead (int pin);

/**
 * @brief Basculement de l'état  d'une broche digitale
 *
 *  @warning Non disponible dans Arduino !
 */
void digitalToggle (int pin); // Not supported by Arduino !

// Interrupts ------------------------------------------------------------------
/**
 * @brief Installe une routine d'interruption (Isr)
 * 
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
void attachInterrupt (int pin, Isr isr, ArduinoIntEdge mode);

/**
 * @brief Désinstalle une routine d'interruption (Isr)
 * 
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */
void detachInterrupt (int pin);

// Time ------------------------------------------------------------------------
/**
 * @brief Pause pour une durée en millisecondes
 * @param ms durée de la pause en ms, -1 pour endormir le thread
 */
void delay (unsigned long ms);

/**
 * @brief Pause pour une durée en microsecondes
 * @param ms durée de la pause en us, -1 pour endormir le thread
 */
void delayMicroseconds (unsigned long us);

/**
 * @brief Nombre de millisecondes depuis le lancement du programme
 */
unsigned long millis();

/**
 * @brief Nombre de microsecondes depuis le lancement du programme
 */
unsigned long micros();
/**
* @}
*/

/* ========================================================================== */
#endif /*_SYSIO_ARDUINO_H_ defined */
