cmake_minimum_required(VERSION 3.5)

set(TARGET sysio_demo_led)

find_package(sysio REQUIRED)
link_directories(${SYSIO_LIBRARY_DIRS})

include (PiBoardInfo)
if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

file(GLOB SRC *.c)

add_executable(${TARGET} ${SRC})

target_link_libraries(${TARGET} ${SYSIO_LIBRARIES})
target_include_directories(${TARGET} PUBLIC ${SYSIO_INCLUDE_DIRS})
target_compile_options(${TARGET} PUBLIC ${SYSIO_CFLAGS})

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
