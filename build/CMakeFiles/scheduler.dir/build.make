# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/dron/Рабочий стол/scheduler/scheduler"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/dron/Рабочий стол/scheduler/scheduler/build"

# Include any dependencies generated for this target.
include CMakeFiles/scheduler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/scheduler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/scheduler.dir/flags.make

CMakeFiles/scheduler.dir/main.c.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/scheduler.dir/main.c.o"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/scheduler.dir/main.c.o   -c "/home/dron/Рабочий стол/scheduler/scheduler/main.c"

CMakeFiles/scheduler.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/scheduler.dir/main.c.i"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/dron/Рабочий стол/scheduler/scheduler/main.c" > CMakeFiles/scheduler.dir/main.c.i

CMakeFiles/scheduler.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/scheduler.dir/main.c.s"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/dron/Рабочий стол/scheduler/scheduler/main.c" -o CMakeFiles/scheduler.dir/main.c.s

CMakeFiles/scheduler.dir/coroutine/coroutine.c.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/coroutine/coroutine.c.o: ../coroutine/coroutine.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/scheduler.dir/coroutine/coroutine.c.o"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/scheduler.dir/coroutine/coroutine.c.o   -c "/home/dron/Рабочий стол/scheduler/scheduler/coroutine/coroutine.c"

CMakeFiles/scheduler.dir/coroutine/coroutine.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/scheduler.dir/coroutine/coroutine.c.i"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/dron/Рабочий стол/scheduler/scheduler/coroutine/coroutine.c" > CMakeFiles/scheduler.dir/coroutine/coroutine.c.i

CMakeFiles/scheduler.dir/coroutine/coroutine.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/scheduler.dir/coroutine/coroutine.c.s"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/dron/Рабочий стол/scheduler/scheduler/coroutine/coroutine.c" -o CMakeFiles/scheduler.dir/coroutine/coroutine.c.s

CMakeFiles/scheduler.dir/context/context.c.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/context/context.c.o: ../context/context.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/scheduler.dir/context/context.c.o"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/scheduler.dir/context/context.c.o   -c "/home/dron/Рабочий стол/scheduler/scheduler/context/context.c"

CMakeFiles/scheduler.dir/context/context.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/scheduler.dir/context/context.c.i"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/dron/Рабочий стол/scheduler/scheduler/context/context.c" > CMakeFiles/scheduler.dir/context/context.c.i

CMakeFiles/scheduler.dir/context/context.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/scheduler.dir/context/context.c.s"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/dron/Рабочий стол/scheduler/scheduler/context/context.c" -o CMakeFiles/scheduler.dir/context/context.c.s

CMakeFiles/scheduler.dir/context/switch_context.S.o: CMakeFiles/scheduler.dir/flags.make
CMakeFiles/scheduler.dir/context/switch_context.S.o: ../context/switch_context.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/scheduler.dir/context/switch_context.S.o"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/scheduler.dir/context/switch_context.S.o   -c "/home/dron/Рабочий стол/scheduler/scheduler/context/switch_context.S"

CMakeFiles/scheduler.dir/context/switch_context.S.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/scheduler.dir/context/switch_context.S.i"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/home/dron/Рабочий стол/scheduler/scheduler/context/switch_context.S" > CMakeFiles/scheduler.dir/context/switch_context.S.i

CMakeFiles/scheduler.dir/context/switch_context.S.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/scheduler.dir/context/switch_context.S.s"
	/bin/clang-10 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/home/dron/Рабочий стол/scheduler/scheduler/context/switch_context.S" -o CMakeFiles/scheduler.dir/context/switch_context.S.s

# Object files for target scheduler
scheduler_OBJECTS = \
"CMakeFiles/scheduler.dir/main.c.o" \
"CMakeFiles/scheduler.dir/coroutine/coroutine.c.o" \
"CMakeFiles/scheduler.dir/context/context.c.o" \
"CMakeFiles/scheduler.dir/context/switch_context.S.o"

# External object files for target scheduler
scheduler_EXTERNAL_OBJECTS =

scheduler: CMakeFiles/scheduler.dir/main.c.o
scheduler: CMakeFiles/scheduler.dir/coroutine/coroutine.c.o
scheduler: CMakeFiles/scheduler.dir/context/context.c.o
scheduler: CMakeFiles/scheduler.dir/context/switch_context.S.o
scheduler: CMakeFiles/scheduler.dir/build.make
scheduler: CMakeFiles/scheduler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable scheduler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/scheduler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/scheduler.dir/build: scheduler

.PHONY : CMakeFiles/scheduler.dir/build

CMakeFiles/scheduler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/scheduler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/scheduler.dir/clean

CMakeFiles/scheduler.dir/depend:
	cd "/home/dron/Рабочий стол/scheduler/scheduler/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/dron/Рабочий стол/scheduler/scheduler" "/home/dron/Рабочий стол/scheduler/scheduler" "/home/dron/Рабочий стол/scheduler/scheduler/build" "/home/dron/Рабочий стол/scheduler/scheduler/build" "/home/dron/Рабочий стол/scheduler/scheduler/build/CMakeFiles/scheduler.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/scheduler.dir/depend
