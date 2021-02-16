/*
 * Smooth_proxy.h
 *
 *  Created on: Apr 30, 2014
 *      Author: dsalinas
 */

#ifndef SMOOTH_PROXY_H_
#define SMOOTH_PROXY_H_

#include <sstream>
#include <iostream>


#include "Smooth_expansion.h"

template <typename Mesh>
class Smooth_proxy{
	template<class T> friend class Smooth_proxy_regions;

public:
	typedef typename Mesh::GT GT;
	typedef typename Mesh::Vertex_handle Vertex_handle;
	typedef typename Mesh::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	typedef typename std::list<Vertex_handle> Vertices_container;
	typedef typename std::set<Planar_proxy_handle> Neighbor_regions_container;
	typedef typename Neighbor_regions_container::const_iterator Neighbor_regions_const_iterator;


private:
	typedef typename GT::Point_2 Point_2;
	typedef typename GT::Point_3 Point_3;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Vector_3 Vector_3;

	Mesh& mesh_;
	double area_;
	double cost_computation_;
	Point_3 centroid_;
	Vertices_container vertices_;
	Neighbor_regions_container neighbors_regions_;

	Smooth_expander<Mesh> expansion_;


public:



	Smooth_proxy(Mesh& mesh):
		mesh_(mesh),area_(0),cost_computation_(-1),centroid_(0,0,0),vertices_(),neighbors_regions_(),expansion_(mesh_){
	}

	void clear(){
		vertices_.clear();
		neighbors_regions_.clear();
	}

	typename Vertices_container::const_iterator begin_vertices() const{
		return vertices_.cbegin();
	}

	typename Vertices_container::const_iterator end_vertices() const{
		return vertices_.cend();
	}

	Neighbor_regions_const_iterator begin_neighbors_regions() const{
		return neighbors_regions_.cbegin();
	}

	Neighbor_regions_const_iterator end_neighbors_regions() const{
		return neighbors_regions_.cend();
	}

	int num_vertices() const{
		return vertices_.size();
	}

	int num_neighbor_regions() const{
		return neighbors_regions_.size();
	}

	double area() const{
		return area_;
	}

	Point_3 centroid() const{
		return centroid_;
	}


	/**
	 * return the l2 cost of the projection to the
	 * expansion
	 */
	double cost() const{
		return expansion_.cost();
	}


	/**
	 * return the linf cost of the projection to the
	 * expansion (ie the max distance)
	 */
	double inf_cost() const{
		return expansion_.inf_cost();
	}


	void assign_propagation(){
		expansion_.assign(*this);
	}

	void project_propagation(){
		expansion_.project(*this);
	}

private:
	/**
	 * this method needs to be called before other propagation methods
	 */
	void compute_propagation(){
		expansion_.compute_propagation(*this);
	}


	void add_vertex(Vertex_handle vertex_to_add){
		vertices_.push_back(vertex_to_add);
	}

	void add_neighbor_regions(Planar_proxy_handle region_to_add){
		neighbors_regions_.insert(region_to_add);
	}

	std::string to_string() const{
		std::ostringstream stream;

		stream <<"vertices: ";
		for(auto vi : vertices_){
			stream<<vi<<" ";
		}
		stream <<"neighbors regions: ";
		for(auto proxy : neighbors_regions_){
			stream<<proxy<<" ";
		}
		return stream.str();
	}
	//	friend std::ostream& operator<< (std::ostream& stream, const Smooth_proxy& plane_primitive) {
	//		stream << "[ "<<plane_primitive.plane_<<", area = "<<plane_primitive.area_<<" ]";
	//		return stream;
	//	}

};



#endif /* SMOOTH_PROXY_H_ */
