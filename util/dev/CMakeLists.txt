###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# util CMakeLists.txt

file(GLOB util_bin ${CMAKE_CURRENT_SOURCE_DIR}/bin/*)
file(GLOB util_bin_rpi ${CMAKE_CURRENT_SOURCE_DIR}/rpi/*)
file(GLOB util_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake)

install (FILES ${util_bin} 
  DESTINATION "${INSTALL_BIN_DIR}" 
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
    WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  )

install (FILES ${util_cmake} 
  DESTINATION "${SYSIO_INSTALL_CMAKE_DIR}" 
  PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  COMPONENT dev
  )

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  install (FILES ${util_bin_rpi} 
    DESTINATION "${INSTALL_BIN_DIR}" 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
      WORLD_EXECUTE WORLD_READ
    COMPONENT dev
    )
endif()
