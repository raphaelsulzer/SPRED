/*
 * Merge_proxies_params.h
 *
 *  Created on: Aug 8, 2014
 *      Author: dsalinas
 */

#ifndef MERGE_PROXIES_PARAMS_H_
#define MERGE_PROXIES_PARAMS_H_



#include <iostream>
#include <string>
#include <sstream>


#include "urban_utils.h"


/**
 * Parameters for proxy detection.
 * Beware that the proxy graph should be constructed before.
 */
struct Merge_proxies_params{
//	/**
//	 * regularization parameter
//	 */
//	double reg_epsilon;
//	double reg_coplanarity;

	/**
	 * merge parameters.
	 * Every pair of proxies with angle less than
	 * max_angle_merge and st the new plane
	 * is at distance lower than max_dist_merge than their points
	 * will be merged.
	 * Afterwards, only proxies with area at least min_area_merge
	 * are kept.
	 */
	double max_angle_merge;
	double max_dist_merge;
	double min_area_merge;

	//to compute the graph
	double max_dist_edges;

public:

	Merge_proxies_params():max_angle_merge(20.),max_dist_merge(3.),min_area_merge(20.),max_dist_edges(2.5)
	{
	}

	Merge_proxies_params(
			double max_angle_merge_,
			double max_dist_merge_,
			double min_area_merge_,
			double max_dist_edges_
	):max_angle_merge(max_angle_merge_),max_dist_merge(max_dist_merge_),min_area_merge(min_area_merge_),max_dist_edges(2.5)
	{
	}




	friend std::ostream & operator<<(std::ostream& oss,const Merge_proxies_params& params){
		oss <<	"%Regularization parameters:\n";
		oss << "max_angle_merge: "<< params.max_angle_merge<<"\n"
			<< "max_dist_merge_: "<<params.max_dist_merge<<"\n"
			<<  "min_area_merge: "<<params.min_area_merge<<"\n"
			<<  "max_dist_edges: "<<params.max_dist_edges<<"\n";
		return oss;
	}


	friend std::istream& operator>>(std::istream& stream, Merge_proxies_params& params)
	{
		read_param(params.max_angle_merge,stream);
		read_param(params.max_dist_merge,stream);
		read_param(params.min_area_merge,stream);
		read_param(params.max_dist_edges,stream);
		return stream;
	}

};



#endif /* MERGE_PROXIES_PARAMS_H_ */
