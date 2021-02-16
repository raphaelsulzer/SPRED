/*
 * Current_policies.h
 *
 *  Created on: Apr 29, 2014
 *      Author: dsalinas
 */

#ifndef CURRENT_POLICIES_H_
#define CURRENT_POLICIES_H_

#include <iostream>
#include <algorithm>
#include <cassert>
#include "interface/Mesh_context.h"
#include "Simplification_policies.h"
#include "contraction/quadric/Quadric_utils.h"
#include "contraction/quadric/Error_quadric.h"
#include "contraction/quadric/Error_quadric_solver.h"
#include "contraction/policies/Urban_edge_profile.h"
#include "contraction/Urban_valid_contraction.h"

#include "geometry/Independent_proxies.h"

/**
 * recompute the quadric with the current mesh when creating an edge profile

 * Same as urban only that  the cost is without the proxy quadrics.
 *
 *
 */
template<typename GeometricSimplifiableComplex> class Current_edge_profile_factory : public Edge_profile_factory<Urban_edge_profile< GeometricSimplifiableComplex> >{
	Mesh_context& mesh_ctxt_;
public:
	typedef Urban_edge_profile< GeometricSimplifiableComplex> EdgeProfile;
	typedef typename EdgeProfile::Complex Complex_;
	typedef typename Complex_::Edge_handle Edge_handle_;

	typedef Cost_policy<EdgeProfile> Cost_policy_;
	typedef Placement_policy<EdgeProfile> Placement_policy_;
	typedef Valid_contraction_policy<EdgeProfile> Valid_contraction_policy_;

	const Cost_policy_* cost_;
	const Placement_policy_* placement_;
	const Valid_contraction_policy_* valid_;


	// todo passer un mesh context plutot
	Current_edge_profile_factory(
			Mesh_context& mesh_ctxt,
			const Cost_policy_* cost,
			const Placement_policy_* placement,
			const Valid_contraction_policy_* valid

	):mesh_ctxt_(mesh_ctxt),cost_(cost),placement_(placement),valid_(valid){
	}

	// todo stocker le mesh context dans le profile
	EdgeProfile make_profile(
			Complex_& complex,
			Edge_handle_ edge)const override{
		auto res = EdgeProfile(complex,edge);

		res.quadric_data_edge_ = (get_sum_quadrics(res.v0(),res.v1())).quadric();

		auto quadrics_around_edge =
				compute_all_quadrics_of_edge(complex,
						res.v0_handle(),res.v1_handle(),
						res.proxies_union(),
						mesh_ctxt_.params
				);
		if(quadrics_around_edge)
			(*res.quadric_data_edge_)+=*quadrics_around_edge;

		if(res.quadric_data_edge_){
//			*(res.quadric_data_edge_)+=mesh_ctxt_.mesh[res.v0_handle()].quadric();
//			*(res.quadric_data_edge_)+=mesh_ctxt_.mesh[res.v1_handle()].quadric();


			// the factory fill the placement/cost in order to compute them once for all
			//xxx dangerous dance
			Error_quadric_svd_solver<EdgeProfile> solver;
			res.placement_ = solver(*(res.quadric_data_edge_),res,mesh_ctxt_.params.svd_eigen_threshold,false);

			if(res.placement_){
				auto quadric_star = (get_sum_quadrics(res.v0(),res.v1())).quadric();
				auto quadric_star2 = compute_all_quadrics_of_edge(
						complex,res.v0_handle(),res.v1_handle(),
						set<typename Complex::Graph_vertex::Planar_proxy_handle>(),
						mesh_ctxt_.params
				);
				if(quadric_star2) quadric_star+=*quadric_star2;
				res.cost_ = quadric_star.cost(*res.placement_);
			}
			else
				UDBG("Could not compute placement");

			//		res.valid_ = (*valid_)(res,res.placement);

			//		Urban_valid_contraction valid_contraction(mesh_context);
		}
		else UDBG("Quadric not valid");
		return res;
	}


	typedef typename Complex_::GT GT;
	typedef typename Complex_::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;

	typedef typename quadric::Error_quadric<GT> Error_quadric;
	typedef typename GT::Triangle_3 Triangle_3;


};





