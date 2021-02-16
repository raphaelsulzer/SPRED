#include <fstream>
#include <vector>
#include <cassert>
#include <boost/graph/adjacency_list.hpp>
#include "Region_growing.h"

using namespace regrow;


int main(int argc,char** argv){
	double max_angle_degree;
	double error_tolerance;
	int min_num_points;
	int size_offset;


	if(argc!=6){
		std::cerr <<"wrong num of args: input.off output.seg max_angle_degree error_tolerance min_num_points\n";
		max_angle_degree = 20;
		error_tolerance = 4;
		min_num_points = 50;
		size_offset = 2;
	}
	else{
		max_angle_degree = atof(argv[3]);
		error_tolerance = atof(argv[4]);
		min_num_points = atoi(argv[5]);
		size_offset = 2;
	}


	compute_proxies(argv[1],argv[2],
			max_angle_degree,error_tolerance,min_num_points,size_offset);

//	Graph graph;
//	Off_graph_reader<Graph> off_reader(argv[1],graph);
//
//	std::cerr << "Graph loaded with "<<boost::num_vertices(graph)<<" vertices and "<<boost::num_edges(graph)<<" edges"<<std::endl;
//
//	Region_growing<Graph> region_growing(graph,angle_tolerance,error_tolerance,min_num_points,size_offset);
//
//	std::cerr << "Computed "<<region_growing.num_regions()<<" regions\n";
//
//	region_growing.save_seg(argv[2]);

}
