# libsysio CMakeLists.txt

find_program(lsb_release_cmd lsb_release)
find_program(gzip_cmd gzip)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

GetGitVersion(SYSIO_VERSION ${CMAKE_CURRENT_BINARY_DIR}/version.h)

include_directories(
  ${SYSIO_SOURCE_DIR} 
  ${SYSIO_SOURCE_DIR}/3rdparty 
  ${SYSIO_SOURCE_DIR}/sysio/src 
  ${CMAKE_CURRENT_BINARY_DIR}
  )
  
file(GLOB src_sysio ${SYSIO_SOURCE_DIR}/sysio/src/*.c)
file(GLOB src_sys ${SYSIO_SOURCE_DIR}/${SYSIO_SYS_DIR}/*.c)
file(GLOB src_arch ${SYSIO_SOURCE_DIR}/${SYSIO_ARCH_DIR}/*.c)
file(GLOB src_ssdv ${SYSIO_SOURCE_DIR}/3rdparty/ssdv/src/*.c)
file(GLOB src_modbus ${SYSIO_SOURCE_DIR}/3rdparty/modbus/src/*.c)

set (hdr_sysio 
  ${SYSIO_SOURCE_DIR}/sysio/defs.h
  ${SYSIO_SOURCE_DIR}/sysio/delay.h
  ${SYSIO_SOURCE_DIR}/sysio/iomap.h
  ${SYSIO_SOURCE_DIR}/sysio/log.h
  ${SYSIO_SOURCE_DIR}/sysio/rpi.h
  ${SYSIO_SOURCE_DIR}/sysio/nanopi.h
  ${SYSIO_SOURCE_DIR}/sysio/serial.h
  ${SYSIO_SOURCE_DIR}/sysio/term.h
  ${SYSIO_SOURCE_DIR}/sysio/xbee.h
  ${SYSIO_SOURCE_DIR}/sysio/dlist.h
  ${SYSIO_SOURCE_DIR}/sysio/vector.h
  ${SYSIO_SOURCE_DIR}/sysio/string.h
  ${SYSIO_SOURCE_DIR}/sysio/tinfo.h
  ${SYSIO_SOURCE_DIR}/sysio/timer.h
  ${SYSIO_SOURCE_DIR}/sysio/ledrgb.h
  ${SYSIO_SOURCE_DIR}/sysio/rgb.h
)

set (hdr_gpio 
  ${SYSIO_SOURCE_DIR}/sysio/dinput.h
  ${SYSIO_SOURCE_DIR}/sysio/doutput.h
  ${SYSIO_SOURCE_DIR}/sysio/gpio.h
  ${SYSIO_SOURCE_DIR}/sysio/pwm.h
  ${SYSIO_SOURCE_DIR}/sysio/blyss.h
)

set (hdr_i2c
  ${SYSIO_SOURCE_DIR}/sysio/i2c.h
  ${SYSIO_SOURCE_DIR}/sysio/hih6130.h
  ${SYSIO_SOURCE_DIR}/sysio/iaq.h
  ${SYSIO_SOURCE_DIR}/sysio/gp2.h
  ${SYSIO_SOURCE_DIR}/sysio/tlc59116.h
)

set (hdr_spi
  ${SYSIO_SOURCE_DIR}/sysio/spi.h
  ${SYSIO_SOURCE_DIR}/sysio/rf69.h
)

set (hdr_gps
  ${SYSIO_SOURCE_DIR}/sysio/gps.h
)

file(GLOB hdr_radio ${SYSIO_SOURCE_DIR}/radio/*.h)
file(GLOB hdr_ssdv ${SYSIO_SOURCE_DIR}/3rdparty/ssdv/*.h)
file(GLOB hdr_modbus ${SYSIO_SOURCE_DIR}/3rdparty/modbus/*.h)

install (FILES ${hdr_sysio} DESTINATION include/sysio COMPONENT dev)
install (FILES ${hdr_radio} DESTINATION include/radio COMPONENT dev)
install (FILES ${hdr_ssdv} DESTINATION include/ssdv COMPONENT dev)
install (FILES ${hdr_modbus} DESTINATION include/modbus COMPONENT dev)

if (SYSIO_WITH_GPIO)
  file(GLOB src_gpio ${SYSIO_SOURCE_DIR}/sysio/src/gpio/*.c)
  install (FILES ${hdr_gpio} DESTINATION include/sysio COMPONENT dev)
endif (SYSIO_WITH_GPIO)

if (SYSIO_WITH_I2C)
  file(GLOB src_i2c ${SYSIO_SOURCE_DIR}/sysio/src/i2c/*.c)
  install (FILES ${hdr_i2c} DESTINATION include/sysio COMPONENT dev)
  
  file(GLOB src_chipio ${SYSIO_SOURCE_DIR}/chipio/src/*.c)
  file(GLOB hdr_chipio ${SYSIO_SOURCE_DIR}/chipio/*.h)
  install (FILES ${hdr_chipio} DESTINATION include/chipio COMPONENT dev)
endif (SYSIO_WITH_I2C)

if (SYSIO_WITH_SPI)
  file(GLOB src_spi ${SYSIO_SOURCE_DIR}/sysio/src/spi/*.c)
  install (FILES ${hdr_spi} DESTINATION include/sysio COMPONENT dev)
endif (SYSIO_WITH_SPI)

if (LIBGPS_FOUND)
  file(GLOB src_gps ${SYSIO_SOURCE_DIR}/sysio/src/libgps/*.c)
  install (FILES ${hdr_gps} DESTINATION include/sysio COMPONENT dev)
endif (LIBGPS_FOUND)

CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/config.h DESTINATION include/sysio 
                COMPONENT dev)

set (libsrc ${SYSIO_SOURCE_DIR}/lib/version.c 
  ${src_sysio} ${src_sys} ${src_arch} 
  ${src_gpio} ${src_i2c} ${src_spi} ${src_gps} 
  ${src_ssdv} ${src_modbus}
  )

add_library(sysio-shared SHARED ${libsrc})
target_compile_definitions(sysio-shared PUBLIC LOG_ASSERT)
set_target_properties(sysio-shared PROPERTIES 
  OUTPUT_NAME sysio 
  CLEAN_DIRECT_OUTPUT 1 
  C_STANDARD 99 
  VERSION ${SYSIO_VERSION_STRING} 
  SOVERSION ${SYSIO_VERSION_SHORT}
  )
install (TARGETS sysio-shared LIBRARY DESTINATION lib COMPONENT lib)

if(SYSIO_WITH_STATIC)
  add_library(sysio-static STATIC ${libsrc})
  target_compile_definitions(sysio-static PUBLIC LOG_ASSERT)
  set_target_properties(sysio-static PROPERTIES
  OUTPUT_NAME sysio 
  CLEAN_DIRECT_OUTPUT 1 
  C_STANDARD 99 
  VERSION ${SYSIO_VERSION_STRING} 
  )
  install (TARGETS sysio-static ARCHIVE DESTINATION lib COMPONENT lib)
endif(SYSIO_WITH_STATIC)

set (SYSIO_LDFLAGS_OTHER "-lm -lpthread -lrt -pthread ${LIBGPS_LDFLAGS}")
set (SYSIO_CFLAGS_OTHER "-std=gnu99 ${LIBGPS_CFLAGS}")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/sysio.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/sysio.pc DESTINATION lib/pkgconfig
  COMPONENT dev)

### Debian Package generation
set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_PACKAGE_COMPONENT 1)
set(CPACK_DEB_COMPONENT_INSTALL 1)
 
set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libsysio")
set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libsysio-dev")
set(CPACK_STRIP_FILES, 1)

set(CPACK_PACKAGE_VERSION "${SYSIO_VERSION_MAJOR}.${SYSIO_VERSION_MINOR}-${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${SYSIO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SYSIO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <epsilonrt@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.epsilonrt.fr/sysio")
set(CPACK_RESOURCE_FILE_LICENSE "${SYSIO_SOURCE_DIR}/LICENSE")

if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
  find_program(dpkg_cmd NAMES dpkg)
  if (dpkg_cmd)
    execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (ret EQUAL 0)
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
    endif (ret EQUAL 0)
  endif (dpkg_cmd)
endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)

set(CPACK_COMPONENT_LIB_DESCRIPTION "${PROJECT_DESCRIPTION} - library")
set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= 2.4), libgcc1 (>= 1:3.5), libgps21 (>= 3.11)")
set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

set(CPACK_COMPONENT_DEV_DESCRIPTION "${PROJECT_DESCRIPTION} - development files")
set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}), pkg-config")
set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")

#set(CPACK_PACKAGE_DESCRIPTION_FILE "${SYSIO_SOURCE_DIR}/doc/README-deb.md")
include(CPack)
