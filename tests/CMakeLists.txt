cmake_minimum_required(VERSION 2.8...3.13)

add_compile_options(
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++2a>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++17>)

if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif(BLEEP_DEBUG STREQUAL ON)
add_subdirectory(lib/googletest)

if(UNITTEST_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                            Start Unit tests")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(unittest)
elseif(TEST_OPT STREQUAL ON)
  message("--------------------------------------------------------------------------------------")
  message("                            Start regression tests")
  message("--------------------------------------------------------------------------------------")
  add_subdirectory(regtest)

endif()


