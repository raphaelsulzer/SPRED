/*
 * Semantized_geometric_traits.h
 *
 *  Created on: Feb 20, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef SEMANTIZED_GEOMETRIC_TRAITS_H_
#define SEMANTIZED_GEOMETRIC_TRAITS_H_

#include <iostream>

#include "urban_utils.h"

//#include "combinatorics/Skeleton_blocker/Skeleton_blocker_simple_traits.h"
//#include "geometry/Skeleton_blocker_simple_geometric_traits.h"
#include "gudhi/gudhi_complex.h"

#include "semantic/Plane_primitive.h"
#include "contraction/quadric/Error_quadric.h"
#include "geometry/geometry_tools.h"

/**
 * @brief store one unique quadric to represent three quadrics (data,plane,border)
 */
template<typename Quadric> class Quadric_simple_storage{
	Quadric quadric_;
public:
	void clear(){
		quadric_ = Quadric();
	}

	const Quadric& quadric() const{
		return quadric_;
	}

	Quadric& quadric() {
		return quadric_;
	}

	Quadric_simple_storage& operator+=(const Quadric_simple_storage& other){
		this->sum_quadric(other);
		return *this;
	}

	Quadric_simple_storage& operator-=(const Quadric_simple_storage& other){
		this->sub_quadric(other);
		return *this;
	}

	/**
	 * adds a quadric that comes from a triangle from the data
	 * (ie a Garland Heckbert quadric).
	 */
	void sum_quadric_data(const Quadric &other_quadric){
		quadric_+= other_quadric;
	}

	/**
	 * adds a quadric that comes from the plane of a proxy
	 */
	void sum_quadric_proxy_plane(const Quadric &other_quadric){
		quadric_+= other_quadric;
	}

	/**
	 * adds a quadric that comes from an edge that
	 * belongs to the border of a proxy
	 */
	void sum_quadric_proxy_border(const Quadric &other_quadric){
		quadric_+= other_quadric;
	}

	void sum_quadric(const Quadric_simple_storage &other_quadric){
		quadric_+= other_quadric.quadric_;
	}

	void sub_quadric(const Quadric_simple_storage &other_quadric){
		quadric_-= other_quadric.quadric_;
	}



	double cost(const typename Quadric::Point& point) const{
		return quadric_.cost(point);
	}

	double determinant() const{
		return quadric_.determinant();
	}

	boost::optional<typename Quadric::Point> min_cost(double scale = 1){
		return quadric_.min_cost(scale);
	}

	boost::optional<typename Quadric::Point> min_cost(const typename Quadric::Point & star_centroid){
		return quadric_.min_cost(star_centroid);
	}
};


// todo faire un héritage de Quadric_simple_storage
/**
 * @brief store three quadrics to represent the different quadrics (data,plane,border)
 */
