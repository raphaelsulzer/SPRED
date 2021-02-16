#ifndef REORIENT__
#define REORIENT__

#include "Reorient.h"
#include <fstream>
#include <iostream>
#include <boost/progress.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/optional.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include "details/typedefs.h"
#include "details/Primal_dual_graph.h"
#include "details/Triangle.h"
#include "details/Off_save_visitor.h"



namespace Reorient{


template < typename Visitor > class bfs_visitor:public boost::default_bfs_visitor {
public:
	bfs_visitor(Dual_graph& graph,const Visitor& vis):graph_(graph),vis_(vis){
		current_vertex_ = Dual_graph_vertex_descriptor(0);
	}

	template < typename Vertex, typename Graph >
	void examine_vertex(Vertex u, const Graph & g) const{
		current_vertex_ = u;
	}

	template < typename Edge, typename Graph >
	void tree_edge(Edge e, const Graph & g) const{

		Graph_vertex_descriptor other = boost::source(e,g);
		if(boost::source(e,g) == current_vertex_)
			other = boost::target(e,g);

		assert(boost::edge(other,current_vertex_,g).second);
		if(same_orientation(graph_[other].t,graph_[current_vertex_].t)){
			graph_[other].t.flip();
		}
	}

	template < typename Vertex, typename Graph >
	void finish_vertex(Vertex u, const Graph & g) const{
		vis_.triangle(graph_[u].t);
	}

	mutable int num_vertices_seen_ = 0;
	Dual_graph& graph_;
	mutable Graph_vertex_descriptor current_vertex_;
	const Visitor& vis_;
};



struct Cost{
	Cost(const Primal_graph& primal_graph,const Dual_graph& dual_graph):
	primal_graph_(primal_graph),dual_graph_(dual_graph){
	}
	double operator()(Dual_graph_edge_descriptor edge){
		const Triangle & t1  = dual_graph_[boost::source(edge,dual_graph_)].t;
		const Triangle & t2  = dual_graph_[boost::target(edge,dual_graph_)].t;
		double res = -std::sqrt((std::max)(squared_area(t1),squared_area(t2)));
		return res;
	}

private:
	double squared_area(const Triangle& t){
		const Point& p0 = primal_graph_[t.v0()].p;
		const Point& p1 = primal_graph_[t.v1()].p;
		const Point& p2 = primal_graph_[t.v2()].p;
		// x1,x2,x3 = p1-p0 and y1,y2,y3 = p2-p0
		//pi = (xi yi zi) then area = (u^2+v^2+w^2)/4
//		with
//		double u = x2 * y3 - x3 * y2;
//		double v = x3 * y1 - x1 * y3;
//		double w = x1 * y2 - x2 * y1;
		double p0p1[3];
		double p0p2[3];
		for(int i = 0; i < 3; ++i){
			p0p1[i] = p1[i]-p0[i];
			p0p2[i] = p2[i]-p0[i];
		}
		double A = p0p1[1] * p0p2[2] - p0p1[2] * p0p2[1];
		double B = p0p1[2] * p0p2[0] - p0p1[0] * p0p2[2];
		double C = p0p1[0] * p0p2[1] - p0p1[1] * p0p2[0];
		return 1./4. * (A*A+B*B+C*C);
	}

	const Primal_graph& primal_graph_;
	const Dual_graph& dual_graph_;
};

inline
Dual_graph build_spanning_tree(const Primal_graph& primal_graph,Dual_graph& dual_graph){
	boost::property_map<Dual_graph, boost::edge_weight_t>::type weightMap;
	Dual_graph_edge_iterator e_it, e_it_end;
	Cost cost(primal_graph,dual_graph);
	for( boost::tie( e_it, e_it_end ) = boost::edges( dual_graph );
			e_it != e_it_end;
			++e_it ){
		weightMap[ *e_it ] = cost(*e_it); //todo put - abs angle between triangle
	}

	Dual_graph spanning_tree;
	for(size_t i = 0 ; i < boost::num_vertices(dual_graph); ++i)
		boost::add_vertex(spanning_tree);

	std::set < Graph_edge_descriptor > spanning_tree_edges;
	boost::kruskal_minimum_spanning_tree(dual_graph, std::inserter(spanning_tree_edges,spanning_tree_edges.begin()));

	for(auto e : spanning_tree_edges)
		boost::add_edge(boost::source(e,spanning_tree),boost::target(e,spanning_tree),spanning_tree);

	return spanning_tree;
}

template<typename Visitor>
void reorient(const Visitor& vis,const std::string& input_off){
	Primal_graph primal_graph;
	Dual_graph dual_graph;
	build_primal_dual_graph(input_off,primal_graph,dual_graph);

	int num_vertices = boost::num_vertices(primal_graph);
	int num_edges = boost::num_edges(primal_graph);
	int num_triangles = boost::num_vertices(dual_graph);

	Dual_graph spanning_tree(build_spanning_tree(primal_graph,dual_graph));


	vis.init(num_vertices,num_triangles,num_edges);

	for(auto vertexPair = boost::vertices(primal_graph); vertexPair.first != vertexPair.second; ++vertexPair.first){
		vis.vertex(primal_graph[*vertexPair.first].p);
	}

    std::vector<int> component(num_triangles);
    int num_cc = boost::connected_components(dual_graph, &component[0]);
    std::vector<bool> seen(num_cc,false);

    for(int index = 0; index < num_triangles ; ++index){
    	int current_cc = component[index];
    	if(!seen[current_cc]){
    		bfs_visitor<Visitor> bfs_vis(dual_graph,vis);
    		breadth_first_search(dual_graph,vertex(index, dual_graph),visitor(bfs_vis));
    		seen[current_cc] = true;
    	}
    }
}


}
#endif
