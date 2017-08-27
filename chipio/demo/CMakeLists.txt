cmake_minimum_required(VERSION 3.5)

project (chipio_demo)

find_package(chipio REQUIRED)
link_directories(${CHIPIO_LIBRARY_DIRS})

include (PiBoardInfo)
if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

file(GLOB_RECURSE demo_list *.c)
#message("demos=${demo_list}")

foreach(f ${demo_list})
  get_filename_component(demo_name ${f} NAME_WE)
  get_filename_component(demo_dir ${f} DIRECTORY)
  if ("${demo_name}" MATCHES "${PROJECT_NAME}*")
  
    message (STATUS "Add ${demo_name} executable...")
    add_executable (${demo_name} ${f}) 
    
    target_link_libraries(${demo_name} ${CHIPIO_LIBRARIES})
    target_include_directories(${demo_name} PUBLIC ${demo_dir} ${CHIPIO_INCLUDE_DIRS})
    target_compile_options(${demo_name} PUBLIC ${CHIPIO_CFLAGS})
    
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${demo_name}
      DESTINATION bin)
  endif()
endforeach(f)
