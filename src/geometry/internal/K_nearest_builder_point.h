/*
 * K_nearest_builder_point.h
 *
 *  Created on: Sep 18, 2014
 *      Author: dsalinas
 */

#ifndef K_NEAREST_BUILDER_POINT_H_
#define K_NEAREST_BUILDER_POINT_H_

#include "typedefs.h"
struct Point_with_id {
	double vec[3];
	Complex::Vertex_handle vertex_handle;
	Point_with_id() {
		vec[0]= vec[1] = vec[2] = 0; }
	Point_with_id (double x, double y, double z) {
		vec[0]=x; vec[1]=y; vec[2]=z; }
	Point_with_id (double x, double y, double z,Vertex_handle v):vertex_handle(v){
		vec[0]=x; vec[1]=y; vec[2]=z; }
	Point_with_id(Gudhi_kernel::Point_3 p){
		vec[0]=p.x(); vec[1]=p.y(); vec[2]=p.z();

	}
	Point_with_id (Gudhi_kernel::Point_3 p,Vertex_handle v):vertex_handle(v){
		vec[0]=p.x(); vec[1]=p.y(); vec[2]=p.z();
	}

	double x() const { return vec[ 0 ]; }
	double y() const { return vec[ 1 ]; }
	double z() const { return vec[ 2 ]; }
	double& x() { return vec[ 0 ]; }
	double& y() { return vec[ 1 ]; }
	double& z() { return vec[ 2 ]; }
	bool operator==(const Point_with_id& p) const
					{
		return (x() == p.x()) && (y() == p.y()) && (z() == p.z()) ;
					}
	bool operator!=(const Point_with_id& p) const { return ! (*this == p); }
}; //end of class
namespace CGAL {
template <>
struct Kernel_traits<Point_with_id> {
	struct Kernel {
		typedef double FT;
		typedef double RT;
	};
};
}
struct Construct_coord_iterator {
	typedef const double* result_type;
	const double* operator()(const Point_with_id& p) const
	{ return static_cast<const double*>(p.vec); }
	const double* operator()(const Point_with_id& p, int) const
	{ return static_cast<const double*>(p.vec+3); }
};


struct Distance {
	typedef Point_with_id Query_item;
	typedef double FT;
	double transformed_distance(const Point_with_id& p1, const Point_with_id& p2) const {
		double distx= p1.x()-p2.x();
		double disty= p1.y()-p2.y();
		double distz= p1.z()-p2.z();
		return distx*distx+disty*disty+distz*distz;
	}
	template <class TreeTraits>
	double min_distance_to_rectangle(const Point_with_id& p,
			const CGAL::Kd_tree_rectangle<TreeTraits>& b) const {
		double distance(0.0), h = p.x();
		if (h < b.min_coord(0)) distance += (b.min_coord(0)-h)*(b.min_coord(0)-h);
		if (h > b.max_coord(0)) distance += (h-b.max_coord(0))*(h-b.max_coord(0));
		h=p.y();
		if (h < b.min_coord(1)) distance += (b.min_coord(1)-h)*(b.min_coord(1)-h);
		if (h > b.max_coord(1)) distance += (h-b.max_coord(1))*(h-b.min_coord(1));
		h=p.z();
		if (h < b.min_coord(2)) distance += (b.min_coord(2)-h)*(b.min_coord(2)-h);
		if (h > b.max_coord(2)) distance += (h-b.max_coord(2))*(h-b.max_coord(2));
		return distance;
	}
	template <class TreeTraits>
	double max_distance_to_rectangle(const Point_with_id& p,
			const CGAL::Kd_tree_rectangle<TreeTraits>& b) const {
		double h = p.x();
		double d0 = (h >= (b.min_coord(0)+b.max_coord(0))/2.0) ?
				(h-b.min_coord(0))*(h-b.min_coord(0)) : (b.max_coord(0)-h)*(b.max_coord(0)-h);
		h=p.y();
		double d1 = (h >= (b.min_coord(1)+b.max_coord(1))/2.0) ?
				(h-b.min_coord(1))*(h-b.min_coord(1)) : (b.max_coord(1)-h)*(b.max_coord(1)-h);
		h=p.z();
		double d2 = (h >= (b.min_coord(2)+b.max_coord(2))/2.0) ?
				(h-b.min_coord(2))*(h-b.min_coord(2)) : (b.max_coord(2)-h)*(b.max_coord(2)-h);
		return d0 + d1 + d2;
	}
	double new_distance(double& dist, double old_off, double new_off,
			int /* cutting_dimension */) const {
		return dist + new_off*new_off - old_off*old_off;
	}
	double transformed_distance(double d) const { return d*d; }
	double inverse_of_transformed_distance(double d) { return std::sqrt(d); }
}; // end of struct Distance

#endif /* K_NEAREST_BUILDER_POINT_H_ */
