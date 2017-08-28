###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# modbus top-level CMakeLists.txt

set (MODBUS_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set (MODBUS_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set (MODBUS_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib)
set (MODBUS_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/modbus/cmake")

add_subdirectory (lib)
