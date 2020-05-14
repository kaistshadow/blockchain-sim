## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake") # Path for Rust Module
enable_language(Rust)
enable_testing()
## required for independent build


set(CARGO_NAME "hellorust")

set(EXE_FILE "${CMAKE_CURRENT_BINARY_DIR}/debug/${CARGO_NAME}") # default rust build type is debug unless the explicit option is given

set(CARGO_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CARGO_ENV_COMMAND ${CMAKE_COMMAND} -E env "CARGO_TARGET_DIR=${CARGO_TARGET_DIR}")

set(CARGO_ARGS "build")
file(GLOB_RECURSE SOURCES "*.rs")

### compilation 
add_custom_command(
  OUTPUT ${EXE_FILE}
  COMMAND ${CARGO_ENV_COMMAND} ${CARGO_EXECUTABLE} ARGS ${CARGO_ARGS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${SOURCES}
  COMMENT "running cargo")

add_custom_target(${CARGO_NAME}_target ALL DEPENDS ${EXE_FILE})


### add test
add_test(NAME BLEEP-regtest-native-bintest-hellorust
  COMMAND ${EXE_FILE})

