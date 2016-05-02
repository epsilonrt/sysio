# SysIo
*Librairie et outils pour l'embarqué*

---
Copyright 2015-2016 (c), epsilonRT

SysIo est un projet Open Source sous 
[CeCILL Free Software License Version 2.1](http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html)
dont l'objectif est d'offrir une bibliothèque C, C++ et Python pour accèder
aux ressources matérielles du système par une interface standardisée.

La partie spécifique à une architecture matérielle a été séparée de
façon à permettre le portage vers différentes plateformes. 

Ce projet est en développement, pour l'instant, il fournit les modules:

* Temporisation [**delay**](http://www.epsilonrt.com/sysio/group__sysio__delay.html)
* Liaison série [**serial**](http://www.epsilonrt.com/sysio/group__sysio__serial.html)
* XBee [**xbee**](http://www.epsilonrt.com/sysio/group__sysio__xbee.html)
* Télé-information client ERDF [**tinfo**](http://www.epsilonrt.com/sysio/group__sysio__tinfo.html)
* Accès mémoire IO [**iomap**](http://www.epsilonrt.com/sysio/group__sysio__iomap.html)
* Liaisons Ax25 [**ax25**](http://www.epsilonrt.com/sysio/group__radio__ax25.html)
* Gestion des logs [**log**](http://www.epsilonrt.com/sysio/group__sysio__log.html)
* Liste doublement chaînée [**dlist**](http://www.epsilonrt.com/sysio/group__sysio__dlist.html)
* Vecteur [**vector**](http://www.epsilonrt.com/sysio/group__sysio__vector.html)
* String [**string**](http://www.epsilonrt.com/sysio/group__sysio__string.html)

sur plateforme Linux/Unix. Il fournit en plus les modules:

* Raspberry Pi [**rpi**](http://www.epsilonrt.com/sysio/group__sysio__rpi.html)
* GPIO [**gpio**](http://www.epsilonrt.com/sysio/group__sysio__gpio.html)
* Port d'entrée [**dinput**](http://www.epsilonrt.com/sysio/group__sysio__dinput.html)
* Port de sortie [**douput**](http://www.epsilonrt.com/sysio/group__sysio__doutput.html)
* Broche PWM [**pwm**](http://www.epsilonrt.com/sysio/group__sysio__pwm.html)
* Bus I2C [**i2c**](http://www.epsilonrt.com/sysio/group__sysio__i2c.html)
* Capteur I2C IAQ [**iaq**](http://www.epsilonrt.com/sysio/group__sysio__iaq.html)
* Capteur I2C HIH6130 [**hih6130**](http://www.epsilonrt.com/sysio/group__sysio__hih6130.html)

sur plateforme Raspberry Pi (A/B/B+/Pi2/Pi3).

Pour chaque module, un ou plusieurs [exemples](http://www.epsilonrt.com/sysio/examples.html) sont fournis.

Vous pouvez récupérer la toute dernière version de développement grâce à git :

    $ git clone http://github.com/epsilonrt/sysio.git

Sa documentation complète est consultable sur le site : 
[http://www.epsilonrt.com/sysio](http://www.epsilonrt.com/sysio)

##Installation rapide

    git clone http://github.com/epsilonrt/sysio.git
    cd sysio
    make
    sudo make install
