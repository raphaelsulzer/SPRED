/*
 * Tmp_policies.h
 *
 *  Created on: Aug 12, 2014
 *      Author: dsalinas
 */

#ifndef TMP_POLICIES_H_
#define TMP_POLICIES_H_


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
 * Same as Urban_policies but it weighed smooth and non smooth cost by
 * params_.ratio_proxy / 1-params_.ratio_proxy
 */
template<typename GeometricSimplifiableComplex> class Tmp_memoryless_edge_profile_factory : public Edge_profile_factory<Urban_edge_profile< GeometricSimplifiableComplex> >{
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
	Tmp_memoryless_edge_profile_factory(
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

		res.quadric_data_edge_ =compute_all_quadrics_of_edge(complex,
				res.v0_handle(),res.v1_handle(),
				res.proxies_union(),
				params_
		);
		if(!res.quadric_data_edge_) res.quadric_data_edge_ = Error_quadric();

		*(res.quadric_data_edge_)+=complex[res.v0_handle()].quadric();
		*(res.quadric_data_edge_)+=complex[res.v1_handle()].quadric();


		if(std::sqrt(res.quadric_data_edge_->squared_norm())> 1e-6){
			// the factory fill the placement/cost in order to compute them once for all
			Error_quadric_svd_solver<EdgeProfile> solver;
			res.placement_ = solver(*(res.quadric_data_edge_),res,params_.svd_eigen_threshold,false);
			if(res.placement_){

				//				// cost without proxies
				//				auto quadric_cost = contraction::compute_all_quadrics_of_edge(complex,
				//						res.v0_handle(),res.v1_handle(),
				//						set<typename Complex_::Graph_vertex::Planar_proxy_handle>(),
				//						params_
				//				);
				//				*quadric_cost+=complex[res.v0_handle()].quadric();
				//				*quadric_cost+=complex[res.v1_handle()].quadric();
				//				res.cost_ = quadric_cost->cost(*res.placement_);

				// cost with proxies
				res.cost_ = res.quadric_data_edge_->cost(*res.placement_);
				if(cost_){
					if(res.v0().empty_plane_primitives() && res.v1().empty_plane_primitives())
						(*res.cost_) *= 1-params_.ratio_proxy;
					else
						(*res.cost_) *= params_.ratio_proxy;
				}
			}
			else
				UDBG("Could not compute placement");
		} else UDBG("Could not compute placement (quadric norm too small)");
		return res;
	}



};





template<typename EdgeProfile> class Tmp_policies: public Simplification_policies<
EdgeProfile> {
	typedef typename EdgeProfile::Complex Complex;

	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy Planar_proxy;
	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy_iterator Planar_proxy_iterator;

	typedef typename Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;
private:

public:
	class Tmp_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		Tmp_visitor(Mesh_context& mesh_context) :
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
						mesh_context_.params.data_border_weight()
				);

			if(mesh_context_.params.abstraction_weight>0 && mesh_context_.params.boundary_weight>0 )
				compute_quadrics_proxy_border_with_border_edges<Complex,Enriched_mesh<Complex> >(
						mesh_context_.mesh,
						mesh_context_.segmentation,
						mesh_context_.params.proxy_border_weight()
				);

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

	class Tmp_placement : public Gudhi::contraction::Placement_policy<EdgeProfile> {

		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Vertex_handle Vertex_handle;
		typedef typename EdgeProfile::Edge_handle Edge_handle;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef quadric::Error_quadric<GT> Error_quadric_;

		typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

		Tmp_placement(Mesh_context& mesh_context) :
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

	class Tmp_cost : public Gudhi::contraction::Cost_policy<EdgeProfile> {
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

		Tmp_cost(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
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

	Tmp_policies(Mesh_context& mesh_context) :
		Simplification_policies<EdgeProfile>(new Tmp_cost(mesh_context),
				new Tmp_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new Tmp_visitor(mesh_context),
				new Tmp_memoryless_edge_profile_factory<Complex>(mesh_context.params,
						new Tmp_cost(mesh_context),
						new Tmp_placement(mesh_context)
				)
		) {
	}

};


#endif /* TMP_POLICIES_H_ */
