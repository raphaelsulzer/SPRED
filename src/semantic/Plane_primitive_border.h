/*
 * Plane_primitive_border.h
 *
 *  Created on: Mar 14, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef PLANE_PRIMITIVE_BORDER_H_
#define PLANE_PRIMITIVE_BORDER_H_


#include "urban_utils.h"

#include <list>
#include <set>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/convex_hull_2.h>
#include "Plane_primitive_border.h"
#include "complex/complex_tools.h"
#include <CGAL/minkowski_sum_2.h>
#include "typedefs.h"



template<typename GudhiKernel>
class Plane_primitive_border{
private :

	typedef CGAL::Polygon_2<GudhiKernel> Polygon_2;
	typedef CGAL::Polygon_with_holes_2<GudhiKernel> Polygon_with_holes_2;
	typedef typename GudhiKernel::Point_2 Point_2;
	typedef typename GudhiKernel::Point Point;

	/**
	 * primitive whose we are constructing the border
	 */
	Planar_proxy* primitive_;

	/*
	 * vertices of a simple polygon that is the border of the primitive
	 */
	std::list<Vertex_handle> border_polygon_vertices_;
	//xxx faire des points3
	std::list<Point_2> border_polygon_points_;


	/**
	 * border edges of the restricted triangulation to the primitive
	 */
	std::list<Edge_handle> border_edges_;

	std::list< std::pair<Edge_handle,Vertex_handle> > border_triangles_;


	std::list<Point> border_edges_points_;

	Polygon_2 border_offset_;



public:
	enum polygon_extracted_type{ CONVEX_HULL, BORDER_EDGES, ALPHA_SHAPE};

private:
	polygon_extracted_type polygon_type_;
public:

	/**
	 * @brief Extracts a simple polygon which approximates the "border" of the primitive and offsets
	 * this polygon by epsilon.
	 * @detail It tries one strategy which builds a close approximation but not always works.
	 * If the strategy fails, the polygon is the border of the convex hull of projected
	 * points onto the primitive.
	 *
	 * The polygon is returned with the following list : [p0, p1, p2, ..., p0]
	 * (i.e. the first element of the list is also present at the end).
	 */
	Plane_primitive_border(const Complex& complex, Planar_proxy* primitive, double epsilon = 0.1):
		primitive_(primitive){
		assert(epsilon>0);

		Simplex_handle vertices_simplex;
		for(auto vi = primitive->begin_initial_vertices() ; vi != primitive->end_initial_vertices() ; ++vi )
			vertices_simplex.add_vertex(*vi);

		//assume that the primitive has AT LEAST three vertices.
		assert(vertices_simplex.dimension()>=2);

		Skeleton_blocker_sub_complex<Complex> complex_restr_to_primitive;
		complex_restr_to_primitive.make_restricted_complex(complex,vertices_simplex);

		//segfault in this loop
		for (auto edge_in_sub_complex : complex_restr_to_primitive.edge_range()){
			// xxx sous sous complexe il y aura ptetre un bug ici
			auto opp_vertex = is_border_edge<Complex>(complex_restr_to_primitive,edge_in_sub_complex);
			if(opp_vertex){
				auto v1_sub = complex_restr_to_primitive.first_vertex(edge_in_sub_complex);
				auto v2_sub = complex_restr_to_primitive.second_vertex(edge_in_sub_complex);

				auto v1( complex.get_address(complex_restr_to_primitive.get_id(v1_sub)));
				auto v2( complex.get_address(complex_restr_to_primitive.get_id(v2_sub)));

				auto edge_in_complex(complex[std::make_pair(*v1,*v2) ]);
				assert(edge_in_complex);

				border_edges_.push_back(*edge_in_complex);

				border_triangles_.push_back(std::make_pair(*edge_in_complex,*opp_vertex));
			}
		}

		// insert the points in border_edges_points
		std::set<Vertex_handle> is_already_inserted;
		for (auto edge : border_edges_){
			auto v1( complex.first_vertex(edge));
			auto v2( complex.second_vertex(edge));
			if ( is_already_inserted.find(v1) == is_already_inserted.end() ){
				is_already_inserted.insert(v1);
				border_edges_points_.push_back( complex.point(v1) );
			}
			if ( is_already_inserted.find(v2) == is_already_inserted.end() ){
				is_already_inserted.insert(v2);
				border_edges_points_.push_back( complex.point(v2) );
			}
		}

		polygon_type_ = BORDER_EDGES;
//
//		bool extraction_polygon_success =
//				extract_polygon_border(complex,complex_restr_to_primitive,primitive,
//						this->border_polygon_vertices_,this->border_polygon_points_	);
//
//		//xxx remove-> allow to always extract the convex hull -> less buggy for the moment
////		extraction_polygon_success = false;
//
//		if(!extraction_polygon_success || !CGAL::is_simple_2(border_polygon_points_.begin(),--(border_polygon_points_.end()),Kernel())){
//			// if not we extract the convex hull of the primitive points to be sure to have
//			// a simplex polygon
//			extract_polygon_convex_hull_border(complex,complex_restr_to_primitive,primitive,this->border_polygon_vertices_,this->border_polygon_points_);
//		}
//
//		extraction_polygon_success? polygon_type_ = BORDER_EDGES :  polygon_type_ = CONVEX_HULL;
//
////		offset_polygon(epsilon);
	}


	polygon_extracted_type polygon_type(){
		return polygon_type_;
	}


	/**
	 * Returns true if the point lies in the interior of the polygon.
	 */
	bool is_in_border_polygon(const Point &point) const{
		auto location =
				CGAL::bounded_side_2(
						border_offset_.container().begin(),border_offset_.container().end(),
						primitive_->project(point),Kernel()
				);
		return (location == CGAL::ON_BOUNDED_SIDE || location ==CGAL::ON_BOUNDARY);
	}


	//xxx to remove
	const std::list<Vertex_handle>& border_polygon_vertices(){
		return this->border_polygon_vertices_;
	}

	std::list<Vertex_handle>::const_iterator begin_polygon_vertices() const{
		return border_polygon_vertices_.cbegin();
	}
	std::list<Vertex_handle>::const_iterator end_polygon_vertices() const{
		return border_polygon_vertices_.cend();
	}

	int num_polygon_vertices() const{
		return border_polygon_vertices_.size();
	}



	typename std::list<Point_2>::const_iterator begin_polygon_points() const{
		return border_polygon_points_.cbegin();
	}
	typename std::list<Point_2>::const_iterator end_polygon_points() const{
		return border_polygon_points_.cend();
	}


	std::list<Edge_handle>::const_iterator begin_border_edges() const{
		return border_edges_.begin();
	}

	std::list<Edge_handle>::const_iterator end_border_edges() const{
		return border_edges_.end();
	}

	typename std::list<Point>::const_iterator begin_border_edges_points() const{
		return border_edges_points_.begin();
	}

	typename std::list<Point>::const_iterator end_border_edges_points() const{
		return border_edges_points_.end();
	}

	int num_border_edges() const{
		return border_edges_.size();
	}



