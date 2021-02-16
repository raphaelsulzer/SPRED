/*
 * SkBl_region_growing.h
 *
 *  Created on: Jul 22, 2014
 *      Author: dsalinas
 */

#ifndef SKBL_REGION_GROWING_H_
#define SKBL_REGION_GROWING_H_


#include <set>

#include "Proxy_detection.h"
#include "urban_utils.h"

#include "boost/iterator/iterator_facade.hpp"


/**
 * Proxy detection with region growing
 */
template<typename SkBlComplex> class SkBl_region_growing : public Proxy_detection<SkBlComplex>{

private:
	typedef typename SkBlComplex::Vertex_handle Vertex_handle;
	typedef typename SkBlComplex::Simplex_handle Simplex_handle;
	typedef typename SkBlComplex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef typename GT::Vector_3 Vector_3;
	typedef typename GT::Point_3 Point_3;



	const SkBlComplex& complex_;
	const Detection_params& proxy_params_;


public:
	class Region{
	private:
		const SkBlComplex& complex_;
		double area_;
		std::set<Vertex_handle> vertices_;
		Plane_3 plane_;
	public:


		Region(const SkBlComplex& complex,SkBl_region_growing& region_growing,const Simplex_handle& t,std::set<Simplex_handle>& seen):
			complex_(complex),area_(0){
			assert(t.dimension()==2);

			add_triangle(t);

			std::list<Simplex_handle> to_be_seen;

			seen.insert(t); // the first triangle is necessarily valid
			to_be_seen.push_back(t);

			while (!to_be_seen.empty()){
				Simplex_handle current = to_be_seen.back();
				to_be_seen.pop_back();

				add_triangle(current);

				// add the neighbors that have not already been seen
				std::set<Simplex_handle> neighbors;
				region_growing.neighbor_triangles(current,neighbors);
				for(const auto& t_neighbor : neighbors )
					if(seen.find(t_neighbor) == seen.end()){
						if(region_growing.is_triangle_valid(*this,t_neighbor)){
							to_be_seen.push_front	(t_neighbor);
							seen.insert(t_neighbor);
						}
					}
			}
		}



		Region(const SkBlComplex& complex,const Simplex_handle& t)
		:complex_(complex),area_(0){
			assert(t.dimension() == 2);
			add_triangle(t);
		}

		void add_triangle(const Simplex_handle& t){
			for(auto v : t)	vertices_.insert(v);

			auto p1 = complex_.point( *t.begin());
			auto p2 = complex_.point( *std::next(t.begin(), 1));
			auto p3 = complex_.point( *std::next(t.begin(), 2));

			if(!CGAL::collinear(p1,p2,p3))
				area_+= std::sqrt(Triangle_3(p1,p2,p3).squared_area ());

			recompute_plane();
		}

		std::string to_string() const{
			std::ostringstream stream;
			stream<<"#plane: "<< plane().a()<<" "<< plane().b()<<" "<< plane().c()<<" "<< plane().d();
			stream<<" #area:" << area() ;
			stream<<" #vertices: ";
			for(auto x: vertices_)	stream<<x<<" ";
			return stream.str();
		}

		size_t size() const{
			return vertices_.size();
		}

	private:


//		: public boost::iterator_facade < Complex_vertex_iterator
//		                                , typename SkeletonBlockerDS::Vertex_handle const
//		                                , boost::forward_traversal_tag
//		                                , typename SkeletonBlockerDS::Vertex_handle const
//		                                >

		class Iterator_vertices_to_points : public boost::iterator_facade <
		Iterator_vertices_to_points
		, Point_3
		, boost::forward_traversal_tag
		, Point_3 	>
		{
			typedef typename std::set<Vertex_handle>::const_iterator It;
			const SkBlComplex& complex_;
			It vertices_it_;

		public:
			Iterator_vertices_to_points(
					const SkBlComplex& complex,
					It it
			):complex_(complex),vertices_it_(it){
			}

			bool equal(const Iterator_vertices_to_points& other) const{
				return vertices_it_ == other.vertices_it_;
			}
			void increment () {
				++vertices_it_;
			}
			const Point_3&  dereference() const	{
				return complex_.point(*vertices_it_);
			}
		};

		void recompute_plane(){

			Iterator_vertices_to_points points_begin(complex_,vertices_.begin());
			Iterator_vertices_to_points points_end(complex_,vertices_.end());
			CGAL::linear_least_squares_fitting_3 (points_begin,points_end,plane_,CGAL::Dimension_tag<0>());

//			std::list<Point_3> points;
//			for(auto v : vertices_) points.push_back(complex_.point(v));
//			CGAL::linear_least_squares_fitting_3 (points.begin(),points.end(),plane_,CGAL::Dimension_tag<0>());

		}

		public:

		//		void add_vertex(Vertex_handle v){
		//			vertices_.insert(v);
		//		}

		Plane_3& plane(){
			return plane_;
		}

		const Plane_3& plane() const{
			return plane_;
		}

		double area() const{
			return area_;
		}

		const std::set<Vertex_handle>& vertices() const{
			return vertices_;
		}

	};


private:
	std::list<Region> regions_;

public:

	//todo cleanup cstor does 2 thing, construct and op >>
	SkBl_region_growing(const SkBlComplex& complex,const std::string& name_seg_file,const Detection_params& proxy_params)
:complex_(complex),proxy_params_(proxy_params){

		std::ofstream stream;
		stream.open (name_seg_file);

//		myfile<<"% unknown nb of proxies / params : "<<std::endl;//proxy_params<<std::endl;
		stream<<"!vertices: ";
		std::copy(
				complex.vertex_range().begin(),
				complex.vertex_range().end(),
				std::ostream_iterator<Vertex_handle>(stream," ")
		);
		stream<<std::endl;

		std::set<Simplex_handle> seen;
		for(const auto& t : complex.triangle_range()){
			Region region(complex,*this,t,seen);
			if(is_region_valid(region)){
				stream<<region.to_string()<<std::endl;
			}
			else
				seen.erase(t);
		}

		stream.close();
	}

private:


	/**
	 * Return the neighbors around a triangle
	 */
	void neighbor_triangles(const Simplex_handle& t,std::set<Simplex_handle>& res) const{
		res.clear();
		for(auto v : t){
			for(const auto& tv : complex_.triangle_range(v))
				res.insert(tv);
		}
		res.erase(t);
	}


	/**
	 * Tells if the triangle is valid for the region growing.
	 */
	bool is_triangle_valid(const Region& region,const Simplex_handle& t) const{
		return is_distance_valid(region,t) && is_triangle_normal_valid(region,t);
	}


	bool is_distance_valid(const Region& region,const Simplex_handle& t) const{
		const auto& p1 = complex_.point( *t.begin());
		const auto& p2 = complex_.point( *std::next(t.begin(), 1));
		const auto& p3 = complex_.point( *std::next(t.begin(), 2));

		double max_dist = CGAL::squared_distance(p1,region.plane());
		max_dist = std::max(max_dist,CGAL::squared_distance(p2,region.plane()));
		max_dist = std::max(max_dist,CGAL::squared_distance(p3,region.plane()));

		return (max_dist <= proxy_params_.max_dist*proxy_params_.max_dist);
	}

	bool is_triangle_normal_valid(const Region& region,const Simplex_handle& t) const{
		const auto& p1 = complex_.point( *t.begin());
		const auto& p2 = complex_.point( *std::next(t.begin(), 1));
		const auto& p3 = complex_.point( *std::next(t.begin(), 2));

		if(!CGAL::collinear(p1,p2,p3)){
			if(std::abs(proxy_params_.max_angle - 90.)<1e-3) return true;

			auto normal_tri = CGAL::unit_normal(p1,p2,p3);

			Vector_3 normal_plane(region.plane().orthogonal_vector());
			normal_plane= normal_plane/std::sqrt(normal_plane.squared_length());

			assert(proxy_params_.max_angle>=0. && proxy_params_.max_angle <= 90.);
			double sc_bound = std::cos(proxy_params_.max_angle * 3.14 / 180.);
			return (std::abs(region.plane().orthogonal_vector() * normal_tri) > sc_bound);
		}
		else return true;

	}



	bool is_region_valid(const Region& region) const{
		return region.area() >= proxy_params_.min_area;
	}



};



#endif /* SKBL_REGION_GROWING_H_ */
