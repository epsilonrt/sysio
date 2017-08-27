###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio++ CMakeLists.txt

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

file(GLOB src_sysiopp ${SYSIOPP_SRC_DIR}/*.cpp)
file(GLOB hdr_sysiopp ${SYSIOPP_INC_DIR}/sysiopp/*.hpp)


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_sysiopp} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc ${src_sysiopp})

include_directories(
  ${SYSIO_INC_DIR} 
  ${SYSIOPP_INC_DIR} 
  ${SYSIOPP_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(sysiopp-shared SHARED ${libsrc})
target_compile_definitions(sysiopp-shared PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
set_target_properties(sysiopp-shared PROPERTIES 
  OUTPUT_NAME sysiopp 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${SYSIO_VERSION} 
  SOVERSION ${SYSIO_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(SYSIO_WITH_STATIC)
  add_library(sysiopp-static STATIC ${libsrc})
  target_compile_definitions(sysiopp-static PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
  set_target_properties(sysiopp-static PROPERTIES
    OUTPUT_NAME sysiopp 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${SYSIO_VERSION} 
  )
endif(SYSIO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (SYSIO_WITH_STATIC)
  set (exported_targets sysiopp-shared sysiopp-static)
else(SYSIO_WITH_STATIC)
  set (exported_targets sysiopp-shared)
endif(SYSIO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/sysiopp.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE sysiopp)

# Create the sysiopp-config.cmake and sysiopp-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${SYSIOPP_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${SYSIOPP_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${SYSIO_INC_DIR}" "${SYSIOPP_INC_DIR}" "${SYSIOPP_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/sysiopp")
configure_file(sysiopp-config.cmake.in
  "${PROJECT_BINARY_DIR}/sysiopp-config.cmake" @ONLY)
   

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${SYSIOPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${SYSIOPP_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(sysiopp-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysiopp-config.cmake" @ONLY)

# ... for both
configure_file(sysiopp-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/sysiopp-config-version.cmake" @ONLY)

# Install the sysiopp-config.cmake and sysiopp-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sysiopp-config.cmake"
  "${PROJECT_BINARY_DIR}/sysiopp-config-version.cmake"
  DESTINATION "${SYSIOPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT sysiopp DESTINATION
  "${SYSIOPP_INSTALL_CMAKE_DIR}" COMPONENT dev)

install (TARGETS sysiopp-shared
  # IMPORTANT: Add the sysio++ library to the "export-set"
  EXPORT sysiopp
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/sysiopp" COMPONENT dev
  )

if(SYSIO_WITH_STATIC)
  install (TARGETS sysiopp-static 
    EXPORT sysiopp
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(SYSIO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------

set (SYSIOPP_LDFLAGS_OTHER "")
set (SYSIOPP_CPPFLAGS_OTHER "")

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/sysiopp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/sysiopp.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/sysiopp.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
