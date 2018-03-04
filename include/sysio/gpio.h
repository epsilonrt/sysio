/**
 * @file
 * @brief GPIO
 *
 * Copyright © 2015-2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_H_
#define _SYSIO_GPIO_H_

/**
 *  @defgroup sysio_gpio GPIO
 *
 *  Ce module fournit les classes permettant de contrôler les broches
 *  d'entrées-sorties à usage général.
 *  @{
 */

// Ancienne interface, sera supprimée en version 2.0
#include <sysio/gpio_c.h>

#if defined(__cplusplus) || defined(DOXYGEN)
// -----------------------------------------------------------------------------
// C++ part --->

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <exception>

/**
 * @enum GpioAccessLayer
 * @brief Choix de la façon d'accéder aux broches du GPIO
 * 
 * En fonction de la plateforme, le module GPIO peut utiliser différentes façon
 * pour accéder aux broches. 
 */
enum GpioAccessLayer {
  AccessLayerAuto  = 0, //< Choix automatique (à utiliser la plupart du temps), c'est la plateforme qui choisit par le GpioDevice
  AccessLayerIoMap = 0x0001, ///< Accès par les registres du SOC, la plus performante mais il faut être sûr que cela est disponible sur la plateforme
  AccessLayerSysFs = 0x0002, ///< Accès par l'interface "utilisateur" dans /sys/class/gpio, la plus générique
  AccessLayerAll = AccessLayerIoMap + AccessLayerSysFs ///< Le mieux, toutes les fonctions sont disponibles
};

#include <sysio/gpioconnector.h>

/* internal public classes ================================================== */
/**
 * @class GpioDescriptor
 * @author epsilonrt
 * @date 02/23/18
 * @brief Descripteur d'une carte équipée d'un GPIO
 */
class GpioDescriptor {
  public:
    std::string name; ///< Nom de la carte
    std::vector<GpioConnectorDescriptor> connector; ///< Descripteurs des connecteurs
};

class GpioDevice;

/**
 * @class Gpio
 * @author epsilonrt
 * @date 02/22/18
 * @brief GPIO
 */
class Gpio {
  public:
    friend class GpioConnector;

    /**
     * @brief Constructeur par défaut
     * @param layer choix de la couche d'accès, AccessLayerAuto par défaut, dans
     * ce cas, le choix est laissé à la couche GpioDevice (conseillé).
     */
    Gpio (GpioAccessLayer layer = AccessLayerAuto);

    /**
     * @brief Destructeur
     */
    virtual ~Gpio();

    /**
     * @brief Lecture de la couche d'accès utilisée
     */
    GpioAccessLayer accessLayer() const;

    /**
     * @brief Ouverture
     * @return true si ouvert
     */
    bool open();
    
    /**
     * @brief Fermeture du GPIO
     */
    void close();

    /**
     * @brief Indique si ouvert
     */
    bool isOpen() const;

    /**
     * @brief Nombre de broches de type GPIO (\c TypeGpio)
     */
    int size() const;

    /**
     * @brief Broche GPIO
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     * @return pointeur sur la broche
     */
    GpioPin * pin (int pin) const;

    /**
     * @brief Liste des broches de type GPIO
     *
     * Permet de parcourir les broches GPIO à l'aide des itérateurs de la STL,
     * par exemple pour mettre en entrée toutes les broches:
     * @code
     *    Gpio * g = new Gpio;
     *    // ....
     *    for (auto p = g->pin().cbegin(); p != g->pin().cend(); ++p) {
     *      // p est une std::pair: first = numéro et second = broche
     *      p->second->setMode(ModeInput);
     *    }
     * @endcode
     *
     * @return Container std::map des broches indexé avec la numérotation
     * \c numbering().
     */
    const std::map<int, std::shared_ptr<GpioPin>> & pin();

    /**
     * @brief Nombre de connecteurs
     */
    int connectors() const;

    /**
     * @brief Connecteur
     *
     * @param conn numéro du connecteur, les numéros ne sont pas forcément 
     * contiguës, pour parcourir les connecteurs on utilisera les itérateurs 
     * de la STL associés à \c connector(). Déclenche une exception 
     * std::out_of_range si en dehors des clous.
     * @return pointeur sur le connecteur
     */
    GpioConnector * connector (int conn) const;

    /**
     * @brief Liste des connecteurs de la carte
     *
     * Permet de parcourir les connecteurs à l'aide des itérateurs de la STL,
     * par exemple pour imprimer tous les connecteurs
     * @code
     *    Gpio * g = new Gpio;
     *    // ....
     *    for (auto p = g->connector().cbegin(); p != g->connector().cend(); ++p) {
     *      // p est une std::pair: first = numéro et second = connecteur
     *      cout << p->second;
     *    }
     * @endcode
     *
     * @return Container std::map des connecteurs indexé sur leur numéro
     */
    const std::map<int, std::shared_ptr<GpioConnector>> & connector();

    /**
     * @brief Mode actuel d'une broche
     *
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinMode mode (int pin) const;

    /**
     * @brief Modification du mode d'une broche
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
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    GpioPinPull pull (int pin) const;

    /**
     * @brief Modification de la résistance de tirage d'une broche
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
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void toggle (int pin);

    /**
     * @brief Lecture de l'état binaire d'une broche
     *
     * Déclenche une exeception std::system_error si la lecture est
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
     * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
     * une exception std::out_of_range si la broche n'existe pas
     */
    void release (int pin);

    /**
     * @brief Modifie la libération des broches à la fermeture
     *
     * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
     * close(). Cette fonction permet de modifier ce comportement.
     *
     * @param enable true active la libération, false la désactive.
     */
    void setReleaseOnClose (bool enable);

    /**
     * @brief Lecture la libération des broches à la fermeture
     *
     * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
     * close(). Cette fonction permet de lire l'état de cette directive
     *
     * @return true si validé, false sinon
     */
    bool releaseOnClose() const;

    /**
     * @brief Numérotation en cours
     *
     * La numérotation par défaut est \c NumberingLogical.
     */
    GpioPinNumbering numbering() const;

    /**
     * @brief Modifie la numérotation
     *
     */
    void setNumbering (GpioPinNumbering numbering);

    /**
     * @brief Nom de la numérotation en cours
     */
    const std::string & numberingName () const;

    /**
     * @brief Indique si le mode mise au point est actif
     */
    bool isDebug() const;
    
    /**
     * @brief Active le mode mise au point
     * 
     * Cela active l'affichage d'informations de mise au point de la couche
     * matérielle (GpioDevice).
     */
    void setDebug (bool enable);

  protected:
    /**
     * @brief Accès à la couche matérielle
     */
    GpioDevice * device() const;

    /**
     * @brief Constructeur sous jacent
     *
     * Doit être appelé par le constucteur par défaut défini dans la partie
     * spécifique à la plateforme.
     * @param device pointeur sur le driver de la plateforme
     * @param layer choix de la couche d'accès
     */
    explicit Gpio (GpioDevice * device, GpioAccessLayer layer);

  private:
    bool _roc; // Release On Close
    bool _isopen;
    GpioAccessLayer _accesslayer;
    GpioDevice * _device; // Accès à la couche matérielle
    GpioPinNumbering _numbering; // Numérotation en cours
    std::map<int, std::shared_ptr<GpioPin>> _pin; // Broches uniquement GPIO
    std::map<int, std::shared_ptr<GpioConnector>> _connector; // Connecteurs avec toutes les broches physiques
};
// <--- C++ part
// -----------------------------------------------------------------------------
#endif /* defined(__cplusplus) || defined(DOXYGEN) */

/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_H_ defined */
