# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/centurion/Documents/OS/fsparse

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/centurion/Documents/OS/fsparse/build_make

# Include any dependencies generated for this target.
include CMakeFiles/vector.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/vector.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/vector.dir/flags.make

CMakeFiles/vector.dir/src/vector.c.o: CMakeFiles/vector.dir/flags.make
CMakeFiles/vector.dir/src/vector.c.o: ../src/vector.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/centurion/Documents/OS/fsparse/build_make/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/vector.dir/src/vector.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/vector.dir/src/vector.c.o   -c /home/centurion/Documents/OS/fsparse/src/vector.c

CMakeFiles/vector.dir/src/vector.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vector.dir/src/vector.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/centurion/Documents/OS/fsparse/src/vector.c > CMakeFiles/vector.dir/src/vector.c.i

CMakeFiles/vector.dir/src/vector.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vector.dir/src/vector.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/centurion/Documents/OS/fsparse/src/vector.c -o CMakeFiles/vector.dir/src/vector.c.s

# Object files for target vector
vector_OBJECTS = \
"CMakeFiles/vector.dir/src/vector.c.o"

# External object files for target vector
vector_EXTERNAL_OBJECTS =

libvector.a: CMakeFiles/vector.dir/src/vector.c.o
libvector.a: CMakeFiles/vector.dir/build.make
libvector.a: CMakeFiles/vector.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/centurion/Documents/OS/fsparse/build_make/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libvector.a"
	$(CMAKE_COMMAND) -P CMakeFiles/vector.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/vector.dir/build: libvector.a

.PHONY : CMakeFiles/vector.dir/build

CMakeFiles/vector.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/vector.dir/cmake_clean.cmake
.PHONY : CMakeFiles/vector.dir/clean

CMakeFiles/vector.dir/depend:
	cd /home/centurion/Documents/OS/fsparse/build_make && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/centurion/Documents/OS/fsparse /home/centurion/Documents/OS/fsparse /home/centurion/Documents/OS/fsparse/build_make /home/centurion/Documents/OS/fsparse/build_make /home/centurion/Documents/OS/fsparse/build_make/CMakeFiles/vector.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/vector.dir/depend

