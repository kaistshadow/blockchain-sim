if (POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)
endif(POLICY CMP0048)

project(BLEEP)

## ensure cmake version
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
## ensure unix environment (CMAKE_SYSTEM_NAME == "Linux")
if((NOT UNIX) OR (NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux")))
    message(FATAL_ERROR "BLEEP requires a Unix/Linux environment.")
endif((NOT UNIX) OR (NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux")))

## ensure out-of-source build
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Shadow requires an out-of-source build. Please create a separate build directory and run 'cmake path/to/shadow [options]' there.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

set(CMAKE_ROOT_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_ROOT_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_VERBOSE_MAKEFILE ON)


# debug option
option(BLEEP_DEBUG "turn on debugging for BLEEP (default: ON)" ON)
option(SHADOW_DEBUG "turn on debugging for verbose program output (default: ON for BLEEP build)" ON)
# enable shadow debug as default. (Because some tests failed with release version of shadow (e.g. shadow-bitcoin-with-wallet)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BLEEP_DEBUG ON)
    set(SHADOW_DEBUG ON)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BLEEP_DEBUG OFF)
    set(SHADOW_DEBUG OFF)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

# Bitcoin option
option(BITCOIN_OPT "Bitcoin on build (default: OFF)" OFF)

# all option
option(ALL_OPT "All on build (deafult : OFF)" OFF)

# git option
option(GIT_OPT "Only run on git action (deafult : OFF)" OFF)

# bleep lib option
option(BLEEPLIB_OPT "BLEEPLIB on build (default: ON)" ON)


message("------------------------------------------------------------------------------------------------------")
message("                                          BITCOIN_OPT : ${BITCOIN_OPT}")
message("")
message("")
message("                                            ALL_OPT   : ${ALL_OPT}")
message("                                            GIT_OPT   : ${GIT_OPT}")
message("                                         BLEEPLIB_OPT : ${BLEEPLIB_OPT}")
message("------------------------------------------------------------------------------------------------------")

find_package (PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.42)
message(STATUS "GLIB_VERSION = ${GLIB_VERSION}")
add_subdirectory(shadow)

# Unit test option
option(UNITTEST_OPT "unittest on build (default : OFF) : OFF")

## add BLEEP
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install")
enable_testing()
add_subdirectory(interfaces)
add_subdirectory(external)

if(BLEEPLIB_OPT STREQUAL ON)
    add_subdirectory(BLEEPlib)
endif(BLEEPLIB_OPT STREQUAL ON)

# add_subdirectory(BLEEPapp)
add_subdirectory(tests)

## add command for lauching BLEEP web interface
add_custom_target(run-web-gui
  COMMAND echo " "
  COMMAND echo "Direct launch of the server using make is not possible"
  COMMAND echo "due to the unresolved problem during relaunching process of shadow."
  COMMAND echo "Move to ${CMAKE_ROOT_SOURCE_DIR}/test/BLEEP/web-gui and type following command"
  COMMAND echo " $$ node chat-server.js"
  COMMAND echo " "
  WORKING_DIRECTORY ${CMAKE_ROOT_SOURCE_DIR}/test/BLEEP/web-gui
  )
