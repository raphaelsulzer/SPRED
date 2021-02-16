/*
 * Length_midpoint_policies.h
 *
 *  Created on: Apr 28, 2014
 *      Author: dsalinas
 */

#ifndef LENGTH_MIDPOINT_POLICIES_H_
#define LENGTH_MIDPOINT_POLICIES_H_

#include <CGAL/squared_distance_3.h>

#include "Simplification_policies.h"
#include "interface/Mesh_context.h"

#include "gudhi/gudhi_contraction.h"
#include "contraction/Urban_valid_contraction.h"

template< typename EdgeProfile> class Length_midpoint_policies : public Simplification_policies<EdgeProfile>{


	class CGAL_middle_placement : public Gudhi::contraction::Placement_policy<EdgeProfile>{

	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Edge_handle Edge_handle;

		typedef typename Gudhi::contraction::Placement_policy<EdgeProfile>::Placement_type Placement_type;

		Placement_type operator()(const EdgeProfile& profile) const override{
			return Placement_type(CGAL::midpoint(profile.p0(),profile.p1()));
		}
	};


	class CGAL_edge_length_cost : public Gudhi::contraction::Cost_policy<EdgeProfile>{
	public:
		typedef typename Gudhi::contraction::Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Point Point;
		Cost_type operator()(const EdgeProfile& profile, const boost::optional<Point>& placement) const override{
			Cost_type res;
			const Point& a = profile.p0();
			const Point& b = profile.p1();
			res = CGAL::squared_distance(a,b);
			return res;
		}

	};

	typedef typename quadric::Urban_valid_contraction<EdgeProfile> Urban_valid_contraction;


	public:
	typedef typename Simplification_policies<EdgeProfile>::Cost_policy Cost_policy;
	typedef typename Simplification_policies<EdgeProfile>::Placement_policy Placement_policy;
	typedef typename Simplification_policies<EdgeProfile>::Valid_contraction_policy Valid_contraction_policy;
	typedef typename Simplification_policies<EdgeProfile>::Contraction_visitor Contraction_visitor;

	Length_midpoint_policies(Mesh_context& mesh_context):
		Simplification_policies<EdgeProfile>(
				new CGAL_edge_length_cost(),
				new CGAL_middle_placement(),
				new Urban_valid_contraction(mesh_context),
				NULL)
				{
				}

};



#endif /* LENGTH_MIDPOINT_POLICIES_H_ */
