/*
 * Region_growing_test.cpp
 *  Created on: Dec 4, 2014
 * This file is part of the Gudhi Library. The Gudhi library 
 *    (Geometric Understanding in Higher Dimensions) is a generic C++ 
 *    library for computational topology.
 *
 *    Author(s):       David Salinas
 *
 *    Copyright (C) 2014  INRIA Sophia Antipolis-Méditerranée (France)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#define BOOST_TEST_MODULE RegionGrowingTest
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include "Graph.h"
#include "Off_reader.h"
#include "Region_growing.h"

using namespace regrow;


const std::string simple_cube_path("../data/cube_14V.off");
const std::string smooth_feature_path("../data/smooth-feature.off");

BOOST_AUTO_TEST_CASE(Region_growing_test1)
{
	const double angle_tolerance = 60;
	const double error_tolerance = 0.01;
	const int min_num_points = 1;
	const int size_offset = 1;

	Graph graph;
	Off_graph_reader<Graph> off_reader(simple_cube_path,graph);

	BOOST_REQUIRE( boost::num_vertices(graph) == 14 && boost::num_edges(graph) == 36);

	Region_growing<Graph> region_growing(graph,angle_tolerance,error_tolerance,min_num_points,size_offset);

	std::cerr << "Computed "<<region_growing.num_regions()<<" regions\n";

	BOOST_REQUIRE( region_growing.num_regions() == 6);

	for(auto region_id = 0u ; region_id < region_growing.num_regions(); ++region_id ){
		BOOST_CHECK(region_growing.num_points(region_id) == 5);
	}

}

BOOST_AUTO_TEST_CASE(Region_growing_test2)
{
	const double angle_tolerance = 60;
	const double error_tolerance = 0.01;
	const int min_num_points = 6;
	const int size_offset = 1;

	Graph graph;
	Off_graph_reader<Graph> off_reader(simple_cube_path,graph);

	BOOST_REQUIRE( boost::num_vertices(graph) == 14 && boost::num_edges(graph) == 36);

	Region_growing<Graph> region_growing(graph,angle_tolerance,error_tolerance,min_num_points,size_offset);

	BOOST_REQUIRE( region_growing.num_regions() == 0);

	std::cerr << "Computed "<<region_growing.num_regions()<<" regions\n";
}

BOOST_AUTO_TEST_CASE(Region_growing_test3)
{
	const double angle_tolerance = 60;
	const double error_tolerance = 0.01;
	const int min_num_points = 6;
	const int size_offset = 1;

	Graph graph;
	Off_graph_reader<Graph> off_reader(smooth_feature_path,graph);

	BOOST_REQUIRE( boost::num_vertices(graph) == 6177 && boost::num_edges(graph) == 18525);

	Region_growing<Graph> region_growing(graph,angle_tolerance,error_tolerance,min_num_points,size_offset);

	BOOST_REQUIRE( region_growing.num_regions() >= 0);

	std::cerr << "Computed "<<region_growing.num_regions()<<" regions\n";
}





