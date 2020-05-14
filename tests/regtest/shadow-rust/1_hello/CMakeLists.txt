## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake") # Path for Rust Module
enable_language(Rust)
enable_testing()
## required for independent build


set(CARGO_NAME "hello")

set(LIB_FILE "${CMAKE_CURRENT_BINARY_DIR}/debug/${CMAKE_STATIC_LIBRARY_PREFIX}${CARGO_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(CARGO_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CARGO_ENV_COMMAND ${CMAKE_COMMAND} -E env "CARGO_TARGET_DIR=${CARGO_TARGET_DIR}")

set(CARGO_ARGS "build")
file(GLOB_RECURSE LIB_SOURCES "*.rs")

### compilation 
add_custom_command(
  OUTPUT ${LIB_FILE}
  COMMAND ${CARGO_ENV_COMMAND} ${CARGO_EXECUTABLE} ARGS ${CARGO_ARGS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${LIB_SOURCES}
  COMMENT "running cargo")

add_custom_target(${CARGO_NAME}_target ALL DEPENDS ${LIB_FILE})

### add test
add_test(NAME regtest-rust-hello
  COMMAND shadow ${CMAKE_CURRENT_SOURCE_DIR}/rust_hello.xml
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/debug/")



####
# set(CARGO_NAME "hello")

# set(LIB_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_BUILD_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}${CARGO_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
# set(LIB_COMPILED_FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_BUILD_TYPE}/${CARGO_NAME}")

# set(CARGO_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
# set(CARGO_ENV_COMMAND ${CMAKE_COMMAND} -E env "CARGO_TARGET_DIR=${CARGO_TARGET_DIR}")

# set(CARGO_ARGS "build")
# file(GLOB_RECURSE LIB_SOURCES "*.rs")

# ### compilation 
# add_custom_command(
#   OUTPUT ${LIB_FILE}
#   COMMAND ${CARGO_ENV_COMMAND} ${CARGO_EXECUTABLE} ARGS ${CARGO_ARGS}
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#   DEPENDS ${LIB_SOURCES}
#   COMMENT "running cargo")

# add_custom_target(${CARGO_NAME}_target ALL DEPENDS ${LIB_FILE})


# ### add test
# add_test(NAME BLEEP-regtest-rust-hello
#   COMMAND shadow ${CMAKE_CURRENT_SOURCE_DIR}/rust_hello.xml
#   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
