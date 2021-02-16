cmake_minimum_required(VERSION 3.6)
project(Urban)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

SET(Gudhi_Path "src/gudhi/")
#to resist to gudhi's change, one way is to add all subdirectories of gudhi
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#if(MSVC)
#	#SET(EIGEN3_INCLUDE_DIR "/usr/include/eigen3/")	 #UGH pourquoi il trouve pas eigen?
#	# Turn off some VC++ warnings
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127 /wd4390 /wd4127 /wd4245 /wd4267 /wd4512 /wd4668 /wd4311 /wd4800 /wd4820 /wd4503 /wd4244 /wd4345 /wd4996 /wd4396 /wd4018")

#	#Add some multithreaded build support
#MARK_AS_ADVANCED(MULTITHREADED_BUILD)
#set(MULTITHREADED_BUILD 8 CACHE STRING "How many threads are used to build the project")
#if(MULTITHREADED_BUILD)
#    if(${CMAKE_GENERATOR} MATCHES "Unix Makefiles")
#            message(STATUS ${CMAKE_BUILD_TOOL})
#            set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${MULTITHREADED_BUILD}")
#            message(STATUS "Added arguments to CMAKE_BUILD_TOOL: ${CMAKE_MAKE_PROGRAM}")
#    elseif(MSVC)
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
#      message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
#    endif()
#endif()

#else()
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math -Wall -pedantic")
#	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
#endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math -Wall -pedantic")


set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})



find_package(GMP REQUIRED)

find_package(MPFR REQUIRED)


FIND_PACKAGE(Eigen REQUIRED)
if(EIGEN_FOUND)
        INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS})
        ADD_DEFINITIONS(${EIGEN_DEFINITIONS} -D_USE_EIGEN)
        SET(_USE_EIGEN TRUE)
endif()

#find_package(CGAL 5.1.1 REQUIRED) # needs to be 4.14, otherwise lots of compilation errors
#if(CGAL_FOUND)
#    set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
#    message(STATUS "Found CGAL ${CGAL_VERSION}")
#    message(STATUS "  Includes : ${CGAL_INCLUDE_DIRS}")
#    message(STATUS "  Libraries : ${CGAL_LIBRARY}")
##    INCLUDE_DIRECTORIES(${CGAL_INCLUDE_DIRS})
##    LINK_DIRECTORIES(${CGAL_LIBRARY})
#    add_definitions("-DCGAL_ENABLED")
#else()
#    message(STATUS "Disabling CGAL support")
#    set(CGAL_ENABLED OFF)
#endif()

find_package(CGAL REQUIRED Core Qt5)


find_package(Boost		REQUIRED COMPONENTS)
if(Boost_FOUND)
  SET(Boost_USE_STATIC_LIBS OFF)
  SET(Boost_USE_MULTITHREAD OFF)
#  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
#  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
else()
  message(STATUS "NOTICE: This demo requires Boost and will not be compiled.")
endif()

find_package(OpenGL REQUIRED)
find_package(LibXml2 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui OpenGL Svg)
#find_package(Qt4)
#  if(QT5_FOUND)
set( QT_USE_QTXML    TRUE )
set( QT_USE_QTMAIN   TRUE )
set( QT_USE_QTSCRIPT  TRUE )
set( QT_USE_QTOPENGL  TRUE )
set( QT_USE_QTSVG  TRUE )
#include(${QT_USE_FILE})
#else()
#  message(STATUS "NOTICE: This demo requires Qt4 and will not be compiled.")
#endif(QT5_FOUND)


# also have to do this: https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=862808
set(QGLVIEWER_LIBRARIES /usr/lib/x86_64-linux-gnu/libQGLViewer-qt5.so)
set(QGLVIEWER_INCLUDE_DIR "/usr/include/QGLViewer")


list(APPEND INCLUDE_DIRS
#            ${GLOG_INCLUDE_DIRS}
#            ${GLEW_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${QGLVIEWER_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${MPFR_INCLUDE_DIRS}
    ${CGAL_INCLUDE_DIRS}
)


include_directories(src
    ${INCLUDE_DIRS})
include_directories(${Gudhi_Path})


# qt : ui file, created wih Qt Designer ###############
set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/interface")
qt5_wrap_ui( uis
	src/interface/Menu_view_param.ui		
	src/interface/MainWindow.ui 
	src/interface/Menu_edge_collapse.ui
	src/interface/Menu_detect_primitive.ui
)	

set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/interface")
#        QT5_AUTOMOC(
#		src/interface/MainWindow.cpp
#		src/interface/Menu_edge_collapse.cpp
#		src/interface/Menu_view_param.cpp
#		src/interface/Menu_detect_primitive.cpp
#		src/interface/Viewer_instructor.cpp
#		src/interface/Viewer.cpp
#		src/interface/Buffered_viewer_instructor.cpp
#		src/interface/Buffered_viewer.cpp
#	)
	#####################################################################

#	add_executable  ( Test_complex_tools
#		test/test_complex_tools.cpp
#	)

	add_executable  ( Urban_simplification 
		test/demo_contraction.cpp
#                src/semantic/Enriched_mesh.cpp
                src/semantic/code_flo/CloudSegmentation.cpp
		src/interface/MainWindow.cpp
		src/interface/Menu_edge_collapse.cpp
		src/interface/Menu_detect_primitive.cpp
		src/interface/Menu_view_param.cpp
		src/interface/Buffered_viewer_instructor.cpp
		src/interface/Buffered_viewer.cpp
		src/interface/Viewer_instructor.cpp
		src/interface/Viewer.cpp
                src/contraction/CGAL_contractor.cpp
		${uis}  
	)
	
#	add_executable  ( benchmark
#		src/benchmark/benchmark.cpp		src/semantic/code_flo/CloudSegmentation.cpp 	src/contraction/CGAL_contractor.cpp
#	)
	
#	add_executable  ( urban_collapse
#		test/urban_collapse.cpp
#	)
	
	



target_link_libraries(Urban_simplification  ${Boost_LIBRARIES}
                                            ${CMAKE_DL_LIBS}
                                            ${GMP_LIBRARY}
                                            ${MPFR_LIBRARIES}
                                           Qt5::Core
                                           Qt5::Widgets
                                           Qt5::Gui
                                           Qt5::OpenGL
                                           Qt5::Svg
                                           ${LIBXML2_LIBRARIES}
                                           ${QGLVIEWER_LIBRARIES}
                                           ${OPENGL_gl_LIBRARY}
                                           ${OPENGL_glu_LIBRARY}
                                           ${OPENMP}
                                           CGAL::CGAL_Core
                                           CGAL::CGAL
                                           CGAL::CGAL_Qt5
                                           ${CGAL_3RD_PARTY_LIBRARIES})
