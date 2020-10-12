## ensure cmake version
cmake_minimum_required(VERSION 2.8)

## we use cmake files from shadow
if(EXISTS "${CMAKE_MODULE_PATH}")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
else()
    ## we need to know where shadow repository is located
    if("${SHADOW_SRC_ROOT}" STREQUAL "")
        set(SHADOW_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../shadow")
    endif("${SHADOW_SRC_ROOT}" STREQUAL "")

    set(CMAKE_MODULE_PATH "${SHADOW_SRC_ROOT}/cmake")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../Install/cmake")
    message(STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
endif()


include(ShadowTools)


add_subdirectory(BLEEP-simpleconnect)
add_subdirectory(BLEEP-gossip-10node)
add_subdirectory(BLEEP-POW-consensus)
# add_subdirectory(shadow-rust) # it works well on local machine, but does not work on github Action, so commentified.
add_subdirectory(native-bintest)
add_subdirectory(shadow-bitcoin)
add_subdirectory(shadow-dependencies)
# add_subdirectory(shadow-geth) # results in deadlock due to futex issue
add_subdirectory(shadow-syscalls)

if(DGIT_MONERO_OFF STREQUAL OFF)
    add_subdirectory(shadow-monero)
endif()

# else(DGIT_MONERO_OFF STREQUAL ON)
#     message(STATUS "xxx Monero start.")
