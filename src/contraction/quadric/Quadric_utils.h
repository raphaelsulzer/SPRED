/*
 * Quadric_utilities.h
 *
 *  Created on: Mar 19, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef QUADRIC_UTILITIES_H_
#define QUADRIC_UTILITIES_H_


#include <algorithm>


#include "semantic/Plane_primitive_border.h"
#include "urban_utils.h"

#include "Error_quadric.h"
#include "interface/Contraction_parameter.h"
#include "semantic/Plane_primitive_border.h"
//#include <CGAL/Segment_3.h>


using namespace Gudhi;
using namespace Gudhi::contraction;
using namespace skbl;


/**
 * @brief compute the quadrics according to Garland Heckbert algorithm
 */
template<typename Complex>
void compute_quadrics_garland_heckbert(Complex& mesh,double weight){
	// For every vertex, we loop through its adjacent triangles.
	// For each triangle, we add the quadric corresponding to the plane passing through the triangle.
	for(auto vi : mesh.vertex_range()){
		mesh[vi].sum_quadric_data(compute_quadrics_data_vertex(mesh,vi,weight));
	}
}



template<typename Point>
Point get_noisy_point(Point & point,double amplitude){
	double nx = point.x() + (rand() % 2 - .5) * amplitude, ny =
			point.y() + (rand() % 2 - .5) * amplitude, nz = point.z()
			+ (rand() % 2 - .5) * amplitude;
	return Point(nx, ny, nz);
}



/**
 * Compute the quadric of data for the edge profile
 * and the area of the star.
 * DO NOT COMPUTE QUADRICS BORDER
 *
 */
template<typename Complex>
boost::optional<quadric::Error_quadric<typename Complex::GT> > compute_all_quadrics_of_vertex(
		Complex& mesh,
		typename Complex::Vertex_handle a,
		const Contraction_parameter& params) {

	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::GT GT;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef quadric::Error_quadric<GT> Error_quadric;

	boost::optional<Error_quadric> quadric = Error_quadric();

	int num_triangles_passing_through_a = 0;


	Skeleton_blocker_link_complex<Complex> link_a(mesh,a);


	for(const auto& t : mesh.triangle_range(a)){
		auto it = t.begin();
		Vertex_handle v1 = *(it++);
		Vertex_handle v2 = *(it++);
		Vertex_handle v3 = *(it);
		Triangle_3 triangle(mesh.point(v1),mesh.point(v2), mesh.point(v3));

		if (!triangle.is_degenerate()) {
			++num_triangles_passing_through_a;
			// the quadric is multiplied by the triangle area and the weight
			(*quadric) += Error_quadric(triangle.supporting_plane(),std::sqrt(triangle.squared_area()) * params.data_weight());

			//add proxy quadrics
			for(auto proxy : mesh[a].plane_primitives()){
				// if all vertices of the ti contains the proxy then
				// we add the corresponding quadric
				if(mesh[v1].contains_plane_primitive(proxy)&&mesh[v2].contains_plane_primitive(proxy)&&mesh[v3].contains_plane_primitive(proxy)){
					(*quadric) +=
							Error_quadric(
									proxy->plane(),
									std::sqrt(triangle.squared_area()) * params.proxy_weight()
							);
				}
			}
		}
	}

	if (num_triangles_passing_through_a == 0){
		UDBG("no triangles through the edge!");
		quadric.reset();
	}
	return quadric;
}




/**
 * Compute the quadric of data for the edge profile
 * and the area of the star.
 *
 ** DO NOT COMPUTE QUADRICS BORDER
 */
