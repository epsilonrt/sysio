/**
 * @file
 * @brief GPIO
 *
 * Copyright © 2015-2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#ifndef _SYSIO_GPIO_H_
#define _SYSIO_GPIO_H_
#include <sysio/defs.h>

/**
 *  @defgroup sysio_gpio GPIO
 *
 *  Ce module fournit les fonctions permettant de contrôler les broches
 *  d'entrées-sorties à usage général.
 *  @{
 */

/*
 *  @example gpio/write/sysio_test_gpio_write.c
 *  Allumage de leds
 *  @example gpio/read/sysio_test_gpio_read.c
 *  Lecture de boutons poussoirs
 *  @example gpio/getmode/sysio_test_gpio_getmode.c
 *  Lecture du mode de toutes les broches du GPIO
 */

#ifdef __cplusplus
extern "C" {
// -----------------------------------------------------------------------------
// C part --->
#endif

/* constants ================================================================ */
/**
 * Permet d'indiquer qu'une broche est inutilisée
 */
#define GPIO_PIN_UNUSED (-1)

/**
 * @enum eGpioMode
 * @brief Type de broche
 * Les modes spécifiques à une plateforme sont définis dans le fichier d'en-tête
 * de la plateforme (rpi.h pour le Raspberry Pi, nanopi.h pour le NanoPi ...)
 */
typedef enum  {
  eModeInput    = 0,  /**< entrée */
  eModeOutput   = 1,  /**< sortie */
  eModeAlt0     = 4,  /**< fonction auxiliaire n° 0 (Raspberry Pi) */
  eModeAlt1     = 5,  /**< fonction auxiliaire n° 1 (Raspberry Pi) */
  eModeAlt2     = 6,  /**< fonction auxiliaire n° 2 (Raspberry Pi) */
  eModeAlt3     = 7,  /**< fonction auxiliaire n° 3 (Raspberry Pi) */
  eModeAlt4     = 3,  /**< fonction auxiliaire n° 4 (Raspberry Pi) */
  eModeAlt5     = 2,  /**< fonction auxiliaire n° 5 (Raspberry Pi) */
  eModeFunc2    = 2,  /**< fonction auxiliaire n° 2 (NanoPi) */
  eModeFunc3    = 3,  /**< fonction auxiliaire n° 3 (NanoPi) */
  eModeFunc4    = 4,  /**< fonction auxiliaire n° 4 (NanoPi) */
  eModeFunc5    = 5,  /**< fonction auxiliaire n° 5 (NanoPi) */
  eModeFunc6    = 6,  /**< fonction auxiliaire n° 6 (NanoPi) */
  eModeDisabled = 7,  /**< Broche désactivée (NanoPi) */
  eModePwm      = 0x8002,  /**< sortie PWM */
  eModeError    = -1 /**< Erreur */
}
                eGpioMode;

/**
 * @enum eGpioPull
 * @brief Type de résistances de tirage
 */
typedef enum  {
  ePullOff   = 0,  /**< résistance désactivée */
  ePullDown  = 1,  /**< résistance de tirage à l'état bas */
  ePullUp    = 2,  /**< résistance de tirage à l'état haut */
  ePullError = -1  /**< Erreur */
} eGpioPull;

/**
 * @enum ePinNumbering
 * @brief Numérotation des broches utilisées par le gpio
 */
typedef enum  {
  eNumberingLogical   = 0,  /**< Numérotation logique spécifique définie pour la plateforme (défaut), débute à 0 */
  eNumberingMcu       = 1,  /**< Numérotation du microcontroleur (BCM pour le RPi), débute à 0 */
  eNumberingPhysical  = 2,  /**< Numérotation du connecteur GPIO de la carte, débute à 1 */
} eGpioNumbering;

/* structures =============================================================== */
/**
 * @brief Port d'entrée-sortie à usage général (GPIO)
 *
 * La structure est opaque pour l'utilisateur.
 */
typedef struct xGpio xGpio;

/* internal public functions ================================================ */
/**
 * @brief Ouverture d'un GPIO
 *
 * @param args pointeur sur une variable de configuration dépendant de
 *        l'architecture, mettre à NULL si inutilisée
 * @return le pointeur sur le GPIO, NULL en cas d'erreur
 */
xGpio * xGpioOpen (void * args);

/**
 * @brief Fermeture d'un GPIO
 *
 * Le port est fermé si plus aucun thread ne l'utilise
 *
 * @param gpio le pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioClose (xGpio * gpio);

/**
 * @brief Indique si le GPIO est ouvert
 *
 * @param gpio le pointeur sur le GPIO
 * @return true si ouvert, false sinon
 */
bool bGpioIsOpen (const xGpio * gpio);

/**
 * @brief Modifie le type de numérotation
 *
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioSetNumbering (eGpioNumbering eNum, xGpio * gpio);

/**
 * @brief Type de numérotation
 *
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
eGpioNumbering eGpioGetNumbering (const xGpio * gpio);

/**
 * @brief Chaîne de caractère correspondant à une numérotation
 * @param eNum numérotation
 * @return Chaîne de caractère correspondant à une numérotation
 */
const char * sGpioNumberingToStr (eGpioNumbering eNum);

/**
 * @brief Modifie le type d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param iPin le numéro de la broche
 * @param eMode le type de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioSetMode (int iPin, eGpioMode eMode, xGpio * gpio);

/**
 * @brief Lecture du type actuel d'une broche
 *
 * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
 * dépend de l'architecture matérielle.
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return le type, eModeError si erreur
 */
eGpioMode eGpioGetMode (int iPin, xGpio * gpio);

/**
 * @brief Modification de la résistance de tirage d'une broche
 *
 * @param iPin le numéro de la broche
 * @param ePull le type de la résistance (ePullOff pour désactiver)
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioSetPull (int iPin, eGpioPull ePull, xGpio * gpio);

/**
 * @brief Lecture du type du type résistance de tirage d'une broche
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return le type de la résistance, ePullError si erreur
 */
eGpioPull eGpioGetPull (int iPin, xGpio * gpio);

/**
 * @brief Teste la validité d'un numéro de broche
 *
 * @param iPin le numéro de la broche (numérotation sélectionnée)
 * @param gpio pointeur sur le GPIO
 * @return true si numéro valide
 */
bool bGpioIsValid (int iPin, xGpio * gpio);

/**
 * @brief Lecture du nombre de broches du port GPIO
 *
 * @param gpio pointeur sur le GPIO
 * @return nombre de broches, -1 si erreur
 */
int iGpioGetSize (const xGpio * gpio);

/**
 * @brief Modification de l'état binaire d'une sortie
 *
 * @param iPin le numéro de la broche
 * @param bValue valeur binaire à appliquer
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioWrite (int iPin, bool bValue, xGpio * gpio);

/**
 * @brief Bascule de l'état binaire d'une sortie
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioToggle (int iPin, xGpio * gpio);

/**
 * @brief Modification de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param iMask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour modifier toutes les
 *        broches.
 * @param bValue valeur binaire à appliquer aux broches
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioWriteAll (int64_t iMask, bool bValue, xGpio * gpio);

/**
 * @brief Bascule de l'état binaire de plusieurs sorties
 *
 * Cette fonction n'a aucune action sur les broches qui ne sont pas en sortie.
 *
 * @param iMask masque de sélection des broches à modifier, un bit à 1 indique
 *        que la broche correspondante doit être modifiée (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour modifier toutes les
 *        broches.
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioToggleAll (int64_t iMask, xGpio * gpio);

/**
 * @brief Lecture de l'état binaire d'une broche
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return true à l'état haut, false à l'état bas, -1 si erreur
 */
int iGpioRead (int iPin, xGpio * gpio);

/**
 * @brief Lecture de l'état binaire d'un ensemble de broches
 *
 * En fonction de l'architecture, la lecture est possible sur les entrées ou
 * sur les entrées et les sorties.
 *
 * @param iMask masque de sélection des broches à lire, un bit à 1 indique
 *        que la broche correspondante doit être lue (bit 0 pour la
 *        broche 0, bit 1 pour la broche 1 ...). -1 pour lire toutes les
 *        broches.
 * @param gpio pointeur sur le GPIO
 * @return l'état des broches, chaque bit correspond à l'état de la broche
 * correspondante (bit 0 pour la broche 0, bit 1 pour la broche 1 ...)
 */
int64_t iGpioReadAll (int64_t iMask, xGpio * gpio);

/**
 * @brief Modifie la libération des broches à la fermeture
 *
 * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
 * iGpioClose(). Cette fonction permet de modifier ce comportement.
 *
 * @param enable true active la libération, false la désactive.
 * @param gpio le pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioSetReleaseOnClose (bool enable, xGpio * gpio);

/**
 * @brief Lecture la libération des broches à la fermeture
 *
 * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
 * iGpioClose(). Cette fonction permet de lire l'état de cette directive
 *
 * @param gpio le pointeur sur le GPIO
 * @return true si validé, false sinon
 */
bool bGpioGetReleaseOnClose (const xGpio * gpio);

/**
 * @brief Libère une broche de son utilisation
 *
 * La broche correspondante est remise dans son état initial. Seule une broche
 * dont le type a été modifié depuis l'ouverture du port par iGpioOpen() sera
 * restaurée. Si le type de broche n'a pas été modifié depuis l'ouverture, cette
 * fonction ne fait rien et renvoie 0.
 *
 * @param iPin le numéro de la broche
 * @param gpio pointeur sur le GPIO
 * @return 0, -1 si erreur
 */
int iGpioRelease (int iPin, xGpio * gpio);


/**
 * @brief Teste si l'itérateur interne peut passer à une broche suivante
 * @param gpio pointeur sur le GPIO
 * @return true si possible
 */
bool bGpioHasNext (const xGpio * gpio);

/**
 * @brief Teste si l'itérateur interne peut passer à une broche précédente
 * @param gpio pointeur sur le GPIO
 * @return true si possible
 */
bool bGpioHasPrevious (const xGpio * gpio);

/**
 * @brief Pointe l'itérateur interne sur la broche suivante
 * @param gpio pointeur sur le GPIO
 * @return le numéro de la broche suivante
 */
int iGpioNext (xGpio * gpio);

/**
 * @brief Pointe l'itérateur interne sur la broche précédente
 * @param gpio pointeur sur le GPIO
 * @return le numéro de la broche précédente
 */
int iGpioPrevious (xGpio * gpio);

/**
 * @brief Pointe l'itérateur interne après la dernière broche
 * @param gpio pointeur sur le GPIO
 * @return 0
 */
int iGpioToBack (xGpio * gpio);

/**
 * @brief Pointe l'itérateur interne avant la première broche
 * @param gpio pointeur sur le GPIO
 * @return 0
 */
int iGpioToFront (xGpio * gpio);

/**
 * @brief Chaîne de caractère correspondant à un mode
 * @param eMode mode
 * @return Chaîne de caractère correspondant à un mode
 */
const char * sGpioModeToStr (eGpioMode eMode);

#ifdef __cplusplus
// <--- C part
// -----------------------------------------------------------------------------
}
#endif

