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

# Include any dependencies generated for this target.
include shadow/src/external/elf-loader/CMakeFiles/vdl.dir/depend.make

# Include the progress variables for this target.
include shadow/src/external/elf-loader/CMakeFiles/vdl.dir/progress.make

# Include the compile flags for this target's objects.
include shadow/src/external/elf-loader/CMakeFiles/vdl.dir/flags.make

shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.o: shadow/src/external/elf-loader/CMakeFiles/vdl.dir/flags.make
shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.o: ../shadow/src/external/elf-loader/libvdl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/victor/blockchain-sim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.o"
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/vdl.dir/libvdl.c.o   -c /home/victor/blockchain-sim/shadow/src/external/elf-loader/libvdl.c

shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vdl.dir/libvdl.c.i"
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/victor/blockchain-sim/shadow/src/external/elf-loader/libvdl.c > CMakeFiles/vdl.dir/libvdl.c.i

shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vdl.dir/libvdl.c.s"
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/victor/blockchain-sim/shadow/src/external/elf-loader/libvdl.c -o CMakeFiles/vdl.dir/libvdl.c.s

# Object files for target vdl
vdl_OBJECTS = \
"CMakeFiles/vdl.dir/libvdl.c.o"

# External object files for target vdl
vdl_EXTERNAL_OBJECTS =

shadow/src/external/elf-loader/libvdl.so: shadow/src/external/elf-loader/CMakeFiles/vdl.dir/libvdl.c.o
shadow/src/external/elf-loader/libvdl.so: shadow/src/external/elf-loader/CMakeFiles/vdl.dir/build.make
shadow/src/external/elf-loader/libvdl.so: shadow/src/external/elf-loader/ldso
shadow/src/external/elf-loader/libvdl.so: shadow/src/external/elf-loader/CMakeFiles/vdl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/victor/blockchain-sim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libvdl.so"
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vdl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
shadow/src/external/elf-loader/CMakeFiles/vdl.dir/build: shadow/src/external/elf-loader/libvdl.so

.PHONY : shadow/src/external/elf-loader/CMakeFiles/vdl.dir/build

shadow/src/external/elf-loader/CMakeFiles/vdl.dir/clean:
	cd /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader && $(CMAKE_COMMAND) -P CMakeFiles/vdl.dir/cmake_clean.cmake
.PHONY : shadow/src/external/elf-loader/CMakeFiles/vdl.dir/clean

shadow/src/external/elf-loader/CMakeFiles/vdl.dir/depend:
	cd /home/victor/blockchain-sim/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/victor/blockchain-sim /home/victor/blockchain-sim/shadow/src/external/elf-loader /home/victor/blockchain-sim/cmake-build-debug /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader /home/victor/blockchain-sim/cmake-build-debug/shadow/src/external/elf-loader/CMakeFiles/vdl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shadow/src/external/elf-loader/CMakeFiles/vdl.dir/depend

