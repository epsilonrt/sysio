###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_sysio ${SYSIO_SRC_DIR}/*.c)
file(GLOB src_sysio_cpp ${SYSIO_SRC_DIR}/*.cpp)
file(GLOB src_sys   ${SYSIO_SRC_SYS_DIR}/*.c)
file(GLOB src_arch  ${SYSIO_SRC_ARCH_DIR}/*.c ${SYSIO_SRC_ARCH_DIR}/*.cpp)

set (hdr_sysio 
  ${SYSIO_INC_DIR}/sysio/defs.h
  ${SYSIO_INC_DIR}/sysio/delay.h
  ${SYSIO_INC_DIR}/sysio/iomap.h
  ${SYSIO_INC_DIR}/sysio/log.h
  ${SYSIO_INC_DIR}/sysio/rpi.h
  ${SYSIO_INC_DIR}/sysio/nanopi.h
  ${SYSIO_INC_DIR}/sysio/serial.h
  ${SYSIO_INC_DIR}/sysio/term.h
  ${SYSIO_INC_DIR}/sysio/xbee.h
  ${SYSIO_INC_DIR}/sysio/dlist.h
  ${SYSIO_INC_DIR}/sysio/vector.h
  ${SYSIO_INC_DIR}/sysio/string.h
  ${SYSIO_INC_DIR}/sysio/tinfo.h
  ${SYSIO_INC_DIR}/sysio/timer.h
  ${SYSIO_INC_DIR}/sysio/ledrgb.h
  ${SYSIO_INC_DIR}/sysio/rgb.h
)

set (hdr_sysio_cpp 
  ${SYSIO_INC_DIR}/sysio/serial.hpp
)

set (hdr_gpio 
  ${SYSIO_INC_DIR}/sysio/dinput.h
  ${SYSIO_INC_DIR}/sysio/doutput.h
  ${SYSIO_INC_DIR}/sysio/gpio.h
  ${SYSIO_INC_DIR}/sysio/gpio_c.h
  ${SYSIO_INC_DIR}/sysio/gpiopin.h
  ${SYSIO_INC_DIR}/sysio/gpioconnector.h
  ${SYSIO_INC_DIR}/sysio/pwm.h
  ${SYSIO_INC_DIR}/sysio/blyss.h
)

set (hdr_i2c
  ${SYSIO_INC_DIR}/sysio/i2c.h
  ${SYSIO_INC_DIR}/sysio/hih6130.h
  ${SYSIO_INC_DIR}/sysio/iaq.h
  ${SYSIO_INC_DIR}/sysio/gp2.h
  ${SYSIO_INC_DIR}/sysio/tlc59116.h
)

set (hdr_spi
  ${SYSIO_INC_DIR}/sysio/spi.h
  ${SYSIO_INC_DIR}/sysio/rf69.h
)

set (hdr_gps
  ${SYSIO_INC_DIR}/sysio/gps.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_sysio} ${hdr_sysio_cpp} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (SYSIO_WITH_GPIO)
  file(GLOB src_gpio ${SYSIO_SRC_DIR}/gpio/*.c ${SYSIO_SRC_DIR}/gpio/*.cpp)
  list(APPEND hdr_public ${hdr_gpio})
endif (SYSIO_WITH_GPIO)

if (SYSIO_WITH_I2C)
  file(GLOB src_i2c ${SYSIO_SRC_DIR}/i2c/*.c)
  list(APPEND hdr_public ${hdr_i2c})
endif (SYSIO_WITH_I2C)

if (SYSIO_WITH_SPI)
  file(GLOB src_spi ${SYSIO_SRC_DIR}/spi/*.c)
  list(APPEND hdr_public ${hdr_spi})
endif (SYSIO_WITH_SPI)

if (SYSIO_WITH_GPS)
  file(GLOB src_gps ${SYSIO_SRC_DIR}/libgps/*.c)
  list(APPEND hdr_public ${hdr_gps})
endif (SYSIO_WITH_GPS)

#message ("hdr_public=${hdr_public}")

set (libsrc ${src_sysio} ${src_sys} ${src_arch} 
  ${src_gpio} ${src_i2c} ${src_spi} ${src_gps} 
  ${src_sysio_cpp}
  )

include_directories(BEFORE
  ${SYSIO_INC_DIR} 
  ${SYSIO_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(sysio-shared SHARED ${libsrc})
target_compile_definitions(sysio-shared PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
set_target_properties(sysio-shared PROPERTIES 
  OUTPUT_NAME sysio 
  CLEAN_DIRECT_OUTPUT 1 
  C_STANDARD 99 
  VERSION ${SYSIO_VERSION} 
  SOVERSION ${SYSIO_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(SYSIO_WITH_STATIC)
  add_library(sysio-static STATIC ${libsrc})
  target_compile_definitions(sysio-static PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
  set_target_properties(sysio-static PROPERTIES
    OUTPUT_NAME sysio 
    CLEAN_DIRECT_OUTPUT 1 
    C_STANDARD 99 
    VERSION ${SYSIO_VERSION} 
  )
endif(SYSIO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (SYSIO_WITH_STATIC)
  set (exported_targets sysio-shared sysio-static)
else(SYSIO_WITH_STATIC)
  set (exported_targets sysio-shared)
endif(SYSIO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/sysio.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Sysio)

# Create the sysio-config.cmake and sysio-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${SYSIO_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${SYSIO_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${SYSIO_INC_DIR}" "${SYSIO_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/sysio")
if(SYSIO_WITH_GPS)
  list(APPEND CONF_LIB_DIRS "${LIBGPS_LIBRARY_DIRS}")
endif(SYSIO_WITH_GPS)
configure_file(sysio-config.cmake.in
  "${PROJECT_BINARY_DIR}/sysio-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${SYSIO_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${SYSIO_CMAKE_DIR}/${REL_LIB_DIR}")
if(SYSIO_WITH_GPS)
  list(APPEND CONF_LIB_DIRS "${LIBGPS_LIBRARY_DIRS}")
endif(SYSIO_WITH_GPS)
configure_file(sysio-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysio-config.cmake" @ONLY)

# ... for both
configure_file(sysio-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/sysio-config-version.cmake" @ONLY)

# Install the sysio-config.cmake and sysio-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysio-config.cmake"
  "${PROJECT_BINARY_DIR}/sysio-config-version.cmake"
  DESTINATION "${SYSIO_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT sysio DESTINATION
  "${SYSIO_INSTALL_CMAKE_DIR}" COMPONENT dev)

install (TARGETS sysio-shared
  # IMPORTANT: Add the sysio library to the "export-set"
  EXPORT sysio
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/sysio" COMPONENT dev
  )

if(SYSIO_WITH_STATIC)
  install (TARGETS sysio-static 
    EXPORT sysio
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(SYSIO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${SYSIO_CFLAGS_OTHER})
  string(APPEND SYSIO_CFLAGS_PKGCONFIG "${item} ")
endforeach(item SYSIO_CFLAGS_OTHER)

foreach(item ${SYSIO_LDFLAGS_OTHER})
  string(APPEND SYSIO_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item SYSIO_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/sysio.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
