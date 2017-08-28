###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# modbus demo CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project (modbus_demo)

find_package(modbus REQUIRED)

link_directories(${MODBUS_LIBRARY_DIRS})

file(GLOB_RECURSE demo_list *.c)
#message("demos=${demo_list}")

foreach(f ${demo_list})
  get_filename_component(demo_name ${f} NAME_WE)
  get_filename_component(demo_dir ${f} DIRECTORY)
  if ("${demo_name}" MATCHES "${PROJECT_NAME}*")
  
    message (STATUS "Add ${demo_name} executable...")
    add_executable (${demo_name} ${f}) 
    
    set_property(TARGET ${demo_name} PROPERTY C_STANDARD 99)
    target_link_libraries(${demo_name} ${MODBUS_LIBRARIES})
    target_include_directories(${demo_name} PUBLIC ${demo_dir} ${MODBUS_INCLUDE_DIRS})
    target_compile_options(${demo_name} PUBLIC ${MODBUS_CFLAGS})
    
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${demo_name}
      DESTINATION bin)
  endif()
endforeach(f)
