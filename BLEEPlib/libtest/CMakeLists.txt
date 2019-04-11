## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## ensure out-of-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "An out-of-source build is required. Please see the README.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++11 -Wall -Werror")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
endif(BLEEP_DEBUG STREQUAL ON)



find_package(Boost REQUIRED serialization filesystem system)

message(STATUS "Boost_LIBRARIES:" ${Boost_LIBRARIES})

include_directories(${Boost_INCLUDE_DIRS})


set(FSM_SRCS
  utility/GlobalClock.cpp
  utility/ArgsManager.cpp
  # statemachine/examples/simple-idle-exit-machine/State.cpp
  # statemachine/examples/simple-idle-exit-machine/StateHandler.cpp
  # statemachine/examples/simple-idle-exit-machine/StateMachine.cpp
  statemachine/examples/singlenode-blockchain-machine/State.cpp
  statemachine/examples/singlenode-blockchain-machine/StateHandler.cpp
  statemachine/examples/singlenode-blockchain-machine/StateMachine.cpp
  # statemachine/examples/doublenode-blockchain-machine/State.cpp
  # statemachine/examples/doublenode-blockchain-machine/StateHandler.cpp
  # statemachine/examples/doublenode-blockchain-machine/StateMachine.cpp
)
  
# if(EXISTS "${CMAKE_INSTALL_PREFIX}")
#   message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
# else()
#   message(FATAL_ERROR "CMAKE_INSTALL_PREFIX path does not exist: '${CMAKE_INSTALL_PREFIX}'")
# endif()

## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
  ## we need to know where shadow is installed
  if("${SHADOW_SRC_ROOT}" STREQUAL "")
    set(SHADOW_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow")
  endif("${SHADOW_SRC_ROOT}" STREQUAL "")

  set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/cmake")
  # set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/share/cmake/Modules")
  message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

  # message(FATAL_ERROR "CMAKE_MODULE_PATH path does not exist: '${CMAKE_MODULE_PATH}'")
endif()

# include shadowtools for making shadow_plugin
include(ShadowTools)

# add_executable(HELLO hello.cpp ${SRCS})

add_shadow_plugin(NODE_FSM node.cpp ${FSM_SRCS})
install(TARGETS NODE_FSM DESTINATION plugins)
target_link_libraries(NODE_FSM -lev)
target_link_libraries(NODE_FSM -lpthread)
target_link_libraries(NODE_FSM ${Boost_LIBRARIES})
target_link_libraries(NODE_FSM SHADOW_INTERFACE)
target_link_libraries(NODE_FSM BLEEP)

add_executable(LOCAL_NODE_FSM node.cpp ${FSM_SRCS})
target_link_libraries(LOCAL_NODE_FSM -lev)
target_link_libraries(LOCAL_NODE_FSM -lpthread)
target_link_libraries(LOCAL_NODE_FSM ${Boost_LIBRARIES})
target_link_libraries(LOCAL_NODE_FSM SHADOW_INTERFACE)
target_link_libraries(LOCAL_NODE_FSM BLEEP)


add_test(NAME BLEEP-FSM-singlenode
  COMMAND python test-rc1-fsm.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../BLEEPeval)