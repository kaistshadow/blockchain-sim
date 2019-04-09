project(BLEEPapp)

## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost REQUIRED serialization filesystem system)

message(STATUS "Boost_LIBRARIES:" ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.bleep")


## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
  ## we need to know where shadow repository is located
  if("${SHADOW_SRC_ROOT}" STREQUAL "")
    set(SHADOW_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../shadow")
  endif("${SHADOW_SRC_ROOT}" STREQUAL "")

  ## we need to find BLEEPlib
  if(EXISTS "${CMAKE_INSTALL_PREFIX}/cmake")
    message(STATUS "BLEEP_MODULE = ${CMAKE_INSTALL_PREFIX}/cmake")
  else()
    message(FATAL_ERROR "BLEEP module install path does not exist. You should install BLEEPlib first.")
  endif()

  set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/cmake" "${CMAKE_INSTALL_PREFIX}/cmake")
  # set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/share/cmake/Modules")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
  # message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()

include(ShadowTools)
find_package(BLEEP REQUIRED)

## compile for shadow example
add_shadow_plugin(NODE_bleep node.cpp)
install(TARGETS NODE_bleep DESTINATION plugins)
target_link_libraries(NODE_bleep BLEEP::BLEEP)

add_shadow_plugin(NODE_client client.cpp)
install(TARGETS NODE_client DESTINATION plugins)
target_link_libraries(NODE_client BLEEP::BLEEP)

## compile for local executable
add_executable(LOCAL_NODE_bleep node.cpp)
target_link_libraries(LOCAL_NODE_bleep BLEEP::BLEEP)

add_executable(LOCAL_NODE_client client.cpp)
target_link_libraries(LOCAL_NODE_client BLEEP::BLEEP)


add_subdirectory(test_nodes)