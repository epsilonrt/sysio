###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# libsysio/doc CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

set (SYSIO_WITH_DOXYGEN_DOC 0 CACHE BOOL "Enable build of the documentation")

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

if (SYSIO_WITH_DOXYGEN_DOC)

  find_package(Doxygen REQUIRED)
  set(DOXYGEN_README ${PROJECT_SOURCE_DIR}/README.md)
  set(DOXYGEN_FILE ${CMAKE_BINARY_DIR}/Doxyfile)
  set(DOXYGEN_MAINPAGE ${CMAKE_BINARY_DIR}/main_page.dox)

  set(DOXYGEN_INPUT_LIST 
    ${PROJECT_SOURCE_DIR}/include/sysio/*.h
    ${PROJECT_SOURCE_DIR}/radio/include/radio/*.h
    ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/include/ssdv/*.h
    ${PROJECT_SOURCE_DIR}/chipio/include/chipio/*.h
    )

  set(DOXYGEN_INPUT "${CMAKE_BINARY_DIR}/main_page.dox")
  foreach(item ${DOXYGEN_INPUT_LIST})
    get_filename_component(item_dir ${item} DIRECTORY)
    string(APPEND DOXYGEN_INPUT " ${item_dir} ")
  endforeach(item ${DOXYGEN_INPUT_LIST})

  #message("DOXYGEN_INPUT=${DOXYGEN_INPUT}")

  configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
  configure_file(build_main_page.sh.in ${CMAKE_BINARY_DIR}/build_main_page.sh @ONLY)

  add_custom_target(main_page.dox
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation Main Page..."
    COMMAND sh ${CMAKE_BINARY_DIR}/build_main_page.sh > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS main_page_header.dox main_page_footer.dox ${PROJECT_SOURCE_DIR}/README.md
    )

  add_custom_target(html
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API HTML Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE} > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS main_page.dox DoxygenLayout.xml Doxyfile.in ${DOXYGEN_INPUT_LIST}
    )
  
  add_custom_target(doc)
  add_dependencies(doc main_page.dox html)
  
  #message("SYSIO_INSTALL_DOC_DIR=${SYSIO_INSTALL_DOC_DIR}")
  
  install(DIRECTORY ${CMAKE_BINARY_DIR}/html/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
endif(SYSIO_WITH_DOXYGEN_DOC)

# SysIO examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/demo/
        DESTINATION ${SYSIO_INSTALL_EXAMPLES_DIR}/sysio 
        COMPONENT doc
        FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")

# Radio examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/radio/demo/
        DESTINATION ${SYSIO_INSTALL_EXAMPLES_DIR}/radio
        COMPONENT doc
        FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")

# Ssdv examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/demo/
        DESTINATION ${SYSIO_INSTALL_EXAMPLES_DIR}/ssdv
        COMPONENT doc
        FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")

# Modbus examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/modbus/demo/
        DESTINATION ${SYSIO_INSTALL_EXAMPLES_DIR}/modbus
        COMPONENT doc
        FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")

# ChipIo examples
if(SYSIO_WITH_I2C)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/chipio/demo/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/chipio
          COMPONENT doc
          FILES_MATCHING PATTERN "*.c" PATTERN "*.cpp" PATTERN "*.project"  PATTERN "*.workspace" PATTERN "CMakeLists.txt")
endif(SYSIO_WITH_I2C)
