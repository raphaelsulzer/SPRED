/*
 * Contraction_parameter.h
 *
 *  Created on: Mar 5, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef CONTRACTION_PARAMETER_H_
#define CONTRACTION_PARAMETER_H_

#include <iostream>
#include <string>

#include "../urban_utils.h"

struct Contraction_parameter{

	double abstraction_weight;
	double boundary_weight;
	double bound_preserve_normals;
	double distance_max_to_primitive_plane; //deprecated
	double distance_max_to_primitive_border; //deprecated
	double distance_max_graph_edges;
	double svd_eigen_threshold;

	bool link_condition;
	bool constant_graph; //deprecated
	bool check_distance_primitive; //deprecated
	bool check_distance_primitive_border; //deprecated
	bool check_primitive_vertices;
	bool check_primitive_edges; //deprecated
	bool check_primitive_corner; //deprecated
	bool check_preserve_normals;
	bool verbose;

	double ratio_proxy;

	Contraction_parameter(){
		abstraction_weight = 0.8;
		boundary_weight = .8;
		bound_preserve_normals = 2.6;
		distance_max_to_primitive_plane = 1e10;
		distance_max_to_primitive_border = 1e10;
		distance_max_graph_edges = 1.5;
		svd_eigen_threshold = 1e-2;
		link_condition = false;
		constant_graph = true;
		check_distance_primitive = false;
		check_distance_primitive_border = false;
		check_primitive_vertices = true;
		check_primitive_edges = false;
		check_primitive_corner = true;
		check_preserve_normals = false;
		verbose = false;
		ratio_proxy = 0.5;
	}

	double proxy_weight() const{
		return (abstraction_weight)*(1-boundary_weight);
	}
	double proxy_border_weight() const{
		return (abstraction_weight)*(boundary_weight);
	}
	double data_weight() const{
		return (1-abstraction_weight)*(1-boundary_weight);
	}
	double data_border_weight() const{
		return (1-abstraction_weight)*(boundary_weight);
	}

	friend std::ostream& operator<<(std::ostream& stream, const Contraction_parameter& params){
		stream << "%Contraction parameters:"<<std::endl;
		stream 	<< "abstraction_weight: "<< params.abstraction_weight<<std::endl;
		stream 	<< "boundary_weight: "<< params.boundary_weight<<std::endl;
		stream 	<< "bound_preserve_normals: "<< params.bound_preserve_normals<<std::endl;
		stream 	<< "distance_max_to_primitive_plane: "<< params.distance_max_to_primitive_plane<<std::endl;
		stream 	<< "distance_max_to_primitive_border: "<< params.distance_max_to_primitive_border<<std::endl;
		stream 	<< "distance_max_graph_edges: "<< params.distance_max_graph_edges<<std::endl;
		stream 	<< "svd_eigen_threshold: "<< params.svd_eigen_threshold<<std::endl;
		stream 	<< "link_condition: "<< params.link_condition<<std::endl;
		stream 	<< "constant_graph: "<< params.constant_graph<<std::endl;
		stream 	<< "check_distance_primitive: "<< params.check_distance_primitive<<std::endl;
		stream 	<< "check_distance_primitive_border: "<< params.check_distance_primitive_border<<std::endl;
		stream 	<< "check_primitive_vertices: "<< params.check_primitive_vertices<<std::endl;
		stream 	<< "check_primitive_edges: "<< params.check_primitive_edges<<std::endl;
		stream 	<< "check_primitive_corner: "<< params.check_primitive_corner<<std::endl;
		stream 	<< "check_preserve_normals: "<< params.check_preserve_normals<<std::endl;
		stream 	<< "verbose: "<< params.verbose<<std::endl;
		return stream;
	}

	friend std::istream& operator>>(std::istream& stream, Contraction_parameter& params){
		read_param(params.abstraction_weight,stream);
		read_param(params.boundary_weight,stream);
		read_param(params.bound_preserve_normals,stream);
		read_param(params.distance_max_to_primitive_plane,stream);
		read_param(params.distance_max_to_primitive_border,stream);
		read_param(params.distance_max_graph_edges,stream);
		read_param(params.svd_eigen_threshold,stream);
		read_param(params.link_condition,stream);
		read_param(params.constant_graph,stream);
		read_param(params.check_distance_primitive,stream);
		read_param(params.check_distance_primitive_border,stream);
		read_param(params.check_primitive_vertices,stream);
		read_param(params.check_primitive_edges,stream);
		read_param(params.check_primitive_corner,stream);
		read_param(params.check_preserve_normals,stream);
		read_param(params.verbose,stream);
		return stream;
	}
};

#endif /* CONTRACTION_PARAMETER_H_ */
