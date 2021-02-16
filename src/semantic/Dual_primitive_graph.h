/*
 * Dual_primitive_graph.h
 *
 *  Created on: Jul 24, 2014
 *      Author: dsalinas
 */

#ifndef Dual_primitive_graph_H_
#define Dual_primitive_graph_H_

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>

#include <vector>

#include "Primitive_graph.h"

/**
 * Dual graph of a proxy graph G.
 *
 * Vertices are corner (maximal cliques with length gt than 2) and contains
 * their proxies and a point (an intersection of a triplet of the clique
 * for example).
 *
 * Two vertices c1,c2 are linked by an edge in the dual
 * if the proxies of c1 intersects the proxies of c2 in an edge of G.
 *
 */


template <typename Mesh>
class Dual_primitive_graph {


public:
	typedef typename Mesh::GT GT;
	typedef Plane_primitive<GT,typename Mesh::Vertex_handle> Plane_primitive_;
	typedef typename Mesh::Point Point;


	typedef Primitive_graph<Mesh> Primal_graph;
	typedef typename Primal_graph::Graph_vertex_descriptor Primal_graph_vertex_descriptor;
	typedef typename Primal_graph::Graph_edge_descriptor Primal_graph_edge_descriptor;

	typedef typename std::vector<Primal_graph_vertex_descriptor> Primal_clique;

	struct Dual_graph_vertex{
		Primal_clique clique ;
		Point point;
	};

	struct Dual_graph_edge{
	};


private:
	typedef boost::adjacency_list
			< boost::setS,
			boost::vecS,
			boost::undirectedS,
			Dual_graph_vertex,
			Dual_graph_edge
			> Dual_graph;

	Dual_graph dual_graph_;


public:
	typedef typename Dual_graph::vertex_descriptor Dual_graph_vertex_descriptor;
	typedef typename Dual_graph::edge_descriptor Dual_graph_edge_descriptor;


	typedef typename boost::graph_traits<Dual_graph>::vertex_iterator Dual_graph_vertex_iterator;
	typedef typename boost::graph_traits<Dual_graph>::edge_iterator Dual_graph_edge_iterator;

	//	typedef typename Dual_graph::vertex_iterator Dual_graph_vertex_iterator;
	//	typedef typename Dual_graph::edge_iterator Dual_graph_edge_iterator;
private:
	std::map<Primal_clique,Dual_graph_vertex_descriptor> clique_to_dual_vertex_;

public:


	Dual_primitive_graph(){}


	void compute_graph(const Primal_graph& primal_graph){
		UDBG("compute dual graph");
		clear();
		compute_vertices(primal_graph);
		compute_edges();
	}

	void clear(){
		UDBG("clear graph");
		clique_to_dual_vertex_.clear();
		dual_graph_.clear();
	}


public:

	friend class clique_visitor;

	struct clique_visitor{
	private:
		Dual_primitive_graph& dual_;
	public:
		clique_visitor(Dual_primitive_graph& dual):dual_(dual){}

		template <typename Clique,typename Graph>
		void clique(const Clique& c, const Graph& g){

			if(c.size()<3) return;

			Primal_clique primal_clique;
			for(unsigned i = 0 ; i<c.size(); ++i)
				primal_clique.push_back(c[i]);

			auto point =  dual_.get_point(g,primal_clique);

			if(point){
				auto vertex_descr = boost::add_vertex(dual_.dual_graph_);
				dual_.dual_graph_[vertex_descr].clique = primal_clique;
				dual_.dual_graph_[vertex_descr].point= *point;
				dual_.clique_to_dual_vertex_[primal_clique] = vertex_descr;
			}
		}
	};

private:

	template<typename graph>
	boost::optional<Point> get_point(const graph& primal,const Primal_clique& primal_clique) const{

		// look for the triplet of proxies in the clique with highest determinant
		unsigned imax=0;
		unsigned jmax=0;
		unsigned kmax=0;
		double det_max=-1;

		for(auto i=0; i< primal_clique.size() ; ++i){
			for(auto j=i; ++j< primal_clique.size() ; ){
				for(auto k=j; ++k< primal_clique.size() ; ){
					double determinant_ijk =
							std::abs(
									determinant(
											(primal[primal_clique[i]].primitive)->plane(),
											(primal[primal_clique[j]].primitive)->plane(),
											(primal[primal_clique[k]].primitive)->plane()
									)
							);
					if( determinant_ijk  >= det_max){
						imax = i;
						jmax = j;
						kmax = k;
						det_max = determinant_ijk;
					}
				}
			}
		}

		const double min_determinant_threshold = 0.07;

		boost::optional<Point> proj;
		if(det_max>min_determinant_threshold)
			proj = proj_on_three_intersection(
					primal[primal_clique[imax]].primitive,
					primal[primal_clique[jmax]].primitive,
					primal[primal_clique[kmax]].primitive
			);
		return proj;
	}

	void compute_vertices(const Primal_graph& primal_graph){
		clique_visitor vis(*this);
		// go for all clique and register them if size >= 3
		boost::bron_kerbosch_all_cliques(primal_graph.graph, vis);
	}

	void compute_edges(){
		// go for all pairs of dual vertices c1 c2and
		// register an edge if c1 \cap c2 is a primal edge

		// un peu bourrin possible de faire plus efficace en stockant
		// plus de structure

		Dual_graph_vertex_iterator ui,ui_end;
		Dual_graph_vertex_iterator vi,vi_end;

		for (boost::tie(ui, ui_end) = vertices_iterator(); ui != ui_end; ++ui){
			vi_end = ui_end;
			for (vi = ui; ++vi != vi_end; ){
				std::vector<Primal_graph_vertex_descriptor> intersection;

				std::set_intersection(
						dual_graph_[*ui].clique.begin(),dual_graph_[*ui].clique.end(),
						dual_graph_[*vi].clique.begin(),dual_graph_[*vi].clique.end(),
						std::back_inserter(intersection)
				);

				if(intersection.size()>=2){
					boost::add_edge(*ui,*vi,dual_graph_);
				}
			}
		}
	}



public:

	std::pair<Dual_graph_vertex_iterator,Dual_graph_vertex_iterator> vertices_iterator(){
		return vertices(dual_graph_);
	}

	std::pair<Dual_graph_edge_iterator,Dual_graph_edge_iterator> edges_iterator(){
		return edges(dual_graph_);
	}


	const Dual_graph_vertex& vertex(Dual_graph_vertex_descriptor vert_descr) const{
		return dual_graph_[vert_descr];
	}

	const Dual_graph_vertex& first_vertex(Dual_graph_edge_descriptor edge_descr) const{
		return dual_graph_[source(edge_descr,dual_graph_)];
	}

	const Dual_graph_vertex& second_vertex(Dual_graph_edge_descriptor edge_descr) const{
		return dual_graph_[target(edge_descr,dual_graph_)];
	}

	unsigned num_vertices() const{
		return boost::num_vertices(dual_graph_);
	}
	unsigned  num_edges() const{
		return boost::num_edges(dual_graph_);
	}

};


#endif /* Dual_primitive_graph_H_ */
