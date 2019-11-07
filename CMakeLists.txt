cmake_minimum_required(VERSION 2.4)

project(openmi-base)

set(CMAKE_VERBOSE_MAKEFILE off)  # on/off

#SET(CMAKE_BUILD_TYPE "Release")

set(BASIC_CXXFLAGS "-std=c++11 -g -Wall -pthread")
set(PIC_FLAGS "-fPIC")
#set(BASIC_CPPDEFINES "-DNDEBUG")

set(CMAKE_CXX_FLAGS "${BASIC_CXXFLAGS} ${PIC_FLAGS}")

message(STATUS "DEPS_STATIC_LIBRARIES: ${DEPS_STATIC_LIBRARIES}")
message(STATUS "DEPS_SHARED_LIBRARIES: ${DEPS_SHARED_LIBRARIES}")

include_directories(
  $ENV{DEPS_ENV_HOME}/include  
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/base
  ${PROJECT_SOURCE_DIR}/../openmi-idl
)

link_directories(
  $ENV{DEPS_ENV_HOME}/lib 
  ${PROJECT_SOURCE_DIR}/../openmi-idl/lib
  ${PROJECT_SOURCE_DIR}/lib
)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
  link_directories($ENV{DEPS_ENV_HOME}/lib64)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) 

#set(dep_static_libraries "")
#set(STATIC_LINKER_FLAGS "-Wl,-Bstatic ${dep_static_libraries}")
#set(dep_dynamic_libraries "")
#set(SHARED_LINKER_FLAGS "-Wl,-Bdynamic ${dep_dynamic_libraries}")
#set(summary_linker_flags "${STATIC_LINKER_FLAGS} ${SHARED_LINKER_FLAGS}") 
set(summary_linker_flags "")

aux_source_directory(base srcs_base)
set(srcs ${srcs_base})

set(libname openmi_base)

add_library(${libname} SHARED ${srcs})
target_link_libraries(${libname} ${summary_linker_flags})
add_library(${libname}_static STATIC ${srcs})
target_link_libraries(${libname}_static ${summary_linker_flags})
set_target_properties(${libname}_static PROPERTIES OUTPUT_NAME ${libname})

add_subdirectory(base/test)
