if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(BLEEP_DEBUG STREQUAL ON)

add_subdirectory(regtest)
add_subdirectory(lib/googletest)
# add_subdirectory(unittest)

