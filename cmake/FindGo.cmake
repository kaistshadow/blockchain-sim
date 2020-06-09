
set(_CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(_CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ${CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

if(CMAKE_HOST_WIN32)
    set(USER_HOME "$ENV{USERPROFILE}")
else()
    set(USER_HOME "$ENV{HOME}")
endif()

if(NOT DEFINED CARGO_HOME)
    if("$ENV{CARGO_HOME}" STREQUAL "")
        set(CARGO_HOME "${USER_HOME}/.cargo")
    else()
        set(CARGO_HOME "$ENV{CARGO_HOME}")
    endif()
endif()

# Find cargo executable
find_program(CARGO_EXECUTABLE cargo
        HINTS "${CARGO_HOME}"
        PATH_SUFFIXES "bin")
mark_as_advanced(CARGO_EXECUTABLE)

# Find go executable
find_program(GO_EXECUTABLE go
        HINTS "${CARGO_HOME}"
        PATH_SUFFIXES "bin")
mark_as_advanced(GO_EXECUTABLE)

# Find godoc executable
find_program(GODOC_EXECUTABLE godoc
        HINTS "${CARGO_HOME}"
        PATH_SUFFIXES "bin")
mark_as_advanced(GODOC_EXECUTABLE)

# Find go-gdb executable
find_program(GO_GDB_EXECUTABLE go-gdb
        HINTS "${CARGO_HOME}"
        PATH_SUFFIXES "bin")
mark_as_advanced(GO_GDB_EXECUTABLE)

# Find rust-lldb executable
find_program(GO_LLDB_EXECUTABLE go-lldb
        HINTS "${CARGO_HOME}"
        PATH_SUFFIXES "bin")
mark_as_advanced(GO_LLDB_EXECUTABLE)

# Find rustup executable
#find_program(RUSTUP_EXECUTABLE rustup
#        HINTS "${CARGO_HOME}"
#        PATH_SUFFIXES "bin")
#mark_as_advanced(RUSTUP_EXECUTABLE)

set(GO_FOUND FALSE CACHE INTERNAL "")

if(CARGO_EXECUTABLE AND GO_EXECUTABLE AND GODOC_EXECUTABLE)
    set(GO_FOUND TRUE CACHE INTERNAL "")

    set(CARGO_HOME "${CARGO_HOME}" CACHE PATH "Go Cargo Home")

    execute_process(COMMAND ${GO_EXECUTABLE} --version OUTPUT_VARIABLE GO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "go ([^ ]+) .*" "\\1" GO_VERSION "${GO_VERSION}")
endif()

if(NOT GO_FOUND)
    message(FATAL_ERROR "Could not find Go!")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ${_CMAKE_FIND_ROOT_PATH_MODE_PROGRAM})
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ${_CMAKE_FIND_ROOT_PATH_MODE_INCLUDE})
