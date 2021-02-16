/*
 * Polyhedron_segmentation.h
 *
 *  Created on: Feb 21, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef POLYHEDRON_SEGMENTATION_H_
#define POLYHEDRON_SEGMENTATION_H_

#include <CGAL/Polyhedron_3.h>
#include <CGAL/HalfedgeDS_vector.h>
#include "typedefs.h"
#include <CGAL/IO/Color.h>

typedef Kernel::Vector_2 Vector_2;
typedef Kernel::Line_3 Line_3;
typedef Kernel::Line_2 Line_2;

/**
 * the pointers to 'Planar_proxy' objects are handled and owned by 'Enriched_mesh'
 */
template <class Refs,class Traits>
class My_face : public CGAL::HalfedgeDS_face_base<Refs> {
    typedef typename Traits::Point_3 Point_3;
    typedef typename Traits::Vector_3 Vector_3;
public:
	My_face():m_id(-1),m_label(0),m_color(0.7f*255,0.7f*255,0.5f*255),
	m_center(),
	m_normal(),
	m_area(-1),
	m_primitive(NULL){};

	inline int& index() { return m_id; }
	inline int index() const { return m_id; }

	void my_clear(){
		m_id = -1;
		m_label = -1;
        m_color = CGAL::white();
		m_center= Point_3();
		m_normal= Vector_3();
		m_area= 0;
		m_primitive = NULL;
	}

	// todo mettre la primitive plutot et faire que la methode color de f renvoie la couleur de sa primitive
	//inline CGAL::Color& color() {
	//return m_primitive->color();
	//}
	inline bool has_primitive(){
		return m_primitive != NULL;
	}
	inline void set_primitive(Planar_proxy* primitive){
		m_primitive = primitive ;
	}
	inline void remove_primitive(){
		m_primitive=NULL;
	}

	inline CGAL::Color color() const {
		return m_color;
//		if (m_primitive)
//			return m_primitive->color();
//		else return CGAL::WHITE;
	}
	inline Vector_3& normal() { return m_normal; }
	inline Vector_3 normal() const { return m_normal; }
	inline float& area() { return m_area; }
	inline float area() const { return m_area; }
	inline Point_3& center() { return m_center; }
	inline Point_3 center() const { return m_center; }
	inline float squared_dist2D(Point_3 p){
		return (p.x()-this->center().x())*(p.x()-this->center().x())+(p.y()-this->center().y())*(p.y()-this->center().y());
	}


private:
	int m_id;
	int m_label;
	CGAL::Color m_color;

	Point_3 m_center;
	Vector_3 m_normal;
	float m_area;
	Planar_proxy* m_primitive;

};

//definition of a perso vertex for the polyhedron
template <class Refs, class Traits,class Point>
class  My_vertex : public CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point> {
    typedef typename Traits::Point_3 Point_3;
    typedef typename Traits::Vector_3 Vector_3;
public:
	My_vertex():m_id(-1),m_type(-1),m_adjacent_planes(){}

	My_vertex(const Point& pt) :
		CGAL::HalfedgeDS_vertex_base<Refs, CGAL::Tag_true, Point>(pt) ,
		m_id(-1),
		m_type(-1),
		m_adjacent_planes()
		{}

	void my_clear(){
		m_id=-1;
        m_color=CGAL::white();
		m_type=-1;
		m_adjacent_planes =std::list< Planar_proxy* >();
		m_normal=Vector_3();
	}

