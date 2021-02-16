/*
 * QP_policies.h
 *
 *  Created on: Apr 10, 2014
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef QP_POLICIES_H_
#define QP_POLICIES_H_


#include <CGAL/squared_distance_3.h>
#include <CGAL/Cartesian.h>
#include <algorithm>

#include "urban_utils.h"

#include "Urban_valid_contraction.h"

#include "combinatorics/Skeleton_blocker/Skeleton_blocker_link_complex.h"
#include "geometry/contraction/policies/Cost_policy.h"
#include "geometry/contraction/policies/Placement_policy.h"

#include "complex/complex_tools.h"
#include "Quadric_utils.h"
#include "Error_quadric.h"
#include "semantic/Enriched_mesh.h"

#include "QP_solver.h"

#include "interface/Contraction_parameter.h"




namespace contraction {


/**
 *@brief : Urban cost
 */
template< typename EdgeProfile> class QP_cost : public Cost_policy<EdgeProfile>{

private:
	const Contraction_parameter& parameters;

public:
	typedef typename Cost_policy<EdgeProfile>::Cost_type Cost_type;
	typedef typename EdgeProfile::Complex Complex;
	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::Graph_vertex Graph_vertex;
	typedef typename Complex::Graph_vertex::QuadricStorage QuadricStorage;
	typedef typename Complex::Point Point;

	typedef typename Complex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Vector_3 Vector_3;

	typedef Error_quadric<GT> Error_quadric_;


	/**
	 * remark assumes that the quadrics have been initialized
	 */
	QP_cost(Complex& complex,Enriched_mesh &segmentation,const Contraction_parameter& params):
		parameters(params)
	{
		//		init_quadrics(complex,segmentation);
	}

	Cost_type operator()( EdgeProfile const& profile, boost::optional<Point> const& new_point )  override {
		Cost_type res;
		if (new_point){
			QuadricStorage quad(get_quadric_edge<EdgeProfile,QuadricStorage>(profile,parameters));
			//			auto quad(get_sum_quadrics(profile.v0(),profile.v1()));
			res = quad.cost(*new_point);
		}
		return res;

	}

};


/**
 * adds to planes the constrains that corresponds to the offset of the proxies planes
 */
template<typename Proxies, typename PlaneContainer,typename GT_>
void get_planes_constrains(const Proxies& proxies_v0v1,PlaneContainer & planes,double offset){
	typedef GT_ GT;
	typedef typename GT::Vector_3 Vector_3;
	typedef typename GT::Direction_3 Direction_3;
	typedef typename GT::Point Point;
	typedef typename GT::Plane_3 Plane_3;

	for(auto proxy : proxies_v0v1){
		Point point_plane(proxy->plane().point());
		Vector_3 normal_plane(proxy->plane().orthogonal_vector());
		normal_plane = normal_plane *(offset/normal_plane.squared_length());

		Plane_3 plane1( point_plane + normal_plane,  Direction_3(normal_plane) );
		if (!plane1.has_on_negative_side(point_plane)){
			plane1 = plane1.opposite();
		}

		Plane_3 plane2( point_plane - normal_plane,  Direction_3(normal_plane) );
		if (!plane2.has_on_negative_side(point_plane)){
			plane2 = plane2.opposite();
		}

		planes.push_back(plane1);
		planes.push_back(plane2);

		//		Plane_3 p1();
		//		Plane_3 p2();
		assert(plane1.has_on_negative_side(point_plane));
		assert(plane2.has_on_negative_side(point_plane));
	}
}

/**
 * @brief Put the point on the position that minimises the quadric cost
 */