#if defined(__cplusplus) || defined(DOXYGEN)
// -----------------------------------------------------------------------------
// C++ part --->
#include <exception>
#include <string>

// -----------------------------------------------------------------------------
class GpioException : public std::exception {
  public:
    enum Code {
      PermissionDenied = 1,
      UnknownCommand,
      ArgumentExpected,
      IllegalMode,
      PinNumberExpected,
      NotBinaryValue,
      NotPwmValue,
      NotOutputPin,
      NotPwmPin
    };

    explicit GpioException (GpioException::Code code, int value = 0);
    explicit GpioException (GpioException::Code code, const std::string& param);

    explicit GpioException (GpioException::Code code, const char* param) : GpioException (code, std::string (param)) {}
    
    /**
     * @brief Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~GpioException() throw () {}

    /**
     *  @brief Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the GpioException object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char * what() const throw () {

      return _msg.c_str();
    }

    /**
     *  @brief Returns the (constant) error code.
     */
    virtual GpioException::Code code() const throw () {

      return _code;
    }

  protected:
    GpioException::Code _code;
    std::string _param;
    int _value;
    std::string _msg;
};

class Gpio {
  public:
    /**
     * @brief Création d'un GPIO
     *
     * @param args pointeur sur une variable de configuration dépendant de
     *        l'architecture
     */
    Gpio (void * args = nullptr);

