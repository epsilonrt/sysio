# top-level CMakeLists.txt

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (SYSIO)
set (PROJECT_DESCRIPTION "Library and tools for embedded development")
string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

set (SYSIO_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig)
pkg_check_modules (LIBGPS libgps)
find_package(Git)

include (GetGitVersion)
include (GetPiBoardInfo)

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
  add_definitions (-DSYS_LINUX)
  set (SYSIO_SYS_DIR sysio/src/unix)
  message (STATUS "Build for Linux target")
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
