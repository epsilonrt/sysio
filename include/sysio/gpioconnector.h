/**
 * @file
 * @brief Connecteurs GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_CONNECTOR_H_
#define _SYSIO_GPIO_CONNECTOR_H_

#include <iostream>
#include <sysio/gpiopin.h>

/**
 *  @addtogroup sysio_gpio
 *
 *  @{
 */

/**
 * @brief Fonction de calcul du numéro d'une broche de connecteur
 * Le numéro d'une broche dépend de sa ligne \c row, de sa colonne \c column,
 * du nombre de colonnes \c columns et du modèle du connecteur.
 *
 * Chaque connecteur dispose d'une de ses fonctions fournis au constructeur par
 * le descripteur.
 */
typedef int (* GpioConnectorPinNumberFunc) (int row, int column, int columns);

/* internal public classes ================================================== */

/**
 * @class GpioConnectorDescriptor
 * @author epsilonrt
 * @date 02/23/18
 * @brief Descripteur d'un connecteur
 */
class GpioConnectorDescriptor {
  public:
    std::string name;
    int number;
    int rows;
    int columns;
    GpioConnectorPinNumberFunc pinNumber;
    std::vector<GpioPinDescriptor> pin;
};

class Gpio;
class GpioDevice;

/**
 * @class GpioConnector
 * @author epsilonrt
 * @date 02/23/18
 * @brief Connecteur
 */
class GpioConnector {

  public:
    friend class GpioPin;

    /**
     * @brief Constructeur
     *
     * @param parent pointeur sur le Gpio parent
     * @param desc pointeur sur la description du connecteur
     */
    GpioConnector (Gpio * parent, const GpioConnectorDescriptor * desc);

    /**
     * @brief Destructeur
     */
    virtual ~GpioConnector();

    /**
     * @brief Nom
     */
    const std::string & name() const;
    
    /**
     * @brief Numéro du connecteur sur la carte (commence à 1)
     */
    int number() const;
    
    /**
     * @brief Nombre de broches
     */
    int size() const;

    /**
     * @brief Nombre de lignes
     */
    int rows() const;

    /**
     * @brief Nombre de colonnes
     */
    int columns() const;

    /**
     * @brief Broche du connecteur
     *
     * @param pin numéro de broche dans la numérotation du connecteur. Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @return pointeur sur la broche
     */
    std::shared_ptr<GpioPin> pin (int pin) const;

    /**
     * @brief Mode actuel
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinMode mode (int pin) const;

    /**
     * @brief Modification du mode
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param mode nouveau mode, une exception std::invalid_argument est
     * déclenchée si le mode demandé n'est pas supporté.
     */
    void setMode (int pin, GpioPinMode mode);

    /**
     * @brief Résistance de tirage d'une broche
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinPull pull (int pin) const;

    /**
     * @brief Modification de la résistance de tirage
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param pull nouvelle résistance, PullOff pour désactiver, une exception
     * std::invalid_argument est déclenchée si la résistance demandée n'est pas
     * supportée.
     */
    void setPull (int pin, GpioPinPull pull);

    /**
     * @brief Modification de l'état binaire d'une sortie
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param value true pour un état logique haut (VccIo).
     */
    void write (int pin, bool value);

    /**
     * @brief Bascule de l'état binaire d'une sortie
     *
     * Si la sortie est à l'état bas, elle passe à l'état haut et inversement.
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void toggle (int pin);

    /**
     * @brief Lecture de l'état binaire d'une broche
     *
     * En fonction de l'architecture, la lecture est possible sur les entrées ou
     * sur les entrées et les sorties.
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio et une exeception std::system_error si la lecture est
     * impossible sur le système.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @return true pour un état logique haut (VccIo).
     */
    bool read (int pin) const;

    /**
     * @brief Libère une broche de son utilisation
     *
     * La broche correspondante est remise dans son état initial (avant
     * l'ouverture du port GPIO parent.
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void release (int pin);

    /**
     * @brief Liste des broches du connecteur
     *
     * Permet de parcourir les broches à l'aide des itérateurs de la STL
     * par exemple pour mettre en entrée toutes les broches:
     * @code
     *    Gpio * g = new Gpio;
     *    // ....
     *    GpioConnector * c = g->connector(1);
     *    // ....
     *    for (auto p = c->pin().begin(); p != c->pin().end(); ++p) {
     *      // p est une std::pair: first = numéro et second = broche
     *      p->second->setMode(ModeInput);
     *    }
     * @endcode
     *
     *
     * @return Container std::map indexé par numéro de broche connecteur
     */
    const std::map<int, std::shared_ptr<GpioPin>> & pin ();

    /**
     * @brief Accès au GPIO parent
     */
    Gpio * gpio() const;

    /**
     * @brief Active le mode mise au point
     * 
     * Cela active l'affichage d'informations de mise au point de la couche
     * matérielle (GpioDevice).
     */
    void setDebug (bool enable);

    /**
     * @brief Indique si le mode mise au point est actif
     */
    bool isDebug() const;

    
    /**
     * @brief Affiche toutes les informations sur le connecteur et ses broches
     * 
     * Voici un exemple d'affichage pour le connecteur DBG_UART d'un NanoPi à
     * partir du code suivant:
     * 
     * @code
        Gpio * gpio = new Gpio;
        ...
        cout << gpio->connector(4);
        ....
     * @endcode
     * 
     * Cela donne à l'affichage :
     * 
     * @verbatim
                           CON2 (#4)
        +-----+-----+----------+------+------+---+----+
        | sOc | sIo |   Name   | Mode | Pull | V | Ph |
        +-----+-----+----------+------+------+---+----+
        |     |     |       5V |      |      |   |  1 |
        |     |     |  USB-DP1 |      |      |   |  2 |
        |     |     |  USB-DM1 |      |      |   |  3 |
        |     |     |  USB-DP2 |      |      |   |  4 |
        |     |     |  USB-DM2 |      |      |   |  5 |
        | 105 |  20 |  GPIOL11 |  OUT |  OFF | 0 |  6 |
        |  17 |  11 |  GPIOA17 |  OUT |  OFF | 0 |  7 |
        |  18 |  31 |  GPIOA18 |  OUT |  OFF | 0 |  8 |
        |  19 |  30 |  GPIOA19 |   IN |   UP | 1 |  9 |
        |  20 |  21 |  GPIOA20 |   IN |   UP | 1 | 10 |
        |  21 |  22 |  GPIOA21 |   IN |   UP | 1 | 11 |
        |     |     |      GND |      |      |   | 12 |
        +-----+-----+----------+------+------+---+----+
        | sOc | sIo |   Name   | Mode | Pull | V | Ph |
        +-----+-----+----------+------+------+---+----+
     * @endverbatim
     *
      * @param os os flux de sortie
      * @param c pointeur sur un connecteur
      */
    friend std::ostream& operator<<(std::ostream& os, const GpioConnector * c);  

  protected:
    /**
     * @brief Accès à la couche matérielle
     */
    GpioDevice * device() const;

    /**
     * @brief Calcul du numéro de broche
     * 
     * Permet à une broche de récupérer son numéro dans le connecteur
     *
     * @param row ligne de la broche
     * @param column colonne de la broche
     * @return Numéro de la broche dans le connecteur
     */
    int pinNumber (int row, int column) const;

  private:
    Gpio * _parent;
    const GpioConnectorDescriptor * _descriptor; // descripteur
    std::map<int, std::shared_ptr<GpioPin>> _pin; // toutes les broches
};
/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_CONNECTOR_H_ defined */
