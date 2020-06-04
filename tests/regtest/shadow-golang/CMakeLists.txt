## required for independent build
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake") # Path for go Module
enable_language(go)
enable_testing()
## required for independent build

# go configrations
# set(GO_SRCS test.go)
file(GLOB_RECURSE LIB_SOURCES "*.go")
set(GO_LIBNAME libgotest.so)

set(GO_NAME "go_test")
#set(CARGO_NAME "hello")

### compilation
# Custom command for 'go build -buildmode=c-shared ...'
# to create a c-shared library
add_custom_command(
        OUTPUT ${GO_LIBNAME}
        DEPENDS ${LIB_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND env go build -buildmode=c-shared
        -o "${CMAKE_CURRENT_BINARY_DIR}/${GO_LIBNAME}"
        COMMENT "building go lib")

add_custom_target(${GO_NAME}_target ALL DEPENDS ${GO_LIBNAME})

message(STATUS "go_test:CMAKE_CURRENT_BINARY_DIR:${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "go_test:CMAKE_CURRENT_SOURCE_DIR:${CMAKE_CURRENT_SOURCE_DIR}")

### add test
add_test(NAME regtest-go-test
        COMMAND shadow ${CMAKE_CURRENT_SOURCE_DIR}/gotest.config.xml
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/debug/")

