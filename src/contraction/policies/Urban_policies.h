/*
 * Urban_policies.h
 *
 *  Created on: Jul 8, 2014
 *      Author: dsalinas
 */

#ifndef URBAN_POLICIES_H_
#define URBAN_POLICIES_H_




#include <algorithm>
#include <cassert>

#include "gudhi/gudhi_contraction.h"

#include "interface/Mesh_context.h"
#include "Simplification_policies.h"
#include "contraction/quadric/Quadric_utils.h"
#include "contraction/quadric/Error_quadric.h"
#include "contraction/quadric/Error_quadric_solver.h"
#include "contraction/policies/Urban_edge_profile.h"
#include "contraction/Urban_valid_contraction.h"

#include "geometry/Independent_proxies.h"

using namespace Gudhi::contraction;

/**
 * recompute the quadric with the current mesh when creating an edge profile.
 * The placement is the minimization of the cost and the cost
 * is the sum of squared distances to planes defined in the paper.
 */
template<typename GeometricSimplifiableComplex> class Urban_memoryless_edge_profile_factory : public Edge_profile_factory<Urban_edge_profile< GeometricSimplifiableComplex> >{
	Contraction_parameter& params_;
public:
	typedef Urban_edge_profile< GeometricSimplifiableComplex> EdgeProfile;
	typedef typename EdgeProfile::Complex Complex_;
	typedef typename Complex_::Edge_handle Edge_handle_;

	typedef Cost_policy<EdgeProfile> Cost_policy_;
	typedef Placement_policy<EdgeProfile> Placement_policy_;

	Cost_policy_* cost_;
	Placement_policy_* placement_;

	// todo passer un mesh context plutot
	Urban_memoryless_edge_profile_factory(
			Contraction_parameter& params,
			Cost_policy_* cost,
			Placement_policy_* placement
	):params_(params),cost_(cost),placement_(placement){
	}

	// todo stocker le mesh context dans le profile
	EdgeProfile make_profile(
			Complex_& complex,
			Edge_handle_ edge)const override{
		auto res = EdgeProfile(complex,edge);

		res.quadric_data_edge_ = compute_all_quadrics_of_edge(complex,
				res.v0_handle(),res.v1_handle(),
				res.proxies_union(),
				params_
		);
		if(!res.quadric_data_edge_) res.quadric_data_edge_ = Error_quadric();

		*(res.quadric_data_edge_)+=complex[res.v0_handle()].quadric();
		*(res.quadric_data_edge_)+=complex[res.v1_handle()].quadric();


		if(std::sqrt(res.quadric_data_edge_->squared_norm())> 1e-6){

			// the factory fill the placement/cost in order to compute them once for all
			//xxx dangerous dance
			Error_quadric_svd_solver<EdgeProfile> solver;
			res.placement_ = solver(*(res.quadric_data_edge_),res,params_.svd_eigen_threshold,false);
			if(res.placement_)
				res.cost_ = (res.quadric_data_edge_)->cost(*res.placement_);
			else
				UDBG("Could not compute placement");
		} else UDBG("Could not compute placement (quadric norm too small)");

		if(cost_ && res.cost_){
			if(res.v0().empty_plane_primitives() && res.v1().empty_plane_primitives())
				(*res.cost_) *= 1-params_.ratio_proxy;
			else
				(*res.cost_) *= params_.ratio_proxy;
		}
		return res;
	}

};





