/*
 * Mesh_context.h
 *
 *  Created on: Apr 28, 2014
 *      Author: dsalinas
 */

#ifndef MESH_CONTEXT_H_
#define MESH_CONTEXT_H_

#include <algorithm>

#include "typedefs.h"
#include "semantic/Enriched_mesh.h"
#include "semantic/Primitive_graph.h"
#include "semantic/Dual_primitive_graph.h"
#include "semantic/Smooth_proxy_regions.h"
#include "semantic/Merge_proxies_params.h"
#include "semantic/Graph_detection_params.h"

#include "contraction/Stats.h"
#include "interface/Contraction_parameter.h"

#include "geometry/Independent_proxies.h"

struct Mesh_context{


	typedef Complex::Point Point;
	typedef Complex Mesh;
	Complex mesh;
	Primitive_graph<Complex> primitive_graph;
	Dual_primitive_graph<Complex> dual_graph;

	typedef Dual_primitive_graph<Complex> Dual_graph;


	std::list<Point> initial_points;
	Contraction_parameter params;
	Stats stats;
	Enriched_mesh<Complex> segmentation;
	Smooth_proxy_regions<Complex> smooth_regions;


	Mesh_context():
		mesh(),
		primitive_graph(),
		initial_points(),
		params(),
		stats(),
		segmentation(mesh, params),
		smooth_regions(mesh,primitive_graph)
	{
	}

	void set_initial_points(){
		initial_points.clear();
		for(auto vi : mesh.vertex_range())
			initial_points.push_back(mesh.point(vi));
	}


	/**
	 * to be called when the segmentation has changed
	 * to compute graphs and corner
	 */
	void update_graphs(double max_dist,bool compute_dual = true){//unsigned num_neighbors,double max_edge_dist){
		UDBG("--- compute graph ---");
		primitive_graph.compute_graph(mesh, segmentation,max_dist,-1);
		UDBG("--- compute corners ---");
		primitive_graph.compute_corners(mesh,segmentation);
		UDBG("--- done proxy detection ---");
		if(compute_dual) dual_graph.compute_graph(primitive_graph);
	}


private:
	struct Edges_set{
		std::set< std::pair<Planar_proxy*,Planar_proxy*> > edges_set;
		Edges_set(Primitive_graph<Complex>& primitive_graph){
			primitive_graph.look_through_edges(this);
		}

		// graph visitor callback
		void edge(Planar_proxy* p0,Planar_proxy* p1){
			add_edge(p0,p1);
		}

		void add_edge(Planar_proxy* p0,Planar_proxy* p1){
			assert(p0!=p1);
			edges_set.insert(std::make_pair(p0,p1));
		}

	};




	//todo do something simpler and not relative
	/*
	 * check that
	 * max_{x\in P0 \cup P1 } d(x, P01 ) <= (1+max_dist) * max [
	 *  (max_{x\in P0 } d(x, P0 )
	 *  (max_{x\in P1 } d(x, P1 )
	 * ]
	 * where P01 is the best fitting plane for points of P0 \cup P1
	 */
	bool check_relative_distance_proxies(Planar_proxy* P0,Planar_proxy* P1,double max_dist) const{
		std::vector<Point> point_for_least_squares;

		double dist_max_P0 = 0;
		for(auto v = P0->begin_initial_vertices(); v != P0->end_initial_vertices(); ++v){
			auto point = mesh.point(*v);
			point_for_least_squares.push_back(point);
			dist_max_P0 = (std::max)( CGAL::squared_distance( point , P0->plane() )  , dist_max_P0 );
		}

		double dist_max_P1 = 0;
		for(auto v = P1->begin_initial_vertices(); v != P1->end_initial_vertices(); ++v){
			auto point = mesh.point(*v);
			point_for_least_squares.push_back(point);
			dist_max_P1 = (std::max)( CGAL::squared_distance( point , P1->plane() )  , dist_max_P1 );
		}

		Plane_3 optimal_plane;
		CGAL::linear_least_squares_fitting_3(point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());

		double dist_max_P01 = 0 ;
		for (auto p : point_for_least_squares  ){
			dist_max_P01 = (std::max)( CGAL::squared_distance( p , optimal_plane )  , dist_max_P01 );
		}
		return (dist_max_P01 < (1+max_dist)*(1+max_dist)* (std::max)(dist_max_P0,dist_max_P1) );
	}



	/*
	 * check that
	 * max_{x\in P0 \cup P1 } d(x, P01 ) <= max_dist
	 * where P01 is the best fitting plane for points of P0 \cup P1
	 */
	bool check_distance_proxies(Planar_proxy* P0,Planar_proxy* P1,double max_dist) const{
		std::vector<Point> point_for_least_squares;
		for(auto v = P0->begin_initial_vertices(); v != P0->end_initial_vertices(); ++v)
			point_for_least_squares.push_back(mesh.point(*v));

		for(auto v = P1->begin_initial_vertices(); v != P1->end_initial_vertices(); ++v)
			point_for_least_squares.push_back(mesh.point(*v));

		Plane_3 optimal_plane;
		CGAL::linear_least_squares_fitting_3(point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());

		double dist_max_P01 = 0 ;
		for (auto p : point_for_least_squares  )
			dist_max_P01 = (std::max)( CGAL::squared_distance( p , optimal_plane )  , dist_max_P01 );
		return dist_max_P01 < max_dist*max_dist;
	}


public:




