###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# chipio top-level CMakeLists.txt


set (CHIPIO_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set (CHIPIO_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set (CHIPIO_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/chipio/cmake")

add_subdirectory (lib)
