/*
 * Middle_placement.h
 *
 *  Created on: Feb 13, 2014
 *      Author: dsalinas
 */

#ifndef MIDDLE_PLACEMENT_H_
#define MIDDLE_PLACEMENT_H_


#include <CGAL/Cartesian.h>
//#include "geometry/contraction/policies/Placement_policy.h"
#include "gudhi/gudhi_contraction.h"
namespace contraction {

template< typename EdgeProfile> class CGAL_middle_placement : public Placement_policy<EdgeProfile>{

public:
	typedef typename EdgeProfile::Point Point;
	typedef typename EdgeProfile::edge_descriptor edge_descriptor;

	typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

	Placement_type operator()(const EdgeProfile& profile){
		return Placement_type(CGAL::midpoint(profile.p0(),profile.p1()));
	}
};

}  // namespace contraction


#endif /* MIDDLE_PLACEMENT_H_ */