	/**
	 * @brief Merge proxies P0P1 whose normals have an angle (in degree)
	 * less than alpha and whose points are at distance less than maxdist
	 * from their best fit plane
	 */
	void merge_proxies(const Merge_proxies_params& merge_params){//double alpha = 0.05,double max_dist=0.01){
		int num_proxies_removed = 0;
		do{
			std::set<Planar_proxy*> removed_proxies;
			Edges_set edges(primitive_graph);

			num_proxies_removed = 0;
			while(!edges.edges_set.empty()){
				auto edge = *(edges.edges_set.begin());
				edges.edges_set.erase(edge);
				bool first_already_removed  = !(removed_proxies.find(edge.first ) == removed_proxies.end());
				bool second_already_removed = !(removed_proxies.find(edge.second) == removed_proxies.end());
				if(!(first_already_removed || second_already_removed)
						&&
						!is_alpha_independent_proxies(edge.first,edge.second,merge_params.max_angle_merge*3.1415926/180.0)
						&&
						check_distance_proxies(edge.first,edge.second,merge_params.max_dist_merge)
				){

					// we keep the proxy with the largest area
					auto kept_proxy = edge.first;
					auto removed_proxy = edge.second;
					if(edge.first->area()<edge.second->area())
						std::swap(kept_proxy,removed_proxy);

					primitive_graph.collapse_edge(kept_proxy,removed_proxy);
					removed_proxies.insert(removed_proxy);
					segmentation.merge_two_proxies(kept_proxy,removed_proxy);

					++num_proxies_removed;
				}
			}
			UDBGVALUE(num_proxies_removed);
		} while(num_proxies_removed!=0);

		filter_proxies(merge_params.min_area_merge);
	}



	int expand_proxies(double max_dist){
		smooth_regions.compute_regions();

		int num_assigned = 0;
		for(
				auto smooth_region = smooth_regions.begin_smooth_proxies();
				smooth_region != smooth_regions.end_smooth_proxies();
				++smooth_region
		){
			if(smooth_region->num_neighbor_regions()==1 &&
					(smooth_region->inf_cost() <= max_dist)){
				smooth_region->assign_propagation();
				++num_assigned;
			}
		}
		return num_assigned;
	}


	void filter_smallest_proxy(){
		auto smallest_proxy =
				std::min_element(
						segmentation.planes.begin(),
						segmentation.planes.end(),
						[](const Planar_proxy& p1,const Planar_proxy& p2){
							return p1.area() < p2.area();
						}
				);
		remove_proxy(&(*smallest_proxy),false);

	}

	void filter_proxies(double min_area = 50.0 ) {

		std::list<Planar_proxy*> proxies_to_remove;

		for(auto primitive = segmentation.planes.begin() ; primitive != segmentation.planes.end(); ++primitive){
			auto primitive_pt = &(*primitive);
			if(primitive_pt->area() <= min_area){
				proxies_to_remove.push_back(primitive_pt);
			}
		}
		for(auto proxy : proxies_to_remove){
			remove_proxy(proxy,false);
		}
		primitive_graph.compute_corners(mesh,segmentation);
	}


	void graph_perturbation(double percent){
		assert(percent>=0 && percent <=100);

		const double ratio_remove_add = 1.;

		const unsigned num_removed_edges = primitive_graph.num_edges() * percent / 100. ;
		const unsigned num_added_edges = primitive_graph.num_edges() * percent / 100. / ratio_remove_add;

		primitive_graph.remove_random_edges(num_removed_edges);
		primitive_graph.add_random_edges(num_added_edges);

		//update everything but the graph
		primitive_graph.compute_corners(mesh,segmentation);
		UDBG("--- done proxy detection ---");
		dual_graph.compute_graph(primitive_graph);
	}


	void primitives_perturbation(double percent){
		assert(percent>=0 && percent <=100);
		remove_random_primitives(segmentation.size()*percent/100.);
	}


private:
	void remove_random_primitives(unsigned number){
		if(number>= segmentation.size()){
			segmentation.clear();
		}
		else{
			while(number--){
				remove_random_primitives();
			}
		}
	}

	void remove_random_primitives(){
		srand(time(NULL));

		if(segmentation.is_empty()) return;
		int random_index = ((double) rand() / RAND_MAX)*(segmentation.size()-1);
		UDBGVALUE(random_index);
		auto it = segmentation.planes.begin();
		std::advance(it,random_index);
		remove_proxy(&(*it),false);
	}



public:



	/**
	 * remove a proxy in the mesh context.
	 */
	bool remove_proxy(
			Planar_proxy * proxy_to_remove,bool update_corner=true ){
		bool res = segmentation.remove_plane_proxy(proxy_to_remove);
		if(!res) return false;
		primitive_graph.remove_primitive_vertex(proxy_to_remove);
		if(update_corner) primitive_graph.compute_corners(mesh,segmentation);
		return true;
	}




};


///**
// * remove a proxy in the mesh context.
// */
//template<typename MeshContext>
//bool remove_proxy(
//		MeshContext& mesh_ctxt,
//		typename MeshContext::Mesh::Graph_vertex::Planar_proxy * proxy_to_remove ){
//	bool res = mesh_ctxt.segmentation.remove_plane_proxy(proxy_to_remove);
//	if(!res) return false;
//	mesh_ctxt.primitive_graph.remove_primitive_vertex(proxy_to_remove);
//	mesh_ctxt.primitive_graph.compute_corners(mesh_ctxt.mesh,mesh_ctxt.segmentation);
//	return true;
//}



#endif /* MESH_CONTEXT_H_ */
