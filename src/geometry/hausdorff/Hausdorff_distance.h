/*
 * Hausdorff_distance.h
 *
 *  Created on: Apr 16, 2014
 *      Author: dsalinas
 */

#ifndef HAUSDORFF_DISTANCE_H_
#define HAUSDORFF_DISTANCE_H_


#include "knn.h"
#include "refine.h"


template<typename Complex>
double distance_hausdorff(Complex& complex, std::list<typename Complex::Point> & points,double sampling){
	typedef typename Complex::GT Kernel;

	typedef CNeighbor_search<Kernel> Neighbor_search;
	Neighbor_search knn;
	knn.init(points.begin(),points.end());

	CRefiner_2<Kernel> refiner;
	for(const  auto& t : complex.triangle_range()){
		auto simplex_it (t.begin());
		auto v0(*simplex_it);
		auto v1(*(++simplex_it));
		auto v2(*(++simplex_it));

		refiner.add(complex.point(v0),complex.point(v1),complex.point(v2),knn);
	}

	double dist = refiner.refine(sampling, knn,1e30);
	return dist;
}



#endif /* HAUSDORFF_DISTANCE_H_ */
