/*
 * Cached_urban_policies.h
 *
 *  Created on: Apr 29, 2014
 *      Author: dsalinas
 */

#ifndef CACHED_URBAN_H_
#define CACHED_URBAN_H_

#include <iostream>
#include <algorithm>
#include <cassert>
#include "interface/Mesh_context.h"
#include "Simplification_policies.h"

#include "gudhi/gudhi_contraction.h"
//#include "geometry/contraction/Edge_profile.h"

#include "contraction/quadric/Quadric_utils.h"
#include "contraction/quadric/Error_quadric.h"
#include "contraction/quadric/Error_quadric_solver.h"
#include "contraction/policies/Urban_edge_profile.h"
#include "contraction/Urban_valid_contraction.h"

#include "geometry/Independent_proxies.h"




template<typename EdgeProfile> class Cached_urban_policies: public Simplification_policies<
EdgeProfile> {
	typedef typename EdgeProfile::Complex Complex;

	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy Planar_proxy;
	typedef typename EdgeProfile::Complex::Graph_vertex::Planar_proxy_iterator Planar_proxy_iterator;

	typedef typename Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;

public:
	class Cached_urban_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		Cached_urban_visitor(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		void on_started(Complex & complex) override {

			for(auto vh : complex.vertex_range()){
				complex[vh].clear_quadric();

				auto quadric = compute_all_quadrics_of_vertex(
						complex,
						vh,
						mesh_context_.params
				);
				if(quadric) complex[vh].sum_quadric_data(*quadric);
			}
			compute_quadrics_border_mesh(mesh_context_.mesh,mesh_context_.params.data_border_weight());
			compute_quadrics_proxy_border_with_border_edges(complex,mesh_context_.segmentation,mesh_context_.params.proxy_border_weight());
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

	};

	class Cached_urban_placement: public Gudhi::contraction::Placement_policy<EdgeProfile> {

		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Vertex_handle Vertex_handle;
		typedef typename EdgeProfile::Edge_handle Edge_handle;
		typedef quadric::Error_quadric<GT> Error_quadric_;


		typedef typename Gudhi::contraction::Placement_policy<EdgeProfile>::Placement_type Placement_type;

		Cached_urban_placement(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		Placement_type operator()(const EdgeProfile& profile) const override{
			auto quadric_edge(get_sum_quadrics(profile.v0(),profile.v1()));

			Error_quadric_svd_solver<EdgeProfile> solver;
			boost::optional<Point> min_quadric_pt(solver(quadric_edge.quadric(),profile,mesh_context_.params.svd_eigen_threshold,false));


			if (min_quadric_pt){
				if(mesh_context_.params.check_primitive_corner)
					return choose_closest_from_corner_if_furthest_from_corner(profile.v0_handle(),profile.v1_handle(),min_quadric_pt);
				else
					return Placement_type(*min_quadric_pt);
			}
			else {
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

	class Cached_urban_cost: public Gudhi::contraction::Cost_policy<EdgeProfile> {
		Mesh_context& mesh_context_;
	public:

		typedef typename Gudhi::contraction::Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Complex Complex;
		typedef typename Complex::Vertex_handle Vertex_handle;
		typedef typename Complex::Point Point;
		typedef quadric::Error_quadric<GT> Error_quadric_;

		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;

		Cached_urban_cost(Mesh_context& mesh_context) :
			mesh_context_(mesh_context) {
		}

		Cost_type operator()(EdgeProfile const& profile,
				boost::optional<Point> const& new_point) const override {
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

	Cached_urban_policies(Mesh_context& mesh_context) :
		Simplification_policies<EdgeProfile>(new Cached_urban_cost(mesh_context),
				new Cached_urban_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new Cached_urban_visitor(mesh_context),
				new Gudhi::contraction::Edge_profile_factory<EdgeProfile>()
		) {

	}

};

#endif /* CACHED_URBAN_H_ */
