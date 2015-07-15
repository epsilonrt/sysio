/**
 * @file sysio++/serial.hpp
 * @brief Liaison série C++
 * 
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_SERIAL_HPP_
#define _SYSIO_SERIAL_HPP_

#include <sysio/defs.h>
#include <string>

/**
 *  @addtogroup sysio_serial
 *  @{
 *  @defgroup sysio_serial_cpp Interface C++
 *
 *  Ce module fournit les fonctions permettant de contrôler une liaison série.
 *  @{
 */
/**
 * Port série
 */
class Serial {

public:
  /**
   * Type de contrôle de flux
   */
    typedef enum {

      NoFlowControl,    /**< Sans */
      HardwareControl,  /**< Matériel (RTS/CTS) */
      SoftwareControl,  /**< Logiciel (XON/XOFF) */
      UnknownFlowControl = -1 /**< Inconnu */
    } FlowControl;
    /**
     * Consctructeur
     */
    Serial(const char * portname, int baudrate);
    /**
     * Destructeur
     */
    ~Serial();
    /**
     * Ouverture
     */
    bool open();
    /**
     * Vitesse
     */
    int baudrate() const;
    /**
     * Modification vitesse
     */
    void setBaudrate (int baudrate);
    /**
     * Nom du port
     */
    const char * port() const;
    /**
     * Modifie le nom du port
     */
    void setPort (const char * portname);
    /**
     * Ferme le port
     */
    void close();
    /**
     * Descripteur de fichier d'un port ouvert
     */
    int fileno() const;
    /**
     * Vide les tampons
     */
    void flush();
    /**
     * Contrôle de flux
     */
    FlowControl flowControl() const;
    /**
     * Modifie le contrôle de flux
     */
    bool setFlowControl (FlowControl newFlowControl);
    /**
     * Contrôle de flux sous forme de texte
     */
    const char * flowControlName() const;
    /**
     * Modifie le contrôle de flux à partir du nom
     */
    bool setFlowControlName (const char * newFlowControl);
    
#if !defined(__DOXYGEN__)
    // Swig access functions (python interface)
    inline int getFileno() const { return fileno(); }
    inline int getBaudrate() const { return baudrate(); }
    inline const char * getPort() const { return port(); }
    inline int getFlowControl() const { return flowControl(); }
    inline const char * getFlowControlName() const { return flowControlName(); }

  private:
    int fd;
    int _baudrate;
    std::string _portname;
#endif /* __DOXYGEN__ not defined */
};
/**
 *  @}
 * @}
 */

/* ========================================================================== */
#endif /* _SYSIO_SERIAL_HPP_ */
