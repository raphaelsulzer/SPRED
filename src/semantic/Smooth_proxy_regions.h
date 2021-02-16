/*
 * Smooth_proxy_regions.h
 *
 *  Created on: Apr 30, 2014
 *      Author: dsalinas
 */

#ifndef SMOOTH_PROXY_REGIONS_H_
#define SMOOTH_PROXY_REGIONS_H_

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "Smooth_proxy.h"
#include "Primitive_graph.h"



template<typename Mesh> class Smooth_proxy_regions{
public:
	typedef typename Mesh::GT GT;
	typedef typename Mesh::Vertex_handle Vertex_handle;
	typedef typename Mesh::Graph_vertex Graph_vertex;
	typedef typename Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	typedef Smooth_proxy<Mesh> Smooth_proxy_;
	typedef Primitive_graph<Mesh> Primitive_graph_;
	typedef std::vector<Smooth_proxy_> Smooth_proxies;
	typedef typename Smooth_proxies::iterator Smooth_proxies_iterator;
	typedef typename Smooth_proxies::const_iterator Smooth_proxies_const_iterator;

private:

	typedef typename Smooth_proxy_::Neighbor_regions_container Neighbor_regions_container;

	Smooth_proxies proxies_;
	std::map<Vertex_handle,Smooth_proxies_iterator> vh_to_proxy_;

	Mesh& mesh_;
	const Primitive_graph_& graph_;


public:


	Smooth_proxy_regions(Mesh& mesh,const Primitive_graph_& graph):mesh_(mesh),graph_(graph){
	}

	void clear(){
		proxies_.clear();
		vh_to_proxy_.clear();
	}

	/**
	 * compute the smooth regions and add it to proxies
	 */
	void compute_regions(){
		clear();
		// 1- compute the graph of the mesh and remove vertices that belongs to proxy
		// 2- compute connected component of the obtained graph
		// 3- for each cc, add a Smooth_proxy to 'proxies_'
		// 4- compute Plane_proxy neighbors for each Smooth_proxy


		// 1- compute the graph of the mesh and remove vertices that belongs to proxy

		struct Graph_vertex{Vertex_handle vh;};
		struct Graph_edge{};
		typedef boost::adjacency_list< boost::setS,boost::vecS,boost::undirectedS,Graph_vertex,Graph_edge> Graph;
		typedef typename Graph::vertex_descriptor Graph_vertex_descriptor;

		Graph graph;
		// vertex_handle to vertex_graph
		std::map<Vertex_handle,Graph_vertex_descriptor> vh_to_vg;

		for(auto vertex_complex : mesh_.vertex_range()){
			if(mesh_[vertex_complex].empty_plane_primitives()){
				auto vertex_descr = boost::add_vertex(graph);
				graph[vertex_descr].vh = vertex_complex;
				vh_to_vg[vertex_complex] = vertex_descr;
			}
		}

		for(auto edge_complex : mesh_.edge_range()){
			auto v0 = mesh_.first_vertex(edge_complex);
			auto v1 = mesh_.second_vertex(edge_complex);
			if(mesh_[v0].empty_plane_primitives() && mesh_[v1].empty_plane_primitives()){
				add_edge(vh_to_vg[mesh_.first_vertex(edge_complex)],vh_to_vg[mesh_.second_vertex(edge_complex)], graph);
			}
		}


		// 2- compute connected component of the obtained graph
		std::vector<int> component(num_vertices(graph));
		int num_cc = connected_components(graph, &component[0]);


		// 3- for each cc, add a Smooth_proxy to 'proxies_'
		proxies_.reserve(num_cc);
		for (int j = 0; j != num_cc; ++j){
			proxies_.push_back(Smooth_proxy_(mesh_));
		}
		typename std::vector<Vertex_handle>::size_type i;
		for (i = 0; i != component.size(); ++i){
			proxies_[ component[i] ].add_vertex(graph[i].vh);
			vh_to_proxy_[graph[i].vh] = proxies_.begin()+component[i];
		}

		// compute neighbor regions
		std::set<Vertex_handle> bdry_vertices;
		for(auto edge_complex : mesh_.edge_range()){
			auto v0 = mesh_.first_vertex(edge_complex);
			auto v1 = mesh_.second_vertex(edge_complex);
			if(!mesh_[v0].empty_plane_primitives()){
				std::swap(v0,v1);
			}
			if(mesh_[v0].empty_plane_primitives()){
				if(!mesh_[v1].empty_plane_primitives()){
					Smooth_proxies_iterator proxy_v0 = (vh_to_proxy_[v0]);
					for(auto proxy_v1 : mesh_[v1].plane_primitives()){
						proxy_v0->add_neighbor_regions(proxy_v1);
					}
				}
				// else both are smooth, we go to the next edge
			}
			// else neither v0 nor v1 is smooth, we go to the next edge
		}
//
		for(auto& proxy : proxies_){
			proxy.compute_propagation();
		}

		std::cout << "Total number of components: " << num_cc << std::endl;
	}

	Smooth_proxies_const_iterator begin_smooth_proxies() const{
		return proxies_.cbegin();
	}
	Smooth_proxies_const_iterator end_smooth_proxies() const{
		return proxies_.cend();
	}

	Smooth_proxies_iterator begin_smooth_proxies(){
		return proxies_.begin();
	}
	Smooth_proxies_iterator end_smooth_proxies() {
		return proxies_.end();
	}

	/**
	 * return a the smooth region passing through v if any.
	 */
	boost::optional<Smooth_proxies_const_iterator> proxy(Vertex_handle v) const{
		boost::optional<Smooth_proxies_const_iterator> res;
		if(mesh_[v].empty_plane_primitives()){
			auto it=vh_to_proxy_.find(v);

			assert(it!=vh_to_proxy_.end());

			res = it->second;
		}
		return res;
	}



	typedef typename Smooth_proxy_::Neighbor_regions_const_iterator Neighbor_regions_const_iterator;
	Neighbor_regions_const_iterator begin_neighbor_regions(Smooth_proxies_const_iterator region) const{
		return region->begin_neighbor_regions();
	}

	Neighbor_regions_const_iterator end_neighbor_regions(Smooth_proxies_const_iterator region) const{
		return region->end_neighbor_regions();
	}

	int num_neighbor_regions(Smooth_proxies_const_iterator region){
		return region->size();
	}

//	/**
//	 * remove a proxy
//	 */
//	void remove_proxy(Smooth_proxies_const_iterator proxy_to_remove );



	std::string to_string() const{
		std::ostringstream stream;
		for (auto proxy: proxies_){
			stream << proxy.to_string() <<"\n";
		}
		return stream.str();
	}
};


#endif /* SMOOTH_PROXY_REGIONS_H_ */