    /**
     * @brief Destruction d'un GPIO
     */
    virtual ~Gpio();

    /**
     * @brief Modifie le type de numérotation
     *
     * @return 0, -1 si erreur
     */
    int setNumbering (eGpioNumbering eNum);

    /**
     * @brief Type de numérotation
     *
     * @return 0, -1 si erreur
     */
    eGpioNumbering numbering();

    /**
     * @brief Chaîne de caractère correspondant à une numérotation
     * @param eNum numérotation
     * @return Chaîne de caractère correspondant à une numérotation
     */
    static const char * numberingToStr (eGpioNumbering eNum);

    /**
     * @brief Modifie le type d'une broche
     *
     * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
     * dépend de l'architecture matérielle.
     *
     * @param pin le numéro de la broche
     * @param mode le type de la broche
     * @return 0, -1 si erreur
     */
    int setMode (int pin, eGpioMode mode);

    /**
     * @brief Lecture du type actuel d'une broche
     *
     * Une broche donnée ne fournit pas forcément toutes les possibilités, cela
     * dépend de l'architecture matérielle.
     *
     * @param pin le numéro de la broche
     * @return le type, eModeError si erreur
     */
    eGpioMode mode (int pin);

    /**
     * @brief Modification de la résistance de tirage d'une broche
     *
     * @param pin le numéro de la broche
     * @param ePull le type de la résistance (ePullOff pour désactiver)
     * @return 0, -1 si erreur
     */
    int setPull (int pin, eGpioPull ePull);

