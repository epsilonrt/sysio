###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio++ CMakeLists.txt

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

file(GLOB src_chipio ${CHIPIO_SRC_DIR}/*.c)
file(GLOB hdr_chipio ${CHIPIO_INC_DIR}/chipio/*.h)


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_chipio} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set (libsrc ${src_chipio})

include_directories(
  ${SYSIO_INC_DIR} 
  ${CHIPIO_INC_DIR} 
  ${CHIPIO_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(chipio-shared SHARED ${libsrc})
target_compile_definitions(chipio-shared PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
set_target_properties(chipio-shared PROPERTIES 
  OUTPUT_NAME chipio 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${SYSIO_VERSION} 
  SOVERSION ${SYSIO_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(SYSIO_WITH_STATIC)
  add_library(chipio-static STATIC ${libsrc})
  target_compile_definitions(chipio-static PUBLIC LOG_ASSERT SYSIO_WITH_CONFIG_H)
  set_target_properties(chipio-static PROPERTIES
    OUTPUT_NAME chipio 
    CLEAN_DIRECT_OUTPUT 1 
    VERSION ${SYSIO_VERSION} 
  )
endif(SYSIO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (SYSIO_WITH_STATIC)
  set (exported_targets chipio-shared chipio-static)
else(SYSIO_WITH_STATIC)
  set (exported_targets chipio-shared)
endif(SYSIO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/chipio.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE chipio)

# Create the chipio-config.cmake and chipio-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${CHIPIO_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${CHIPIO_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${SYSIO_INC_DIR}" "${CHIPIO_INC_DIR}" "${CHIPIO_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/chipio")
configure_file(chipio-config.cmake.in
  "${PROJECT_BINARY_DIR}/chipio-config.cmake" @ONLY)
   

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${CHIPIO_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${CHIPIO_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(chipio-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/chipio-config.cmake" @ONLY)

# ... for both
configure_file(chipio-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/chipio-config-version.cmake" @ONLY)

# Install the chipio-config.cmake and chipio-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/chipio-config.cmake"
  "${PROJECT_BINARY_DIR}/chipio-config-version.cmake"
  DESTINATION "${CHIPIO_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT chipio DESTINATION
  "${CHIPIO_INSTALL_CMAKE_DIR}" COMPONENT dev)

install (TARGETS chipio-shared
  # IMPORTANT: Add the sysio++ library to the "export-set"
  EXPORT chipio
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/chipio" COMPONENT dev
  )

if(SYSIO_WITH_STATIC)
  install (TARGETS chipio-static 
    EXPORT chipio
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(SYSIO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/chipio.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/chipio.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/chipio.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
