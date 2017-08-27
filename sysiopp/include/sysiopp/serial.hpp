/**
 * @file sysiopp/serial.hpp
 * @brief Liaison série C++
 *
 * Copyright © 2015 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */

#ifndef _SYSIO_SERIAL_HPP_
#define _SYSIO_SERIAL_HPP_

#include <sysio/serial.h>
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
     * Nombre de bits de données
     */
    typedef enum {

      Data5 = SERIAL_DATABIT_5,
      Data6 = SERIAL_DATABIT_6,
      Data7 = SERIAL_DATABIT_7,
      Data8 = SERIAL_DATABIT_8,
      UnknownDataBits = SERIAL_DATABIT_UNKNOWN 
    } DataBits;
    
    /**
     * Nombre de bits de stop
     */
    typedef enum {

      OneStop = SERIAL_STOPBIT_ONE,
      TwoStop = SERIAL_STOPBIT_TWO,
      OneAndHalfStop = SERIAL_STOPBIT_ONEHALF,
      UnknownStopBits = SERIAL_STOPBIT_UNKNOWN 
    } StopBits;
    
    /**
     * Parité
     */
    typedef enum {

      NoParity = SERIAL_PARITY_NONE,
      EvenParity = SERIAL_PARITY_EVEN,
      OddParity = SERIAL_PARITY_ODD,
      SpaceParity = SERIAL_PARITY_SPACE,
      MarkParity = SERIAL_PARITY_MARK,
      UnknownParity = SERIAL_PARITY_UNKNOWN 
    } Parity;

    /**
     * Contrôle de flux
     */
    typedef enum {

      NoFlowControl = SERIAL_FLOW_NONE,    /**< Sans */
      HardwareControl = SERIAL_FLOW_RTSCTS,  /**< Matériel (RTS/CTS) */
      SoftwareControl = SERIAL_FLOW_XONXOFF,  /**< Logiciel (XON/XOFF) */
      UnknownFlowControl = SERIAL_FLOW_UNKNOWN /**< Inconnu */
    } FlowControl;
    /**
     * Consctructeur
     */
    Serial (const char * portname, int baudrate);
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
    inline int getFileno() const {
      return fileno();
    }
    inline int getBaudrate() const {
      return baudrate();
    }
    inline const char * getPort() const {
      return port();
    }
    inline int getFlowControl() const {
      return flowControl();
    }
    inline const char * getFlowControlName() const {
      return flowControlName();
    }

  private:
    int fd;
    xSerialIos ios;
    std::string _portname;
#endif /* __DOXYGEN__ not defined */
};
/**
 *  @}
 * @}
 */

/* ========================================================================== */
#endif /* _SYSIO_SERIAL_HPP_ */
