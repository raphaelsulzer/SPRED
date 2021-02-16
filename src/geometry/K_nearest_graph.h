/*
 * K_nearest_builder.h
 *
 *  Created on: Sep 18, 2014
 *      Author: dsalinas
 */

#ifndef K_NEAREST_BUILDER_H_
#define K_NEAREST_BUILDER_H_

/*
 * K_nearest_builder.h
 *
 *  Created on: Sep 10, 2014
 *      Author: dsalinas
 */

#include <unordered_map>
#include <boost/iterator/iterator_facade.hpp>
#include <CGAL/Euclidean_distance.h>
#include <CGAL/Search_traits.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Simple_cartesian.h>

#include "internal/K_nearest_builder_point.h"


// Graph is a boost graph whose vertices have a field 'Vertex_handle vertex'
template<typename SkBlComplex> class K_nearest_graph{
	typedef typename SkBlComplex::GT GT;
	typedef typename GT::Point Point_3;

public:

	struct Graph_vertex{
		Vertex_handle vertex;
		bool seen;
	};
	struct Graph_edge{};
	typedef boost::adjacency_list
			< boost::setS,
			boost::vecS,
			boost::undirectedS,
			Graph_vertex,
			Graph_edge
			> Graph;
	typedef typename Graph::vertex_descriptor Graph_vertex_descriptor;
	typedef typename Graph::edge_descriptor Graph_edge_descriptor;
	typedef typename Graph::vertex_iterator Graph_vertex_iterator;
	typedef typename Graph::edge_iterator Graph_edge_iterator;
	typedef typename boost::graph_traits<Graph>::adjacency_iterator GAit;

private:




	typedef CGAL::Search_traits<double, Point_with_id, const double*, Construct_coord_iterator> TreeTraits;
	//
	//	typedef typename CGAL::Search_traits_3<K> TreeTraits;
	typedef typename CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
	typedef typename Neighbor_search::Tree Tree;

	const SkBlComplex& complex_;
	Graph graph_;

	std::map<Vertex_handle,Graph_vertex_descriptor> vh_to_graph_vertex_;
public:

	/**
	 * @brief Modify complex to be the expansion of the k-nearest neighbor
	 * symetric graph.
	 */
	K_nearest_graph(const SkBlComplex& complex,unsigned k):complex_(complex){
		graph_.clear();
		vh_to_graph_vertex_.clear();
		compute_graph_vertices();
		compute_graph_edges(k);
	}


//	Graph_adjacency_iterator neigh(Vertex_handle v){
//		return boost::adjacent_vertices(vh_to_graph_vertex_[v], graph_);
//	}


	std::list<Vertex_handle> neighbors(Vertex_handle v){
		std::list<Vertex_handle> res;
		GAit n_v,n_v_end;
		for (boost::tie(n_v, n_v_end) = adjacent_vertices(vh_to_graph_vertex_[v], graph_); n_v != n_v_end; ++n_v){
			res.push_back(vertex(*n_v));
		}
		res.push_back(v);
		return res;
	}


	GAit neigh_it(Vertex_handle v){
		return adjacent_vertices(vh_to_graph_vertex_[v], graph_);
	}


	Graph graph(){
		return graph_;
	}

	//	std::list<Vertex_handle> neighbors(Vertex_handle v){
	//		Graph_adjacency_iterator n_v,n_v_end;
	//		for (tie(n_v, n_v_end) = adjacent_vertices(vh_to_graph_vertex_[v], graph_); n_v != n_v_end; ++n_v){
	//		}
	//		return res;
	//	}


	Vertex_handle vertex(Graph_vertex_descriptor v){
		return graph_[v].vertex;
	}

	void set_all_unexplored(){
		for(auto vh : complex_.vertex_range())
			set_unexplored(vh);
	}

	void set_explored(Vertex_handle v){
		graph_[vh_to_graph_vertex_[v]].seen = true;
	}

	void set_unexplored(Vertex_handle v){
		graph_[vh_to_graph_vertex_[v]].seen = false;
	}


	bool is_explored(Vertex_handle v){
		return graph_[vh_to_graph_vertex_[v]].seen;
	}



private:
	const Point_3& point(Vertex_handle vh){
		return complex_.point(vh);
	}

	void compute_graph_vertices(){
		UDBG("adding vertices");
		for(auto vh : complex_.vertex_range()){
			Graph_vertex_descriptor addr = boost::add_vertex(graph_);
			graph_[addr].vertex = vh ;
			graph_[addr].seen = false;
			vh_to_graph_vertex_[vh] = addr;
		}
	}

	void compute_graph_edges(unsigned k){
		std::list<Point_with_id> points_with_id;
		for(Vertex_handle v: complex_.vertex_range()){
			Point_with_id point_v(point(v).x(),point(v).y(),point(v).z(),v);
			points_with_id.push_back(point_v);
		}
		Tree tree(points_with_id.begin(),points_with_id.end(),k+1);

		for (Vertex_handle p : complex_.vertex_range()){
			Neighbor_search search(tree, Point_with_id(point(p)),k+1);

			for(auto it = search.begin(); it != search.end(); ++it){
				auto q = it->first.vertex_handle;
				if(p!=q) add_edge(p,q);
			}
		}
	}

	void add_edge(Vertex_handle v1,Vertex_handle v2 ){
		boost::add_edge(vh_to_graph_vertex_[v1],vh_to_graph_vertex_[v2],graph_);
	}






};


#endif /* K_NEAREST_BUILDER_H_ */
