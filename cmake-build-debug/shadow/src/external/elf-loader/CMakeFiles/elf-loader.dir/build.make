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

# Utility rule file for elf-loader.

# Include the progress variables for this target.
include shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/progress.make

shadow/src/external/elf-loader/CMakeFiles/elf-loader: shadow/src/external/elf-loader/ldso
shadow/src/external/elf-loader/CMakeFiles/elf-loader: shadow/src/external/elf-loader/libvdl.so
shadow/src/external/elf-loader/CMakeFiles/elf-loader: shadow/src/external/elf-loader/elfedit
shadow/src/external/elf-loader/CMakeFiles/elf-loader: shadow/src/external/elf-loader/display-relocs


elf-loader: shadow/src/external/elf-loader/CMakeFiles/elf-loader
elf-loader: shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/build.make

.PHONY : elf-loader

# Rule to build all files generated by this target.
shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/build: elf-loader

.PHONY : shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/build

shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/clean:
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && $(CMAKE_COMMAND) -P CMakeFiles/elf-loader.dir/cmake_clean.cmake
.PHONY : shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/clean

shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/depend:
	cd /home/victor/blockchain-sim/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/victor/blockchain-sim /home/victor/blockchain-sim/shadow/src/external/elf-loader /home/victor/blockchain-sim/cmake-build-debug /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shadow/src/external/elf-loader/CMakeFiles/elf-loader.dir/depend

