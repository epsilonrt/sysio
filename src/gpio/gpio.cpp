/**
 * @file gpio/gpio.cpp
 * @brief GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <iostream>
#include <sstream>
#include <sysio/gpio.h>

/*
 * @brief Création d'un GPIO
 *
 * @param args pointeur sur une variable de configuration dépendant de
 *        l'architecture
 */
Gpio::Gpio (void * args)  {

  g = xGpioOpen (args);
  if (!g) {
    
  }
}

/*
 * @brief Destruction d'un GPIO
 */
Gpio::~Gpio() {

  (void) iGpioClose (g);
}

/*
 * @brief Modifie le type de numérotation
 *
 * @return 0, -1 si erreur
 */
int
Gpio::setNumbering (eGpioNumbering eNum) {

  return iGpioSetNumbering (eNum, g);
}

/*
 * @brief Type de numérotation
 *
 * @return 0, -1 si erreur
 */
eGpioNumbering
Gpio::numbering() {

  return eGpioGetNumbering (g);
}

/*
 * @brief Chaîne de caractère correspondant à une numérotation
 * @param eNum numérotation
 * @return Chaîne de caractère correspondant à une numérotation
 */
const char *
Gpio::numberingToStr (eGpioNumbering eNum) {

  return sGpioNumberingToStr (eNum);
}

/*
 * @brief Modifie le type d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param pin le numéro de la broche
 * @param mode le type de la broche
 * @return 0, -1 si erreur
 */
int
Gpio::setMode (int pin, eGpioMode mode) {

  return iGpioSetMode (pin, mode, g);
}

/*
 * @brief Lecture du type actuel d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param pin le numéro de la broche
 * @return le type, eModeError si erreur
 */
eGpioMode
Gpio::mode (int pin) {

  return eGpioGetMode (pin, g);
}

/*
 * @brief Modification de la résistance de tirage d'une broche
 *
 * @param pin le numéro de la broche
 * @param ePull le type de la résistance (ePullOff pour désactiver)
 * @return 0, -1 si erreur
 */
int
Gpio::setPull (int pin, eGpioPull p) {

  return iGpioSetPull (pin, p, g);
}

/*
 * @brief Lecture du type du type résistance de tirage d'une broche
 *
 * @param pin le numéro de la broche
 * @return le type de la résistance, ePullError si erreur
 */
eGpioPull
Gpio::pull (int pin) {

  return eGpioGetPull (pin, g);
}

/*
 * @brief Teste la validité d'un numéro de broche
 *
 * @param pin le numéro de la broche (numérotation sélectionnée)
 * @return true si numéro valide
 */
bool
Gpio::isValid (int pin) {

  return bGpioIsValid (pin, g);
}

/*
 * @brief Lecture du nombre de broches du port GPIO
 *
 * @return nombre de broches, -1 si erreur
 */
int
Gpio::size() {

  return iGpioGetSize (g);
}

/*
 * @brief Modification de l'état binaire d'une sortie
 *
 * @param pin le numéro de la broche
 * @param value valeur binaire à appliquer
 * @return 0, -1 si erreur
 */
int
Gpio::write (int pin, bool value) {

  return iGpioWrite (pin, value, g);
}

/*
 * @brief Bascule de l'état binaire d'une sortie
 *
 * @param pin le numéro de la broche
 * @return 0, -1 si erreur
 */
int
Gpio::toggle (int pin) {

  return iGpioToggle (pin, g);
}

/*
 * @brief Modification de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param mask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour modifier toutes les
 *        broches.
 * @param value valeur binaire à appliquer aux broches
 * @return 0, -1 si erreur
 */
int
Gpio::writeAll (int64_t mask, bool value) {

  return iGpioWriteAll (mask, value, g);
}

/*
 * @brief Bascule de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param mask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour modifier toutes les
 *        broches.
 * @return 0, -1 si erreur
 */
int
Gpio::toggleAll (int64_t mask) {

  return iGpioToggleAll (mask, g);
}

/*
 * @brief Lecture de l'état binaire d'une broche
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param pin le numéro de la broche
 * @return true à l'état haut, false à l'état bas, -1 si erreur
 */
int
Gpio::read (int pin) {

  return iGpioRead (pin, g);
}

