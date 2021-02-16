/*
 * K_nearest_graph.h
 *  Created on: Dec 11, 2014
 * This file is part of the Gudhi Library. The Gudhi library 
 *    (Geometric Understanding in Higher Dimensions) is a generic C++ 
 *    library for computational topology.
 *
 *    Author(s):       David Salinas
 *
 *    Copyright (C) 2014  INRIA Sophia Antipolis-Méditerranée (France)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#ifndef K_NEAREST_GRAPH_H_
#define K_NEAREST_GRAPH_H_


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
#include "Point_with_id.h"

using namespace CGAL;

namespace regrow {


// Graph is a boost graph whose vertices have a field 'Vertex_handle vertex'
class K_nearest_graph{

public:
	struct Graph_vertex{
		Point_with_id point;
	};
	typedef boost::adjacency_list
			< boost::setS,
			boost::vecS,
			boost::undirectedS,
			Graph_vertex
			> Graph;
	typedef typename Graph::vertex_descriptor Graph_vertex_descriptor;
	typedef typename Graph::edge_descriptor Graph_edge_descriptor;
	typedef typename Graph::vertex_iterator Graph_vertex_iterator;
	typedef typename Graph::edge_iterator Graph_edge_iterator;
	typedef typename boost::graph_traits<Graph>::adjacency_iterator GAit;

private:

	typedef Search_traits<double, Point_with_id, const double*, Construct_coord_iterator> TreeTraits;
	//
	//	typedef typename CGAL::Search_traits_3<K> TreeTraits;
	typedef Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
	typedef Neighbor_search::Tree Tree;
	Graph graph_;
public:

	/**
	 * @brief compute k_nearest_graph from points in an off file
	 */
	K_nearest_graph(std::string name_file_points,std::string name_file_out,unsigned k){
		graph_.clear();
		compute_graph_vertices(name_file_points);
		compute_graph_edges(k);
		save_to_off(name_file_out);
	}

	Graph graph(){
		return graph_;
	}



private:
	struct Off_vis{
		Off_vis(Graph& graph):graph_(graph){}

		void init(int dim,int num_vertices,int num_faces,int num_edges){
		}


		void point(const std::vector<double>& point){
			Graph_vertex_descriptor addr = boost::add_vertex(graph_);
			graph_[addr].point = Point_with_id(point[0],point[1],point[2],boost::num_vertices(graph_)-1);
		}

		void maximal_face(const std::vector<int>& face){}

		void done(){}
		Graph& graph_;
	};

	void compute_graph_vertices(std::string name_file_points){
		std::ifstream stream(name_file_points);
		if(!stream.is_open()){
			std::cerr<<"problem loading file "<<name_file_points<<std::endl;
		}
		else{
			Off_reader off_reader(stream);
			Off_vis vis(graph_);
			off_reader.read(vis);
		}
	}

	void compute_graph_edges(unsigned k){
		std::vector<Point_with_id> points_with_id;
		auto v_beg_end = boost::vertices(graph_);
		for(auto v_it = v_beg_end.first; v_it != v_beg_end.second; ++v_it){
			points_with_id.push_back(graph_[*v_it].point);
		}
		Tree tree(points_with_id.begin(),points_with_id.end(),k+1);

		v_beg_end = boost::vertices(graph_);
		for(auto v_it = v_beg_end.first; v_it != v_beg_end.second; ++v_it){
			auto p = *v_it;
			Neighbor_search search(tree, Point_with_id(graph_[p].point),k+1);
			for(auto it = search.begin(); it != search.end(); ++it){
				auto q = it->first;
				if(p!=q.vertex_handle) boost::add_edge(p,q.vertex_handle,graph_);
			}
		}
	}

	void save_to_off(const std::string& name_file){
		std::ofstream stream(name_file);
		stream<<"OFF\n";
		stream<<boost::num_vertices(graph_)<<" "<<boost::num_edges(graph_)<<" 0\n";
		auto v_beg_end = boost::vertices(graph_);
		for(auto v_it = v_beg_end.first; v_it != v_beg_end.second; ++v_it){
			stream<<graph_[*v_it].point.x()<<" "<<graph_[*v_it].point.y()<<" "<<graph_[*v_it].point.z()<<"\n";
		}
		auto e_beg_end = boost::edges(graph_);
		for(auto e_it = e_beg_end.first; e_it != e_beg_end.second; ++e_it){
			stream<<"2 "<<boost::target(*e_it,graph_)<<" "<<boost::source(*e_it,graph_)<<"\n";
		}
		stream.close();
	}

};



}

#endif /* K_NEAREST_GRAPH_H_ */
