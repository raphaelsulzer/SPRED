/*
 * Triangle_graph.h
 *
 *  Created on: Nov 13, 2014
 *      Author: dsalinas
 */

#ifndef TRIANGLE_GRAPH_H_
#define TRIANGLE_GRAPH_H_

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include "Off_reader.h"
#include "Triangle.h"
#include "typedefs.h"

using Reorient::Point;

namespace Reorient{

struct Dual_graph_vertex{
	Triangle t;
	Dual_graph_vertex(){}
};

//struct edge_weight_t { };

struct Dual_graph_edge{
	double weight;
};

typedef boost::property<boost::edge_weight_t, double> edge_weight_t;

typedef boost::adjacency_list
		< boost::setS,
		boost::vecS,
		boost::undirectedS,
		Dual_graph_vertex,
		edge_weight_t
		> Dual_graph;

typedef Dual_graph::vertex_descriptor Dual_graph_vertex_descriptor;
typedef Dual_graph::edge_descriptor Dual_graph_edge_descriptor;
typedef Dual_graph::vertex_iterator Dual_graph_vertex_iterator;
typedef Dual_graph::edge_iterator Dual_graph_edge_iterator;


struct Primal_graph_vertex{
	Point p;
	Primal_graph_vertex():p(){}
};


struct Primal_graph_edge{
	std::vector<Dual_graph_vertex_descriptor> adj_triangles;
};


typedef boost::adjacency_list
		< boost::setS,
		boost::vecS,
		boost::undirectedS,
		Primal_graph_vertex,
		Primal_graph_edge
		> Primal_graph;


typedef Primal_graph::vertex_descriptor Graph_vertex_descriptor;
typedef Primal_graph::edge_descriptor Graph_edge_descriptor;
typedef Primal_graph::vertex_iterator Graph_vertex_iterator;
typedef Primal_graph::edge_iterator Graph_edge_iterator;



/**
 * While going through off file, it saves the primal graph
 * and the dual graph whose vertices are triangles.
 */
struct Off_dual_visitor{
	Off_dual_visitor(Primal_graph& primal_graph,Dual_graph& dual_graph):
		primal_graph_(primal_graph),dual_graph_(dual_graph){
		primal_graph.clear();
		dual_graph.clear();
	}

	void point(const std::vector<double>& point){
		auto v_descr = boost::add_vertex(primal_graph_);
		primal_graph_[v_descr].p = point;
	}

	void maximal_face(const std::vector<int>& triangle){
		Dual_graph_vertex_descriptor t_descr = boost::add_vertex(dual_graph_);

		if(triangle.size()!=3) return; //todo currently does not register edges -> only has to register it in primal and then save primal
		dual_graph_[t_descr].t = Triangle(triangle[0],triangle[1],triangle[2]);
		for(size_t i = 0 ; i < triangle.size();++i){
			for(size_t j = i+1 ; j < triangle.size();++j){
				Graph_edge_descriptor e_descr = (boost::add_edge(triangle[i],triangle[j],primal_graph_)).first;
				primal_graph_[e_descr].adj_triangles.push_back(t_descr);
			}
		}
	}

	void done(){
		// add dual edges
		for(auto edgePair = boost::edges(primal_graph_); edgePair.first != edgePair.second; ++edgePair.first){
			const std::vector<Dual_graph_vertex_descriptor>& adj_triangles = primal_graph_[*edgePair.first].adj_triangles;
			for(auto t1 = adj_triangles.begin(); t1!= adj_triangles.end(); ++t1){
				for(auto t2 = t1; ++t2!= adj_triangles.end(); ){
					boost::add_edge(*t1,*t2,dual_graph_);
				}
			}
		}
	}

	Primal_graph& primal_graph_;
	Dual_graph& dual_graph_;
};

inline
void build_primal_dual_graph(const std::string& name_file_off,Primal_graph& primal_graph,Dual_graph& dual_graph){
	std::ifstream stream(name_file_off.c_str());
	if(!stream.is_open()){
		std::cerr <<"problem loading input file"<<name_file_off<<std::endl;
	}
	else{
		Off_reader off_reader(stream);
		Off_dual_visitor off_vis(primal_graph,dual_graph);
		bool ok = off_reader.read(off_vis);
		if(!ok) std::cerr <<"found error in off file "<<name_file_off<<std::endl;
	}
}

}

#endif /* TRIANGLE_GRAPH_H_ */