    /**
     * @brief Lecture du type du type résistance de tirage d'une broche
     *
     * @param pin le numéro de la broche
     * @return le type de la résistance, ePullError si erreur
     */
    eGpioPull pull (int pin);

    /**
     * @brief Teste la validité d'un numéro de broche
     *
     * @param pin le numéro de la broche (numérotation sélectionnée)
     * @return true si numéro valide
     */
    bool isValid (int pin);

    /**
     * @brief Lecture du nombre de broches du port GPIO
     *
     * @return nombre de broches, -1 si erreur
     */
    int size();

    /**
     * @brief Modification de l'état binaire d'une sortie
     *
     * @param pin le numéro de la broche
     * @param value valeur binaire à appliquer
     * @return 0, -1 si erreur
     */
    int write (int pin, bool value);

    /**
     * @brief Bascule de l'état binaire d'une sortie
     *
     * @param pin le numéro de la broche
     * @return 0, -1 si erreur
     */
    int toggle (int pin);

    /**
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
    int writeAll (int64_t mask, bool value);

    /**
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
    int toggleAll (int64_t mask);

    /**
     * @brief Lecture de l'état binaire d'une broche
     *
     * En fonction de l'architecture, la lecture est possible sur les entrées ou
     * sur les entrées et les sorties.
     *
     * @param pin le numéro de la broche
     * @return true à l'état haut, false à l'état bas, -1 si erreur
     */
    int read (int pin);

    /**
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
    int64_t readAll (int64_t mask);

    /**
     * @brief Modifie la libération des broches à la fermeture
     *
     * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
     * iGpioClose(). Cette fonction permet de modifier ce comportement.
     *
     * @param enable true active la libération, false la désactive.
     * @return 0, -1 si erreur
     */
    int setReleaseOnClose (bool enable);

    /**
     * @brief Lecture la libération des broches à la fermeture
     *
     * Par défaut, l'ensemble des broches utilisées sont libérées à l'appel de
     * iGpioClose(). Cette fonction permet de lire l'état de cette directive
     *
     * @return true si validé, false sinon
     */
    bool releaseOnClose();

    /**
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
    int release (int pin);


    /**
     * @brief Teste si l'itérateur interne peut passer à une broche suivante
     * @return true si possible
     */
    bool hasNext();

    /**
     * @brief Teste si l'itérateur interne peut passer à une broche précédente
     * @return true si possible
     */
    bool hasPrevious();

    /**
     * @brief Pointe l'itérateur interne sur la broche suivante
     * @return le numéro de la broche suivante
     */
    int next();

    /**
     * @brief Pointe l'itérateur interne sur la broche précédente
     * @return le numéro de la broche précédente
     */
    int previous();

    /**
     * @brief Pointe l'itérateur interne après la dernière broche
     */
    void toBack();

    /**
     * @brief Pointe l'itérateur interne avant la première broche
     */
    void toFront();

    /**
     * @brief Chaîne de caractère correspondant à un mode
     *
     * @param mode mode
     * @return Chaîne de caractère correspondant à un mode
     */
    static const char * modeToStr (eGpioMode mode);

  protected:
    xGpio * g;
  private:
};

// <--- C++ part
// -----------------------------------------------------------------------------
#endif /* defined(__cplusplus) || defined(DOXYGEN) */
/**
 * @}
 */

/* ========================================================================== */
#endif /*_SYSIO_GPIO_H_ defined */
