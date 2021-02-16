/*
 * Graph.h
 *
 *  Created on: Nov 17, 2014
 *      Author: dsalinas
 */

#ifndef GRAPH_H_
#define GRAPH_H_


#include "reg_grow_typedefs.h"
#include <boost/graph/adjacency_list.hpp>
#include <vector>

namespace regrow {


struct Graph_vertex{
	Point point;
	Graph_vertex():point(){}
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


} //namespace regrow

#endif /* GRAPH_H_ */
