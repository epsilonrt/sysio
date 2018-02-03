###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio/utils CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

add_subdirectory (gpio)
add_subdirectory (i2c)
add_subdirectory (rpi)
add_subdirectory (serial)