template<typename Quadric> class Quadric_multiple_storage{
	Quadric quadric_data_;
	Quadric quadric_proxy_plane_;
	Quadric quadric_proxy_border_;
	Quadric quadric_spring_;

	// quadric_sum_ = quadric_data_ + quadric_proxy_plane_ + quadric_proxy_border_
	Quadric quadric_sum_;

public:
	void clear(){
		quadric_data_ = Quadric();
		quadric_proxy_plane_ = Quadric();
		quadric_proxy_border_ = Quadric();
		quadric_spring_ = Quadric();
		quadric_sum_ = Quadric();
	}

	void clear_data(){
		quadric_data_ = Quadric();
	}

	const Quadric& quadric() const{
		return quadric_sum_;
	}

	Quadric& quadric(){
		return quadric_sum_;
	}

	const Quadric& quadric_proxy_border() const{
		return quadric_proxy_border_;
	}

	Quadric& quadric_data() {
		return quadric_data_;
	}


	Quadric_multiple_storage& operator+=(const Quadric_multiple_storage& other){
		quadric_data_ += other.quadric_data_;
		quadric_proxy_plane_ += other.quadric_proxy_plane_;
		quadric_proxy_border_ += other.quadric_proxy_border_;
		quadric_sum_ += other.quadric_sum_;
		quadric_spring_ = Quadric();
		return *this;
	}

	Quadric_multiple_storage& operator-=(const Quadric_multiple_storage& other){
		quadric_data_ -= other.quadric_data_;
		quadric_proxy_plane_ -= other.quadric_proxy_plane_;
		quadric_proxy_border_ -= other.quadric_proxy_border_;
		quadric_sum_ -= other.quadric_sum_;
		quadric_spring_ = Quadric();
		return *this;
	}


	/**
	 * adds a quadric that comes from a triangle from the data
	 * (ie a Garland Heckbert quadric).
	 * updates the sum of all quadrics.
	 */
	void sum_quadric_data(const Quadric &quadric_data){
		quadric_sum_+=quadric_data;
		quadric_data_ += quadric_data;
	}


	/**
	 * adds a quadric that is a spring quadric.
	 * updates the sum of all quadrics.
	 */
	void sum_quadric_spring(const Quadric &quadric_data){
		quadric_sum_+=quadric_data;
		quadric_spring_ += quadric_data;
	}

	/**
	 * adds a quadric that comes from the plane of a proxy.
	 * updates the sum of all quadrics.
	 */
	void sum_quadric_proxy_plane(const Quadric &quadric_proxy_plane){
		quadric_sum_+=quadric_proxy_plane;
		quadric_proxy_plane_ += quadric_proxy_plane;
	}

	/**
	 * adds a quadric that comes from an edge that
	 * belongs to the border of a proxy.
	 * updates the sum of all quadrics.
	 */
	void sum_quadric_proxy_border(const Quadric &quadric_proxy_border){
		quadric_sum_+=quadric_proxy_border;
		quadric_proxy_border_ += quadric_proxy_border;
	}

	void sum_quadric(const Quadric_multiple_storage &other_quadric){
		(*this)+= other_quadric;
	}

	void sub_quadric(const Quadric_multiple_storage &other_quadric){
		(*this)-= other_quadric;
	}

	void print_costs(const typename Quadric::Point& point) const{
		double total_cost(quadric_sum_.cost(point));
		double data_cost(quadric_data_.cost(point));
		double plane_cost(quadric_proxy_plane_.cost(point));
		double border_cost(quadric_proxy_border_.cost(point));
		double spring_cost(quadric_spring_.cost(point));

		if(total_cost==0) {
			std::cerr<<"Zero total cost:"<<quadric_sum_.cost(point)<<std::endl;
		}
		else{
			std::cerr<<"Data cost :"<<data_cost<<"  --  "<< (int)(data_cost/total_cost*100)<<"%"<<std::endl;
			std::cerr<<"Proxy plane cost:"<<plane_cost<<"  --  "<< (int)(plane_cost/total_cost*100)<<"%"<<std::endl;
			std::cerr<<"Proxy border cost :"<<border_cost<<"  --  "<< (int)(border_cost/total_cost*100)<<"%"<<std::endl;
			std::cerr<<"Spring cost :"<<spring_cost<<"  --  "<< (int)(spring_cost/total_cost*100)<<"%"<<std::endl;
			std::cerr<<"Total cost:"<<quadric_sum_.cost(point)<<std::endl;
		}
	}


	double data_cost(const typename Quadric::Point& point) const{
		return quadric_data_.cost(point);
	}

	double proxy_plane_cost(const typename Quadric::Point& point) const{
		return quadric_proxy_plane_.cost(point);
	}

	double proxy_border_cost(const typename Quadric::Point& point) const{
		return quadric_proxy_border_.cost(point);
	}


	double grad_determinant() const{
		return quadric_sum_.grad_determinant();
	}

	double cost(const typename Quadric::Point& point) const{
		return quadric_sum_.cost(point);
	}

	boost::optional<typename Quadric::Point> min_cost(double scale=1){
		return quadric_sum_.min_cost(scale);
	}


	boost::optional<typename Quadric::Point> min_cost(const typename Quadric::Point & star_centroid){
		return quadric_sum_.min_cost(star_centroid);
	}

	friend std::ostream& operator<< (std::ostream& stream, const Quadric_multiple_storage& quadric) {
		stream<<"Data quadric:"<<quadric.quadric_data_<<std::endl;
		stream<<"Proxy plane quadric:"<<quadric.quadric_proxy_plane_<<std::endl;
		stream<<"Proxy border quadric:"<<quadric.quadric_proxy_border_<<std::endl;
		stream<<"Total quadric:"<<quadric.quadric_sum_<<std::endl;
		return stream;
	}
};




template<typename GT_,typename QuadricStorage_>
struct Semantized_geometric_traits : public Skeleton_blocker_simple_geometric_traits<GT_> {

	template<typename T> friend class Primitive_corners;

	typedef typename Skeleton_blocker_simple_geometric_traits<GT_>::Root_vertex_handle Root_vertex_handle;
	typedef typename Skeleton_blocker_simple_geometric_traits<GT_>::Vertex_handle Vertex_handle;
	typedef typename GT_::Point Point;

	typedef typename Skeleton_blocker_simple_geometric_traits<GT_>::Graph_vertex Simple_geometric_vertex;

public:
	typedef typename quadric::Error_quadric<GT_> Error_quadric;
	typedef GT_ GT ;

