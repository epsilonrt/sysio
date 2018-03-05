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

namespace Sysio {

  class Gpio;
  class Device;

  /**
   *  @addtogroup sysio_gpio_connector
   *  @{
   */

  /**
   * @class Connector
   * @author epsilonrt
   * @date 02/23/18
   * @brief Connecteur
   */
  class Connector {

    public:
      friend class Pin;
      friend class Gpio;

      /**
       * @brief Fonction de calcul du numéro d'une broche de connecteur
       * Le numéro d'une broche dépend de sa ligne \c row, de sa colonne \c column,
       * du nombre de colonnes \c columns et du modèle du connecteur.
       *
       * Chaque connecteur dispose d'une de ses fonctions fournis au constructeur par
       * le descripteur.
       */
      typedef int (* PinNumberFunc) (int row, int column, int columns);

      /**
       * @class Descriptor
       * @author epsilonrt
       * @date 02/23/18
       * @brief Descripteur d'un connecteur
       */
      class Descriptor {
        public:
          std::string name;
          int number;
          int rows;
          int columns;
          PinNumberFunc pinNumber;
          std::vector<Pin::Descriptor> pin;
      };

      /**
       * @brief Indique si ouvert
       */
      bool isOpen() const;

      /**
       * @brief Nom du connecteur
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
          Sysio::Gpio * board = new Sysio::Gpio;
          ...
          cout << board->connector(4);
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
      friend std::ostream& operator<< (std::ostream& os, const Connector * c);

      /**
       * @brief Accès au GPIO parent
       */
      Gpio * gpio() const;

      //------------------------------------------------------------------------
      //                          Accès aux broches
      //------------------------------------------------------------------------

      /**
       * @brief Broche du connecteur
       *
       * @param num numéro de broche dans la numérotation du connecteur. Déclenche
       * une exception std::out_of_range si la broche n'existe pas
       * @return référence sur la broche
       */
      Pin & pin (int num) const;

      /**
       * @brief Liste des broches du connecteur
       *
       * Permet de parcourir les broches à l'aide des itérateurs de la STL
       * par exemple pour mettre en entrée toutes les broches:
       * @code
       *    Gpio * g = new Gpio;
       *    // ....
       *    Connector * c = g->connector(1);
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
      const std::map<int, std::shared_ptr<Pin>> & pin ();

      /**
       * @brief Constructeur
       * 
       * Un connecteur ne peut être instancié que par une classe Board.
       *
       * @param parent pointeur sur le Gpio parent
       * @param desc pointeur sur la description du connecteur
       */
      Connector (Gpio * parent, const Descriptor * desc);

      /**
       * @brief Destructeur
       */
      virtual ~Connector();

    protected:
      /**
       * @brief Accès à la couche matérielle
       */
      Device * device() const;

      /**
       * @brief Ouverture
       */
      bool open();

      /**
       * @brief Fermeture du GPIO
       */
      void close();

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
      bool _isopen;
      Gpio * _parent;
      const Descriptor * _descriptor; // descripteur
      std::map<int, std::shared_ptr<Pin>> _pin; // toutes les broches
  };
}
/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_CONNECTOR_H_ defined */
