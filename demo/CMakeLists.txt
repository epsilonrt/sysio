###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# sysio demos CMakeLists.txt

cmake_minimum_required(VERSION 3.5)

project (sysio_demo)

find_package(sysio REQUIRED)
link_directories(${SYSIO_LIBRARY_DIRS})

find_package(PkgConfig)
pkg_check_modules (LIBGPS libgps)

include (PiBoardInfo)
if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

file(GLOB demo_list *.c)
file(GLOB serial_list serial/*.c)
file(GLOB gpio_list gpio/*.c)
file(GLOB i2c_list i2c/*.c)
file(GLOB spi_list spi/*.c)
file(GLOB gps_list gps/*.c)

file(GLOB demo_cpp_list *.cpp)
file(GLOB serial_cpp_list serial/*.cpp)

if (SYSIO_WITH_SERIAL)
  list(APPEND demo_list ${serial_list})
  list(APPEND demo_cpp_list ${serial_cpp_list})
endif (SYSIO_WITH_SERIAL)
if (SYSIO_WITH_GPIO)
  list(APPEND demo_list ${gpio_list})
endif (SYSIO_WITH_GPIO)
if (SYSIO_WITH_I2C)
  list(APPEND demo_list ${i2c_list})
endif (SYSIO_WITH_I2C)
if (SYSIO_WITH_SPI)
  list(APPEND demo_list ${spi_list})
endif (SYSIO_WITH_SPI)
if (SYSIO_WITH_GPS)
  list(APPEND demo_list ${gps_list})
endif (SYSIO_WITH_GPS)

#message("demos=${demo_list};${demo_cpp_list}")

foreach(f ${demo_list} ${demo_cpp_list})
  get_filename_component(demo_name ${f} NAME_WE)
  get_filename_component(demo_dir ${f} DIRECTORY)
  get_filename_component(demo_ext ${f} EXT)
  
  if ("${demo_name}" MATCHES "${PROJECT_NAME}*")
  
    message (STATUS "Add ${demo_name} executable...")
    add_executable (${demo_name} ${f}) 
    
    if (demo_ext STREQUAL ".c")
      set_property(TARGET ${demo_name} PROPERTY C_STANDARD 99)
    endif()
    target_link_libraries(${demo_name} ${SYSIO_LIBRARIES})
    target_include_directories(${demo_name} PUBLIC ${demo_dir} ${SYSIO_INCLUDE_DIRS})
    target_compile_options(${demo_name} PUBLIC ${SYSIO_CFLAGS})
    
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${demo_name}
      DESTINATION bin)
  endif()
endforeach(f)
