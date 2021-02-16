/*
 * GH_policies.h
 *
 *  Created on: Apr 28, 2014
 *      Author: dsalinas
 */

#ifndef GH_POLICIES_H_
#define GH_POLICIES_H_

#include "Simplification_policies.h"
#include "contraction/quadric/Quadric_utils.h"
#include "contraction/quadric/Error_quadric.h"
#include "contraction/quadric/Error_quadric_solver.h"

#include "contraction/Urban_valid_contraction.h"


template< typename EdgeProfile> class GH_policies : public Simplification_policies<EdgeProfile>{
	typedef typename EdgeProfile::Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;


	class GH_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		GH_visitor(Mesh_context& mesh_context):mesh_context_(mesh_context){
		}


		void on_started(Complex & complex)
		override{
			UDBG("GH : init quadrics");

			for(auto vi : complex.vertex_range()){
				(complex[vi]).clear_quadric();
			}

			if(mesh_context_.params.abstraction_weight<1)
				compute_quadrics_garland_heckbert(complex,mesh_context_.params.data_weight());

			if(mesh_context_.params.boundary_weight>0 && mesh_context_.params.abstraction_weight<1)
				compute_quadrics_border_mesh(complex,mesh_context_.params.data_border_weight());
		}

		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_contracting(EdgeProfile const &profile, boost::optional< Point > placement)
		override{
			sum_quadrics(profile.v0(),profile.v1());

			// update the graph, num proxies, etc
			update_info_after_collapse(mesh_context_.primitive_graph,
					profile.v0().plane_primitives(),
					profile.v1().plane_primitives(),
					profile.proxies_union(),
					profile.v0_handle(),
					profile.v1_handle(),
					placement);
		}
	};




	class GH_placement : public Gudhi::contraction::Placement_policy<EdgeProfile>{
		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Edge_handle Edge_handle;

		typedef typename Gudhi::contraction::Placement_policy<EdgeProfile>::Placement_type Placement_type;


		GH_placement(Mesh_context& mesh_context):mesh_context_(mesh_context){
		}

		Placement_type operator()(const EdgeProfile& profile) const override{
			// data quadric
			auto quadric_edge(get_sum_quadrics(profile.v0(),profile.v1()));

			Error_quadric_direct_solver<EdgeProfile> solver;
//			Error_quadric_svd_solver<EdgeProfile> solver;
			boost::optional<Point> min_quadric_pt(solver(quadric_edge.quadric(),profile));

			//		boost::optional<Point> min_quadric_pt(quadric_edge.min_cost());
//						boost::optional<Point> min_quadric_pt(quadric_edge.min_cost((squared_distance(profile.p0(),profile.p1()))));
			if (min_quadric_pt){
				return Placement_type(*min_quadric_pt);
			}
			else {
				return profile.p0();
			}
		}
	};


	class GH_cost : public Gudhi::contraction::Cost_policy<EdgeProfile>{
		Mesh_context& mesh_context_;
	public:

		typedef typename Gudhi::contraction::Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Complex Complex;
		typedef typename Complex::Vertex_handle Vertex_handle;
		typedef typename Complex::Point Point;

		typedef typename Complex::GT GT;
		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;


		GH_cost(Mesh_context& mesh_context):mesh_context_(mesh_context)
		{
		}

		Cost_type operator()( EdgeProfile const& profile, boost::optional<Point> const& new_point ) const override {
			Cost_type res;
			if (new_point){
				auto quad(get_sum_quadrics(profile.v0(),profile.v1()));
				res = quad.cost(*new_point);
			}
			return res;
		}
	};

	typedef typename quadric::Urban_valid_contraction<EdgeProfile> Urban_valid_contraction;


	public:
	typedef typename Simplification_policies<EdgeProfile>::Cost_policy Cost_policy;
	typedef typename Simplification_policies<EdgeProfile>::Placement_policy Placement_policy;
	typedef typename Simplification_policies<EdgeProfile>::Valid_contraction_policy Valid_contraction_policy;
	typedef typename Simplification_policies<EdgeProfile>::Contraction_visitor Contraction_visitor;

	GH_policies(Mesh_context& mesh_context):
		Simplification_policies<EdgeProfile>(
				new GH_cost(mesh_context),
				new GH_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new GH_visitor(mesh_context))
				{
				}

};



#endif /* GH_POLICIES_H_ */
