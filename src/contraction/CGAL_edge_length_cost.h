/*
 * Edge_length_cost.h
 *
 *  Created on: Feb 13, 2014
 *      Author: dsalinas
 */

#ifndef CGAL_EDGE_LENGTH_COST_H_
#define CGAL_EDGE_LENGTH_COST_H_

//#include "geometry/contraction/policies/Cost_policy.h"
#include <CGAL/squared_distance_3.h>
#include "gudhi/gudhi_contraction.h"


namespace contraction {

template< typename EdgeProfile> class CGAL_edge_length_cost : public Cost_policy<EdgeProfile>{
public:
	typedef typename Cost_policy<EdgeProfile>::Cost_type Cost_type;
	typedef typename EdgeProfile::Point Point;
	Cost_type operator()(const EdgeProfile& profile, const boost::optional<Point>& placement){
		Cost_type res;
		const Point& a = profile.p0();
		const Point& b = profile.p1();
		res = CGAL::squared_distance(a,b);
		return res;
	}

};

}  // namespace contraction

#endif /* CGAL_EDGE_LENGTH_COST_H_ */