/*
 * @brief Lecture de l'état binaire d'un ensemble de broches
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param mask masque de sélection des broches à lire, un bit à 1 indique
 *        que la broche correspondante doit être lue (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour lire toutes les
 *        broches.
 * @return l'état des broches, chaque bit correspond à l'état de la broche
 * correspondante (bit 0 pour la broche 0, bit 1 pour la broche 1 ...)
 */
int64_t
Gpio::readAll (int64_t mask) {

  return iGpioReadAll (mask, g);
}

/*
 * @brief Modifie la libération des broches à la fermeture
 *
 * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
 * iGpioClose(). Cette fonction permet de modifier ce comportement.
 *
 * @param enable true active la libération, false la désactive.
 * @return 0, -1 si erreur
 */
int
Gpio::setReleaseOnClose (bool enable) {

  return iGpioSetReleaseOnClose (enable, g);
}

/*
 * @brief Lecture la libération des broches à la fermeture
 *
 * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
 * iGpioClose(). Cette fonction permet de lire l'état de cette directive
 *
 * @return true si validé, false sinon
 */
bool
Gpio::releaseOnClose() {

  return bGpioGetReleaseOnClose (g);
}

/*
 * @brief Libère une broche de son utilisation
 *
 * La broche correspondante est remise dans son état initial. Seule une broche
 * dont le type a été modifié depuis l'ouverture du port par iGpioOpen() sera
 * restaurée. Si le type de broche n'a pas été modifié depuis l'ouverture, cette
 * fonction ne fait rien et renvoie 0.
 *
 * @param pin le numéro de la broche
 * @return 0, -1 si erreur
 */
int
Gpio::release (int pin) {

  return iGpioRelease (pin, g);
}


/*
 * @brief Teste si l'itérateur interne peut passer à une broche suivante
 * @return true si possible
 */
bool
Gpio::hasNext() {

  return bGpioHasNext (g);
}

/*
 * @brief Teste si l'itérateur interne peut passer à une broche précédente
 * @return true si possible
 */
bool
Gpio::hasPrevious() {


  return bGpioHasPrevious (g);
}

/*
 * @brief Pointe l'itérateur interne sur la broche suivante
 * @return le numéro de la broche suivante
 */
int
Gpio::next() {

  return iGpioNext (g);
}

/*
 * @brief Pointe l'itérateur interne sur la broche précédente
 * @return le numéro de la broche précédente
 */
int
Gpio::previous() {

  return iGpioPrevious (g);
}

/*
 * @brief Pointe l'itérateur interne après la dernière broche
 */
void
Gpio::toBack() {

  (void) iGpioToBack (g);
}

/*
 * @brief Pointe l'itérateur interne avant la première broche
 */
void
Gpio::toFront() {

  (void) iGpioToFront (g);
}

/*
 * @brief Chaîne de caractère correspondant à un mode
 *
 * @param mode mode
 * @return Chaîne de caractère correspondant à un mode
 */
const char *
Gpio::modeToStr (eGpioMode mode) {

  return sGpioModeToStr (mode);
}

// -----------------------------------------------------------------------------
GpioException::GpioException (GpioException::Code code, int value) :
  _code (code), _value (value) {
  std::ostringstream s;

  switch (_code) {
    case ArgumentExpected:
      _msg.assign ("Arguments expected");
      break;
    case PinNumberExpected:
      _msg.assign ("Pin number expected");
      break;
    case NotBinaryValue:
      s << _value << " is not a binary value";
      _msg = s.str();
      break;
    case NotPwmValue:
      s << _value << " is not a pwm value";
      _msg = s.str();
      break;
    case NotOutputPin:
      s << "Pin #" << value << " is not an output";
      _msg = s.str();
    case NotPwmPin:
      s << "Pin #" << value << " is not an pwm output";
      _msg = s.str();
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
GpioException::GpioException (GpioException::Code code, const std::string& param) :
  _code (code), _param (param) {

  switch (_code) {
      _msg.assign ("Unknown command: ");
      break;
    case IllegalMode:
      _msg.assign ("Illegal mode: ");
      break;
    default:
      break;
  }
  _msg.append (_param);
}

/* ========================================================================== */
