/*
 * Primitive.h
 *
 *  Created on: Jun 27, 2014
 *      Author: dsalinas
 */

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include <set>
#include <CGAL/linear_least_squares_fitting_3.h>
#include "urban_utils.h"
#include "typedefs.h"


/**
 * A generic (and abstract) primitive.
 * Concrete and base class can be planar, spherical, conical and so on.
 *
 * A primitive knows its vertices since it
 * store a vertex handle set (has to be updated by the user).
 * Every vertex also knows its set of primitives since it contains
 * sets of planar, spherical and conical primtives.
 *
 * Adding/removing primitives will impose to update
 *  - its vertex handles
 *  - the vertices that used to contain that proxy
 *  - the primitive graph
 */
template <typename GT_, typename VertexHandle>
class Primitive{
	template<class ComplexType> friend class Enriched_mesh;

public:
	typedef GT_ GT;

protected:
	typedef typename GT::Point_2 Point_2;
	typedef typename GT::Point_3 Point_3;
	typedef typename GT::Vector_3 Vector_3;
	typedef VertexHandle Vertex_handle;

	double area_;
	Point_3 centroid_;
	CGAL::Color color_;

	std::set<Vertex_handle> initial_vertices_;
	std::set<Vertex_handle> current_vertices_;


	// constructor are protected because only base class should be used
protected:
	virtual ~Primitive(){}

	Primitive():area_(0),centroid_(0,0,0){
	}

	Primitive(CGAL::Color color):area_(0),centroid_(0,0,0),color_(color){
	}

	Primitive(
			const std::list<Vertex_handle>& vertices,
			const std::vector<Point_3> vertices_coord
	):
		area_(0),
		initial_vertices_(vertices.begin(),vertices.end()),
		current_vertices_(vertices.begin(),vertices.end())
	{
		int red=(int)floor((double)200*rand()/RAND_MAX)+55;
		int green=(int)floor((double)200*rand()/ RAND_MAX)+55;
		int blue=(int)floor((double)200*rand()/ RAND_MAX)+55;
		color_ = CGAL::Color(std::max(0,std::min(255,red)),std::max(0,std::min(255,green)),std::max(0,std::min(255,blue)),120);

		centroid_ = Point_3(0,0,0);
		for (auto p : vertices_coord){
			centroid_=Point_3(centroid_.x()+p.x(),centroid_.y()+p.y(),centroid_.z()+p.z());
		}
		centroid_=Point_3(centroid_.x()/vertices_coord.size(),centroid_.y()/vertices_coord.size(),centroid_.z()/vertices_coord.size());
		//		centroid_ = CGAL::centroid(vertices_coord.begin(),vertices_coord.end(),GT());
	}


protected:
	/**
	 * recompute the centroid and the geometric paremeters
	 * for example plane should recompute its centroid and do sthing like
	 * CGAL::linear_least_squares_fitting_3 ( points.begin(), points.end(),plane_,CGAL::Dimension_tag<0>());
	 *
	 * new points are the new points of the proxy
	 */
	virtual void recompute_proxy_parameters_after_merge(const std::list<Point_3>& new_points) = 0 ;

public:
	typename std::set<Vertex_handle>::const_iterator begin_initial_vertices() const{
		return initial_vertices_.cbegin();
	}
	typename std::set<Vertex_handle>::const_iterator end_initial_vertices() const{
		return initial_vertices_.cend();
	}

	typename std::set<Vertex_handle>::const_iterator begin_current_vertices() const{
		return current_vertices_.cbegin();
	}
	typename std::set<Vertex_handle>::const_iterator end_current_vertices() const{
		return current_vertices_.cend();
	}

	int num_initial_vertices() const{
		return initial_vertices_.size();
	}

	int num_vertices() const{
		return current_vertices_.size();
	}

	void insert_vertices(
			typename std::set<Vertex_handle>::const_iterator beg,
			typename  std::set<Vertex_handle>::const_iterator end){
		this->initial_vertices_.insert(beg,end);
		this->current_vertices_.insert(beg,end);
	}



	void add_vertex(Vertex_handle v){
		current_vertices_.insert(v);
	}


	void remove_vertex(Vertex_handle v){
		current_vertices_.erase(v);
	}

	bool contains_vertex(Vertex_handle v) const{
		return current_vertices_.find(v)!= current_vertices_.end();
	}

	bool contains_vertex_initially(Vertex_handle v) const{
		return initial_vertices_.find(v)!= initial_vertices_.end();
	}




	CGAL::Color color() const{
		return color_;
	}

	/**
	 * Given a 3d point, it returns a 2d-point whose
	 * coordinates corresponds to its projection on the coordinate system
	 * of the proxy
	 */
	virtual Point_2 project(const Point_3& p) const = 0;

