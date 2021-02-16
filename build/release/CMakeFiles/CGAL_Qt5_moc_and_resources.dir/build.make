# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/adminlocal/PhD/cpp/spred

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adminlocal/PhD/cpp/spred/build/release

# Include any dependencies generated for this target.
include CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CGAL_Qt5_moc_and_resources.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make

ui_ImageInterface.h: /usr/local/include/CGAL/Qt/resources/ImageInterface.ui
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ui_ImageInterface.h"
	/usr/lib/qt5/bin/uic -o /home/adminlocal/PhD/cpp/spred/build/release/ui_ImageInterface.h /usr/local/include/CGAL/Qt/resources/ImageInterface.ui

qrc_CGAL.cpp: /usr/local/lib/cmake/CGAL/demo/resources/about_CGAL.html
qrc_CGAL.cpp: /usr/local/lib/cmake/CGAL/demo/resources/cgal_logo_ipe_2013.png
qrc_CGAL.cpp: /usr/local/lib/cmake/CGAL/demo/resources/cgal_logo_ipe_2013.png
qrc_CGAL.cpp: /usr/local/lib/cmake/CGAL/demo/resources/cgal_logo.xpm
qrc_CGAL.cpp: /usr/local/lib/cmake/CGAL/demo/resources/CGAL.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating qrc_CGAL.cpp"
	/usr/lib/qt5/bin/rcc --name CGAL --output /home/adminlocal/PhD/cpp/spred/build/release/qrc_CGAL.cpp /usr/local/lib/cmake/CGAL/demo/resources/CGAL.qrc

qrc_Input.cpp: /usr/local/lib/cmake/CGAL/demo/icons/inputPoint.png
qrc_Input.cpp: /usr/local/lib/cmake/CGAL/demo/icons/fit-page-32.png
qrc_Input.cpp: /usr/local/lib/cmake/CGAL/demo/icons/inputPolyline.png
qrc_Input.cpp: /usr/local/lib/cmake/CGAL/demo/icons/Input.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating qrc_Input.cpp"
	/usr/lib/qt5/bin/rcc --name Input --output /home/adminlocal/PhD/cpp/spred/build/release/qrc_Input.cpp /usr/local/lib/cmake/CGAL/demo/icons/Input.qrc

qrc_File.cpp: /usr/local/lib/cmake/CGAL/demo/icons/fileNew.png
qrc_File.cpp: /usr/local/lib/cmake/CGAL/demo/icons/fileOpen.png
qrc_File.cpp: /usr/local/lib/cmake/CGAL/demo/icons/fileSave.png
qrc_File.cpp: /usr/local/lib/cmake/CGAL/demo/icons/File.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Generating qrc_File.cpp"
	/usr/lib/qt5/bin/rcc --name File --output /home/adminlocal/PhD/cpp/spred/build/release/qrc_File.cpp /usr/local/lib/cmake/CGAL/demo/icons/File.qrc

qrc_Triangulation_2.cpp: /usr/local/lib/cmake/CGAL/demo/icons/Delaunay_triangulation_2.png
qrc_Triangulation_2.cpp: /usr/local/lib/cmake/CGAL/demo/icons/Voronoi_diagram_2.png
qrc_Triangulation_2.cpp: /usr/local/lib/cmake/CGAL/demo/icons/Triangulation_2.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Generating qrc_Triangulation_2.cpp"
	/usr/lib/qt5/bin/rcc --name Triangulation_2 --output /home/adminlocal/PhD/cpp/spred/build/release/qrc_Triangulation_2.cpp /usr/local/lib/cmake/CGAL/demo/icons/Triangulation_2.qrc

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o: CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o -c /home/adminlocal/PhD/cpp/spred/build/release/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adminlocal/PhD/cpp/spred/build/release/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp > CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.i

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adminlocal/PhD/cpp/spred/build/release/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.s

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o: qrc_CGAL.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o -c /home/adminlocal/PhD/cpp/spred/build/release/qrc_CGAL.cpp

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adminlocal/PhD/cpp/spred/build/release/qrc_CGAL.cpp > CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.i

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adminlocal/PhD/cpp/spred/build/release/qrc_CGAL.cpp -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.s

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o: qrc_Input.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o -c /home/adminlocal/PhD/cpp/spred/build/release/qrc_Input.cpp

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adminlocal/PhD/cpp/spred/build/release/qrc_Input.cpp > CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.i

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adminlocal/PhD/cpp/spred/build/release/qrc_Input.cpp -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.s

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o: qrc_File.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o -c /home/adminlocal/PhD/cpp/spred/build/release/qrc_File.cpp

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adminlocal/PhD/cpp/spred/build/release/qrc_File.cpp > CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.i

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adminlocal/PhD/cpp/spred/build/release/qrc_File.cpp -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.s

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/flags.make
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o: qrc_Triangulation_2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o -c /home/adminlocal/PhD/cpp/spred/build/release/qrc_Triangulation_2.cpp

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/adminlocal/PhD/cpp/spred/build/release/qrc_Triangulation_2.cpp > CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.i

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/adminlocal/PhD/cpp/spred/build/release/qrc_Triangulation_2.cpp -o CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.s

# Object files for target CGAL_Qt5_moc_and_resources
CGAL_Qt5_moc_and_resources_OBJECTS = \
"CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o" \
"CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o" \
"CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o" \
"CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o"

# External object files for target CGAL_Qt5_moc_and_resources
CGAL_Qt5_moc_and_resources_EXTERNAL_OBJECTS =

libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/CGAL_Qt5_moc_and_resources_autogen/mocs_compilation.cpp.o
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_CGAL.cpp.o
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Input.cpp.o
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_File.cpp.o
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/qrc_Triangulation_2.cpp.o
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/build.make
libCGAL_Qt5_moc_and_resources.a: CMakeFiles/CGAL_Qt5_moc_and_resources.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX static library libCGAL_Qt5_moc_and_resources.a"
	$(CMAKE_COMMAND) -P CMakeFiles/CGAL_Qt5_moc_and_resources.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CGAL_Qt5_moc_and_resources.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/build: libCGAL_Qt5_moc_and_resources.a

.PHONY : CMakeFiles/CGAL_Qt5_moc_and_resources.dir/build

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CGAL_Qt5_moc_and_resources.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CGAL_Qt5_moc_and_resources.dir/clean

CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend: ui_ImageInterface.h
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend: qrc_CGAL.cpp
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend: qrc_Input.cpp
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend: qrc_File.cpp
CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend: qrc_Triangulation_2.cpp
	cd /home/adminlocal/PhD/cpp/spred/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adminlocal/PhD/cpp/spred /home/adminlocal/PhD/cpp/spred /home/adminlocal/PhD/cpp/spred/build/release /home/adminlocal/PhD/cpp/spred/build/release /home/adminlocal/PhD/cpp/spred/build/release/CMakeFiles/CGAL_Qt5_moc_and_resources.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CGAL_Qt5_moc_and_resources.dir/depend

