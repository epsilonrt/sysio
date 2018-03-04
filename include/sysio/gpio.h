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

namespace Gpio {

  /**
   * @enum AccessLayer
   * @brief Choix de la façon d'accéder aux broches du GPIO
   *
   * En fonction de la plateforme, le module GPIO peut utiliser différentes façon
   * pour accéder aux broches.
   */
  enum AccessLayer {
    AccessLayerAuto  = 0, //< Choix automatique (à utiliser la plupart du temps), c'est la plateforme qui choisit par le GpioDevice
    AccessLayerIoMap = 0x0001, ///< Accès par les registres du SOC, la plus performante mais il faut être sûr que cela est disponible sur la plateforme
    AccessLayerSysFs = 0x0002, ///< Accès par l'interface "utilisateur" dans /sys/class/gpio, la plus générique
    AccessLayerAll = AccessLayerIoMap + AccessLayerSysFs ///< Le mieux, toutes les fonctions sont disponibles
  };
}

#include <sysio/gpioconnector.h>

namespace Gpio {

  class Device;

  /**
   * @class Board
   * @author epsilonrt
   * @date 02/22/18
   * @brief GPIO
   */
  class Board {
    public:
      friend class Connector;

      /**
       * @class Descriptor
       * @author epsilonrt
       * @date 02/23/18
       * @brief Descripteur d'une carte équipée d'un GPIO
       */
      class Descriptor {
        public:
          std::string name; ///< Nom de la carte
          std::vector<Connector::Descriptor> connector; ///< Descripteurs des connecteurs
      };

      /**
       * @brief Constructeur par défaut
       * @param layer choix de la couche d'accès, AccessLayerAuto par défaut, dans
       * ce cas, le choix est laissé à la couche GpioDevice (conseillé).
       */
      Board (AccessLayer layer = AccessLayerAuto);

      /**
       * @brief Destructeur
       */
      virtual ~Board();

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
       * @brief Nom de la carte
       */
      const std::string & name() const;

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
      Pin::Numbering numbering() const;

      /**
       * @brief Modifie la numérotation
       *
       */
      void setNumbering (Pin::Numbering numbering);

      /**
       * @brief Nom de la numérotation en cours
       */
      const std::string & numberingName () const;

      /**
       * @brief Lecture de la couche d'accès utilisée
       */
      AccessLayer accessLayer() const;

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

      //------------------------------------------------------------------------
      //                          Accès aux connecteurs
      //------------------------------------------------------------------------
      
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
      Connector * connector (int conn) const;

      /**
       * @brief Liste des connecteurs de la carte
       *
       * Permet de parcourir les connecteurs à l'aide des itérateurs de la STL,
       * par exemple pour imprimer tous les connecteurs
       * @code
       *    Board * g = new Board;
       *    // ....
       *    for (auto p = g->connector().cbegin(); p != g->connector().cend(); ++p) {
       *      // p est une std::pair: first = numéro et second = connecteur
       *      cout << p->second;
       *    }
       * @endcode
       *
       * @return Container std::map des connecteurs indexé sur leur numéro
       */
      const std::map<int, std::shared_ptr<Connector>> & connector();
      
      
      //------------------------------------------------------------------------
      //                          Accès aux broches
      //------------------------------------------------------------------------

      /**
       * @brief Nombre de broches de type GPIO sur la carte(\c TypeGpio)
       */
      int size() const;

      /**
       * @brief Broche GPIO
       *
       * @param pin numéro de broche dans la numérotation \c numbering(). Déclenche
       * une exception std::out_of_range si la broche n'existe pas
       * @return pointeur sur la broche
       */
      Pin * pin (int pin) const;

      /**
       * @brief Liste des broches de type GPIO
       *
       * Permet de parcourir les broches GPIO à l'aide des itérateurs de la STL,
       * par exemple pour mettre en entrée toutes les broches:
       * @code
       *    Gpio::Board * g = new Gpio::Board;
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
      const std::map<int, std::shared_ptr<Pin>> & pin();

    protected:
      /**
       * @brief Accès à la couche matérielle
       */
      Device * device() const;

      /**
       * @brief Constructeur sous jacent
       *
       * Doit être appelé par le constucteur par défaut défini dans la partie
       * spécifique à la plateforme.
       * @param device pointeur sur le driver de la plateforme
       * @param layer choix de la couche d'accès
       */
      explicit Board (Device * device, AccessLayer layer);

    private:
      bool _roc; // Release On Close
      bool _isopen;
      AccessLayer _accesslayer;
      Device * _device; // Accès à la couche matérielle
      Pin::Numbering _numbering; // Numérotation en cours
      std::map<int, std::shared_ptr<Pin>> _pin; // Broches uniquement GPIO
      std::map<int, std::shared_ptr<Connector>> _connector; // Connecteurs avec toutes les broches physiques
  };
}
// <--- C++ part
// -----------------------------------------------------------------------------
#endif /* defined(__cplusplus) || defined(DOXYGEN) */

/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_H_ defined */
