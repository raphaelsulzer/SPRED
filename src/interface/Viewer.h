#ifndef VIEWER_H
#define VIEWER_H
#include <CGAL/bounding_box.h>
#include <QGLViewer/qglviewer.h>
#include "typedefs.h"
#include "interface/View_parameter.h"
#include "Viewer_instructor.h"

typedef Kernel::Iso_cuboid_3 bounding_3;

//xxx!!!! remove!!!
using namespace std;

class Viewer_instructor;

//typedef Skeleton_blocker_contractor<Triangulation> Complex_contractor;


class Viewer : public QGLViewer {
	Q_OBJECT


	typedef Gudhi_kernel::Point Point;
	typedef Gudhi_kernel::Vector_3 Vector;
public:
	Viewer(QWidget* parent): QGLViewer(parent),instructor(0),theta(0),phi(0){
//		QObject::connect(this, SIGNAL(click(const Point&)), parent, SLOT(click(const Point&)));
	}

	void set_instructor(Viewer_instructor* instructor_){
		instructor = instructor_;
	}
	void show_entire_scene();
	void draw();
	void set_bounding_box(const bounding_3 & bbox);
	void update_GL();
	void init_scene();

	void init_light();
	void set_light();

	/**
	 * set the light in a direction (theta,phi)
	 */
	void set_light_direction(double theta,double phi);

	/**
	 * set the light in the direction of the observer
	 */
	void set_light_direction();


protected:
	virtual void postSelection(const QPoint& point);
public:

	////////////////////////
	// draw
	////////////////////////
	void set_size_point(double size_points);
	void draw_point(const Point& p,double red,double green,double blue,double size_points);
	void begin_draw_points(double red,double green,double blue,double size,bool light=false);
	void set_color(const CGAL::Color& primitive_color);
	void set_color(double r,double g,double b);
	void draw_points(const Point & point);
	void end_draw_points();
	void draw_edge(const Point &a,const Point &b,double red,double green,double blue,double size);
	void begin_draw_edges(double red,double green,double blue,double size,bool light=false);
	void draw_edges(const Point &a,const Point &b);
	void end_draw_edges();
	void begin_draw_triangles(double red,double green,double blue,double size,bool light,bool transparent = false);
	void draw_triangles(const Point& p1,const Point& p2,const Point& p3,Vector triangle_normal);
	void draw_triangles(const  std::vector<Point*>& points,	Vector triangle_normal);
	void end_draw_triangles();


	signals:
	void click(const Point& position);

private:
	Viewer_instructor * instructor;
	/**
	 * light angles
	 */
	double theta,phi;
};

#endif