	/**
	 * Given a 2d point whose coordinates are expressed with the proxy system,
	 * it returns the corresponding 3d point.
	 */
	virtual Point_3 expand(const Point_2& p) const = 0;

	virtual double distance(const Point_3& p) const = 0;

	double area() const{
		return area_;
	}

	double& area() {
		return area_;
	}

	const Point_3& centroid(){
		return centroid_;
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


	virtual bool operator==(const Primitive & other){
		UDBG("call bad op<");
		assert(false);
		return false;

	}


	virtual bool operator<(const Primitive& other){
		UDBG("call bad op<");
		assert(false);
		return false;
	}

	//	virtual bool operator<(const Primitive& other) const = 0;
	//	{
	//		//xxx faire mieux
	//		// For instance, if P and Q have respectively two lists of points lP and lQ
	//		// P==Q IF | c(lP\cup lQ,P) - c(lP\cup lQ,Q) | < epsilon
	//		// Where c(lP\cup lQ,P) is the sum of distance from points in lP\cup lQ to the plane P
	//		// Or test if there is a plane R such that c(lP \cup lQ, R ) < epsilon
	//
	//
	//		//		const double epsilon = 0.2;
	//		//		double abs_angle = std::abs(angle_between_two_planes(this->plane(), other.plane()));
	//		//		if (abs_angle < epsilon ) return false;
	//		//		return lexicographic_order(this->plane(),other.plane());
	//
	//		double a1 = this->plane().a(),
	//				b1 = this->plane().b(),
	//				c1 = this->plane().c(),
	//				d1 = this->plane().d();
	//		//		double norm1 = std::sqrt(a1*a1+b1*b1+c1*c1+d1*d1);
	//		//		a1/=norm1;b1/=norm1;c1/=norm1;d1/=norm1;
	//		double a2 = other.plane().a(),
	//				b2 = other.plane().b(),
	//				c2 = other.plane().c(),
	//				d2 = other.plane().d();
	//		//		double norm2 = std::sqrt(a2*a2+b2*b2+c2*c2+d2*d2);
	//		//		a2/=norm2;b2/=norm2;c2/=norm2;d2/=norm2;
	//		double squared_dist = (a1-a2)*(a1-a2)+ (b1-b2)*(b1-b2) + (c1-c2)*(c1-c2) + (d1-d2)*(d1-d2);
	//		double epsilon = 0.001;
	//		if (squared_dist < epsilon ) return false;
	//		return lexicographic_order(this->plane(),other.plane());
	//	}

	virtual std::string to_string() const{
		std::string res;
		res.append("area = ");
		//		res.append(atof(area_));
		return res;
	}

	friend std::ostream& operator<< (std::ostream& stream, const Primitive& primitive) {
		stream << primitive.to_string();
		return stream;
	}


	//	template <typename Mesh>
	//	void merge_plane_primitive(Mesh& mesh,Primitive p1,Primitive p2);
	//
	//	/**
	//	 * the proxy p2 is merged into the proxy p1.
	//	 * Its vertices are redirected to the one of p1.
	//	 */
	//	friend template<typename Mesh>
	//	void merge_plane_primitive(Mesh& mesh,
	//			const Primitive p1,
	//			const Primitive p2){
	//		//remove p2 from the proxies of its vertices
	//		std::for_each(p2->initial_vertices_.begin(),p2->initial_vertices_.end(),
	//				[&](Vertex_handle vh){
	//			assert(mesh[vh].contains_primitive(&p2));
	//			mesh[vh].remove_plane_primitive(&p2);
	//			assert(!mesh[vh].contains_plane_primitive(&p2));
	//			mesh[vh].add_plane_primitive(p1);
	//			assert(mesh[vh].contains_plane_primitive(p1));
	//		}
	//		);
	//
	//		//add vertices of p2 to the vertices of the current proxy
	//		p1->initial_vertices_.insert(p2->initial_vertices_.begin(),p2->initial_vertices_.end());
	//		p1->current_vertices_.insert(p2->current_vertices_.begin(),p2->current_vertices_.end());
	//
	//		// add area
	//		p1->area_ += p2->area_;
	//
	//
	//		typedef typename Mesh::GT::Point_3 Point_3;
	//		// recompute proxy info
	//		std::list<Point_3> points;
	//		std::for_each(
	//				p1->initial_vertices_.begin(),
	//				p1->initial_vertices_.end(),
	//				[&](Vertex_handle vh){
	//			points.push_back(mesh.point(vh));}
	//		);
	//		p1->recompute_proxy_parameters(points);
	//	}

};




#endif /* PRIMITIVE_H_ */
