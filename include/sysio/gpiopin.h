/**
 * @file
 * @brief Broche et connecteurs GPIO
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

/**
 *  @addtogroup sysio_gpio
 *
 *  @{
 */

/**
 * @enum GpioPinMode
 * @brief Mode de fonctionnement d'une broche
 */
enum GpioPinMode {
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
 * @enum GpioPinPull
 * @brief Type de résistances de tirage d'une broche
 */
enum GpioPinPull {
  PullOff = 0, ///< Résistance désactivée
  PullDown, ///< Résistance de tirage à l'état bas
  PullUp, ///< Résistance de tirage à l'état haut
  PullUnknown = -1
};

/**
 * @enum GpioPinEdge
 * @brief Front de déclenchement d'une broche
 */
enum GpioPinEdge {
  EdgeNone = 0, ///< Pas de détection
  EdgeRising,   ///< Front montant
  EdgeFalling,  ///< Front Descendant
  EdgeBoth,     ///< Front montant et descendant
  EdgeUnknown = -1
};

/**
 * @enum GpioPinNumbering
 * @brief Numérotation des broches utilisées par le GPIO
 */
enum GpioPinNumbering {
  NumberingLogical = 0, ///< Numérotation logique (défaut), débute à 0
  NumberingMcu, ///< Numérotation du microcontroleur, débute à 0
  NumberingSystem, ///< Numérotation du système d'exploitation, débute à 0
  NumberingUnknown = -1
};

/**
 * @enum GpioPinType
 * @brief Types de broches d'un connecteur
 */
enum  GpioPinType {
  TypeGpio = 0,
  TypePower,
  TypeUsb,
  TypeSound,
  TypeVideo,
  TypeNotConnected,
  TypeUnknown = -1
};

/* internal public classes ================================================== */

/**
 * @class GpioPinNumber
 * @author epsilonrt
 * @date 02/23/18
 * @brief Numéro de broche
 */
class GpioPinNumber {
  public:
    int logical; ///< Numéro dans numérotation logique (commence à 0)
    int mcu; ///< Numéro dans numérotation du microcontrôleur (commence à 0)
    int system; ///< Numéro dans numérotation du système d'exploitation (commence à 0)
    int row; ///< Numéro de ligne dans la connecteur (commence à 1)
    int column; ///< Numéro de colonne dans la connecteur (commence à 1)
};

/**
 * @class GpioPinDescriptor
 * @author epsilonrt
 * @date 02/22/18
 * @brief Descripteur d'une broche
 */
class GpioPinDescriptor {
  public:
    GpioPinType type; ///< Type de broche
    GpioPinNumber num; ///< Numéros
    std::map<GpioPinMode, std::string> name; ///< Noms
};

class Gpio;
class GpioDevice;
class GpioConnector;

/**
 * @class GpioPin
 * @author epsilonrt
 * @date 02/22/18
 * @brief Broche d'un connecteur
 */
class GpioPin {

  public:
    friend class GpioConnector;

    /**
     * @brief Constructeur
     *
     * @param parent pointeur sur le GPIO parent
     * @param desc pointeur sur la description
     */
    GpioPin (GpioConnector * parent, const GpioPinDescriptor * desc);

    /**
     * @brief Desctructeur
     */
    virtual ~GpioPin();

    /**
     * @brief Indique si ouvert
     */
    bool isOpen() const;

    /**
     * @brief Type de broche
     */
    GpioPinType type() const;

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
    int number (GpioPinNumbering numbering) const;

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
    const std::string & name() const;

    /**
     * @brief Mode actuel
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     * @return le mode actuel
     */
    GpioPinMode mode() const;

    /**
     * @brief Modification du mode
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param mode nouveau mode, une exception std::invalid_argument est
     * déclenchée si le mode demandé n'est pas supporté.
     */
    void setMode (GpioPinMode mode);

    /**
     * @brief Résistance de tirage d'une broche
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     * @return la résistance actuellement appliquée
     */
    GpioPinPull pull() const;

    /**
     * @brief Modification de la résistance de tirage
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     *
     * @param pull nouvelle résistance, PullOff pour désactiver, une exception
     * std::invalid_argument est déclenchée si la résistance demandée n'est pas
     * supportée.
     */
    void setPull (GpioPinPull pull);

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
     * @brief Lecture de l'état binaire d'une broche
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio et une exeception std::system_error si la lecture est
     * impossible sur le système.
     *
     * @return true pour un état logique haut (VccIo).
     */
    bool read() const;

    /**
     * @brief Restaure le mode et la résistance de pull-up d'une broche
     *
     * La broche correspondante est remise dans son état initial mémorisé avant
     * le premier appel à \c setMode() ou/et \c setPull().
     *
     * Déclenche une exception std::domain_error si la broche n'est pas de
     * type \c TypeGpio.
     */
    void release();
    
    /**
     * @brief Force ou non l'utilisation de SysFs
     * 
     * Si le Gpio parent n'autorise que la couche d'accès SysFs (AccessLayerSysFs), 
     * cette fonction ne fait rien.
     */
    bool forceUseSysFs (bool enable);
    
    /**
     * @brief Indique si la broche utilise SysFs
     */
    bool useSysFs() const;
    
