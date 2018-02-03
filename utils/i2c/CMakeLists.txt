###############################################################################
# Copyright © 2018 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils/i2c CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

if (SYSIO_WITH_I2C)
  add_subdirectory (i2cping)
endif()
