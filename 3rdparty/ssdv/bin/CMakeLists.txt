###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# ssdv demo CMakeLists.txt

cmake_minimum_required(VERSION 3.5)


link_directories(${SSDV_LIB_DIR})

add_executable (ssdv-codec main.c) 

target_link_libraries(ssdv-codec ssdv)
target_include_directories(ssdv-codec PUBLIC ${SSDV_INC_DIR})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/ssdv-codec
  DESTINATION bin  COMPONENT utils)
