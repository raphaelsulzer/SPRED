/*
 * Plane_primitive.h
 *
 *  Created on: 21 janv. 2014
 *      Author: dsalinas
 */

#ifndef PLANE_PRIMITIVE_H_
#define PLANE_PRIMITIVE_H_

#include <set>
#include <CGAL/linear_least_squares_fitting_3.h>
#include <list>
#include <string>
#include <sstream>

#include "Primitive.h"
#include "urban_utils.h"
#include "typedefs.h"


template <typename GT_, typename VertexHandle>
class Plane_primitive : public Primitive<GT_,VertexHandle>{
	template<class ComplexType> friend class Enriched_mesh;

public:
	typedef GT_ GT;
private:
	typedef typename GT::Point_2 Point_2;
	typedef typename GT::Point_3 Point_3;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Vector_3 Vector_3;
	typedef VertexHandle Vertex_handle;
	Plane_3 plane_;

public:

	Plane_primitive()://Primitive<GT_,VertexHandle>(),
		plane_(0,0,0,0)
{
}

	Plane_primitive(const Plane_3 & plane, CGAL::Color color):
		Primitive<GT_,VertexHandle>(color),plane_(plane)
		{
		}

	Plane_primitive(
			const Plane_3 & plane,
			const std::list<Vertex_handle>& vertices,
			const std::vector<Point_3> vertices_coord
	):
		Primitive<GT_,VertexHandle>(vertices,vertices_coord),
		plane_(plane)
		{
		}


	void recompute_proxy_parameters_after_merge(const std::list<Point_3>& points) override{
		assert(points.size()==this->initial_vertices_.size());
		// recompute proxy info
		this->centroid_ = CGAL::centroid(points.begin(), points.end());
		CGAL::linear_least_squares_fitting_3 ( points.begin(), points.end(),plane_,CGAL::Dimension_tag<0>());
	}

public:
	void set_plane(Plane_3& new_plane){
		plane_ = new_plane;
	}

	const Plane_3& plane() const{
		return plane_;
	}

	/**
	 * Given a 3d point, it returns a 2d-point whose
	 * coordinates corresponds to its projection on the coordinate system
	 * of the proxy
	 */
	Point_2 project(const Point_3& p) const override{
		return plane_.to_2d(p);
	}

	/**
	 * Given a 2d point whose coordinates are expressed with the proxy system,
	 * it returns the corresponding 3d point.
	 */
	Point_3 expand(const Point_2& p) const override{
		return plane_.to_3d(p);
	}


	bool operator==(const Plane_primitive & other) const{
		return this->plane_ == other.plane_;
	}


	double distance(const Point_3& p) const{
		Point_3 proj = expand(project(p));
		return std::sqrt(CGAL::squared_distance(p,proj));
	}


	//	static bool lexicographic_order(const Plane_3 & plane_1,const Plane_3& plane_2){
	//		double a1 = plane_1.a(),
	//				b1 = plane_1.b(),
	//				c1 = plane_1.c(),
	//				d1 = plane_1.d();
	//		double a2 = plane_2.a(),
	//				b2 = plane_2.b(),
	//				c2 = plane_2.c(),
	//				d2 = plane_2.d();
	//		return a1<a2 || (a1==a2 && (b1<b2)) || (a1==a2 && b1==b2 && (c1<c2)) || (a1==a2 && b1==b2 && c1==c2&& (d1<d2));
	//	}

	inline bool operator<(const Plane_primitive& other) const{
		//xxx faire mieux
		// For instance, if P and Q have respectively two lists of points lP and lQ
		// P==Q IF | c(lP\cup lQ,P) - c(lP\cup lQ,Q) | < epsilon
		// Where c(lP\cup lQ,P) is the sum of distance from points in lP\cup lQ to the plane P
		// Or test if there is a plane R such that c(lP \cup lQ, R ) < epsilon


		//		const double epsilon = 0.2;
		//		double abs_angle = std::abs(angle_between_two_planes(this->plane(), other.plane()));
		//		if (abs_angle < epsilon ) return false;
		//		return lexicographic_order(this->plane(),other.plane());

		double a1 = this->plane().a(),
				b1 = this->plane().b(),
				c1 = this->plane().c(),
				d1 = this->plane().d();
		//		double norm1 = std::sqrt(a1*a1+b1*b1+c1*c1+d1*d1);
		//		a1/=norm1;b1/=norm1;c1/=norm1;d1/=norm1;
		double a2 = other.plane().a(),
				b2 = other.plane().b(),
				c2 = other.plane().c(),
				d2 = other.plane().d();
		//		double norm2 = std::sqrt(a2*a2+b2*b2+c2*c2+d2*d2);
		//		a2/=norm2;b2/=norm2;c2/=norm2;d2/=norm2;
		double squared_dist = (a1-a2)*(a1-a2)+ (b1-b2)*(b1-b2) + (c1-c2)*(c1-c2) + (d1-d2)*(d1-d2);
		double epsilon = 0.001;
		if (squared_dist < epsilon ) return false;
		return lexicographic_order(this->plane(),other.plane());
	}


	std::string to_string() const{
		std::ostringstream stream;
		stream << "[" << this->plane_ << ", area = "<<this->area()<<" ]";
		return stream.str();
	}
};



#endif /* PLANE_PRIMITIVE_H_ */
