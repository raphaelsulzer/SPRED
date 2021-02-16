/*
 * Buffered_viewer.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: dsalinas
 */

#include "Buffered_viewer.h"
#include "complex/complex_tools.h"
#include <CGAL/bounding_box.h>
#include <QGLViewer/vec.h>
#include <QGLViewer/camera.h>

typedef Kernel::Iso_cuboid_3 bounding_3;


void Buffered_viewer::show_entire_scene(){
	this->showEntireScene();
}

void Buffered_viewer::draw(){
	if(is_draw_points()) draw_points();
	if(is_draw_edges()) draw_edges();
	if(is_draw_triangles()) draw_triangles();
}

void Buffered_viewer::set_bounding_box(const bounding_3 & bbox){
	std::cout << "Bounding box: "<< bbox<<endl;
	this->camera()->setSceneBoundingBox(
			qglviewer::Vec(bbox.xmin(), bbox.ymin(), bbox.zmin()),
			qglviewer::Vec(bbox.xmax(),bbox.ymax(),bbox.zmax())
	);
}

void Buffered_viewer::update_GL(){
	this->updateGL();
}

void Buffered_viewer::init_scene(){

	this->setBackgroundColor(Qt::white);
	::glEnable(GL_LINE_SMOOTH);

	init_light();
}


void
Buffered_viewer::init_light(){
	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}




void
Buffered_viewer::set_light_direction(double theta_, double phi_){
	theta = theta_;
	phi = phi_;
}

void
Buffered_viewer::set_light_direction(){
	theta = -1;
	phi = -1;
}


void
Buffered_viewer::set_light(){
	if(theta>=0 && phi >=0){
		const GLfloat pos[4] = {(float)(sin(phi)*cos(theta)),(float)(sin(phi)*sin(theta)),(float)(cos(phi)),0.};
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
	}
}


void
Buffered_viewer::postSelection(const QPoint& point)
{
	bool found;
	auto vec = this->camera()->pointUnderPixel(point,found);
	if(found){
		Point position(vec[0],vec[1],vec[2]);
		emit(click(position));
	}
}


void Buffered_viewer::draw_points() const{
	::glDisable(GL_LIGHTING);
	::glEnable(GL_POINT_SMOOTH);
	::glPointSize(point_size_);
	::glBegin(GL_POINTS);

	for(unsigned i = 0 ; i< points_.size(); ++i){
		::glColor3d(points_colors_[i].r(),points_colors_[i].g(),points_colors_[i].b());
		::glVertex3d(points_[i].x(),points_[i].y(),points_[i].z());
	}

	::glEnd();
	::glDisable(GL_POINT_SMOOTH);
}

bool Buffered_viewer::is_draw_points() const{
	return draw_points_;
}
void Buffered_viewer::set_draw_points(bool new_value){
	draw_points_ = new_value;
}

void Buffered_viewer::draw_edges() const{
	::glLineWidth(edge_size_);
	::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	::glEnable(GL_POLYGON_OFFSET_LINE);
	::glPolygonOffset(edge_size_,-edge_size_);
	::glDisable(GL_LIGHTING);
	::glBegin(GL_LINES);
	for(unsigned i = 0 ; i< edges_indices_.size(); ++i){
		::glColor3d(edges_colors_[i].r(),edges_colors_[i].g(),edges_colors_[i].b());
		const auto &a = points_[std::get<0>(edges_indices_[i])];
		const auto &b = points_[std::get<1>(edges_indices_[i])];
		::glVertex3f(a.x(),a.y(),a.z());
		::glVertex3f(b.x(),b.y(),b.z());
	}
	std::vector< Edge_indices > edges_indices_;
	::glEnd();
}

bool Buffered_viewer::is_draw_edges() const{
	return draw_edges_;
}
void Buffered_viewer::set_draw_edges(bool new_value){
	draw_edges_ = new_value;
}


void Buffered_viewer::draw_triangles() const{
	::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	::glEnable(GL_POLYGON_OFFSET_FILL);
	::glPolygonOffset(3.0f,-3.0f);
	::glEnable(GL_LIGHTING);
	::glBegin(GL_TRIANGLES);

	for(unsigned i = 0 ; i< triangles_indices_.size(); ++i){
		::glColor3d(triangles_colors_[i].r(),triangles_colors_[i].g(),triangles_colors_[i].b());
		const Point& p0 = points_[std::get<0>(triangles_indices_[i])];
		const Point& p1 = points_[std::get<1>(triangles_indices_[i])];
		const Point& p2 = points_[std::get<2>(triangles_indices_[i])];
		auto triangle_normal = CGAL::unit_normal(p0,p1,p2);
		::glNormal3d(triangle_normal.x(),triangle_normal.y(),triangle_normal.z());
		::glVertex3d(p0.x(),p0.y(),p0.z());
		::glVertex3d(p1.x(),p1.y(),p1.z());
		::glVertex3d(p2.x(),p2.y(),p2.z());
	}
	::glEnd();
}

bool Buffered_viewer::is_draw_triangles() const{
	return draw_triangles_;
}

void Buffered_viewer::set_draw_triangles(bool new_value){
	draw_triangles_ = new_value;
}




void Buffered_viewer::clear_points(){
	points_.clear();
	points_colors_.clear();
}
void Buffered_viewer::clear_edges(){
	edges_indices_.clear();
	edges_colors_.clear();
}
void Buffered_viewer::clear_triangles(){
	triangles_indices_.clear();
	triangles_colors_.clear();
}



#include "Buffered_viewer.moc"

