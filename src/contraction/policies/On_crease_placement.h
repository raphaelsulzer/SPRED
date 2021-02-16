/*
 * On_crease_placement.h
 *
 *  Created on: Mar 5, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef ON_CREASE_PLACEMENT_H_
#define ON_CREASE_PLACEMENT_H_


#include <CGAL/Cartesian.h>
#include "geometry/contraction/policies/Placement_policy.h"

namespace contraction {

template< typename EdgeProfile> class On_crease_placement : public Placement_policy<EdgeProfile>{

public:
	typedef typename EdgeProfile::Point Point;
	typedef typename EdgeProfile::edge_descriptor edge_descriptor;
	typedef typename EdgeProfile::Graph_vertex Graph_vertex;

	typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

	Placement_type operator()(const EdgeProfile& profile){
		auto primitives_edge = profile.v0().plane_primitives();
		primitives_edge.insert(profile.v1().plane_primitives().begin(),profile.v1().plane_primitives().end());
		int nb_planes = primitives_edge.size();
		Placement_type pt_proj;
		if (nb_planes==0){
			pt_proj = profile.p0();
		}
		if (nb_planes==1){
			auto plane=*(primitives_edge.begin());
			pt_proj=plane->plane().projection(CGAL::midpoint(profile.p0(),profile.p1()));
		}
		if (nb_planes==2){
			auto plane1=*(primitives_edge.begin());
			auto plane2=*(++primitives_edge.begin());
			Line_3 line;
			CGAL::Object ob_temp=intersection(plane1->plane(),plane2->plane());
			if(!assign(line, ob_temp)) {
				std::cerr<<"Error. plane1 / / plane2";
				assert(false);
			}
			pt_proj=line.projection(CGAL::midpoint(profile.p0(),profile.p1()));
		}
		if (nb_planes>=3){
			// maybe pick the 3-subset with higher mimimum angles between them (with greedy furthest).
			auto primitives_it = primitives_edge.begin();
			auto plane1=*(primitives_it);
			auto plane2=*(++primitives_it);
			auto plane3=*(++primitives_it);

			Line_3 line;
			CGAL::Object ob_temp=intersection(plane1->plane(),plane2->plane());
			if(!assign(line, ob_temp)) {
				std::cout<<"Error. P1 / / P2"<<endl;
				assert(false);
			}
			CGAL::Object ob_temp2=intersection(plane3->plane(),line);
			if(!assign(pt_proj, ob_temp2)) {
				std::cerr<<"P3 / / (P1 cap P2)"<<endl;
			}

		}
		return pt_proj;
	}

};

}  // namespace contraction



#endif /* ON_CREASE_PLACEMENT_H_ */
