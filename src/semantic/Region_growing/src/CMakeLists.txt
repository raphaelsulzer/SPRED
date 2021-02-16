cmake_minimum_required(VERSION 2.8)
project(External_region_growing)


if(MSVC)	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127 /wd4390 /wd4127 /wd4245 /wd4267 /wd4512 /wd4668 /wd4311 /wd4800 /wd4820 /wd4503 /wd4244 /wd4345 /wd4996 /wd4396 /wd4018")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frounding-math -Wall -pedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

find_package(CGAL)
#find_package(CGAL)
if(CGAL_FOUND)
  include(${CGAL_USE_FILE})
else()
  message(STATUS "NOTICE: This demo requires CGAL and will not be compiled.")
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework REQUIRED) 


if(Boost_FOUND)
  SET(Boost_USE_STATIC_LIBS OFF)
  SET(Boost_USE_MULTITHREAD OFF)
  ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
else()
  message(STATUS "NOTICE: This demo requires Boost and will not be compiled.")
endif()

add_executable  ( region_growing region_growing.cpp)

add_executable  ( point_region_growing point_region_growing.cpp)

add_executable  ( region_growing_test Region_growing_test.cpp)


target_link_libraries( region_growing_test ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})	
