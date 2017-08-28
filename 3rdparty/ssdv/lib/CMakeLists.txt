###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# ssdv CMakeLists.txt

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

file(GLOB src_ssdv ${SSDV_SRC_DIR}/*.c)
file(GLOB hdr_ssdv ${SSDV_INC_DIR}/ssdv/*.h)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_ssdv} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc ${src_ssdv})

include_directories(
  ${SSDV_INC_DIR} 
  ${SSDV_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(ssdv-shared SHARED ${libsrc})
target_compile_definitions(ssdv-shared PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
set_target_properties(ssdv-shared PROPERTIES 
  OUTPUT_NAME ssdv 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${SYSIO_VERSION} 
  SOVERSION ${SYSIO_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(SYSIO_WITH_STATIC)
  add_library(ssdv-static STATIC ${libsrc})
  target_compile_definitions(ssdv-static PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
  set_target_properties(ssdv-static PROPERTIES
    OUTPUT_NAME ssdv 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${SYSIO_VERSION} 
  )
endif(SYSIO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (SYSIO_WITH_STATIC)
  set (exported_targets ssdv-shared ssdv-static)
else(SYSIO_WITH_STATIC)
  set (exported_targets ssdv-shared)
endif(SYSIO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/ssdv.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ssdv)

# Create the ssdv-config.cmake and ssdv-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${SSDV_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${SSDV_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${SYSIO_INC_DIR}" "${SSDV_INC_DIR}" "${SSDV_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/ssdv")
configure_file(ssdv-config.cmake.in
  "${PROJECT_BINARY_DIR}/ssdv-config.cmake" @ONLY)
   

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${SSDV_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${SSDV_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(ssdv-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ssdv-config.cmake" @ONLY)

# ... for both
configure_file(ssdv-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/ssdv-config-version.cmake" @ONLY)

# Install the ssdv-config.cmake and ssdv-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ssdv-config.cmake"
  "${PROJECT_BINARY_DIR}/ssdv-config-version.cmake"
  DESTINATION "${SSDV_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ssdv DESTINATION
  "${SSDV_INSTALL_CMAKE_DIR}" COMPONENT dev)

install (TARGETS ssdv-shared
  # IMPORTANT: Add the sysio++ library to the "export-set"
  EXPORT ssdv
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/ssdv" COMPONENT dev
  )

if(SYSIO_WITH_STATIC)
  install (TARGETS ssdv-static 
    EXPORT ssdv
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(SYSIO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------

#set (SSDV_LDFLAGS_OTHER "")
#set (SSDV_CPPFLAGS_OTHER "")

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ssdv.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/ssdv.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/ssdv.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
