/**
 * @file
 * @brief GPIO Pin
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_PIN_H_
#define _SYSIO_GPIO_PIN_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

namespace Sysio {

  class Gpio;
  class Device;
  class Connector;

  /**
   *  @addtogroup sysio_gpio
   *  @{
   */


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
  /**
   * @}
   */

  /**
   *  @addtogroup sysio_gpio_pin
   *  @{
   */

  /**
   * @class Pin
   * @author epsilonrt
   * @date 02/22/18
   * @brief Broche d'un connecteur
   */
  class Pin {

    public:
      friend class Connector;

      /**
       * @enum Mode
       * @brief Mode de fonctionnement d'une broche
       */
      enum Mode {
        ModeInput = 0, ///< Entrée
        ModeOutput, ///< Sortie
        ModeDisabled, ///< Broche désactivée
        ModePwm, ///< sortie PWM
        ModeAlt0, ///< Fonction alternative 0
        ModeAlt1, ///< Fonction alternative 1
        ModeAlt2, ///< Fonction alternative 2
        ModeAlt3, ///< Fonction alternative 3
        ModeAlt4, ///< Fonction alternative 4
        ModeAlt5, ///< Fonction alternative 5
        ModeAlt6, ///< Fonction alternative 6
        ModeAlt7, ///< Fonction alternative 7
        ModeAlt8, ///< Fonction alternative 8
        ModeAlt9, ///< Fonction alternative 9
        ModeUnknown = -1
      };

      /**
       * @enum Pull
       * @brief Type de résistances de tirage d'une broche
       */
      enum Pull {
        PullOff = 0, ///< Résistance désactivée
        PullDown, ///< Résistance de tirage à l'état bas
        PullUp, ///< Résistance de tirage à l'état haut
        PullUnknown = -1
      };

      /**
       * @enum Edge
       * @brief Front de déclenchement d'une broche
       */
      enum Edge {
        EdgeNone = 0, ///< Pas de détection
        EdgeRising,   ///< Front montant
        EdgeFalling,  ///< Front Descendant
        EdgeBoth,     ///< Front montant et descendant
        EdgeUnknown = -1
      };

      /**
       * @enum Numbering
       * @brief Numérotation des broches utilisées par le GPIO
       */
      enum Numbering {
        NumberingLogical = 0, ///< Numérotation logique (défaut), débute à 0
        NumberingMcu, ///< Numérotation du microcontroleur, débute à 0
        NumberingSystem, ///< Numérotation du système d'exploitation, débute à 0
        NumberingUnknown = -1
      };

      /**
       * @enum Type
       * @brief Types de broches d'un connecteur
       */
      enum  Type {
        TypeGpio = 0, ///< Broche GPIO
        TypePower, ///< Broche d'alimentation
        TypeUsb, ///< Broche port USB
        TypeSound, ///< Broche audio
        TypeVideo, ///< Broche video
        TypeNotConnected, ///< Broche non conectée
        TypeUnknown = -1
      };

      /**
       * @class Number
       * @author epsilonrt
       * @date 02/23/18
       * @brief Numéro de broche
       */
      class Number {
        public:
          int logical; ///< Numéro dans numérotation logique (commence à 0)
          int mcu; ///< Numéro dans numérotation du microcontrôleur (commence à 0)
          int system; ///< Numéro dans numérotation du système d'exploitation (commence à 0)
          int row; ///< Numéro de ligne dans la connecteur (commence à 1)
          int column; ///< Numéro de colonne dans la connecteur (commence à 1)
      };

      /**
       * @class Descriptor
       * @author epsilonrt
       * @date 02/22/18
       * @brief Descripteur d'une broche
       */
      class Descriptor {
        public:
          Type type; ///< Type de broche
          Number num; ///< Numéros
          std::map<Mode, std::string> name; ///< Noms
      };


      //------------------------------------------------------------------------
      //                          Opérations
      //------------------------------------------------------------------------

      /**
       * @brief Mode actuel d'une broche de type GPIO
       *
       * @return le mode actuel, si la broche n'est pas
       * de type GPIO, ModeUnknown est retourné.
       */
      Mode mode();

      /**
       * @brief Modification du mode d'une broche de type GPIO
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio.
       *
       * @param mode nouveau mode, une exception std::invalid_argument est
       * déclenchée si le mode demandé n'est pas supporté.
       */
      void setMode (Mode mode);

      /**
       * @brief Résistance de tirage d'une broche de type GPIO
       *
       * @return la résistance actuellement appliquée, si la broche n'est pas
       * de type GPIO, PullUnknown est retourné.
       */
      Pull pull();

      /**
       * @brief Modification de la résistance de tirage d'une broche de type GPIO
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio.
       *
       * @param pull nouvelle résistance, PullOff pour désactiver, une exception
       * std::invalid_argument est déclenchée si la résistance demandée n'est pas
       * supportée.
       */
      void setPull (Pull pull);

      /**
       * @brief Front de déclenchement
       *
       * @return le front actuel, si la broche n'est pas en mode SysFs,
       * EdgeUnknown est retourné.
       */
      Edge edge();

      /**
       * @brief Modification du front de déclenchement
       *
       * L'accès par SysFs doit donc être autorisée dans le Gpio parent
       * (AccessLayerSysFs ou AccessLayerAll) ou par l'appel à forceUseSysFs(true). \n
       * Si la broche n'est pas en mode SysFs et que cet accès est autorisé, la
       * broche est passée automatiquement en mode SysFs.
       * @param edge choix du front
       */
      void setEdge (Edge edge);

      /**
       * @brief Modification de l'état binaire d'une sortie
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
       *
       * @param value true pour un état logique haut (VccIo).
       */
      void write (bool value);

      /**
       * @brief Bascule de l'état binaire d'une sortie
       *
       * Si la sortie est à l'état bas, elle passe à l'état haut et inversement.
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio ou si elle n'est pas en sortie (ModeOutput).
       */
      void toggle();

      /**
       * @brief Lecture de l'état binaire d'une broche de type GPIO
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio et une exeception std::system_error si la lecture est
       * impossible sur le système.
       *
       * @return true pour un état logique haut (VccIo).
       */
      bool read() const;

      /**
       * @brief Restaure le mode et la résistance de pull-up d'une broche de type GPIO
       *
       * La broche correspondante est remise dans son état initial mémorisé avant
       * le premier appel à \c setMode() ou/et \c setPull().
       *
       * Déclenche une exception std::domain_error si la broche n'est pas de
       * type \c TypeGpio.
       */
      void release();

      /**
       * @brief Attente passive d'un front
       *
       * L'implémentation utilise les interruptions matérielles gérées par SysFs. \n
       * Toutes les broches du GPIO ne disposent pas ce cette fonctionnalité (cela
       * dépend de la plateforme). Généralement, on pourra identifier une telle
       * broche du GPIO ou du SOC par un INT se trouvant dans le nom.
       * Par exemple, la broche PA0 sur un SOC AllWinner H3 a cette fonctionnalité
       * car sa fonction ALT6 est PA_EINT0. Si les interruptions ne sont pas
       * supportées par la broche, un appel à \c waitForInterrupt() déclenchera une
       * exeception std::system_error avec le code ENOSYS (function_not_supported). \n
       * L'accès par SysFs doit donc être autorisée dans le Gpio parent
       * (AccessLayerSysFs ou AccessLayerAll) ou par l'appel à forceUseSysFs(true). \n
       * Si la broche n'est pas en mode SysFs et que cet accès est autorisé, la
       * broche est passée automatiquement en mode SysFs.
       *
       * @param edge front de déclenchement
       * @param timeout temps maximal d'attente en millisecondes. -1 pour l'infini.
       * Si une autre valeur est fournie et que le délai est atteint avant que le
       * front survienne, une exception std::system_error avec le code ETIME est
       * déclenchée.
       */
      void waitForInterrupt (Edge edge, int timeout_ms = -1);

      /**
       * @brief Routine d'interruption
       * 
       * Une routine d'interruption ne prend et ne renvoie aucun paramètre.
       */
      typedef void (* Isr) (void);

      /**
       * @brief Installe une routine d'interruption (Isr)
       * 
       * Cette fonction créée un thread qui attend l'arrivée d'une interruption
       * déclenchée par chaque front edge et exécute la fonction isr.
       * 
       * @param isr fonction exécuté à chaque interruption
       * @param edge front déclenchant l'interruption
       */
      void attachInterrupt (Isr isr, Edge edge);

      /**
       * @brief Désinstalle la routine d'interruption
       * 
       * Le thread est détruit.
       */
      void detachInterrupt();

      //------------------------------------------------------------------------
      //                          Propriétés
      //------------------------------------------------------------------------

      /**
       * @brief Type de broche
       */
      Type type() const;

      /**
       * @brief Numéro de la broche dans la numérotation logique \c NumberingLogical
       *
       * Cette numérotation commence à 0.
       *
       * @return Numéro de broche logique, -1 si la broche n'est pas de type GPIO ( \c TypeGpio)
       */
      int logicalNumber () const;

      /**
       * @brief Numéro de la broche dans la numérotation microcontrôleur \c NumberingMcu
       *
       * Cette numérotation commence à 0.
       *
       * @return Numéro de broche microcontrôleur, -1 si la broche n'est pas de type GPIO ( \c TypeGpio)
       */
      int mcuNumber () const;

      /**
       * @brief Numéro de la broche dans la numérotation système \c NumberingSystem
       *
       * Cette numérotation commence à 0.
       *
       * @return Numéro de broche système, -1 si la broche n'est pas de type GPIO ( \c TypeGpio)
       */
      int systemNumber () const;

      /**
       * @brief Numéro de la broche dans son connecteur
       *
       * Cette numérotation commence à 1.
       *
       * @return Numéro de broche connecteur
       */
      int physicalNumber () const;

      /**
      * @brief Numéro de la broche dans la numérotation demandé.
       *
       * La numérotation commence à 0.
      *
      * @param numbering numérotation demandée, déclenche
      * une exception std::invalid_argument si incorrecte
      * @return Numéro de broche, -1 si la broche n'est pas de type GPIO ( \c TypeGpio)
      */
      int number (Numbering numbering) const;

      /**
       * @brief Numéro de ligne dans la connecteur
       *
       * @return Numéro de ligne, commence à 1
       */
      int row() const;

      /**
       * @brief Numéro de colonne dans la connecteur
       *
       * @return Numéro de colonne, commence à 1
       */
      int column() const;

      /**
       * @brief Nom de la broche
       *
       * @return nom qui correspond au mode renvoyé par \c mode() ou le nom
       * générique si le mode actuel n'est pas associé à un nom ou si la broche
       * n'est pas de type TypeGpio.
       */
      const std::string & name();

      /**
       * @brief Indique si la broche utilise SysFs
       */
      bool useSysFs() const;

      /**
       * @brief Force ou non l'utilisation de SysFs
       *
       * Si le Gpio parent n'autorise que la couche d'accès SysFs (AccessLayerSysFs),
       * cette fonction ne fait rien.
       */
      bool forceUseSysFs (bool enable);

      /**
      * @brief Accès au connecteur parent
      */
      Connector * connector() const;

      /**
       * @brief Accès au GPIO parent
       */
      Gpio * gpio() const;

      /**
       * @brief Couches d'accès autorisées
       */
      AccessLayer accessLayer() const;

      /**
       * @brief Indique si ouvert
       */
      bool isOpen() const;

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

      /**
       * @brief Nom correspondant à un mode
       *
       * @param mode mode demandée, déclenche une exception std::out_of_range il
       * n'est pas supporté par cette broche.
       * @return le nom de la broche correspondant au mode
       */
      const std::string & name (Mode mode) const;

      /**
       * @brief Nom du mode actuel
       */
      const std::string & modeName();

      /**
       * @brief Nom d'un mode
       */
      const std::string & modeName (Mode mode) const;

      /**
       * @brief Nom de la résistance de tirage actuelle
       */
      const std::string & pullName ();

      /**
       * @brief Nom d'une résistance de tirage
       */
      static const std::string & pullName (Pull n);

      /**
       * @brief Nom du type de la broche
       */
      const std::string & typeName() const;

      /**
       * @brief Nom d'un type
       */
      static const std::string & typeName (Type t);

      /**
       * @brief Nom d'une numérotation
       */
      static const std::string & numberingName (Numbering n);

      /**
       * @brief Nom d'un front
       */
      static const std::string & edgeName (Edge e);

      /**
       * @brief Listes des modes avec leurs noms
       *
       * Permet de parcourir les modes à l'aide des itérateurs de la STL
       */
      const std::map<Mode, std::string> & modes () const;

      /**
       * @brief Listes des résistances de tirage avec leurs noms
       *
       * Permet de parcourir les numérotations à l'aide des itérateurs de la STL.
       */
      static const std::map<Pull, std::string> & pulls ();

      /**
       * @brief Listes des types avec leurs noms
       *
       * Permet de parcourir les modes à l'aide des itérateurs de la STL
       */
      static const std::map<Type, std::string> & types ();

      /**
       * @brief Listes des numérotations avec leurs noms
       *
       * Permet de parcourir les numérotations à l'aide des itérateurs de la STL.
       */
      static const std::map<Numbering, std::string> & numberings ();

      /**
       * @brief Listes des fronts posibles avec leurs noms
       *
       * Permet de parcourir les fronts à l'aide des itérateurs de la STL.
       */
      static const std::map<Edge, std::string> & edges ();

      /**
       * @brief Constructeur
       *
       * Une broche ne peut être instanciée que par une classe Connector.
       *
       * @param parent pointeur sur le GPIO parent
       * @param desc pointeur sur la description
       */
      Pin (Connector * parent, const Descriptor * desc);

      /**
       * @brief Desctructeur
       */
      virtual ~Pin();

    protected:
      /**
       * @brief Accès à la couche matérielle
       */
      Device * device() const;

      /**
       * @brief Ouverture
       * @return true si ouvert
       */
      bool open();

      /**
       * @brief Fermeture du GPIO
       */
      void close();

    private:
      bool _isopen;
      Connector * _parent;
      const Descriptor * _descriptor;
      Mode _holdMode;
      Pull _holdPull;
      bool _holdState;
      bool _useSysFs;
      int _holdExported;
      int _valueFd;

      Edge _edge;
      Mode _mode;
      Pull _pull;

      std::atomic<int> _run;  // indique au thread de continuer
      std::thread _thread;

      static const std::map<Pull, std::string> _pulls;
      static const std::map<Type, std::string> _types;
      static const std::map<Numbering, std::string> _numberings;
      static const std::map<Mode, std::string> _sysfsmodes;
      static const std::map<Edge, std::string> _edges;
      static const std::map<std::string, Edge> _str2edge;
      static const std::map<std::string, Mode> _str2mode;
      static std::string _syspath;

      static bool directoryExist (const std::string & dname);
      static void * irqThread (std::atomic<int> & run, int fd, Isr isr);

      void holdMode();
      void holdPull();
      void readPull ();
      void writePull();
      void readMode();
      void writeMode ();
      void readEdge();
      void writeEdge ();

      bool sysFsEnable (bool enable);
      void sysFsExport (bool enable);
      bool sysFsIsExport () const;
      bool sysFsOpen();
      void sysFsClose();

      void sysFsGetEdge();
      void sysFsSetEdge ();
      void sysFsGetMode();
      void sysFsSetMode ();

      static int sysFsPoll (int fd, int timeout_ms = -1);
      static int sysFsRead (int fd);
      static int sysFsWrite (int fd, bool value);

      void sysFsWriteFile (const char * n, const std::string & v);
      std::string sysFsReadFile (const char * n) const;
      bool sysFsFileExist (const char * n) const;
  };
}
/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_PIN_H_ defined */
