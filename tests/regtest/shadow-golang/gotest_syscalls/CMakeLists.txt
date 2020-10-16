## required for independent build
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake") # Path for go Module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "../../../../cmake")

#enable_language(go)
enable_testing()
## required for independent build
include(${CMAKE_MODULE_PATH}/golang.cmake)
include(${CMAKE_MODULE_PATH}/flags.cmake)

set(SOURCE_FILE gotest_syscalls.go)


ADD_GO_LIBRARY(gotest_syscalls ${SOURCE_FILE})


### add test
add_test(NAME regtest-go-test
        COMMAND shadow gotest_syscalls.config.xml
        WORKING_DIRECTORY "..")

