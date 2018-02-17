###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils/gpio CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

add_custom_target(setuid
  COMMAND sysio-setuid  ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Setting set-uid root bit for executable")

set(PROGRAM_PERMISSIONS_DEFAULT
    OWNER_WRITE OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)

if (SYSIO_WITH_GPIO)
  add_subdirectory (blink)
  add_subdirectory (bsend)
  add_subdirectory (gpio)
endif()
