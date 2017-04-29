# SysIo
*Librairie et outils pour l'embarqué*

---
Copyright 2015-2017 (c), epsilonRT

<a href="http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html">
  <img src="https://raw.githubusercontent.com/epsilonrt/gxPL/master/doc/images/osi.png" alt="osi.png" align="right" valign="top">
</a>

SysIo est un projet Open Source sous 
[CeCILL Free Software License Version 2.1](http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html)
dont l'objectif est d'offrir une bibliothèque C, C++ et Python pour accèder
aux ressources matérielles du système par une interface standardisée.

La partie spécifique à une architecture matérielle a été séparée de
façon à permettre le portage vers différentes plateformes. 

Ce projet est en développement, pour l'instant sur plateforme Linux/Unix, 
il fournit les modules:

* Temporisation [**delay**](http://www.epsilonrt.fr/sysio/group__sysio__delay.html)
* Timers de temporisation [**timer**](http://www.epsilonrt.fr/sysio/group__sysio__timer.html)
* Liaison série [**serial**](http://www.epsilonrt.fr/sysio/group__sysio__serial.html)
* XBee [**xbee**](http://www.epsilonrt.fr/sysio/group__sysio__xbee.html)
* Télé-information client ERDF [**tinfo**](http://www.epsilonrt.fr/sysio/group__sysio__tinfo.html)
* Accès mémoire IO [**iomap**](http://www.epsilonrt.fr/sysio/group__sysio__iomap.html)
* Liaisons Ax25 [**ax25**](http://www.epsilonrt.fr/sysio/group__radio__ax25.html)
* Gestion des logs [**log**](http://www.epsilonrt.fr/sysio/group__sysio__log.html)
* Liste doublement chaînée [**dlist**](http://www.epsilonrt.fr/sysio/group__sysio__dlist.html)
* Vecteur [**vector**](http://www.epsilonrt.fr/sysio/group__sysio__vector.html)
* String [**string**](http://www.epsilonrt.fr/sysio/group__sysio__string.html)
* Terminaux [**term**](http://www.epsilonrt.fr/sysio/group__sysio__term.html)
* Gps avec gpsd [**gps**](http://www.epsilonrt.fr/sysio/group__sysio__gps.html)

sur plateformes Raspberry Pi (A/B/B+/Pi2/Pi3) et NanoPi (Neo/NeoAir/M1), il 
fournit en plus les modules:

* Raspberry Pi [**rpi**](http://www.epsilonrt.fr/sysio/group__sysio__rpi.html)
* NanoPi [**rpi**](http://www.epsilonrt.fr/sysio/group__sysio__nanopi.html)
* GPIO [**gpio**](http://www.epsilonrt.fr/sysio/group__sysio__gpio.html)
* Port d'entrée [**dinput**](http://www.epsilonrt.fr/sysio/group__sysio__dinput.html)
* Port de sortie [**douput**](http://www.epsilonrt.fr/sysio/group__sysio__doutput.html)
* Broche PWM [**pwm**](http://www.epsilonrt.fr/sysio/group__sysio__pwm.html)
* Bus I2C [**i2c**](http://www.epsilonrt.fr/sysio/group__sysio__i2c.html)
* Bus SPI [**spi**](http://www.epsilonrt.fr/sysio/group__sysio__spi.html)
* Capteur I2C IAQ [**iaq**](http://www.epsilonrt.fr/sysio/group__sysio__iaq.html)
* Capteur I2C HIH6130 [**hih6130**](http://www.epsilonrt.fr/sysio/group__sysio__hih6130.html)
* Modules Blyss™ [**blyss**](http://www.epsilonrt.fr/sysio/group__sysio__blyss.html)
* Module UHF RFM69 [**rf69**](http://www.epsilonrt.fr/sysio/group__sysio__rf69.html)
* Leds RGB [**ledrgb**](http://www.epsilonrt.fr/sysio/group__sysio__ledrgb.html)

Pour chaque module, un ou plusieurs [exemples](http://www.epsilonrt.fr/sysio/examples.html) sont fournis.

Vous pouvez récupérer la toute dernière version de développement grâce à git :

    $ git clone http://github.com/epsilonrt/sysio.git

Sa documentation complète est consultable sur le site : 
[http://www.epsilonrt.fr/sysio](http://www.epsilonrt.fr/sysio)

##Installation rapide

    git clone http://github.com/epsilonrt/sysio.git
    cd sysio
    sudo make install_utils
    make
    sudo make install
