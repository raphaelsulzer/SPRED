/*
 * Distance_point_to_triangle.h
 *
 *  Created on: Apr 23, 2014
 *      Author: dsalinas
 */

#ifndef DISTANCE_POINT_TO_TRIANGLE_H_
#define DISTANCE_POINT_TO_TRIANGLE_H_


#include <list>
#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>

template<typename Complex>
class Distance_point_to_triangle{

public:
	Distance_point_to_triangle(const std::list<typename Complex::Point> &initial_points):points_(initial_points),max_o_to_s_(-1),mean_o_to_s_(-1),rms_o_to_s_(-1){
	}


	void compute_distance(const Complex &simplified_mesh){
		typedef typename Complex::GT K;
		typedef typename K::Triangle_3 Triangle;
		typedef typename std::list<Triangle>::iterator Iterator;
		typedef typename CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
		typedef typename CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
		typedef typename CGAL::AABB_tree<AABB_triangle_traits> Tree;

		std::list<Triangle> triangles;
		for(const  auto& t : simplified_mesh.triangle_range()){
			auto simplex_it (t.begin());
			auto v0(*simplex_it);
			auto v1(*(++simplex_it));
			auto v2(*(++simplex_it));
			triangles.push_back(Triangle(simplified_mesh.point(v0),simplified_mesh.point(v1),simplified_mesh.point(v2)));
		}

		// constructs AABB tree
		Tree tree(triangles.begin(),triangles.end());
		tree.accelerate_distance_queries();


		max_o_to_s_ = -1;
		mean_o_to_s_ = -1;
		rms_o_to_s_ = -1;
		for(auto & point : points_){
			double current_dist = std::sqrt(tree.squared_distance(point));
			max_o_to_s_ = (std::max)(max_o_to_s_,current_dist);
			mean_o_to_s_ += current_dist;
			rms_o_to_s_ += current_dist*current_dist;
		}

		mean_o_to_s_/=points_.size();
		rms_o_to_s_/=points_.size();
		rms_o_to_s_ = std::sqrt(rms_o_to_s_);
	}

private:
	const std::list<typename Complex::Point> &points_;
	double max_o_to_s_;
	double mean_o_to_s_;
	double rms_o_to_s_;


public:
	/*
	 * compute_distance(mesh) must be called before calling these members
	 * simplified to original distances
	 *
	 */
	double max_o_to_s() const{
		return max_o_to_s_;
	}
	double mean_o_to_s() const{
		return mean_o_to_s_;
	}

	double rms_o_to_s() const{
		return rms_o_to_s_;
	}

};

#endif /* DISTANCE_POINT_TO_TRIANGLE_H_ */
