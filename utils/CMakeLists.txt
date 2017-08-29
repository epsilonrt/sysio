###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# utils CMakeLists.txt

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)

file(GLOB sysio_utils *.c)
file(GLOB gpio_utils gpio/*.c)
file(GLOB i2c_utils i2c/*.c)
file(GLOB rpi_utils rpi/*.c)

if (SYSIO_WITH_GPIO)
  list(APPEND sysio_utils ${gpio_utils})
endif (SYSIO_WITH_GPIO)

if (SYSIO_WITH_I2C)
  list(APPEND sysio_utils ${i2c_utils})
endif (SYSIO_WITH_I2C)

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  list(APPEND sysio_utils ${rpi_utils})
endif()

link_directories(${SYSIO_LIB_DIR})

foreach(f ${sysio_utils} ${util_cpp_list})
  get_filename_component(util_name ${f} NAME_WE)
  get_filename_component(util_dir ${f} DIRECTORY)
  get_filename_component(util_ext ${f} EXT)
  
  message (STATUS "Add ${util_name} executable...")
  add_executable (${util_name} ${f}) 
  
  if (util_ext STREQUAL ".c")
    set_property(TARGET ${util_name} PROPERTY C_STANDARD 99)
  endif()
  target_link_libraries(${util_name} sysio Threads::Threads ${LIBGPS_LIBRARIES})
  target_include_directories(${util_name} PUBLIC ${util_dir} ${SYSIO_INC_DIR} ${CMAKE_CURRENT_BINARY_DIR})
  target_compile_options(${util_name} PUBLIC ${SYSIO_CFLAGS_OTHER})
  add_dependencies(${util_name} sysio-shared)
  
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${util_name}
    DESTINATION "${INSTALL_BIN_DIR}" COMPONENT utils)
endforeach(f)
