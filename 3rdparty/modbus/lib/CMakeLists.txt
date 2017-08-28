###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# modbus CMakeLists.txt

set(MODBUS_VERSION "3.1.0-1")
set(MODBUS_VERSION_SHORT "3.1.0")

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

file(GLOB src_modbus ${MODBUS_SRC_DIR}/*.c)
file(GLOB hdr_modbus ${MODBUS_INC_DIR}/modbus/*.h)

set(hdr_public ${hdr_modbus} ${MODBUS_SRC_DIR}/config.h)

set (libsrc ${src_modbus})

include_directories(
  ${MODBUS_INC_DIR} 
  ${MODBUS_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(modbus-shared SHARED ${libsrc})
target_compile_definitions(modbus-shared PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
set_target_properties(modbus-shared PROPERTIES 
  OUTPUT_NAME modbus 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${MODBUS_VERSION} 
  SOVERSION ${MODBUS_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(SYSIO_WITH_STATIC)
  add_library(modbus-static STATIC ${libsrc})
  target_compile_definitions(modbus-static PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
  set_target_properties(modbus-static PROPERTIES
    OUTPUT_NAME modbus 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${MODBUS_VERSION} 
  )
endif(SYSIO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (SYSIO_WITH_STATIC)
  set (exported_targets modbus-shared modbus-static)
else(SYSIO_WITH_STATIC)
  set (exported_targets modbus-shared)
endif(SYSIO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/modbus.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE modbus)

# Create the modbus-config.cmake and modbus-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${MODBUS_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${MODBUS_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${SYSIO_INC_DIR}" "${MODBUS_INC_DIR}" "${MODBUS_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/modbus")
configure_file(modbus-config.cmake.in
  "${PROJECT_BINARY_DIR}/modbus-config.cmake" @ONLY)
   

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${MODBUS_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${MODBUS_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(modbus-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/modbus-config.cmake" @ONLY)

# ... for both
configure_file(modbus-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/modbus-config-version.cmake" @ONLY)

# Install the modbus-config.cmake and modbus-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/modbus-config.cmake"
  "${PROJECT_BINARY_DIR}/modbus-config-version.cmake"
  DESTINATION "${MODBUS_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT modbus DESTINATION
  "${MODBUS_INSTALL_CMAKE_DIR}" COMPONENT dev)

install (TARGETS modbus-shared
  # IMPORTANT: Add the sysio++ library to the "export-set"
  EXPORT modbus
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/modbus" COMPONENT dev
  )

if(SYSIO_WITH_STATIC)
  install (TARGETS modbus-static 
    EXPORT modbus
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(SYSIO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/modbus.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/modbus.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/modbus.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
