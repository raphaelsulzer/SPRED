/*
 * View_parameter.h
 *
 *  Created on: Mar 10, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef VIEW_PARAMETER_H_
#define VIEW_PARAMETER_H_

#include <iostream>

class View_parameter{
public:
	bool draw_primitives;
	bool draw_graph_primitives;
	bool draw_primitives_border;
	bool draw_primitives_corner;
	bool draw_best_edge;
	bool draw_new_point;
	bool light;
	bool relative_light;

	double size_vertices;
	double size_edges;
	double light_edges; // in 0-1

	double light_triangles;// in 0-1

	/**
	 * light angle
	 */
	double theta;
	double phi;

	enum VERTEX_MODE{ V_NONE,V_SIMPLE,V_PROXY_NUMBER,V_PROXY_COLOR,V_COUNT};
	enum EDGE_MODE{ E_NONE,E_SIMPLE,E_BORDER,E_COUNT};
	enum TRIANGLE_MODE{ T_NONE,T_SIMPLE,T_PROXY_COLOR,T_ONLY_PROXY,T_COUNT};
	enum CORNER_MODE{ C_NONE,C_SIMPLE,C_LINKED,C_DUAL,C_COUNT};




	VERTEX_MODE vertex_mode;
	EDGE_MODE edge_mode;
	TRIANGLE_MODE triangle_mode;
	CORNER_MODE corner_mode;

	void change_vertex_mode(){
		int current_value = vertex_mode;
		vertex_mode = static_cast<VERTEX_MODE>(++current_value % V_COUNT);
		std::cout<<"Vertex mode : ";
		switch (vertex_mode) {
		case V_NONE:
			std::cout<<"empty\n";
			break;
		case V_SIMPLE:
			std::cout<<"simple\n";
			break;
		case V_PROXY_NUMBER:
			std::cout<<"proxy card\n";
			break;
		case V_PROXY_COLOR:
			std::cout<<"proxy color\n";
			break;
		default:
			break;
		}
	}

	void change_vertex_mode(int new_mode){
		vertex_mode = static_cast<VERTEX_MODE>(new_mode % V_COUNT);
	}

	void change_edge_mode(){
		int current_value = edge_mode;
		edge_mode = static_cast<EDGE_MODE>(++current_value % E_COUNT);
	}

	void change_edge_mode(int new_mode){
		edge_mode = static_cast<EDGE_MODE>(new_mode % E_COUNT);
	}


	void change_triangle_mode(){
		int current_value = triangle_mode;
		triangle_mode = static_cast<TRIANGLE_MODE>(++current_value % T_COUNT);
	}



	void change_corner_mode(){
		int current_value = corner_mode;
		corner_mode = static_cast<CORNER_MODE>(++current_value % C_COUNT);
		std::cout<<"Corner mode : ";

		switch (corner_mode) {
		case C_NONE:
			std::cout<<"empty\n";
			break;
		case C_SIMPLE:
			std::cout<<"simple\n";
			break;
		case C_LINKED:
			std::cout<<"linked\n";
			break;
		case C_DUAL:
			std::cout<<"dual\n";
			break;
		default:
			break;
		}
	}


	View_parameter(){
		draw_primitives = false;
		draw_graph_primitives = false;
		draw_primitives_border = false;
		draw_primitives_corner = false;
		draw_best_edge=false;
		draw_new_point=false;
		light = true;
		relative_light = true;
		vertex_mode = V_PROXY_COLOR;
		edge_mode = E_NONE;
		triangle_mode = T_NONE;
		corner_mode = C_NONE;

		size_vertices = 3;
		size_edges = 2;

		light_edges = 0.3;
		light_triangles = 0.85;
		theta = 0;
		phi = 0;
	}

	friend std::ostream& operator<<(std::ostream& stream, const View_parameter& param){
		stream << param.draw_primitives<< " ";
		stream << param.draw_graph_primitives<< " ";
		stream << param.draw_primitives_border<< " ";
		stream << param.draw_primitives_corner<< " ";
		stream << param.draw_best_edge<< " ";
		stream << param.draw_new_point<< " ";
		stream << param.light<< " ";
		stream << param.relative_light<< " ";
		stream << param.vertex_mode<< " ";
		stream << param.edge_mode<< " ";
		stream << param.triangle_mode<< " ";
		stream << param.corner_mode<< " ";
		stream << param.size_vertices<< " ";
		stream << param.size_edges<< " ";
		stream << param.light_edges<< " ";
		stream << param.light_triangles<< " ";
		stream << param.theta<< " ";
		stream << param.phi<< " ";
		return stream;
	}

	friend std::istream& operator>>(std::istream& stream, View_parameter& param){
		stream >> param.draw_primitives;
		stream >> param.draw_graph_primitives;
		stream >> param.draw_primitives_border;
		stream >> param.draw_primitives_corner;
		stream >> param.draw_best_edge;
		stream >> param.draw_new_point;
		stream >> param.light;
		stream >> param.relative_light;
		int a;
		stream>>a;
		param.vertex_mode = static_cast<VERTEX_MODE>(a % V_COUNT);
		stream>>a;
		param.edge_mode = static_cast<EDGE_MODE>(a % E_COUNT);
		stream>>a;
		param.triangle_mode = static_cast<TRIANGLE_MODE>(a % T_COUNT);
		stream>>a;
		param.corner_mode = static_cast<CORNER_MODE>(a % C_COUNT);
		stream >> param.size_vertices;
		stream >> param.size_edges;
		stream >> param.light_edges;
		stream >> param.light_triangles;
		stream >> param.theta;
		stream >> param.phi;
		return stream;
	}

};

#endif /* VIEW_PARAMETER_H_ */
