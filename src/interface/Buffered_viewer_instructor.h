/*
 * Buffered_viewer_instructor.h
 *
 *  Created on: Sep 15, 2014
 *      Author: dsalinas
 */

#ifndef BUFFERED_VIEWER_INSTRUCTOR_H_
#define BUFFERED_VIEWER_INSTRUCTOR_H_


#include <QFileDialog>
#include <QKeyEvent>
#include <QGLViewer/camera.h>
#include <boost/iterator/iterator_facade.hpp>


#include "typedefs.h"

#include "interface/View_parameter.h"
#include "interface/Viewer.h"
#include "interface/Mesh_context.h"
#include "interface/Color.h"

class Buffered_viewer;
struct Mesh_context;

class Buffered_viewer_instructor : public QWidget{
	Q_OBJECT

    template<class C> friend class Primitive_graph;


	typedef Gudhi_kernel::Point Point;
	typedef Gudhi_kernel::Point_2 Point_2;
	typedef Gudhi_kernel::Vector_3 Vector;

	Buffered_viewer* viewer_;
	View_parameter view_params_;
	Mesh_context& mesh_ctxt_;
	boost::optional< std::pair<Point,Point> > * best_edge_;
	boost::optional< Point > * new_point_;

public:
	boost::optional< Point > placement_selected_edge;
	boost::optional< std::pair<Point,Point> > selected_edge;


	Buffered_viewer_instructor(QWidget* parent,
			Buffered_viewer* viewer,
			Mesh_context& mesh_context,
			boost::optional< std::pair<Point,Point> > *best_edge,
			boost::optional< Point > * new_point
	);

	void clear();
	void initialize_bounding_box();
	void show_entire_scene();

	void snap_shot() const;
	qglviewer::Camera* 	camera() const;
	int 	width() const;
	int 	height() const;
	void set_dimension(int width,int height);


	/**
	 * to change display parameters
	 */
	View_parameter& view_params(){
		return view_params_;
	}

public:

	/**
	 * fills the Buffered_viewer buffer with points/edges/triangles info.
	 */
	void init_buffer();

private:

	void set_viewer_parameters();

	void load_vertices();
	void load_vertices_colors();
	Color vertex_color(Vertex_handle vh) const;

	void load_edges();
	void load_edges_colors();
	Color edge_color(Edge_handle eh) const;

	void load_triangles();
	void load_triangles_colors();
	Color triangle_color(Simplex_handle t) const;


	std::map<Vertex_handle,unsigned> vh_to_index_;


	public slots :

	void sceneChanged();

	void update_view_params();

	void change_draw_vertices();
	void change_draw_edges();
	void change_draw_triangles();
	void change_draw_primitives();
	void change_draw_primitives_graph();
	void change_draw_primitives_border();
	void change_draw_primitives_corner();
	void change_draw_best_edge(bool);
	void change_draw_new_point(bool);
	void change_light();
};


#endif /* BUFFERED_VIEWER_INSTRUCTOR_H_ */