template<typename EdgeProfile> class Urban_policies: public Simplification_policies<
EdgeProfile> {
	typedef typename EdgeProfile::Complex Complex;

	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy Planar_proxy;
	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy_iterator Planar_proxy_iterator;

	typedef typename Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;
private:


public:
	class Urban_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		Urban_visitor(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {

		}

		void on_started(Complex & complex) override {
			UDBGVALUE("UB visitor init quadrics");

			for(auto v :  mesh_context_.mesh.vertex_range()){
				mesh_context_.mesh[v].clear_quadric();
			}

			if(mesh_context_.params.boundary_weight>0 && mesh_context_.params.abstraction_weight<1)
				compute_quadrics_border_mesh(
						mesh_context_.mesh,
						mesh_context_.params.boundary_weight
//						mesh_context_.params.data_border_weight()
				);

//			if(mesh_context_.params.abstraction_weight>0 &&
//					mesh_context_.params.boundary_weight>0 )
//				contraction::compute_quadrics_proxy_border_with_border_edges<Complex,Enriched_mesh<Complex> >(
//						mesh_context_.mesh,
//						mesh_context_.segmentation,
//						mesh_context_.params.boundary_weight * 0.05
////						mesh_context_.params.proxy_border_weight()
//				);

		}

		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_selected(const EdgeProfile &profile,
				boost::optional<double> cost, int initial_count,
				int current_count) override {

			if(mesh_context_.params.verbose){
				if(cost){
					std::cerr <<"Selected edge with cost :"<< std::sqrt(*cost)<<" -- "
							<<profile.complex().num_vertices()<<" vertices left -- "<< profile.complex().num_blockers()<<" blockers \n";
				}
				else
					std::cerr <<"Selected edge with non computable cost :"<< " -- "
					<<profile.complex().num_vertices()<<" vertices left -- "<< profile.complex().num_blockers()<<" blockers \n";
			}


		}

		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_contracting(EdgeProfile const &profile,
				boost::optional<Point> placement) override {

			sum_quadrics(profile.v0(),profile.v1());


			assert((profile.complex()).contains_vertex(profile.v0_handle()));
			assert((profile.complex().contains_vertex(profile.v1_handle())));
			assert(profile.v0_handle() != profile.v1_handle());

			//			// update the graph, num proxies, etc
			update_info_after_collapse(mesh_context_.primitive_graph,
					profile.v0().plane_primitives(),
					profile.v1().plane_primitives(),
					profile.proxies_union(),
					profile.v0_handle(),
					profile.v1_handle(),
					placement);

			//todo il faudrait faire un update pour les edges qui passent par
			// les corners qui ont ete modifiés (pe sont elles a nvo collapsables)

		}

		/**s
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_contracted(EdgeProfile const &profile,
				boost::optional<Point> placement) override {
		}

	};

	class Urban_placement: public Gudhi::contraction::Placement_policy<EdgeProfile> {

		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Vertex_handle Vertex_handle;
		typedef typename EdgeProfile::Edge_handle Edge_handle;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef quadric::Error_quadric<GT> Error_quadric_;

		typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

		Urban_placement(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		Placement_type operator()(const EdgeProfile& profile) const override{
			if(profile.placement_ && mesh_context_.params.check_primitive_corner)
				return choose_closest_from_corner_if_furthest_from_corner(profile.v0_handle(),profile.v1_handle(),profile.placement_);
			return profile.placement_;
		}

	private:

		/**
		 * if the placement goes further from a corner, we choose either v0 or v1 or in last option
		 * the intersection of the corner itself
		 */
		boost::optional<Point> choose_closest_from_corner_if_furthest_from_corner(Vertex_handle v0,Vertex_handle v1,
				boost::optional<Point> placement) const{
			if(!placement) return mesh_context_.mesh.point(v0);

			// if v0 is a corner and goes further from its corner intersection
			// then we choose the intersection point instead

			bool check_v0_corner = mesh_context_.primitive_graph.check_not_going_further_from_corner(v0,*placement);
			if(!check_v0_corner){
				auto v0_corner = mesh_context_.primitive_graph.corner(v0);
				return v0_corner->intersection_point();
			}

			// same with v1
			bool check_v1_corner = mesh_context_.primitive_graph.check_not_going_further_from_corner(v1,*placement);
			if(!check_v1_corner){
				auto v1_corner = mesh_context_.primitive_graph.corner(v1);
				return v1_corner->intersection_point();
			}

			return *placement;
		}
	};

	class Urban_cost: public Gudhi::contraction::Cost_policy<EdgeProfile> {
		Mesh_context& mesh_context_;
	public:

		typedef typename Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Complex Complex;
		typedef typename Complex::Vertex_handle Vertex_handle;
		typedef typename Complex::Point Point;
		typedef typename Complex::GT GT;
		typedef quadric::Error_quadric<GT> Error_quadric_;
		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;

		Urban_cost(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		~Urban_cost(){
		}

		Cost_type operator()(EdgeProfile const& profile,
				boost::optional<Point> const& new_point) const override {
			return profile.cost_;
		}
	};

	typedef typename quadric::Urban_valid_contraction<EdgeProfile> Urban_valid_contraction;

	public:
	typedef typename Simplification_policies<EdgeProfile>::Cost_policy Cost_policy;
	typedef typename Simplification_policies<EdgeProfile>::Placement_policy Placement_policy;
	typedef typename Simplification_policies<EdgeProfile>::Valid_contraction_policy Valid_contraction_policy;
	typedef typename Simplification_policies<EdgeProfile>::Contraction_visitor Contraction_visitor;

	Urban_policies(Mesh_context& mesh_context) :
		Simplification_policies<EdgeProfile>(new Urban_cost(mesh_context),
				new Urban_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new Urban_visitor(mesh_context),
				new Urban_memoryless_edge_profile_factory<Complex>(mesh_context.params,
						new Urban_cost(mesh_context),
						new Urban_placement(mesh_context)
				)
		) {
	}

};


#endif /* URBAN_POLICIES_H_ */
