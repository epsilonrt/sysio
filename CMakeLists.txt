###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# Sysio top-level CMakeLists.txt

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (SYSIO)
set (PROJECT_DESCRIPTION "Library and tools for embedded development")
set (PROJECT_TOOLS_PATHS ${PROJECT_SOURCE_DIR}/dev/bin ${PROJECT_SOURCE_DIR}/dev/rpi)

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake" "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})

set (SYSIO_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set (SYSIO_CFLAGS_OTHER ${CMAKE_THREAD_LIBS_INIT})

find_package(PkgConfig)
pkg_check_modules (LIBGPS libgps)
if (LIBGPS_FOUND)
  set (SYSIO_LDFLAGS_OTHER  ${LIBGPS_LDFLAGS})
  list(APPEND SYSIO_CFLAGS_OTHER ${LIBGPS_CFLAGS})
  set (SYSIO_WITH_GPS 0 CACHE BOOL "Enable GPS")
endif (LIBGPS_FOUND)

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
  set(DEF_INSTALL_DOC_DIR Doc)
  set(DEF_INSTALL_DATA_DIR Data)
else()
  set(DEF_INSTALL_CMAKE_DIR lib)
  set(DEF_INSTALL_DOC_DIR share/doc)
  set(DEF_INSTALL_DATA_DIR share)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
set(INSTALL_DOC_DIR ${DEF_INSTALL_DOC_DIR} CACHE PATH
  "Installation directory for documentation")
set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR} CACHE PATH
  "Installation directory for data")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE DATA DOC)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set (SYSIO_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (SYSIO_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set (SYSIO_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set (SYSIO_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/sysio/cmake")
set (SYSIO_INSTALL_DOC_DIR "${INSTALL_DOC_DIR}/sysio")
set (SYSIO_INSTALL_DATA_DIR "${INSTALL_DATA_DIR}/sysio")

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

add_definitions (-DPIBOARD_ID=${PIBOARD_ID})

# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (SYSIO_WITH_DOC 0 CACHE BOOL "Enable build of the documentation")

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  # Raspberry Pi ---------------------------------------------------------------
  set (SYSIO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (SYSIO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (SYSIO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_SRC_ARCH_DIR ${SYSIO_SRC_DIR}/arch/arm/rpi)

elseif (("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_NEO") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_AIR") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_M1"))
  # NanoPi Neo / Neo Air / M1  -------------------------------------------------
  set (SYSIO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (SYSIO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (SYSIO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_SRC_ARCH_DIR ${SYSIO_SRC_DIR}/arch/arm/nanopi)

elseif ("${PIBOARD_ID}" STREQUAL "BOARD_GENERIC_LINUX")
  # Linux Generic  -------------------------------------------------------------
  set (SYSIO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (SYSIO_SRC_ARCH_DIR ${SYSIO_SRC_DIR}/arch/generic)
  set (SYSIO_WITH_GPIO 0)
  set (SYSIO_WITH_I2C 0)
  set (SYSIO_WITH_SPI 0)
endif ()

if (CMAKE_HOST_UNIX)
  set (SYSIO_SRC_SYS_DIR ${SYSIO_SRC_DIR}/unix)
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

add_subdirectory (dev)
add_subdirectory (lib)
add_subdirectory (3rdparty)
add_subdirectory (radio)
if(SYSIO_WITH_I2C)
  add_subdirectory (chipio)
endif(SYSIO_WITH_I2C)
add_subdirectory (utils)
if (SYSIO_WITH_DOC)
  add_subdirectory (doc)
endif()

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_VERSION "${SYSIO_VERSION_MAJOR}.${SYSIO_VERSION_MINOR}-${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${SYSIO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SYSIO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SYSIO_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <epsilonrt@gmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

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
      else (ret EQUAL 0)
        message(WARNING "Unable to get debian architecture !")
      endif (ret EQUAL 0)
    endif (dpkg_cmd)
  endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)


  if (dpkg_query)
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libc6 OUTPUT_VARIABLE libc6_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libc6_ver} "-" libc6_ver_dash)
    #message ("libc6_ver_dash=${libc6_ver_dash}")
    #message ("libc6_ver=${libc6_ver}")
    if (libc6_ver_dash)
      string(SUBSTRING ${libc6_ver} 0 ${libc6_ver_dash} libc6_ver)
    endif (libc6_ver_dash)
    
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libgcc1 OUTPUT_VARIABLE libgcc1_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libgcc1_ver} "-" libgcc1_ver_dash)
    #message ("libgcc1_ver_dash=${libgcc1_ver_dash}")
    #message ("libgcc1_ver=${libgcc1_ver}")
    if (libgcc1_ver_dash)
      string(SUBSTRING ${libgcc1_ver} 0 ${libgcc1_ver_dash} libgcc1_ver)
    endif (libgcc1_ver_dash)
    
    #message ("libc6_ver=${libc6_ver}")
    #message ("libgcc1_ver=${libgcc1_ver}")
  endif (dpkg_query)
  
  if (SYSIO_WITH_GPS)
  
    if (dpkg_query)
      execute_process(COMMAND ${dpkg_query} --showformat=\${Package} --show libgps2? OUTPUT_VARIABLE package OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (package)
        execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show ${package} OUTPUT_VARIABLE ver OUTPUT_STRIP_TRAILING_WHITESPACE)
        string(FIND ${ver} "-" dash)
        #message ("dash=${dash}")
        #message ("ver=${ver}")
        if (dash)
          string(SUBSTRING ${ver} 0 ${dash} ver)
        endif (dash)
        set(libgps_depends ", ${package} (>= ${ver})")
        set(libgps_dev_depends ", libgps-dev (>= ${ver})")
        #message ("libgps_depends=${libgps_depends}")
        #message ("libgps_dev_depends=${libgps_dev_depends}")
        unset(ver)
        unset(dash)
      endif (package)
      unset(package)
    endif (dpkg_query)
  endif(SYSIO_WITH_GPS)

  message(STATUS "Debian architecture ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libsysio")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${PROJECT_DESCRIPTION} (library)")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= ${libc6_ver}), libgcc1 (>= ${libgcc1_ver})${libgps_depends}")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")
  set(CPACK_DEBIAN_LIB_PACKAGE_CONFLICTS "libmodbus5")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libsysio-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${PROJECT_DESCRIPTION} (development files)")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}), cmake, bash, git-core, pkg-config${libgps_dev_depends}")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${SYSIO_SRC_DIR}/doc/README-deb.md")
  
  set(CPACK_DEBIAN_UTILS_PACKAGE_NAME "libsysio-utils")
  set(CPACK_COMPONENT_UTILS_DESCRIPTION "${PROJECT_DESCRIPTION} (utilities)")
  set(CPACK_DEBIAN_UTILS_FILE_NAME "lib${PROJECT_NAME}-utils_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_UTILS_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_UTILS_PACKAGE_SECTION "utils")

  if (SYSIO_WITH_DOC)
    set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libsysio-doc")
    set(CPACK_COMPONENT_DOC_DESCRIPTION "${PROJECT_DESCRIPTION} (documentation)")
    set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
    set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
    set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")
  endif()
  
endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
