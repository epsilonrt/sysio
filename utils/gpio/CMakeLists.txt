###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils/gpio CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

if (SYSIO_WITH_GPIO)
  add_subdirectory (blink)
  add_subdirectory (bsend)
endif()
