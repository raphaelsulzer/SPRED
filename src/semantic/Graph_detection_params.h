/*
 * Graph_detection_params.h
 *
 *  Created on: Aug 8, 2014
 *      Author: dsalinas
 */

#ifndef GRAPH_DETECTION_PARAMS_H_
#define GRAPH_DETECTION_PARAMS_H_



#include <iostream>
#include <string>
#include <sstream>
#include <boost/optional.hpp>

#include "urban_utils.h"


/**
 * Parameters for proxy graph detection
 */
struct Graph_detection_params{

	/**
	 * if max_num>0, two proxies are connected if they are at distance lower than max_dist.
	 * if max_num<0, their is a pair of vertices at distance less than max_num in the graph with distance
	 * lower than max_dist.
	 */
	double max_dist;
	int max_num;

public:

	Graph_detection_params():max_dist(2.5),max_num(-1){
	}

	Graph_detection_params(double max_dist_):max_dist(max_dist_),max_num(-1){
	}

	Graph_detection_params(double max_dist_,int num_):max_dist(max_dist_),max_num(num_){
		}


	friend std::ostream & operator<<(std::ostream& oss,const Graph_detection_params& params){
		oss <<	"%Regularization parameters:\n";
		oss << "max_dist: "<< params.max_dist<<"\n";
		oss << "max_num: "<<params.max_num<<"\n";
		return oss;
	}


	friend std::istream& operator>>(std::istream& stream, Graph_detection_params& params)
	{
		read_param(params.max_dist,stream);
		read_param(params.max_num,stream);
		return stream;
	}

};



#endif /* GRAPH_DETECTION_PARAMS_H_ */
