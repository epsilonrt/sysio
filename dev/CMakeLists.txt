###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# dev CMakeLists.txt

file(GLOB dev_bin ${CMAKE_CURRENT_SOURCE_DIR}/bin/*)
file(GLOB dev_bin_rpi ${CMAKE_CURRENT_SOURCE_DIR}/rpi/*)
file(GLOB dev_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake)

install (FILES ${dev_bin} 
  DESTINATION "${INSTALL_BIN_DIR}" 
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
    WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  )

install (FILES ${dev_cmake} 
  DESTINATION "${SYSIO_INSTALL_CMAKE_DIR}" 
  PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  COMPONENT dev
  )

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  install (FILES ${dev_bin_rpi} 
    DESTINATION "${INSTALL_BIN_DIR}" 
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
      WORLD_EXECUTE WORLD_READ
    COMPONENT dev
    )
endif()
