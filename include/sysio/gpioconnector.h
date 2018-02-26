/**
 * @file
 * @brief Connecteurs GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_CONNECTOR_H_
#define _SYSIO_GPIO_CONNECTOR_H_

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
     * @param number numéro de broche dans la numérotation du connecteur. Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @return pointeur sur la broche
     */
    std::shared_ptr<GpioPin> pin (int number) const;

    /**
     * @brief Mode actuel
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinMode mode (int number) const;

    /**
     * @brief Modification du mode
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param mode nouveau mode, une exception std::invalid_argument est
     * déclenchée si le mode demandé n'est pas supporté.
     */
    void setMode (int number, GpioPinMode mode);

    /**
     * @brief Résistance de tirage d'une broche
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinPull pull (int number) const;

    /**
     * @brief Modification de la résistance de tirage
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param pull nouvelle résistance, PullOff pour désactiver, une exception
     * std::invalid_argument est déclenchée si la résistance demandée n'est pas
     * supportée.
     */
    void setPull (int number, GpioPinPull pull);

    /**
     * @brief Modification de l'état binaire d'une sortie
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @param value true pour un état logique haut (VccIo).
     */
    void write (int number, bool value);

    /**
     * @brief Bascule de l'état binaire d'une sortie
     *
     * Si la sortie est à l'état bas, elle passe à l'état haut et inversement.
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void toggle (int number);

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
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @return true pour un état logique haut (VccIo).
     */
    bool read (int number);

    /**
     * @brief Libère une broche de son utilisation
     *
     * La broche correspondante est remise dans son état initial (avant
     * l'ouverture du port GPIO parent.
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param number numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void release (int number);

    /**
     * @brief Calcul du numéro de broche
     *
     * @param row ligne de la broche
     * @param column colonne de la broche
     * @return Numéro de la broche dans le connecteur
     */
    int pinNumber (int row, int column) const;

    /**
     * @brief Liste des broches du connecteur
     *
     * Permet de parcourir les broches à l'aide des itérateurs de la STL
     * par exemple pour mettre en entrée toutes les broches:
     * @code
     *    Gpio * g = new Gpio;
     *    // ....
     *    GpioConnector * c = g->connector(0);
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

  protected:
    /**
     * @brief Accès au GPIO parent
     */
    Gpio * gpio() const;

    /**
     * @brief Accès à la couche matérielle
     */
    GpioDevice * device() const;

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
