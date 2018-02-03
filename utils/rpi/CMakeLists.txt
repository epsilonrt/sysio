###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils/rpi CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

include(PiBoardInfo)
if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  add_subdirectory (rpi-info)
endif()