template< typename EdgeProfile> class QP_placement : public Placement_policy<EdgeProfile>{

public:
	typedef typename EdgeProfile::Point Point;
	typedef typename EdgeProfile::edge_descriptor edge_descriptor;
	typedef typename EdgeProfile::Graph_vertex Graph_vertex;
	typedef typename Complex::Graph_vertex::QuadricStorage QuadricStorage;


	typedef typename Complex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Vector_3 Vector_3;

	typedef Error_quadric<GT> Error_quadric_;



	typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

	const Contraction_parameter& parameters;

	/**
	 * remark assumes that the quadrics have been initialized
	 */
	QP_placement(const Contraction_parameter& parameters_):parameters(parameters_){}



	Placement_type operator()(const  EdgeProfile & profile) override {
		typedef typename EdgeProfile::Complex::Graph_vertex::Proxies   Proxies;
		boost::optional<Point> returned_point;

		if (parameters.keep_boundary){
			bool is_border_v0 = is_border_vertex(profile.complex(),profile.v0_handle());
			bool is_border_v1 = is_border_vertex(profile.complex(),profile.v1_handle());
			if(is_border_v0 && is_border_v1) {
				returned_point.reset();
				return returned_point;
			}
			if (is_border_v0){
				returned_point = Placement_type(profile.p0());
			}
			if (is_border_v1){
				returned_point = Placement_type(profile.p1());
			}
		}

		// data quadric
		//auto quadric_edge(get_quadric_edge(profile.v0(),profile.v1()));
		//		auto quad(get_sum_quadrics(profile.v0(),profile.v1()));

		QuadricStorage quad(get_quadric_edge<EdgeProfile,QuadricStorage>(profile,parameters));


		if (is_neighborhood_degenerate(profile,quad)){
			returned_point.reset();
			if (!profile.v0().primitives().empty()){
				auto& first_proxy (*profile.v0().primitives().begin());
				auto pt(((first_proxy)->plane()).projection(profile.p0()));
				returned_point = pt;
			}
			else {
				if (!profile.v1().primitives().empty()){
					auto& first_proxy (*profile.v1().primitives().begin());
					auto pt(((first_proxy)->plane()).projection(profile.p1()));
					returned_point = pt;
				}
			}
		}
		else{

			boost::optional<Point> lin_point(quad.min_cost((squared_distance(profile.p0(),profile.p1())))) ;

			// if the point founded with unconstrained search is not in the tolerance
			// volume then we launch QP
			if(!parameters.check_distance_primitive ||
					check_distance_primitive(profile,lin_point,parameters.distance_max_to_primitive_plane))
			{
				returned_point = lin_point;
			}
			else{
				//			auto proxies_v0v1 = profile.v0().different_proxies();
				//			proxies_v0v1.insert(profile.v1().different_proxies().cbegin(),profile.v1().different_proxies().cend());
				Proxies proxies_v0v1 = profile.v0().primitives();
				proxies_v0v1.insert(profile.v1().primitives().cbegin(),profile.v1().primitives().cend());

				std::list<Plane_3> planes;
				get_planes_constrains<Proxies,std::list<Plane_3>, GT>(proxies_v0v1,planes,parameters.distance_max_to_primitive_plane);
				boost::optional<Point> qp_point = min_cost_qp_solver(quad.quadric(),planes);


				if(!check_distance_primitive(profile,qp_point,parameters.distance_max_to_primitive_plane)){
					returned_point = lin_point;
					returned_point.reset();
				}
				else
					returned_point = qp_point;
			}

			//xxx

		}

		if (parameters.check_primitive_corner){
			bool is_v0_corner = is_corner_vertex(profile.complex(),profile.v0_handle(),parameters.distance_max_to_primitive_corner);
			bool is_v1_corner = is_corner_vertex(profile.complex(),profile.v1_handle(),parameters.distance_max_to_primitive_corner);

			if(is_v0_corner && is_v1_corner){
				if(!has_same_primitives(profile.v0(),profile.v1()))
					returned_point.reset();
			}
			else{
				if(is_v0_corner){
					returned_point = profile.p0();
				}
				if(is_v1_corner){
					returned_point = profile.p1();
				}
			}
		}

		return returned_point;
	}
};

} // namespace contraction

#endif /* QP_POLICIES_H_ */
