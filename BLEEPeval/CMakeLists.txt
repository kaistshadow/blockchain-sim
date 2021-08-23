add_compile_options(
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++2a>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.3.0>>:-std=c++17>)
if (BLEEP_DEBUG STREQUAL ON)
    message(STATUS "Debug enabled.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else (BLEEP_DEBUG STREQUAL ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif (BLEEP_DEBUG STREQUAL ON)

add_subdirectory(sybiltest-library/src)
add_subdirectory(sybiltest-app)
add_subdirectory(performtest-library/src)
add_subdirectory(performtest-app)