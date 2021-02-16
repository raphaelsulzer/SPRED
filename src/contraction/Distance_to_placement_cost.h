/*
 * Distance_to_placement_cost.h
 *
 *  Created on: Mar 5, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef DISTANCE_TO_PLACEMENT_COST_H_
#define DISTANCE_TO_PLACEMENT_COST_H_

#include "geometry/contraction/policies/Cost_policy.h"
#include <CGAL/squared_distance_3.h>



namespace contraction {

template< typename EdgeProfile> class Distance_to_placement_cost : public Cost_policy<EdgeProfile>{
public:
	typedef typename Cost_policy<EdgeProfile>::Cost_type Cost_type;
	typedef typename EdgeProfile::Point Point;
	Cost_type operator()(const EdgeProfile& profile, const boost::optional<Point>& placement){
		Cost_type res;
		if (placement)
			res = std::max(CGAL::squared_distance(profile.p0(),*placement),CGAL::squared_distance(profile.p1(),*placement) )  ;
		return res;
	}

};

}  // namespace contraction

#endif /* DISTANCE_TO_PLACEMENT_COST_H_ */
