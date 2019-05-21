project(BLEEPapp)

## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# debug option
if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++11 -Wall -Werror -Wno-switch")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-switch")
endif(BLEEP_DEBUG STREQUAL ON)


# link_directories($ENV{HOME}/opt/libev4/lib)   # for debuggin libev

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

add_shadow_plugin(NODE_POW node_pow.cpp)
install(TARGETS NODE_POW DESTINATION plugins)
target_link_libraries(NODE_POW BLEEP::BLEEP)

add_shadow_plugin(NODE_POW_TX node_pow_tx.cpp)
install(TARGETS NODE_POW_TX DESTINATION plugins)
target_link_libraries(NODE_POW_TX BLEEP::BLEEP)

add_shadow_plugin(NODE_MINING node_mining.cpp)
install(TARGETS NODE_MINING DESTINATION plugins)
target_link_libraries(NODE_MINING BLEEP::BLEEP)


## compile for local executable
add_executable(LOCAL_NODE_bleep node.cpp)
target_link_libraries(LOCAL_NODE_bleep BLEEP::BLEEP)

add_executable(LOCAL_NODE_client client.cpp)
target_link_libraries(LOCAL_NODE_client BLEEP::BLEEP)

add_executable(LOCAL_NODE_POW node_pow.cpp)
target_link_libraries(LOCAL_NODE_POW BLEEP::BLEEP)

add_executable(LOCAL_NODE_POW_TX node_pow_tx.cpp)
target_link_libraries(LOCAL_NODE_POW_TX BLEEP::BLEEP)

add_executable(LOCAL_NODE_MINING node_mining.cpp)
target_link_libraries(LOCAL_NODE_MINING BLEEP::BLEEP)

# add tests
enable_testing()
add_subdirectory(test_nodes)
