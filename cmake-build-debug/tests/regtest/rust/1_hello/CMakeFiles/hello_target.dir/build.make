# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/107/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/107/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/victor/blockchain-sim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/victor/blockchain-sim/cmake-build-debug

# Utility rule file for hello_target.

# Include the progress variables for this target.
include tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/progress.make

tests/regtest/rust/1_hello/CMakeFiles/hello_target: tests/regtest/rust/1_hello/debug/libhello.so


tests/regtest/rust/1_hello/debug/libhello.so: ../tests/regtest/rust/1_hello/src/lib.rs
tests/regtest/rust/1_hello/debug/libhello.so: ../tests/regtest/rust/1_hello/src/main.rs
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/victor/blockchain-sim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "running cargo"
	cd /home/victor/blockchain-sim/tests/regtest/rust/1_hello && /snap/clion/107/bin/cmake/linux/bin/cmake -E env CARGO_TARGET_DIR=/home/victor/blockchain-sim/cmake-build-debug/tests/regtest/rust/1_hello /usr/bin/cargo build

hello_target: tests/regtest/rust/1_hello/CMakeFiles/hello_target
hello_target: tests/regtest/rust/1_hello/debug/libhello.so
hello_target: tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/build.make

.PHONY : hello_target

# Rule to build all files generated by this target.
tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/build: hello_target

.PHONY : tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/build

tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/clean:
	cd /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/rust/1_hello && $(CMAKE_COMMAND) -P CMakeFiles/hello_target.dir/cmake_clean.cmake
.PHONY : tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/clean

tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/depend:
	cd /home/victor/blockchain-sim/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/victor/blockchain-sim /home/victor/blockchain-sim/tests/regtest/rust/1_hello /home/victor/blockchain-sim/cmake-build-debug /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/rust/1_hello /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/regtest/rust/1_hello/CMakeFiles/hello_target.dir/depend