	inline int& index() { return m_id; }
	inline int index() const { return m_id; }
	void set_color(const CGAL::Color& color){
		m_color = color;
	}
	inline CGAL::Color& color() { return m_color; }
	inline CGAL::Color color() const { return m_color; }
	inline int& type() { return m_type; }
	inline int type() const { return m_type; }
	inline std::list< Planar_proxy* >& primitives() {
		return m_adjacent_planes;
	}
	inline std::list< Planar_proxy* >& primitives() const {
		return m_adjacent_planes;
	}
	void add_primitive(Planar_proxy* p){
		m_adjacent_planes.push_back(p);
	}
	//inline void set_adjacent_planes(std::set< Plane_3 >& new_adjacent_planes) { m_adjacent_planes = new_adjacent_planes;}
	inline Vector_3& normal() { return m_normal; }
	inline Vector_3 normal() const { return m_normal; }
	inline float squared_dist2D(My_vertex& v) const{
		//xxx use CGAL::squared_dist!!
		return (v->point().x()-this->point().x())*(v->point().x()-this->point().x())+(v->point().y()-this->point().y())*(v->point().y()-this->point().y());}

	friend std::ostream& operator<< (std::ostream& stream, const My_vertex& vertex) {
		stream << "[[ "<<vertex.index()<<"]]";
		return stream;
	}

private:


	int m_id;
	CGAL::Color m_color;
	int m_type;
	std::list< Planar_proxy* > m_adjacent_planes;
	Vector_3 m_normal;
};





//definition of a perso halfedge for the polyhedron
template <class Kernel, class Refs, class Tprev, class Tvertex, class Tface, class Normal>
class My_halfedge : public CGAL::HalfedgeDS_halfedge_base<Refs,Tprev,Tvertex,Tface>
{
public:
	typedef typename Kernel::FT FT;
	typedef typename Kernel::Point_3 Point_3;
	typedef typename Kernel::Segment_3 Segment_3;

public:
	My_halfedge() { /*m_temp_boolean = false;*/}

public:

	inline int& index() { return m_id; }
	inline int index() const { return m_id; }
	/*inline int& id() { return m_id; }
	inline int id() const { return m_id; }*/

	FT length() const
	{
		Segment_3 segment = make_segment();
		return std::sqrt(segment.squared_length());
	}

	Segment_3 make_segment() const
	{
		const Point_3& s = this->opposite()->vertex()->point();
		const Point_3& t = this->vertex()->point();
		return Segment_3(s, t);
	}

	// bool m_temp_boolean;   //use as mark in cluster "found border" method
private:
	int m_id;
};



// An items type using my face, halfedge and vertex.
struct My_items : public CGAL::Polyhedron_items_3 {
	template <class Refs, class Traits>
	struct Face_wrapper {
		typedef My_face<Refs,Traits> Face;
	};

	template <class Refs, class Traits>
	struct Vertex_wrapper {
		typedef typename Traits::Point_3 Point;
		typedef My_vertex<Refs,Traits, Point> Vertex;
	};


	// wrap halfedge
	template<class Refs, class Traits> struct Halfedge_wrapper
	{
		typedef typename Traits::Vector_3 Normal;
		typedef My_halfedge<Traits, Refs, CGAL::Tag_true, CGAL::Tag_true, CGAL::Tag_true, Normal> Halfedge;
	};


};

typedef CGAL::Polyhedron_3<Kernel, My_items>  Polyhedron_3;



typedef Polyhedron_3::Vertex_handle Polyhedron_vertex_handle;
typedef Polyhedron_3::Vertex_const_handle Vertex_const_handle;
typedef Polyhedron_3::Vertex_iterator Vertex_iterator;
typedef Polyhedron_3::Vertex_const_iterator Vertex_const_iterator;
typedef Polyhedron_3::Halfedge_handle Halfedge_handle;
typedef Polyhedron_3::Halfedge_iterator Halfedge_iterator;
typedef Polyhedron_3::Edge_iterator Edge_iterator;
typedef Polyhedron_3::Facet_handle Facet_handle;
typedef Polyhedron_3::Facet_iterator Facet_iterator;
typedef Polyhedron_3::Point_iterator Point_iterator;
typedef Polyhedron_3::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;
typedef Polyhedron_3::Halfedge_around_facet_circulator HF_circulator;




#endif /* POLYHEDRON_SEGMENTATION_H_ */
