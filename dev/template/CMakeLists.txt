cmake_minimum_required(VERSION 3.5)

project(template)

find_package(sysio REQUIRED)
link_directories(${SYSIO_LIBRARY_DIRS})

include (PiBoardInfo)
if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

file(GLOB SRC *.c)

add_executable(${CMAKE_PROJECT_NAME} ${SRC})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SYSIO_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${SYSIO_INCLUDE_DIRS})
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC ${SYSIO_CFLAGS})

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)
