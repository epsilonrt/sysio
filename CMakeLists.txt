###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# Sysio top-level CMakeLists.txt

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (SYSIO)
set (PROJECT_DESCRIPTION "Library and tools for embedded development")
set (PROJECT_TOOLS_PATHS ${PROJECT_SOURCE_DIR}/util/dev/bin ${PROJECT_SOURCE_DIR}/util/dev/rpi)

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/util/dev/cmake ${CMAKE_MODULE_PATH})

set (SYSIO_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

#find_package(rt)
#find_package(math)

find_package(PkgConfig)
pkg_check_modules (LIBGPS libgps)

include (GitVersion)
include (PiBoardInfo)

GetGitVersion(SYSIO_VERSION)
set(SYSIO_VERSION
  ${SYSIO_VERSION_MAJOR}.${SYSIO_VERSION_MINOR}.${SYSIO_VERSION_PATCH})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/sysio/cmake)
endif()

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  # Détection du type de carte sur le système hôte, doit être forcé en cross-compile
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

if (NOT PIBOARD_FOUND)
  # Linux Generic
  message(STATUS "BOARD not defined !")
  if (CMAKE_HOST_UNIX)
    set (PIBOARD_ID BOARD_GENERIC_LINUX)
    set (PIBOARD_NAME "Linux")
  else (CMAKE_HOST_UNIX)
    message (FATAL_ERROR "This system is not supported !")
  endif (CMAKE_HOST_UNIX)
endif (NOT PIBOARD_FOUND)

add_definitions (-D${PIBOARD_ID})

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  # Raspberry Pi ---------------------------------------------------------------
  set (SYSIO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (SYSIO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (SYSIO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_ARCH_DIR sysio/src/arch/arm/rpi)

elseif (("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_NEO") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_AIR") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_M1"))
  # NanoPi Neo / Neo Air / M1  -------------------------------------------------
  set (SYSIO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (SYSIO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (SYSIO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_ARCH_DIR sysio/src/arch/arm/nanopi)

elseif ("${PIBOARD_ID}" STREQUAL "BOARD_GENERIC_LINUX")
  # Linux Generic  -------------------------------------------------------------
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_ARCH_DIR sysio/src/arch/generic)
endif ()

if (CMAKE_HOST_UNIX)
  set (SYSIO_SYS_DIR sysio/src/unix)
  message (STATUS "Build on Unix host")
endif (CMAKE_HOST_UNIX)

message (STATUS "Build for ${PIBOARD_NAME} target, ${PIBOARD_ID} defined...")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_subdirectory (lib)
add_subdirectory (util)

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_VERSION "${SYSIO_VERSION_MAJOR}.${SYSIO_VERSION_MINOR}-${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${SYSIO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SYSIO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <epsilonrt@gmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${SYSIO_SOURCE_DIR}/LICENSE")

set(CPACK_GENERATOR "DEB")


if (CPACK_GENERATOR STREQUAL "DEB")

  set(CPACK_DEB_PACKAGE_COMPONENT 1)
  set(CPACK_DEB_COMPONENT_INSTALL 1)
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.epsilonrt.fr/sysio")

  find_program(dpkg_query NAMES dpkg-query)
  find_program(dpkg_cmd NAMES dpkg)

  if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    if (dpkg_cmd)
      execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (ret EQUAL 0)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
      endif (ret EQUAL 0)
    endif (dpkg_cmd)
  endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)


  if (dpkg_query)
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libc6 OUTPUT_VARIABLE libc6_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libgcc1 OUTPUT_VARIABLE libgcc1_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    #message ("libc6_ver=${libc6_ver}")
    #message ("libgcc1_ver=${libgcc1_ver}")
  endif (dpkg_query)
  
  if (LIBGPS_FOUND)
  
    if (dpkg_query)
      execute_process(COMMAND ${dpkg_query} --showformat=\${Package} --show libgps2? OUTPUT_VARIABLE package OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (package)
        execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show ${package} OUTPUT_VARIABLE ver OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(libgps_depends ", ${package} (>= ${ver})")
        set(libgps_dev_depends ", libgps-dev (>= ${ver})")
        #message ("libgps_depends=${libgps_depends}")
        #message ("libgps_dev_depends=${libgps_dev_depends}")
        unset(ver)
      endif (package)
      unset(package)
    endif (dpkg_query)
  endif(LIBGPS_FOUND)

  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libsysio")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${PROJECT_DESCRIPTION} (library)")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= ${libc6_ver}), libgcc1 (>= ${libgcc1_ver})${libgps_depends}")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libsysio-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${PROJECT_DESCRIPTION} (development files)")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}), cmake, bash, git-core, pkg-config${libgps_dev_depends}")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${SYSIO_SOURCE_DIR}/doc/README-deb.md")

endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
