/*
 * Example.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#include <boost/progress.hpp>
#include "Reorient.h"
#include "details/Off_save_visitor.h"




int main(int argc,char** argv){
	if(argc!=3){
		std::cerr <<"Usage ./"<<argv[0]<<" input.off output.off\n";
		std::cerr <<"To reorient Off mesh in input.off file and to save it into output.off file\n";
		return EXIT_FAILURE;
	}
	{
		//save the reoriented mesh in a off file argv[2]
		boost::progress_timer t;
		Reorient::reorient(Reorient::OFF_save_visitor(argv[2]),argv[1]);
		std::cout <<"elapsed time: ";
	}
}
