###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# ssdv demo CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project (ssdv_demo)

find_package(ssdv REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

link_directories(${SSDV_LIBRARY_DIRS})

file(GLOB_RECURSE demo_list *.c)
#message("demos=${demo_list}")

foreach(f ${demo_list})
  get_filename_component(demo_name ${f} NAME_WE)
  get_filename_component(demo_dir ${f} DIRECTORY)
  if ("${demo_name}" MATCHES "${PROJECT_NAME}*")
  
    message (STATUS "Add ${demo_name} executable...")
    add_executable (${demo_name} ${f}) 
    
#    set_property(TARGET ${demo_name} PROPERTY C_STANDARD 99)
    target_link_libraries(${demo_name} ${SSDV_LIBRARIES} ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARIES})
    target_include_directories(${demo_name} PUBLIC ${demo_dir} ${SSDV_INCLUDE_DIRS} ${SDL_INCLUDE_DIR} ${SDL_IMAGE_INCLUDE_DIRS})
    target_compile_options(${demo_name} PUBLIC ${SSDV_CFLAGS})
    
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${demo_name}
      DESTINATION bin)
  endif()
endforeach(f)
