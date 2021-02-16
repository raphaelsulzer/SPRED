/*
 * vector_reader.h
 *
 *  Created on: Apr 16, 2014
 *      Author: dsalinas
 */

#ifndef VECTOR_READER_H_
#define VECTOR_READER_H_

#include <iostream>

/**
 * \brief Read a set of points and store it
 * into a vector< vector<double> >
 * File format: 1 point per line
 * X11 X12 ... X1d
 * X21 X22 ... X2d
 *  	...
 */
inline void
read_vectors_from_file( std::string file_name,
		std::vector< std::vector< double > > & points)
{
  std::ifstream in_file (file_name.c_str(),std::ios::in);
  if(!in_file.is_open()) {
    std::cerr << "Unable to open file " << file_name << std::endl;
    return;
  }

  points.clear();

  std::string line;
  double x;
  while( getline ( in_file , line ) )
  {
    std::vector< double > point;
    std::istringstream iss( line );
    while(iss >> x) { point.push_back(x); }
    points.push_back(point);
  }
  in_file.close();
}


#endif /* VECTOR_READER_H_ */
