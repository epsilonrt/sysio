###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio/dev CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/bin/sysio-prj.in"
    "${CMAKE_CURRENT_BINARY_DIR}/sysio-prj"
    IMMEDIATE @ONLY)

#file(GLOB dev_bin ${CMAKE_CURRENT_SOURCE_DIR}/bin/*)
file(GLOB dev_bin_rpi ${CMAKE_CURRENT_SOURCE_DIR}/rpi/*)
file(GLOB dev_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake)

install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/armbian-board 
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/git-version 
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/hardware-cpu 
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/sysio-setuid
  ${CMAKE_CURRENT_BINARY_DIR}/sysio-prj
  DESTINATION "${INSTALL_BIN_DIR}" 
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
    WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  )

install (FILES ${dev_cmake} 
  DESTINATION "${SYSIO_INSTALL_CMAKE_DIR}" 
  PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  COMPONENT dev
  )

install (DIRECTORY template 
  DESTINATION "${SYSIO_INSTALL_DATA_DIR}" 
  DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  )

# SysIO examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/demo/
        DESTINATION ${SYSIO_INSTALL_EXAMPLES_DIR}/sysio 
        COMPONENT dev
        FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  install (FILES ${dev_bin_rpi} 
    DESTINATION "${INSTALL_BIN_DIR}" 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
      WORLD_EXECUTE WORLD_READ
    COMPONENT dev
    )
endif()
