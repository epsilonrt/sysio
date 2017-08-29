###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# sysio doc CMakeLists.txt

find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message("Doxygen not found, I will not generate/install the documentation")
else()

  set(DOXYGEN_README ${PROJECT_SOURCE_DIR}/README.md)
  set(DOXYGEN_FILE ${CMAKE_BINARY_DIR}/doc/Doxyfile)
  set(DOXYGEN_MAINPAGE ${CMAKE_BINARY_DIR}/doc/main_page.dox)

  set(DOXYGEN_INPUT_LIST 
    ${PROJECT_SOURCE_DIR}/include/sysio/*.h
    ${PROJECT_SOURCE_DIR}/radio/include/radio/*.h
    ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/include/ssdv/*.h
    ${PROJECT_SOURCE_DIR}/chipio/include/chipio/*.h
    )

  set(DOXYGEN_INPUT "main_page.dox")
  foreach(item ${DOXYGEN_INPUT_LIST})
    get_filename_component(item_dir ${item} DIRECTORY)
    string(APPEND DOXYGEN_INPUT " ${item_dir} ")
  endforeach(item ${DOXYGEN_INPUT_LIST})

  #message("DOXYGEN_INPUT=${DOXYGEN_INPUT}")

  configure_file(Doxyfile.in Doxyfile @ONLY)
  configure_file(build_main_page.sh.in build_main_page.sh @ONLY)

  add_custom_target(main_page.dox
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation Main Page..."
    COMMAND sh ${CMAKE_BINARY_DIR}/doc/build_main_page.sh > /dev/null
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
  
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/demo/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/examples/sysio COMPONENT doc)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/radio/demo/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/examples/radio COMPONENT doc)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/ssdv/demo/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/examples/ssdv COMPONENT doc)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/modbus/demo/
          DESTINATION ${SYSIO_INSTALL_DOC_DIR}/examples/modbus COMPONENT doc)
  if(SYSIO_WITH_I2C)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/chipio/demo/
            DESTINATION ${SYSIO_INSTALL_DOC_DIR}/chipio/radio COMPONENT doc)
  endif(SYSIO_WITH_I2C)

endif()