template<typename Complex>
boost::optional<quadric::Error_quadric<typename Complex::GT> > compute_all_quadrics_of_edge(
		Complex& mesh,
		typename Complex::Vertex_handle a,
		typename Complex::Vertex_handle b,
		const set<typename Complex::Graph_vertex::Planar_proxy_handle>& proxies_v0v1_union_,
		const Contraction_parameter& params
) {

	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::GT GT;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef quadric::Error_quadric<GT> Error_quadric;

	boost::optional<Error_quadric> quadric = Error_quadric();



	std::list<typename Complex::Simplex_handle> triangles_ab =
			star_triangles(mesh,a,b);

	if(triangles_ab.empty()) {
		quadric.reset();
		return quadric;
	}


	for(const auto& t_ab : triangles_ab){
		auto it = t_ab.begin();
		Vertex_handle v1 = *(it++);
		Vertex_handle v2 = *(it++);
		Vertex_handle v3 = *(it);

		Triangle_3 triangle(mesh.point(v1),mesh.point(v2), mesh.point(v3));
		if (!triangle.is_degenerate()) {
			bool triangle_included_in_a_proxy = false;

			//add proxy quadrics
			for(auto proxy : proxies_v0v1_union_){
				// if all vertices of the ti contains the proxy then
				// we add the corresponding quadric
				if(mesh[v1].contains_plane_primitive(proxy)&&mesh[v2].contains_plane_primitive(proxy)&&mesh[v3].contains_plane_primitive(proxy)){
					(*quadric) +=
							Error_quadric(
									proxy->plane(),
									std::sqrt(triangle.squared_area()) * params.proxy_weight()
							);
					triangle_included_in_a_proxy = true;
				}
			}
			//			triangle_included_in_a_proxy = true;
			if(!triangle_included_in_a_proxy){
				// the quadric is multiplied by the triangle area and the weight
				(*quadric) += Error_quadric(
						triangle.supporting_plane(),
						std::sqrt(triangle.squared_area()) * (1-params.boundary_weight)
				);
			}
			else{
				(*quadric) += Error_quadric(
						triangle.supporting_plane(),
						std::sqrt(triangle.squared_area()) * params.data_weight()
				);
			}

		}
	}
	return quadric;
}



/**
 * @brief compute the quadrics of Garland Heckbert around a vertex
 */
template<typename Complex>
quadric::Error_quadric<typename Complex::GT>
compute_quadrics_data_vertex(Complex& mesh,typename Complex::Vertex_handle vi, double weight){
	typedef typename Complex::GT GT;
	//	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	Error_quadric_ res;

	for (const auto& ti : mesh.triangle_range(vi)){
		auto vertex_ti = ti.begin();
		auto v1 = *vertex_ti;
		auto v2 = *(++vertex_ti);
		auto v3 = *(++vertex_ti);

		Triangle_3 triangle(mesh.point(v1),mesh.point(v2),mesh.point(v3));

		// the quadric is multiplied by the triangle area and the weight
		res += Error_quadric_(triangle.supporting_plane(),weight*std::sqrt(triangle.squared_area()));
	}
	return res;
}







/**
 * @brief compute the quadrics of Garland Heckbert for the triangles
 * that pass through an edge
 */
template<typename Complex>
quadric::Error_quadric<typename Complex::GT>
compute_quadrics_data_edge_intersection(Complex& mesh,typename Complex::Edge_handle edge, double weight=1){
	typedef typename Complex::GT GT;
	//	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	Error_quadric_ res;

	auto v0(mesh.first_vertex(edge));
	auto v1(mesh.second_vertex(edge));
	Skeleton_blocker_link_complex<Complex> link_v0v1(mesh,edge);
	for (auto v : link_v0v1.vertex_range()){
		auto v_complex(mesh.get_address(link_v0v1.get_id(v)));
		Triangle_3 triangle(
				mesh.point(v0),
				mesh.point(v1),
				mesh.point(v)
		);
		res += Error_quadric_(triangle.supporting_plane(),weight*std::sqrt(triangle.squared_area()));
	}
	return res;
}


/**
 * @brief compute the quadrics corresponding to primitives.
 * For every point, if a primitive is attached to this point, then a
 * quadric corresponding to this primitive is added to the point.
 */
template<typename Complex>
void compute_quadrics_proxy_plane(Complex& mesh,double generalization_weight){
	typedef typename Complex::GT GT;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	// For every vertex, we loop through its adjacent triangles.
	// For each triangle, we add the quadric corresponding to the plane passing through the triangle.
	for(auto vi : mesh.vertex_range()){
		for(auto primitive : mesh[vi].plane_primitives()){
			// we multiply every proxy by its weigth times the area of the primitive divided by the number
			// of points (ie the number of triangles)
			double ratio = generalization_weight * primitive->area()/(double)primitive->num_initial_vertices();
			//			double ratio = generalization_weight;
			mesh[vi].sum_quadric_proxy_plane(Error_quadric_(primitive->plane(),ratio));
		}
	}

}


/**
 * Given points v0,v1,v2 defining a non degenerate triangle,
 * it returns the quadric corresponding to the plane orthogonal
 * to the triangle that passes through v0v1 (weighted by area).
 */
