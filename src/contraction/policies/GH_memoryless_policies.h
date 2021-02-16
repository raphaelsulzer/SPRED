/*
 * GH_memoryless.h
 *
 *  Created on: Apr 28, 2014
 *      Author: dsalinas
 */

#ifndef GH_MEMORYLESS_H_
#define GH_MEMORYLESS_H_

#include "interface/Mesh_context.h"
#include "Simplification_policies.h"
#include "contraction/quadric/Quadric_utils.h"
#include "contraction/quadric/Error_quadric.h"
#include "contraction/quadric/Error_quadric_solver.h"



template< typename EdgeProfile> class GH_memoryless_policies : public Simplification_policies<EdgeProfile>{
	typedef typename EdgeProfile::Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric;
private:
	static
	quadric::Error_quadric<typename EdgeProfile::Complex::GT>
	compute_quadrics_data_edge(Mesh_context& mesh_context,typename EdgeProfile::Complex::Edge_handle edge,double data_weight){
		typedef typename EdgeProfile::Complex::GT GT;
		typedef quadric::Error_quadric<GT> Error_quadric_;
		typedef typename GT::Triangle_3 Triangle_3;
		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;


		Error_quadric_ res;

		auto a(mesh_context.mesh.first_vertex(edge));
		auto b(mesh_context.mesh.second_vertex(edge));

		//		if(a>b) std::swap(a,b);

		for (auto ti : mesh_context.mesh.triangle_range(a)){
			auto vertex_ti = ti.begin();
			auto v1 = *vertex_ti;
			auto v2 = *(++vertex_ti);
			auto v3 = *(++vertex_ti);

			Triangle_3 triangle(mesh_context.mesh.point(v1),mesh_context.mesh.point(v2),mesh_context.mesh.point(v3));


			if(!triangle.is_degenerate())

				// the quadric is multiplied by the triangle area and the weight
				res += Error_quadric_(triangle.supporting_plane(),std::sqrt(triangle.squared_area())*data_weight);
		}

		for (auto ti : mesh_context.mesh.triangle_range(b)){
			auto vertex_ti = ti.begin();
			auto v1 = *vertex_ti;
			auto v2 = *(++vertex_ti);
			auto v3 = *(++vertex_ti);
			if(a != v1 && a!= v2 && a!= v3){
				Triangle_3 triangle(mesh_context.mesh.point(v1),mesh_context.mesh.point(v2),mesh_context.mesh.point(v3));
				// the quadric is multiplied by the triangle area and the weight
				if(!triangle.is_degenerate())
					res += Error_quadric_(triangle.supporting_plane(),std::sqrt(triangle.squared_area())*data_weight);
			}
		}


		for(auto eh : mesh_context.mesh.edge_range(a)){
			auto vh = is_border_edge(mesh_context.mesh,eh);
			if(vh){
				auto eh1 = mesh_context.mesh.first_vertex(eh);
				auto eh2 = mesh_context.mesh.second_vertex(eh);
				auto pt1 = mesh_context.mesh.point(eh1);
				auto pt2 = mesh_context.mesh.point(eh2);
				auto pt_vh = mesh_context.mesh.point(*vh);
				auto vec = CGAL::cross_product(Vector_3(pt1,pt2),Vector_3(pt1,pt_vh));
				auto pt_c = pt1+vec;
				Plane_3 plane(pt1,pt2,pt_c);
				// the quadric is weight by the length of the edge divided by two
				// (to be scaled invariant) and by the weight
				double ratio = squared_distance(mesh_context.mesh.point(eh1),mesh_context.mesh.point(eh2)) * mesh_context.params.boundary_weight*data_weight;
				res += Error_quadric_(plane, ratio );
			}
		}

		for(auto eh : mesh_context.mesh.edge_range(b)){
			if(a!=mesh_context.mesh.first_vertex(eh)&& a!=mesh_context.mesh.second_vertex(eh)){
				auto vh = is_border_edge(mesh_context.mesh,eh);
				if(vh){
					auto eh1 = mesh_context.mesh.first_vertex(eh);
					auto eh2 = mesh_context.mesh.second_vertex(eh);
					auto pt1 = mesh_context.mesh.point(eh1);
					auto pt2 = mesh_context.mesh.point(eh2);
					auto pt_vh = mesh_context.mesh.point(*vh);
					auto vec = CGAL::cross_product(Vector_3(pt1,pt2),Vector_3(pt1,pt_vh));
					auto pt_c = pt1+vec;
					Plane_3 plane(pt1,pt2,pt_c);
					// the quadric is weight by the length of the edge divided by two
					// (to be scaled invariant) and by the weight
					double ratio = squared_distance(mesh_context.mesh.point(eh1),mesh_context.mesh.point(eh2)) * mesh_context.params.boundary_weight *data_weight;
					res += Error_quadric_(plane, ratio );

				}
			}
		}

		return res;
	}

public:
	class GH_memoryless_visitor: public Simplification_policies<EdgeProfile>::Contraction_visitor {
		typedef typename EdgeProfile::Complex Complex;
		typedef typename EdgeProfile::Complex::GT GT;
		typedef double FT;
		typedef typename Complex::Point Point;

		Mesh_context& mesh_context_;
	public:
		GH_memoryless_visitor(Mesh_context& mesh_context):mesh_context_(mesh_context){
		}


		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_started(Complex & complex)
		override{
		}


		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_selected (const EdgeProfile &profile, boost::optional< double > cost, int initial_count, int current_count)
		override{
//			std::cerr <<"Selected edge with cost :"<< std::sqrt(*cost)<<" -- "<<profile.complex().num_vertices()<<" vertices left\n";
		}


		/**
		 * @brief Called when an edge is about to be contracted and replaced by a vertex whose position is *placement.
		 */
		void on_contracting(EdgeProfile const &profile, boost::optional< Point > placement)
		override{
			update_info_after_collapse(mesh_context_.primitive_graph,
					profile.v0().plane_primitives(),
					profile.v1().plane_primitives(),
					profile.proxies_union(),
					profile.v0_handle(),
					profile.v1_handle(),
					placement);
		}
	};




	class GH_memoryless_placement : public Gudhi::contraction::Placement_policy<EdgeProfile>{

		Mesh_context& mesh_context_;
	public:
		typedef typename EdgeProfile::Point Point;
		typedef typename EdgeProfile::Edge_handle Edge_handle;

		typedef typename Gudhi::contraction::Placement_policy<EdgeProfile>::Placement_type Placement_type;


		GH_memoryless_placement(Mesh_context& mesh_context):mesh_context_(mesh_context) {
		}

		Placement_type operator()(const EdgeProfile& profile) const override{
			typedef typename EdgeProfile::Complex::GT GT;
			typedef quadric::Error_quadric<GT> Error_quadric_;

			auto proxies = profile.v0().plane_primitives();
			proxies.insert(profile.v1().plane_primitives().begin(),profile.v1().plane_primitives().end());

			Error_quadric_ quadric_edge = (get_sum_quadrics(profile.v0(),profile.v1())).quadric();

			for(auto proxy : proxies){
				double ratio = proxy->area()/(double)proxy->num_vertices() * mesh_context_.params.abstraction_weight ;
				quadric_edge += (Error_quadric_(proxy->plane(),ratio));
			}

			if(mesh_context_.params.abstraction_weight == 1 && proxies.empty()){
				quadric_edge+=compute_quadrics_data_edge(mesh_context_,profile.edge_handle(),1);
			}
			else{
				if(mesh_context_.params.abstraction_weight!=1)
					quadric_edge+=compute_quadrics_data_edge(mesh_context_,profile.edge_handle(),(1-mesh_context_.params.abstraction_weight));
			}

			Error_quadric_svd_solver<EdgeProfile> solver;
			boost::optional<Point> min_quadric_pt(solver(quadric_edge,profile,1e-3,false));

			return min_quadric_pt;

		}
	};


	class GH_memoryless_cost : public Gudhi::contraction::Cost_policy<EdgeProfile>{
		Mesh_context& mesh_context_;
	public:

		typedef typename Gudhi::contraction::Cost_policy<EdgeProfile>::Cost_type Cost_type;
		typedef typename EdgeProfile::Complex Complex;
		typedef typename Complex::Vertex_handle Vertex_handle;
		typedef typename Complex::Point Point;

		typedef typename Complex::GT GT;
		typedef typename GT::Plane_3 Plane_3;
		typedef typename GT::Vector_3 Vector_3;


		GH_memoryless_cost(Mesh_context& mesh_context):mesh_context_(mesh_context)
		{
		}

		Cost_type operator()( EdgeProfile const& profile, boost::optional<Point> const& new_point ) const override {
			typedef typename EdgeProfile::Complex::GT GT;
			typedef quadric::Error_quadric<GT> Error_quadric_;

			Cost_type res;
			if (new_point){
				Error_quadric_ quadric_edge = (get_sum_quadrics(profile.v0(),profile.v1())).quadric();
				quadric_edge+=compute_quadrics_data_edge(mesh_context_,profile.edge_handle(),1);
				res = quadric_edge.cost(*new_point);
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

	GH_memoryless_policies(Mesh_context& mesh_context):
		Simplification_policies<EdgeProfile>(
				new GH_memoryless_cost(mesh_context),
				new GH_memoryless_placement(mesh_context),
				new Urban_valid_contraction(mesh_context),
				new GH_memoryless_visitor(mesh_context))
				{
				}
};



#endif /* GH_MEMORYLESS_H_ */
