###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# PiBoardInfo.cmake
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
  
  set (board_found FALSE)
  
  find_program(hardware_cpu NAMES hardware-cpu PATHS ${PROJECT_TOOLS_PATHS})
  if (hardware_cpu)

    execute_process(COMMAND ${hardware_cpu} OUTPUT_VARIABLE cpu OUTPUT_STRIP_TRAILING_WHITESPACE)
    message (STATUS "Detecting host CPU - ${cpu} found")

    if (${cpu} MATCHES ".*sun8i.*")
    
      # Allwinner sunxi  -------------------------------------------------------
      find_program(armbian_board NAMES armbian-board PATHS ${PROJECT_TOOLS_PATHS})
      if (armbian_board)
      
        execute_process(COMMAND ${armbian_board} OUTPUT_VARIABLE board OUTPUT_STRIP_TRAILING_WHITESPACE)
        message (STATUS "Detecting Armbian board - ${board} found")
        set (board_found TRUE)
        
        if ("${board}" STREQUAL "nanopineo")
          set (PIBOARD_ID BOARD_NANOPI_NEO PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi Neo" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 4 PARENT_SCOPE)
        elseif ("${board}" STREQUAL "nanopiair")
          set (PIBOARD_ID BOARD_NANOPI_AIR PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi Neo Air" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 4 PARENT_SCOPE)
        elseif ("${board}" STREQUAL "nanopim1")
          set (PIBOARD_ID BOARD_NANOPI_M1 PARENT_SCOPE)
          set (PIBOARD_NAME "NanoPi M1" PARENT_SCOPE)
          set (PIBOARD_GPIO_REVISION 3 PARENT_SCOPE)
        else()
          message (WARNING "Unknown ArmBian board !")
          set (board_found FALSE)
        endif ()
        
        if (board_found)
          set (PIBOARD_BUILDER "Friendly ARM" PARENT_SCOPE)
          set (PIBOARD_CPU ${cpu} PARENT_SCOPE)
          message (STATUS "Detecting Friendly ARM board - ${board} found")
        endif (board_found)
        
      else (armbian_board)
        message(FATAL_ERROR "armbian-board utility not found")
      endif (armbian_board)
    
    elseif (("${cpu}" STREQUAL "bcm2708") OR ("${cpu}" STREQUAL "bcm2835") OR 
            ("${cpu}" STREQUAL "bcm2709") OR ("${cpu}" STREQUAL "bcm2836") OR 
            ("${cpu}" STREQUAL "bcm2710") OR ("${cpu}" STREQUAL "bcm2837"))

      # Raspberry Pi -----------------------------------------------------------
      find_program(rpi_info NAMES rpi-info PATHS ${PROJECT_TOOLS_PATHS})

      if (rpi_info)
      
        execute_process(COMMAND ${rpi_info} -c RESULT_VARIABLE ret OUTPUT_VARIABLE cpu OUTPUT_STRIP_TRAILING_WHITESPACE)
        #message (STATUS "RPI_CPU='${cpu}'")
        if (ret EQUAL 0)
          set (PIBOARD_ID BOARD_RASPBERRYPI PARENT_SCOPE)
          set (PIBOARD_CPU ${cpu} PARENT_SCOPE)
          set (board_found TRUE)
          execute_process(COMMAND ${rpi_info} -r OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${str} found")
          set (PIBOARD_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -n OUTPUT_VARIABLE board OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${board} found")
          set (PIBOARD_NAME ${board} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -g OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${str} found")
          set (PIBOARD_GPIO_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -p OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${str} found")
          set (PIBOARD_PCB_REVISION ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -m OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${str} found")
          set (PIBOARD_MEM ${str} PARENT_SCOPE)
          execute_process(COMMAND ${rpi_info} -b OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
          #message (STATUS "Detecting Raspberry Pi board - ${str} found")
          set (PIBOARD_BUILDER ${str} PARENT_SCOPE)
          message (STATUS "Detecting Raspberry Pi board - ${board} found")
        endif (ret EQUAL 0)
      else (rpi_info)
        message(FATAL_ERROR "rpi-info utility not found")
      endif (rpi_info)

    endif (${cpu} MATCHES ".*sun8i.*")
  else (hardware_cpu)
    message(FATAL_ERROR "hardware-cpu not found")
  endif (hardware_cpu)

  if (NOT board_found)
    message(STATUS "No Pi board found.")
  endif (NOT board_found)
  
  set (PIBOARD_FOUND ${board_found} PARENT_SCOPE)

endfunction(GetPiBoardInfo)
#-------------------------------------------------------------------------------
