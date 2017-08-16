###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# GetPiBoardInfo.cmake
# Dépendances: hardware-cpu, armbian-board, rpi-info
# Variables en sortie :
# PIBOARD_FOUND
# Si PIBOARD_FOUND = TRUE:
#   PIBOARD_ID
#   PIBOARD_NAME
#   PIBOARD_GPIO_REVISION
#   PIBOARD_BUILDER
# Si PIBOARD_ID = BOARD_RASPBERRYPI:
#   PIBOARD_REVISION
#   PIBOARD_PCB_REVISION
#   PIBOARD_MEM

function(GetPiBoardInfo)
  
  set (PIBOARD_FOUND FALSE PARENT_SCOPE)

  find_program(hardware_cpu NAMES hardware-cpu PATHS ${PROJECT_SOURCE_DIR}/util/bin)
  if (hardware_cpu)
    
    execute_process(COMMAND ${hardware_cpu} OUTPUT_VARIABLE cpu OUTPUT_STRIP_TRAILING_WHITESPACE)
    #message (STATUS "CPU='${cpu}'")

    if (${cpu} MATCHES ".*sun8i.*")
    
      # Allwinner sunxi  -------------------------------------------------------
      find_program(armbian_board NAMES armbian-board PATHS ${PROJECT_SOURCE_DIR}/util/bin)
      if (armbian_board)
      
        execute_process(COMMAND ${armbian_board} OUTPUT_VARIABLE board OUTPUT_STRIP_TRAILING_WHITESPACE)
        #message (STATUS "ARMBIAN_BOARD='${board}'")
        
        if ("${board}" STREQUAL "nanopineo")
          set (PIBOARD_ID BOARD_NANOPI_NEO PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi Neo" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 4 PARENT_SCOPE)
          set (PIBOARD_FOUND TRUE PARENT_SCOPE)
        elseif ("${board}" STREQUAL "nanopiair")
          set (PIBOARD_ID BOARD_NANOPI_AIR PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi Neo Air" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 4 PARENT_SCOPE)
          set (PIBOARD_FOUND TRUE PARENT_SCOPE)
        elseif ("${board}" STREQUAL "nanopim1")
          set (PIBOARD_ID BOARD_NANOPI_M1 PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi M1" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 3 PARENT_SCOPE)
          set (PIBOARD_FOUND TRUE PARENT_SCOPE)
        else()
          message (STATUS "Unknown ArmBian board !")
        endif ()
        
        if (PIBOARD_FOUND)
          set (PIBOARD_BUILDER "Friendly ARM" PARENT_SCOPE)
          set (PIBOARD_CPU ${cpu} PARENT_SCOPE)
        endif (PIBOARD_FOUND)
        
      endif (armbian_board)
    
    elseif (("${cpu}" STREQUAL "bcm2708") OR ("${cpu}" STREQUAL "bcm2835") OR 
            ("${cpu}" STREQUAL "bcm2709") OR ("${cpu}" STREQUAL "bcm2836") OR 
            ("${cpu}" STREQUAL "bcm2710") OR ("${cpu}" STREQUAL "bcm2837"))

      # Raspberry Pi -----------------------------------------------------------
      find_program(rpi_info NAMES rpi-info PATHS ${PROJECT_SOURCE_DIR}/util/bin)

      if (rpi_info)
      
        execute_process(COMMAND ${rpi_info} -c RESULT_VARIABLE ret OUTPUT_VARIABLE cpu OUTPUT_STRIP_TRAILING_WHITESPACE)
        #message (STATUS "RPI_CPU='${cpu}'")
        if (ret EQUAL 0)
          set (PIBOARD_ID BOARD_RASPBERRYPI PARENT_SCOPE)
          set (PIBOARD_CPU ${cpu} PARENT_SCOPE)
          set (PIBOARD_FOUND TRUE PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -r OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -n OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_NAME ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -g OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_GPIO_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -p OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_PCB_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -m OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_MEM ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -b OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          set (PIBOARD_BUILDER ${str} PARENT_SCOPE)
        endif (ret EQUAL 0)
      endif (rpi_info)

    endif (${cpu} MATCHES ".*sun8i.*")
  endif (hardware_cpu)
endfunction(GetPiBoardInfo)
#-------------------------------------------------------------------------------
