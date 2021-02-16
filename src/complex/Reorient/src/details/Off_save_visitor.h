/*
 * Off_save_visitor.h
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#ifndef OFF_SAVE_VISITOR_H_
#define OFF_SAVE_VISITOR_H_

#include <fstream>
#include "typedefs.h"
#include "Triangle.h"

namespace Reorient{
using Reorient::Point;
/**
 * call order :
 * - init
 * - vertex for all vertices
 * - triangle for all triangles
 */
struct OFF_save_visitor{
	OFF_save_visitor(const std::string& name_file_out):name_file_out_(name_file_out){
	}

	~OFF_save_visitor(){
		std::cerr <<"close file\n";
		file_.close();
	}

	void init(unsigned num_vertices,unsigned num_faces,unsigned num_edges = 0) const{
		file_.open(name_file_out_.c_str());
		if (!file_.is_open())
			std::cerr << "could not open output file :"<<name_file_out_<<std::endl;
		else{
			file_<<"OFF\n";
		}

		file_<<num_vertices<<" "<<num_faces<<" "<<num_edges<<std::endl;
	}

	void vertex(const Point& p) const{
		for(auto x : p)
			file_<< x << " ";
		file_<<std::endl;
	}

	void triangle(const Triangle& t) const{
		file_<<"3 ";
		for(auto x : t)
			file_<< x << " ";
		file_<<std::endl;
	}
private:
	std::string name_file_out_;
	mutable std::ofstream file_;
};

}

#endif /* OFF_SAVE_VISITOR_H_ */
