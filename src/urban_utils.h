/*
 * urban_utils.h
 *
 *  Created on: Apr 1, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef URBAN_UTILS_H_
#define URBAN_UTILS_H_

//#include <sys/time.h>
#include <sstream>
#include <iostream>

//#define UDBG_VERBOSE
#ifdef UDBG_VERBOSE
#define UDBG(a) std::cerr << "UDBG: " << (a)<<std::endl
#define UDBGMSG(a,b) std::cerr << "UDBG: " << a<<b<<std::endl
#define UDBGVALUE(a) std::cerr << "UDBG: " <<  #a << ": " << a<<std::endl
#define UDBGCONT(a) std::cerr << "UDBG: container "<< #a<<" -> "; for(auto x:a) std::cerr<< x << ","; std::cerr<<std::endl
#else
#define UDBG(a)
#define UDBGMSG(a,b)
#define UDBGVALUE(a)
#define UDBGCONT(a)
#endif


/**
 * Read a parameter in a line such as "Token:   2.0". In that
 * case the param is set to 2.0.
 * All lines that starts with '%' are ignored.
 */
template<typename T>
bool read_param(T& param,std::istream& stream){
	std::string token,line;
	// ignores all line that start with a '%'
	do{
		getline (stream,line);
	} while(line[0]=='%');
	std::istringstream iss(line);
	if(!(iss>>token)) return false;
	if(!(iss>> param)) return false;


	return true;
}






#endif /* URBAN_UTILS_H_ */
