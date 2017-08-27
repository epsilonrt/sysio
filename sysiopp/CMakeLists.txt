###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# sysio++ top-level CMakeLists.txt


set (SYSIOPP_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set (SYSIOPP_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set (SYSIOPP_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/sysiopp/cmake")

add_subdirectory (lib)