private:
//	friend typename std::list<Point_2> project_on_primitive(
//			const Complex& complex, Planar_proxy* primitive,std::list<Vertex_handle> vertices
//	);

	/**
	 * extract a polygon from the border of 'complex_restr_to_primtive' included in 'complex'.
	 * returns true if it manages to extract a polygon. In this case, the result is stored in
	 * the two lists border_vertices_res and border_points_res
	 */
	bool extract_polygon_border(
			const Complex& full_complex, const Complex& complex_restr_to_primitive,
			Planar_proxy* primitive,
			std::list<Vertex_handle>&border_polygon_vertices_res,
			std::list<Point_2>& border_polygon_points_res
	){
		border_polygon_vertices_res.clear();
		border_polygon_points_res.clear();

		std::list<Root_vertex_handle> border_root_vertices;

		bool success = extract_border<Complex>(complex_restr_to_primitive,border_root_vertices);
		if (success){

			for (auto vi : border_root_vertices){
				border_polygon_vertices_res.push_back(full_complex[vi]);
			}
			for (auto vi : border_polygon_vertices_res){
				border_polygon_points_res.push_back(primitive->project(full_complex.point(vi)));
			}

		}

		return success;
	}
	//	bool extract_polygon_border(
	//			const Complex& complex,const Complex& complex_restr_to_primtive,
	//			Planar_proxy* primitive,std::list<Vertex_handle>&border_vertices_res,std::list<Point_2>& border_points_res
	//	);


	class Point_2_with_id : public Point_2{
	public:
		Vertex_handle vertex_handle;
		Point_2_with_id():Point_2(){}
		Point_2_with_id(double x,double y,Vertex_handle vi):Point_2(x,y),vertex_handle(vi){}
		Point_2_with_id(const Point_2& pt,Vertex_handle vi):Point_2(pt),vertex_handle(vi){}
	};

	struct Kernel_with_id : public Kernel{
		typedef Point_2_with_id Point_2;

	};

	void extract_polygon_convex_hull_border(
					const Complex& complex,const Complex& complex_restr_to_primitive,
					Planar_proxy* primitive,
					std::list<Vertex_handle>&border_polygon_vertices_res,
					std::list<Point_2>& border_polygon_points_res
	){
		border_polygon_vertices_res.clear();
		border_polygon_points_res.clear();

		std::vector<Point_2_with_id> points_with_vh;
		for(auto vi : complex_restr_to_primitive.vertex_range()){
			Root_vertex_handle root_vi( complex_restr_to_primitive.get_id(vi) );
			Point_2 proj_pt(  primitive->project(complex.point(root_vi)) );
			Point_2_with_id p2( proj_pt , complex[root_vi] );
			points_with_vh.push_back( Point_2_with_id( proj_pt , complex[root_vi] ));
		}

		std::vector<Point_2_with_id> border_hull_points;
		CGAL::convex_hull_2( points_with_vh.begin(), points_with_vh.end(), std::back_inserter(border_hull_points),
				Kernel_with_id());

		for (auto x : border_hull_points){
			border_polygon_vertices_res.push_back(x.vertex_handle);
			border_polygon_points_res.push_back(x);
		}
	}

	void offset_polygon(double epsilon){
		Polygon_2 epsilon_ball;
		epsilon_ball.push_back (Point_2 (-epsilon/2,-epsilon/2));
		epsilon_ball.push_back (Point_2 (-epsilon/2, epsilon/2));
		epsilon_ball.push_back (Point_2 (epsilon/2, epsilon/2));
		epsilon_ball.push_back (Point_2 (epsilon/2, -epsilon/2));

		Polygon_2 border;

		for(auto x:border_polygon_points_){
			border.push_back(x);
		}
	}

};



#endif /* PLANE_PRIMITIVE_BORDER_H_ */
