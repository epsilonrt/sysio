# SysIo
*Library and tools for embedded boards*

---
Copyright 2015-2018 (c), epsilonRT

<a href="http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html">
  <img src="https://raw.githubusercontent.com/epsilonrt/gxPL/master/doc/images/osi.png" alt="osi.png" align="right" valign="top">
</a>

SysIo is an open source under [CeCILL Free Software License Version 2.1](http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html) 
that aims to provide a C, C ++ and Python library to access system hardware 
resources through a standardized interface.

The specific part of a hardware architecture has been separated to allow 
porting to different platforms.

This project is in development, for the moment on Linux/Unix platform, it provides the modules:
* Delay [**delay**](http://www.epsilonrt.fr/sysio/group__sysio__delay.html)
* Timers [**timer**](http://www.epsilonrt.fr/sysio/group__sysio__timer.html)
* Serial link [**serial**](http://www.epsilonrt.fr/sysio/group__sysio__serial.html)
* XBee [**xbee**](http://www.epsilonrt.fr/sysio/group__sysio__xbee.html)
* ERDF customer information [**tinfo**](http://www.epsilonrt.fr/sysio/group__sysio__tinfo.html)
* IO memory access [**iomap**](http://www.epsilonrt.fr/sysio/group__sysio__iomap.html)
* Ax25 links [**ax25**](http://www.epsilonrt.fr/sysio/group__radio__ax25.html)
* Log management [**log**](http://www.epsilonrt.fr/sysio/group__sysio__log.html)
* Doubly linked list [**dlist**](http://www.epsilonrt.fr/sysio/group__sysio__dlist.html)
* Vector [**vector**](http://www.epsilonrt.fr/sysio/group__sysio__vector.html)
* String [**string**](http://www.epsilonrt.fr/sysio/group__sysio__string.html)
* Terminals [**term**](http://www.epsilonrt.fr/sysio/group__sysio__term.html)
* Gps with gpsd [**gps**](http://www.epsilonrt.fr/sysio/group__sysio__gps.html)

on Raspberry Pi platforms (A/B/B+/Pi2/Pi3/Zero) and NanoPi (Neo/NeoAir/M1), 
it also provides the modules

* Raspberry Pi [**rpi**](http://www.epsilonrt.fr/sysio/group__sysio__rpi.html)
* NanoPi [**rpi**](http://www.epsilonrt.fr/sysio/group__sysio__nanopi.html)
* GPIO [**gpio**](http://www.epsilonrt.fr/sysio/group__sysio__gpio.html)
* Digital input ports [**dinput**](http://www.epsilonrt.fr/sysio/group__sysio__dinput.html)
* Digital output ports [**douput**](http://www.epsilonrt.fr/sysio/group__sysio__doutput.html)
* PWM pins [**pwm**](http://www.epsilonrt.fr/sysio/group__sysio__pwm.html)
* I2C Bus [**i2c**](http://www.epsilonrt.fr/sysio/group__sysio__i2c.html)
* SPI Bus [**spi**](http://www.epsilonrt.fr/sysio/group__sysio__spi.html)
* IAQ I2C sensor [**iaq**](http://www.epsilonrt.fr/sysio/group__sysio__iaq.html)
* HIH6130/ChipCap2 I2C sensor [**hih6130**](http://www.epsilonrt.fr/sysio/group__sysio__hih6130.html)
* Blyss™ modules [**blyss**](http://www.epsilonrt.fr/sysio/group__sysio__blyss.html)
* UHF RFM69 modules [**rf69**](http://www.epsilonrt.fr/sysio/group__sysio__rf69.html)
* RGB Leds [**ledrgb**](http://www.epsilonrt.fr/sysio/group__sysio__ledrgb.html)

For each module, one or more [examples](http://www.epsilonrt.fr/sysio/examples.html) are provided.

You can get the latest version of development with git :

    $ git clone http://github.com/epsilonrt/sysio.git

Its complete documentation is available on the site: 
[http://www.epsilonrt.fr/sysio](http://www.epsilonrt.fr/sysio)

## Quick installation

To compile, it is necessary to install gcc, g ++, cmake, pkg-config and
optionally libgps

* Install dependencies:

        $ sudo apt-get install build-essential cmake libgps-dev

* Generate the Makefile with cmake:

        $ cd sysio
        $ mkdir build-release && cd build-release
        $ cmake .. 

* Compile and install sysio:

        $ make
        $ sudo make install
        $ sudo ldconfig

We can uninstall with a `sudo make uninstall` in the same directory.

If you prefer, instead of direct compilation, you can create packages and install them:

        $ cmake -DSYSIO_WITH_DOXYGEN_DOC=1 -DCMAKE_INSTALL_PREFIX=/usr ..
        $ make package
        $ sudo dpkg -i *.deb

### Build the doc

If you want to build the documentation it will be necessary to pass the option 
-DSYSIO_WITH_DOXYGEN_DOC=1 to cmake:

        $ cmake -DSYSIO_WITH_DOXYGEN_DOC=1 ..
        $ make doc

## Creating a SysIo application

To create a SysIo project, it is necessary to install the Dev module which 
contains the header files '* .h' and the utilities necessary for the compilation.  
This is the case if you have installed "manually" with `sudo make install` or 
if you have installed the` libsysio-dev` package.

Sysio provides a pkg-config file that allows you to simply compile a program that uses it:

    $ gcc -o myapp main.c $(pkg-config --cflags --libs)

To make things easier, you can use the `sysio-prj` utility, for example, to create a C project:

    $ sysio-prj -s myCproject

or to create a C ++ project:

    $ sysio-prj -s -p myCXXproject

For help with the program:

    $ sysio-prj -h

You should open the project file with [Codelite](https://codelite.org/) then:

* right click on the project `Run CMake`, then
* right click on the project `Build`.

That's all !

### For those who like the difficulty

SysIo uses `cmake`, so you can integrate SysIo into` CMakeLists.txt` files, just add:

    $ find_package(sysio)

With the line above, sysio_FOUND was defined and the variables below are accessible:

* `SYSIO_INCLUDE_DIRS`: include directories for sysio  
* `SYSIO_LIBRARY_DIRS`: where to find libraries  
* `SYSIO_LIBRARIES`: libraries to link against  
* `SYSIO_CFLAGS`: all required cflags  
* `SYSIO_WITH_GPIO`: Boolean that indicates that SysIo has been compiled with GPIO support  
* `SYSIO_WITH_I2C`: Boolean that indicates that SysIo has been compiled with I2C bus support  
* `SYSIO_WITH_SPI`: Boolean that indicates that SysIo has been compiled with SPI bus support  
* `SYSIO_WITH_SERIAL`: Boolean that indicates that SysIo has been compiled with serial link support  
* `SYSIO_WITH_GPS`: Boolean that indicates that SysIo has been compiled with GPS support  

Here is an example of a `CMakeLists.txt` file for SysIo:  

    cmake_minimum_required(VERSION 3.5)
    project(template)
    find_package(sysio QUIET)
    if (sysio_FOUND)
      message (STATUS "SysIo found !")
      link_directories(${SYSIO_LIBRARY_DIRS})
      add_definitions(${SYSIO_CFLAGS})
      include_directories(${SYSIO_INCLUDE_DIRS})
      target_link_libraries(${CMAKE_PROJECT_NAME} ${SYSIO_LIBRARIES})
      include (PiBoardInfo)
      if (NOT PIBOARD_ID)
        message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
        GetPiBoardInfo()
      endif (NOT PIBOARD_ID)
    else (sysio_FOUND)
      message (STATUS "<WARNING> SysIo not found !")
    endif (sysio_FOUND)
    file(GLOB SRC *.cpp)
    add_executable(${CMAKE_PROJECT_NAME} ${SRC})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${SYSIO_LIBRARIES})
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${SYSIO_INCLUDE_DIRS})
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC ${SYSIO_CFLAGS})
    install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

After you can create the make file and compile the project:

1. Create a folder to build:  
        $ mkdir build-release  
2. Go to this folder:  
        $ cd build-release  
3. Run CMake:  
        $ cmake ..
4. Compile your project:  
        $ make
