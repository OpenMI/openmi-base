cmake_minimum_required(VERSION 2.4) 

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

function (add_program arg)
  list(LENGTH ARGV  argv_len)
  if (NOT (${argv_len} STREQUAL "1")) 
    message(FATAL_ERROR "argment number should be equals to 1")
  endif()

  set(prog_name ${ARGV0})
  message(STATUS "exec_name: ${prog_name}, src_name: ${prog_name}.cc")
  add_executable(${prog_name} "${prog_name}.cc")
  target_link_libraries(${prog_name} ${DEPS_STATIC_LIBRARIES} ${DEPS_SHARED_LIBRARIES} openmi_base gflags)
endfunction()

add_program("allocator_test")
add_program("logging_test")
add_program("sarray_test")
add_program("timer_test")
add_program("errors_test")
add_program("status_test")
