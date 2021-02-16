/*
 * stats.h
 *
 *  Created on: Mar 24, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef STATS_H_
#define STATS_H_


struct Stats {

	double total_data_cost;
	double total_proxy_plane_cost;
	double total_proxy_border_cost;
	int total_iterations;

	Stats() {
		total_data_cost = 0;
		total_proxy_plane_cost = 0;
		total_proxy_border_cost = 0;
		total_iterations =0;
	}

};  // namespace stats


#endif /* STATS_H_ */