template<typename GT>
quadric::Error_quadric<GT>
compute_quadric_orthogonal_to_triangle(
		const typename GT::Point_3 & v0,const typename GT::Point_3 & v1,const typename GT::Point_3 & v2
){
	typedef typename GT::Triangle_3 Triangle_3;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	Triangle_3 v0v1v2(v0,v1,v2);
	assert(!v0v1v2.is_degenerate());

	auto point_of_plane(v0+v0v1v2.supporting_plane().orthogonal_vector());

	// check if the plane is orthogonal to the triangle
	assert(std::abs(
			Error_quadric_(Plane_3(v0,v1,point_of_plane),1).cost(v2)
			-squared_distance(v2,Plane_3(v0,v1,point_of_plane)))<1e-5
	);

	return Error_quadric_(Plane_3(v0,v1,point_of_plane));
	//	return Error_quadric_(Plane_3(v0,v1,point_of_plane),weight*std::sqrt(v0v1v2.squared_area()));
}


/**
 * @brief init the quadrics for all edges that are in the border
 * For every border edge, we add a quadric corresponding to the plane that passes
 * through the edge and is orthogonal to the unique facet passing through
 * the edge. This quadric is weighed by 'boundary_weight'.
 */
template<typename Complex>
void compute_quadrics_border_mesh(Complex& mesh, double boundary_weight){
	typedef typename Complex::GT GT;

	typedef quadric::Error_quadric<GT> Error_quadric_;
	typedef typename Complex::Vertex_handle Vertex_handle;
	for(auto ei : mesh.edge_range()){
		//ie if the edge is a "border" edge
		boost::optional<Vertex_handle> vertex_triangle = is_border_edge(mesh,ei);
		if(vertex_triangle){
			auto v0(mesh.first_vertex(ei));
			auto v1(mesh.second_vertex(ei));

			Error_quadric_ quadric(
					compute_quadric_orthogonal_to_triangle<GT>(
							mesh.point(v0),
							mesh.point(v1),
							mesh.point(*vertex_triangle)
					)
			);
			double triangle_area = std::sqrt(
					CGAL::squared_area(
							mesh.point(v0),
							mesh.point(v1),
							mesh.point(*vertex_triangle)));
			quadric *= boundary_weight * triangle_area;
			mesh[v0].sum_quadric_data(quadric);
			mesh[v1].sum_quadric_data(quadric);
		}
	}
}



/**
 * @brief For every proxy, it goes for all its border edges.
 * For each of them, it computes a quadric corresponding to the line of the edge
 * and adds it to the two vertices of the edge.
 */
template<typename Complex,typename Segmentation>
void compute_quadrics_proxy_border_with_border_edges(
		Complex& mesh,
		Segmentation& segmentation,
		double proxy_border_weight=1){
	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Point_3 Point_3;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	for ( auto p : segmentation){

		typedef typename Complex::GT Gudhi_kernel;

		Plane_primitive_border<Gudhi_kernel> proxy_border(mesh,&p);

		for(auto edge = proxy_border.begin_border_edges();  edge!= proxy_border.end_border_edges(); ++edge){

			Vertex_handle v0(mesh.first_vertex(*edge));
			Vertex_handle v1(mesh.second_vertex(*edge));
			const Plane_3& proxy_plane = p.plane();
			Point_3 point_of_orthogonal_plane(mesh.point(v0)+proxy_plane.orthogonal_vector());
			Plane_3 orthogonal_plane_through_edge(mesh.point(v0),mesh.point(v1),point_of_orthogonal_plane);

			// the quadric is weight by the length of the edge divided by two
			// (to be scaled invariant) and by the weight
			double ratio = (squared_distance(mesh.point(v0),mesh.point(v1)))*.5 * proxy_border_weight;
			Error_quadric_ quadric(orthogonal_plane_through_edge, ratio );

			mesh[v0].sum_quadric_proxy_border(quadric);
			mesh[v1].sum_quadric_proxy_border(quadric);
		}
	}
}




/**
 * @brief adds a quadric to v0 and v1 that is obtained with the line passing through the
 * two points and weighted by 'weight' times half the squared distance between v0 and v1
 */
