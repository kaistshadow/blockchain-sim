
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

enable_language(Rust)
include(CMakeCargo)

if(NOT CMAKE_BUILD_TYPE)
  set(LIB_BUILD_TYPE "debug")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  set(LIB_BUILD_TYPE "release")
else()
  set(LIB_BUILD_TYPE "debug")
endif()

add_subdirectory(1_hello)

