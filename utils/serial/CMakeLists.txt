###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils/serial CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

if (SYSIO_WITH_SERIAL)
  add_subdirectory (sping)
  add_subdirectory (ssend)
  add_subdirectory (tinfo)
endif()
