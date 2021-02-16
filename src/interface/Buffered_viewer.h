/*
 * Buffered_viewer.h
 *
 *  Created on: Sep 15, 2014
 *      Author: dsalinas
 */

#ifndef BUFFERED_VIEWER_H_
#define BUFFERED_VIEWER_H_


#include <vector>
#include <CGAL/bounding_box.h>
#include <QGLViewer/qglviewer.h>
#include <tuple>
#include "typedefs.h"
#include "interface/View_parameter.h"
#include "Viewer_instructor.h"
#include "Color.h"

typedef Kernel::Iso_cuboid_3 bounding_3;
class Viewer_instructor;

class Buffered_viewer : public QGLViewer {
	Q_OBJECT


	/**
	 * light angles
	 */
	double theta,phi;

	typedef Gudhi_kernel::Point Point;
	typedef Gudhi_kernel::Vector_3 Vector;

public:

	Buffered_viewer(QWidget* parent):
		QGLViewer(parent),
		theta(0),
		phi(0),
		point_size_(1.),
		draw_points_(true),
		edge_size_(1.),
		draw_edges_(true),
		draw_triangles_(true){
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

	template<typename PointsIterator>
	void load_new_points(PointsIterator begin,PointsIterator end){
		clear_points();
		points_.insert(points_.begin(),begin,end);
	}

	template<typename ColorIterator>
	void load_new_points_colors(ColorIterator begin,ColorIterator end){
		points_colors_.clear();
		points_colors_.insert(points_colors_.begin(),begin,end);
	}

	void draw_points() const;
	bool is_draw_points() const;
	void set_draw_points(bool new_value);
	void set_point_size(double new_value){ point_size_ = new_value; }


	template<typename EdgeIterator>
	void load_new_edges(EdgeIterator begin,EdgeIterator end){
		clear_edges();
		edges_indices_.insert(edges_indices_.begin(),begin,end);
	}
	template<typename ColorIterator>
	void load_new_edges_colors(ColorIterator begin,ColorIterator end){
		edges_colors_.clear();
		edges_colors_.insert(edges_colors_.begin(),begin,end);
	}
	void draw_edges() const;
	bool is_draw_edges() const;
	void set_draw_edges(bool new_value);
	void set_edge_size(double new_value){ edge_size_ = new_value; }


	// A Triangle must have begin/end methods that return iterator to
	// increasing indices in the range [0,number of points-1].
	template<typename TriangleIterator>
	void load_new_triangles(TriangleIterator begin,TriangleIterator end){
		clear_triangles();
		triangles_indices_.insert(triangles_indices_.begin(),begin,end);
	}

	template<typename ColorIterator>
	void load_new_triangles_colors(ColorIterator begin,ColorIterator end){
		triangles_colors_.clear();
		triangles_colors_.insert(triangles_colors_.begin(),begin,end);
	}

	void draw_triangles() const;
	bool is_draw_triangles() const;
	void set_draw_triangles(bool new_value);


private:
	void clear_points();
	void clear_edges();
	void clear_triangles();

private:

	// should be increasingly ordered
	typedef std::tuple<unsigned,unsigned> Edge_indices;
	typedef std::tuple<unsigned,unsigned,unsigned> Triangle_indices;

	double point_size_;
	std::vector<Point> points_;
	std::vector<Color> points_colors_;
	bool draw_points_;

	double edge_size_;
	std::vector< Edge_indices > edges_indices_;
	std::vector<Color> edges_colors_;
	bool draw_edges_;

	std::vector< Triangle_indices > triangles_indices_;
	std::vector<Color> triangles_colors_;
	bool draw_triangles_;

public:
	signals:
	void click(const Point& position);


};




#endif /* BUFFERED_VIEWER_H_ */