    /**
     * @brief Couches d'accès autorisées
     */
    GpioAccessLayer accessLayer() const;

    /**
     * @brief Attente passive d'un front
     *
     * L'implémentation utilise les interruptions matérielles gérées par SysFs. \n
     * Toutes les broches du GPIO ne disposent pas ce cette fonctionnalité (cela
     * dépend de la plateforme). Généralement, on pourra identifier une telle 
     * broche du GPIO ou du SOC par un INT se trouvant dans le nom.
     * Par exemple, la broche PA0 sur un SOC AllWinner H3 a cette fonctionnalité
     * car sa fonction ALT6 est PA_EINT0. Si les interruptions ne sont pas 
     * supportées par la broche, un appel à \c waitForInt() déclenchera une
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
     * @return la valeur binaire après le front
     */
    bool waitForInt (GpioPinEdge edge, int timeout_ms = -1);

    /**
     * @brief Modification du front de déclenchement
     * 
     * L'accès par SysFs doit donc être autorisée dans le Gpio parent 
     * (AccessLayerSysFs ou AccessLayerAll) ou par l'appel à forceUseSysFs(true). \n
     * Si la broche n'est pas en mode SysFs et que cet accès est autorisé, la 
     * broche est passée automatiquement en mode SysFs. 
     * @param edge choix du front
     */
    void setEdge (GpioPinEdge edge);

    /**
     * @brief Front de déclenchement
     * 
     * @return le front actuel, si la broche n'est pas en mode SysFs, 
     * EdgeUnknown est retourné.
     */
    GpioPinEdge edge() const;

    /**
     * @brief Nom correspondant à un mode
     *
     * @param mode mode demandée, déclenche une exception std::out_of_range il
     * n'est pas supporté par cette broche.
     * @return le nom de la broche correspondant au mode
     */
    const std::string & name (GpioPinMode mode) const;

    /**
     * @brief Nom du mode actuel
     */
    const std::string & modeName() const;

    /**
     * @brief Nom d'un mode
     */
    const std::string & modeName (GpioPinMode mode) const;

    /**
     * @brief Nom de la résistance de tirage actuelle
     */
    const std::string & pullName () const;

    /**
     * @brief Nom d'une résistance de tirage
     */
    static const std::string & pullName (GpioPinPull n);

    /**
     * @brief Nom du type de la broche
     */
    const std::string & typeName() const;

    /**
     * @brief Nom d'un type
     */
    static const std::string & typeName (GpioPinType t);

    /**
     * @brief Nom d'une numérotation
     */
    static const std::string & numberingName (GpioPinNumbering n);

    /**
     * @brief Nom d'un front
     */
    static const std::string & edgeName (GpioPinEdge e);

    /**
     * @brief Listes des modes avec leurs noms
     *
     * Permet de parcourir les modes à l'aide des itérateurs de la STL
     */
    const std::map<GpioPinMode, std::string> & modes () const;

    /**
     * @brief Listes des résistances de tirage avec leurs noms
     *
     * Permet de parcourir les numérotations à l'aide des itérateurs de la STL.
     */
    static const std::map<GpioPinPull, std::string> & pulls ();

    /**
     * @brief Listes des types avec leurs noms
     *
     * Permet de parcourir les modes à l'aide des itérateurs de la STL
     */
    static const std::map<GpioPinType, std::string> & types ();

    /**
     * @brief Listes des numérotations avec leurs noms
     *
     * Permet de parcourir les numérotations à l'aide des itérateurs de la STL.
     */
    static const std::map<GpioPinNumbering, std::string> & numberings ();

    /**
     * @brief Listes des fronts posibles avec leurs noms
     *
     * Permet de parcourir les fronts à l'aide des itérateurs de la STL.
     */
    static const std::map<GpioPinEdge, std::string> & edges ();

    /**
    * @brief Accès au connecteur parent
    */
    GpioConnector * connector() const;

    /**
     * @brief Accès au GPIO parent
     */
    Gpio * gpio() const;

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
    GpioConnector * _parent;
    const GpioPinDescriptor * _descriptor;
    GpioPinMode _holdMode;
    GpioPinPull _holdPull;
    bool _holdState;
    bool _useSysFs;
    int _holdExported;
    int _valueFd;
    static const std::map<GpioPinPull, std::string> _pulls;
    static const std::map<GpioPinType, std::string> _types;
    static const std::map<GpioPinNumbering, std::string> _numberings;
    static const std::map<GpioPinMode, std::string> _sysfsmodes;
    static const std::map<GpioPinEdge, std::string> _edges;
    static const std::map<std::string, GpioPinEdge> _str2edge;
    static const std::map<std::string, GpioPinMode> _str2mode;
    static std::string _syspath;

    static bool directoryExist (const std::string & dname);

    void holdMode();
    void holdPull();
    bool sysFsEnable (bool enable);
    void sysFsExport (bool enable);
    bool sysFsOpen();
    void sysFsClose();
    bool sysFsIsExport () const;
    void sysFsWriteFile (const char * n, const std::string & v);
    std::string sysFsReadFile (const char * n) const;
    bool sysFsFileExist (const char * n) const;
};

/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_PIN_H_ defined */