template<typename Complex>
void register_proxy_border(Complex& mesh,
		typename Complex::Vertex_handle v0,
		typename Complex::Vertex_handle v1,
		double weight
){
	typedef typename Complex::GT GT;
	typedef typename GT::Line_3 Line_3;
	//	typedef typename GT::Point Point;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	Line_3 line(mesh.point(v0),mesh.point(v1));

	// the quadric is weight by the length of the edge divided by two
	// (to be scaled invariant) and by the weight
	double ratio = std::sqrt(squared_distance(mesh.point(v0),mesh.point(v1)))*.5 * weight;
	Error_quadric_ quadric(line, ratio );

	// check the quadric by asserting that the distance from the origin to the line
	// is the same whether we compute it by quadric or by CGAL
	assert( std::abs(squared_distance(typename GT::Point(CGAL::ORIGIN),line) * ratio - quadric.cost(typename GT::Point(CGAL::ORIGIN)))/quadric.cost(typename GT::Point(CGAL::ORIGIN)) < 1e-4);
	mesh[v0].sum_quadric_proxy_border(quadric);
	mesh[v1].sum_quadric_proxy_border(quadric);

}


/**
 * @brief For every proxy, it goes for all its polygon border edges.
 * For each of them, it computes a quadric corresponding to the line of the edge
 * and adds it to the two vertices of the edge.
 */
template<typename Complex,typename Segmentation>
void compute_quadrics_proxy_border_with_polygon_edges(
		Complex& mesh,
		Segmentation& segmentation,
		double proxy_border_weight=1){

	for ( auto p : segmentation){

		auto proxy_border_pt(segmentation.get_border_from_proxy(&p));

		if(proxy_border_pt->polygon_type() == Plane_primitive_border<Gudhi_kernel>::BORDER_EDGES){
			auto first_vertex(* proxy_border_pt->begin_polygon_vertices());
			auto last_vertex(((* (--proxy_border_pt->end_polygon_vertices()))));
			register_proxy_border(mesh,first_vertex,last_vertex,proxy_border_weight);

			auto vi = proxy_border_pt->begin_polygon_vertices();
			auto next_vi = vi; ++next_vi;

			do {
				register_proxy_border(mesh,*vi,*next_vi,proxy_border_weight);
				++vi;
				++next_vi;
			} while (next_vi!=proxy_border_pt->end_polygon_vertices());
		}
	}


}






/**
 * For every pair of non parralel primitives (p,q),
 * it adds two quadrics corresponding to the two planes
 * passing trough p \cap q and perpendicular to
 * respectively np and nq, the normal of p and q.
 */
template< typename EdgeProfile>
quadric::Error_quadric<typename EdgeProfile::GT>  compute_orth_primitives_to_quadric(
		const EdgeProfile & profile,
		const Contraction_parameter& params
){
	typedef quadric::Error_quadric<typename EdgeProfile::GT> Error_quadric_;
	typedef typename EdgeProfile::GT::Vector_3 Vector_3;
	typedef typename EdgeProfile::GT::Plane_3 Plane_3;

	auto primitives =  profile.v0().primitives();
	primitives.insert(profile.v1().primitives().begin(),profile.v1().primitives().end());

	Error_quadric_ res;

	for(auto p = primitives.begin(); p!=primitives.end(); ++p){
		for (auto q=p; ++q != primitives.end();){
			const double parralel_tolerance=0.15;

			// two planes (p,q) are colinear if the scalar product of their normals
			// verifies  |(p|q)|>1-espilon
			bool are_colinear = std::abs(((*p)->plane().orthogonal_vector()) * ((*q)->plane().orthogonal_vector()))>1-parralel_tolerance;
			if(!are_colinear){
				// we add two quadrics corresponding to the two planes
				// passing trough p \cap q and perpendicular to
				// respectively np and nq, the normal of p and q.
				Line_3 line;
				CGAL::Object ob_temp=intersection((*p)->plane(),(*q)->plane());
				if(!assign(line, ob_temp)) {
					std::cerr<<"Error. plane1 / / plane2";
					assert(false);
				}
				else{
					auto pt_p_q ( line.point() );
					auto line_vect (line.to_vector());
					Vector_3 normal_p_orth  =  CGAL::cross_product((*p)->plane().orthogonal_vector() , line_vect ) ;
					Vector_3 normal_q_orth  =  CGAL::cross_product((*p)->plane().orthogonal_vector() , line_vect ) ;
					Plane_3 plane_p_orth(line.point(), normal_p_orth);
					Plane_3 plane_q_orth(line.point(), normal_q_orth);
					res+=Error_quadric_(plane_p_orth,params.abstraction_weight);
					res+=Error_quadric_(plane_q_orth,params.abstraction_weight);
				}
			}
		}
	}
	return res;
}




#endif /* QUADRIC_UTILITIES_H_ */
