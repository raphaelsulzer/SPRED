/*
 * reorient.h
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#ifndef REORIENT_H_
#define REORIENT_H_

#include "Reorient/src/Reorient.h"

template<typename Complex>
void reorient(const std::string& complex_file,const std::string& complex_file_out){
	std::cout << "Reorient triangles\n";
	Reorient::reorient(Reorient::OFF_save_visitor(complex_file_out),complex_file);
}





#endif /* REORIENT_H_ */
