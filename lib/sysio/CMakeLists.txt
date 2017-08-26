###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio CMakeLists.txt

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_sysio ${PROJECT_SOURCE_DIR}/sysio/src/*.c)
file(GLOB src_sys ${PROJECT_SOURCE_DIR}/${SYSIO_SYS_DIR}/*.c)
file(GLOB src_arch ${PROJECT_SOURCE_DIR}/${SYSIO_ARCH_DIR}/*.c)

#file(GLOB src_radio ${PROJECT_SOURCE_DIR}/radio/src/*.c)
#file(GLOB src_chipio ${PROJECT_SOURCE_DIR}/chipio/src/*.c)
#file(GLOB src_ssdv ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/src/*.c)
#file(GLOB src_modbus ${PROJECT_SOURCE_DIR}/3rdparty/modbus/src/*.c)
#file(GLOB hdr_radio ${PROJECT_SOURCE_DIR}/radio/*.h)
#file(GLOB hdr_chipio ${PROJECT_SOURCE_DIR}/chipio/*.h)
#file(GLOB hdr_ssdv ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/*.h)
#file(GLOB hdr_modbus ${PROJECT_SOURCE_DIR}/3rdparty/modbus/*.h)
#  ${src_radio} ${src_chipio} ${src_ssdv} ${src_modbus}
#install (FILES ${hdr_sysio} DESTINATION include/sysio COMPONENT dev)
#install (FILES ${hdr_radio} DESTINATION include/radio COMPONENT dev)
#install (FILES ${hdr_ssdv} DESTINATION include/ssdv COMPONENT dev)
#install (FILES ${hdr_modbus} DESTINATION include/modbus COMPONENT dev)
#install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION include/sysio COMPONENT dev)
#install (FILES ${hdr_chipio} DESTINATION include/chipio COMPONENT dev)

set (hdr_sysio 
  ${PROJECT_SOURCE_DIR}/sysio/defs.h
  ${PROJECT_SOURCE_DIR}/sysio/delay.h
  ${PROJECT_SOURCE_DIR}/sysio/iomap.h
  ${PROJECT_SOURCE_DIR}/sysio/log.h
  ${PROJECT_SOURCE_DIR}/sysio/rpi.h
  ${PROJECT_SOURCE_DIR}/sysio/nanopi.h
  ${PROJECT_SOURCE_DIR}/sysio/serial.h
  ${PROJECT_SOURCE_DIR}/sysio/term.h
  ${PROJECT_SOURCE_DIR}/sysio/xbee.h
  ${PROJECT_SOURCE_DIR}/sysio/dlist.h
  ${PROJECT_SOURCE_DIR}/sysio/vector.h
  ${PROJECT_SOURCE_DIR}/sysio/string.h
  ${PROJECT_SOURCE_DIR}/sysio/tinfo.h
  ${PROJECT_SOURCE_DIR}/sysio/timer.h
  ${PROJECT_SOURCE_DIR}/sysio/ledrgb.h
  ${PROJECT_SOURCE_DIR}/sysio/rgb.h
)

set (hdr_gpio 
  ${PROJECT_SOURCE_DIR}/sysio/dinput.h
  ${PROJECT_SOURCE_DIR}/sysio/doutput.h
  ${PROJECT_SOURCE_DIR}/sysio/gpio.h
  ${PROJECT_SOURCE_DIR}/sysio/pwm.h
  ${PROJECT_SOURCE_DIR}/sysio/blyss.h
)

set (hdr_i2c
  ${PROJECT_SOURCE_DIR}/sysio/i2c.h
  ${PROJECT_SOURCE_DIR}/sysio/hih6130.h
  ${PROJECT_SOURCE_DIR}/sysio/iaq.h
  ${PROJECT_SOURCE_DIR}/sysio/gp2.h
  ${PROJECT_SOURCE_DIR}/sysio/tlc59116.h
)

set (hdr_spi
  ${PROJECT_SOURCE_DIR}/sysio/spi.h
  ${PROJECT_SOURCE_DIR}/sysio/rf69.h
)

set (hdr_gps
  ${PROJECT_SOURCE_DIR}/sysio/gps.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_sysio} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (SYSIO_WITH_GPIO)
  file(GLOB src_gpio ${PROJECT_SOURCE_DIR}/sysio/src/gpio/*.c)
  list(APPEND hdr_public ${hdr_gpio})
endif (SYSIO_WITH_GPIO)

if (SYSIO_WITH_I2C)
  file(GLOB src_i2c ${PROJECT_SOURCE_DIR}/sysio/src/i2c/*.c)
  list(APPEND hdr_public ${hdr_i2c})
endif (SYSIO_WITH_I2C)

if (SYSIO_WITH_SPI)
  file(GLOB src_spi ${PROJECT_SOURCE_DIR}/sysio/src/spi/*.c)
  list(APPEND hdr_public ${hdr_spi})
endif (SYSIO_WITH_SPI)

if (LIBGPS_FOUND)
  file(GLOB src_gps ${PROJECT_SOURCE_DIR}/sysio/src/libgps/*.c)
  list(APPEND hdr_public ${hdr_gps})
endif (LIBGPS_FOUND)

#message ("hdr_public=${hdr_public}")

set (libsrc ${PROJECT_SOURCE_DIR}/lib/version.c 
  ${src_sysio} ${src_sys} ${src_arch} 
  ${src_gpio} ${src_i2c} ${src_spi} ${src_gps} 
  )

include_directories(
  ${PROJECT_SOURCE_DIR} 
  ${PROJECT_SOURCE_DIR}/sysio/src
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
file(RELATIVE_PATH REL_LIB_DIR "${INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/sysio")
if(LIBGPS_LIBRARY_DIRS)
  list(APPEND CONF_LIB_DIRS "${LIBGPS_LIBRARY_DIRS}")
endif(LIBGPS_LIBRARY_DIRS)
configure_file(sysio-config.cmake.in
  "${PROJECT_BINARY_DIR}/sysio-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${SYSIO_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${SYSIO_CMAKE_DIR}/${REL_LIB_DIR}")
if(LIBGPS_LIBRARY_DIRS)
  list(APPEND CONF_LIB_DIRS "${LIBGPS_LIBRARY_DIRS}")
endif(LIBGPS_LIBRARY_DIRS)
configure_file(sysio-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysio-config.cmake" @ONLY)

# ... for both
configure_file(sysio-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/sysio-config-version.cmake" @ONLY)

# Install the sysio-config.cmake and sysio-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysio-config.cmake"
  "${PROJECT_BINARY_DIR}/sysio-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT sysio DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)

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

set (SYSIO_LDFLAGS_OTHER "${LIBGPS_LDFLAGS}")
set (SYSIO_CFLAGS_OTHER "-std=gnu99 ${LIBGPS_CFLAGS} ${CMAKE_THREAD_LIBS_INIT}")

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/sysio.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
