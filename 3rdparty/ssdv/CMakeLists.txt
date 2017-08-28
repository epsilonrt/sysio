###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# ssdv top-level CMakeLists.txt

set (SSDV_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set (SSDV_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set (SSDV_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
set (SSDV_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/ssdv/cmake")

add_subdirectory (lib)
add_subdirectory (bin)
add_dependencies(ssdv-codec ssdv-shared)
