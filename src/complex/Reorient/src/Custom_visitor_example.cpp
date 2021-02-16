/*
 * Example.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#include <boost/progress.hpp>
#include "Reorient.h"
#include "details/Triangle.h"
#include "details/Off_save_visitor.h"

using namespace Reorient;

/**
 * call order :
 * - init -> before anything is explored
 * - vertex -> for all vertices
 * - triangle -> for all triangles after being reoriented
 */
struct Custom_visitor{
	void init(unsigned num_vertices,unsigned num_faces,unsigned num_edges = 0) const{
		std::cout<<"num vertices,faces,edges:"<<num_vertices<<" "<<num_faces<<" "<<num_edges<<std::endl;
	}

	void vertex(const Point& p) const{
		std::cout<<"see point:";
		for(auto x : p)
			std::cout<< x << " ";
		std::cout<<std::endl;
	}

	void triangle(const Triangle& t) const{
		std::cout<<"see triangle:";
		for(auto x : t)
			std::cout<< x << " ";
		std::cout<<std::endl;
	}
};



int main(int argc,char** argv){
	if(argc!=3){
		std::cerr <<"Usage ./"<<argv[0]<<" input.off \n";
		return EXIT_FAILURE;
	}
	boost::progress_timer t2;
	reorient(Custom_visitor(),argv[1]);
	std::cout <<"elapsed time: ";
}
