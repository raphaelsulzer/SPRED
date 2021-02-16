#ifndef VIEWER_INSTRUCTOR_H
#define VIEWER_INSTRUCTOR_H


#include <QFileDialog>
#include <QKeyEvent>
#include <QGLViewer/camera.h>

#include "typedefs.h"

#include "interface/View_parameter.h"
#include "interface/Viewer.h"
#include "interface/Mesh_context.h"


template<typename C> class Primitive_graph;

class Viewer;
struct Mesh_context;

class Viewer_instructor : public QWidget{
	Q_OBJECT

	typedef Gudhi_kernel::Point Point;
	typedef Gudhi_kernel::Point_2 Point_2;
	typedef Gudhi_kernel::Vector_3 Vector;
	typedef Primitive_graph<Complex> Primitive_graph_;

	Viewer* viewer;
	View_parameter view_params_;
	Mesh_context& mesh_ctxt;
	boost::optional< std::pair<Point,Point> > * best_edge;
	boost::optional< Point > * new_point;

public:
	boost::optional< Point > placement_selected_edge;
	boost::optional< std::pair<Point,Point> > selected_edge;


	Viewer_instructor(QWidget* parent_,
			Viewer* viewer_,
			Mesh_context& mesh_ctxt_,
			boost::optional< std::pair<Point,Point> > *best_edge_,
			boost::optional< Point > * new_point_
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
	 * gives instructions to the viewer
	 */
	void give_instructions();




	public slots :

	void sceneChanged();
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



	private:
	void gl_draw_mesh(float red,float green,float blue);
	void gl_draw_edge(const Point&,const Point&,float red,float green,float blue,double size );
	void gl_draw_point(Point const&,float red,float green,float blue, double size);
	void gl_draw_points(float red,float green,float blue,double size);

	/**
	 * set the right color of vertex/edge/triangle considering the view_params choice
	 */
	void set_color_vertex(Vertex_handle vh);
	void set_color_edge(Edge_handle eh);

	/**
	 * return true if it is a non proxy triangle (ie if the vertices of the triangle
	 * does not contains a identical proxy)
	 */
	bool set_color_triangle(const Simplex_handle& triangle);
	void gl_draw_edges(float red,float green,float blue,double size);
	void gl_draw_triangles(float red,float green,float blue);
	void gl_draw_primitive_graph();

	void gl_draw_primitives();
	void gl_draw_primitives_border();
	void gl_draw_primitives_offset_border();
	void gl_draw_primitives_border_edges();
	void gl_draw_primitives_corners();

};

#endif //VIEWER_INSTRUCTOR_H