template<typename EdgeProfile> class Current_policies: public Simplification_policies<
EdgeProfile> {
	typedef typename EdgeProfile::Complex Complex;

	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy Planar_proxy;
	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy_iterator Planar_proxy_iterator;

	typedef typename Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;

public:
	class Current_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		Current_visitor(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		void on_started(Complex & complex) override {
			compute_quadrics_border_mesh(mesh_context_.mesh,mesh_context_.params.data_border_weight());
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

			// update the graph, num proxies, merge the set of proxies, etc
			update_info_after_collapse(mesh_context_.primitive_graph,
					profile.v0().plane_primitives(),
					profile.v1().plane_primitives(),
					profile.proxies_union(),
					profile.v0_handle(),
					profile.v1_handle(),
					placement);

		}

		/**s
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_contracted(EdgeProfile const &profile,
				boost::optional<Point> placement) override {
		}

		bool check_vertices_proxies(EdgeProfile const &profile){
			bool res = true;

			// check that all vertices registered to a proxy are alive
			for(auto& proxy : mesh_context_.segmentation){
				for(auto v = proxy.begin_current_vertices();
						v!= proxy.end_current_vertices();
						++v){
					if(!mesh_context_.mesh.contains_vertex(*v)) return false;
				}
			}

			for(auto v : mesh_context_.mesh.vertex_range()){
				for(auto proxy : mesh_context_.mesh[v].plane_primitives()){
					if(!proxy->contains_vertex(v)) return false;
				}
			}
			return true;
		}



		bool check_number_vertices_proxies(EdgeProfile const &profile){
			for(auto& proxy : mesh_context_.segmentation){
				int num_brutal = 0 ;
				for(auto v = proxy.begin_initial_vertices();
						v!= proxy.end_initial_vertices();
						++v){
					if(mesh_context_.mesh.contains_vertex(*v)) ++num_brutal;
				}

				int num_cached = proxy.num_vertices();
				if(num_cached != num_brutal) {
					UDBGVALUE(profile.v0_handle());
					UDBGVALUE(profile.v1_handle());
					UDBGCONT(profile.v0().plane_primitives());
					UDBGCONT(profile.v1().plane_s());
					UDBGVALUE(&proxy);
					UDBGVALUE(num_brutal);
					UDBGVALUE(num_cached);
					return false;
				}
			}
			return true;
		}

	};

	class Current_placement : public Gudhi::contraction::Placement_policy<EdgeProfile> {

		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Vertex_handle Vertex_handle;
		typedef typename EdgeProfile::Edge_handle Edge_handle;
		typedef quadric::Error_quadric<GT> Error_quadric_;


		typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

		Current_placement(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		Placement_type operator()(const EdgeProfile& profile) const override{
			auto min_quadric_pt = profile.placement_;
			if(profile.placement_) {
				if(mesh_context_.params.check_primitive_corner)
					return choose_closest_from_corner_if_furthest_from_corner(profile.v0_handle(),profile.v1_handle(),min_quadric_pt);
				else
					return min_quadric_pt;
			}
			else{
				UDBG("!profile.quadric_data_edge_");
				return profile.p0();
			}
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

	class Current_cost : public Gudhi::contraction::Cost_policy<EdgeProfile> {
		Mesh_context& mesh_context_;
	public:

		typedef typename Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Complex Complex;
		typedef typename Complex::Vertex_handle Vertex_handle;
		typedef typename Complex::Point Point;
		typedef quadric::Error_quadric<GT> Error_quadric_;

		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;

		Current_cost(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		Cost_type operator()(EdgeProfile const& profile,
				boost::optional<Point> const& new_point) const override {
			if(profile.cost_) return profile.cost_;
			else {
				UDBG("Cost not valid");
				return Cost_type();
			}
		}


	};

	typedef typename quadric::Urban_valid_contraction<EdgeProfile> Urban_valid_contraction;

	public:
	typedef typename Simplification_policies<EdgeProfile>::Cost_policy Cost_policy;
	typedef typename Simplification_policies<EdgeProfile>::Placement_policy Placement_policy;
	typedef typename Simplification_policies<EdgeProfile>::Valid_contraction_policy Valid_contraction_policy;
	typedef typename Simplification_policies<EdgeProfile>::Contraction_visitor Contraction_visitor;

	Current_policies(Mesh_context& mesh_context) :
		Simplification_policies<EdgeProfile>(new Current_cost(mesh_context),
				new Current_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new Current_visitor(mesh_context),
				new Current_edge_profile_factory<Complex>(mesh_context,
						new Current_cost(mesh_context),
						new Current_placement(mesh_context),
						new Urban_valid_contraction(mesh_context)
				)
		) {

	}

};

#endif /* CURRENT_POLICIES_H_ */