	typedef Plane_primitive<GT,Vertex_handle> Planar_proxy;

	typedef Planar_proxy* Planar_proxy_handle;

	class Semantized_geometric_vertex : public Simple_geometric_vertex{
	public:
		typedef GT_ GT ;
		typedef Plane_primitive<GT,Vertex_handle> Planar_proxy;
		typedef Planar_proxy* Planar_proxy_handle;

		typedef QuadricStorage_ QuadricStorage;
		typedef std::set<Planar_proxy_handle> Planar_proxies;
		typedef typename std::set<Planar_proxy_handle>::const_iterator Planar_proxy_iterator;
		typedef typename quadric::Error_quadric<GT_> Error_quadric;

	private:

		Planar_proxies plane_primitives_;
		//		Error_quadric quadric_;
		QuadricStorage quadric_storage_;

	public:

		///////////////////////////////////////////////////////////////////
		/** @name Primitive manipulation
		 */
		//@{
		void clear_primitives(){
			plane_primitives_.clear();
		}


		bool contains_primitive(const Planar_proxy_handle primitive) const{
			return plane_primitives_.find(primitive)!=plane_primitives_.end();
		}



		void remove_plane_primitive(Planar_proxy_handle primitive){
			plane_primitives_.erase(primitive);
		}

		void add_plane_primitive(Planar_proxy_handle primitive){
			plane_primitives_.insert(primitive);
		}

		bool contains_plane_primitive(Planar_proxy_handle primitive) const{
			return plane_primitives_.find(primitive)!=plane_primitives_.end();
		}
		Planar_proxies& plane_primitives(){	return plane_primitives_; }
		const Planar_proxies& plane_primitives() const{	return plane_primitives_; }

		unsigned num_plane_primitives() const{
			return plane_primitives_.size();
		}
		bool empty_plane_primitives() const{
			return plane_primitives_.empty();
		}


		//@}
		///////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////
		/** @name Quadric manipulation
		 */
		//@{
		void clear_quadric(){
			quadric_storage_.clear();
		}

		//		Error_quadric& quadric(){
		//			return quadric_storage_.quadric();
		//		}
		const Error_quadric& quadric() const{
			return quadric_storage_.quadric();
		}

		QuadricStorage& quadric_storage() {
			return quadric_storage_;
		}

		const QuadricStorage& quadric_storage() const{
			return quadric_storage_;
		}

		//	private:
		//		void sum_quadric(Semantized_geometric_vertex & other){
		//			this->quadric_+= other.quadric_;
		//		}
	public:


		/**
		 * adds a quadric that comes from a triangle from the data
		 * (ie a Garland Heckbert quadric).
		 */
		void sum_quadric_data(const Error_quadric &other_quadric){
			quadric_storage_.sum_quadric_data(other_quadric);
		}

		/**
		 * adds a quadric that comes from the plane of a proxy
		 */
		void sum_quadric_proxy_plane(const Error_quadric &other_quadric){
			quadric_storage_.sum_quadric_proxy_plane(other_quadric);
		}

		/**
		 * adds a quadric that comes from an edge that
		 * belongs to the border of a proxy
		 */
		void sum_quadric_proxy_border(const Error_quadric &other_quadric){
			quadric_storage_.sum_quadric_proxy_border(other_quadric);
		}

		/**
		 * adds a quadric
		 */
		void sum_quadric(const QuadricStorage &other_quadric){
			quadric_storage_.sum_quadric(other_quadric);
		}

		/**
		 * substracts a quadric
		 */
		void sub_quadric(const QuadricStorage &other_quadric){
			quadric_storage_.sub_quadric(other_quadric);
		}


		friend QuadricStorage get_sum_quadrics(Semantized_geometric_vertex& v0, Semantized_geometric_vertex& v1){
			QuadricStorage res = v0.quadric_storage_;
			res+= v1.quadric_storage_;
			return res;
		}

		friend void sum_quadrics(Semantized_geometric_vertex& v0, Semantized_geometric_vertex& v1){
			v0.quadric_storage_+=v1.quadric_storage_;
			v1.quadric_storage_ = v0.quadric_storage_ ;
		}
		//@}
		///////////////////////////////////////////////////////////////////

	};

	typedef Semantized_geometric_vertex Graph_vertex;

};




#endif /* SEMANTIZED_GEOMETRIC_TRAITS_H_ */


//mesh[corner_vertex].is_corner() = true;
//		mesh[corner_vertex].corner_position() = corner_position;
//		//		mesh[v0].is_on_corner() = true;
