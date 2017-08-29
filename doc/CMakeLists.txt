###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# sysio doc CMakeLists.txt

find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message("Doxygen or build_main_page.sh not found, I will not generate/install the documentation")
else()

  set(DOXYGEN_README ${PROJECT_SOURCE_DIR}/README.md)
  set(DOXYGEN_FILE ${CMAKE_BINARY_DIR}/doc/Doxyfile)
  set(DOXYGEN_MAINPAGE ${CMAKE_BINARY_DIR}/doc/main_page.dox)

  #set(DOXYGEN_OUTPUT ${APIDOC_DIR}/html/index.html)

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
  #add_dependencies(main_page.dox main_page_header.dox main_page_footer.dox ${PROJECT_SOURCE_DIR}/README.md)

  add_custom_target(html
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API HTML Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE} > /dev/null
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    DEPENDS main_page.dox DoxygenLayout.xml Doxyfile.in ${DOXYGEN_INPUT_LIST}
    )
  #add_dependencies(html main_page.dox DoxygenLayout.xml Doxyfile.in)
  
  add_custom_target(doc)
  add_dependencies(doc main_page.dox html)
  
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html 
          DESTINATION share/doc/${CMAKE_PROJECT_NAME} COMPONENT doc)

endif()
