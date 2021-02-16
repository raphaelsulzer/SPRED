cmake_minimum_required(VERSION 2.6)
project(Reorient)

find_package(Boost	REQUIRED COMPONENTS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wpedantic -Wall -lboost_system")

include_directories(.)	

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")


add_executable ( Reorient Reorient_main.cpp)

add_executable ( Custom_visitor Custom_visitor_example.cpp)

