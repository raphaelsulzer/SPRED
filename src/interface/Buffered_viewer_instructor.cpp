/*
 * Buffered_viewer_instructor.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: dsalinas
 */
#include <vector>
#include <CGAL/bounding_box.h>
#include <QGLViewer/vec.h>
#include <tuple>

#include "Buffered_viewer_instructor.h"
#include "Buffered_viewer.h"

#include "urban_utils.h"

#include "complex/complex_tools.h"
#include "geometry/geometry_tools.h"

typedef Kernel::Iso_cuboid_3 bounding_3;


Buffered_viewer_instructor::Buffered_viewer_instructor(QWidget* parent,
		Buffered_viewer* viewer,
		Mesh_context& mesh_ctxt,
		boost::optional< std::pair<Point,Point> > *best_edge,
		boost::optional< Point > * new_point
)
: QWidget(parent),
  viewer_(viewer),
  view_params_(),
  mesh_ctxt_(mesh_ctxt),
  best_edge_(best_edge),new_point_(new_point)
{
	viewer_->set_draw_points(true);
	viewer_->set_draw_edges(false);
	viewer_->set_draw_triangles(false);
}



void Buffered_viewer_instructor::initialize_bounding_box(){
	std::list<Point> points;
	for(auto vh : mesh_ctxt_.mesh.vertex_range())
		points.push_back(mesh_ctxt_.mesh.point(vh));
	bounding_3 bb = CGAL::bounding_box(points.begin(), points.end());
	cout << "Bounding box: "<< bb<<endl;
	viewer_->set_bounding_box(bb);
	viewer_->init_scene();
}

void Buffered_viewer_instructor::show_entire_scene(){
	viewer_->show_entire_scene();
}

qglviewer::Camera*
Buffered_viewer_instructor::camera() const{
	return viewer_->camera();
}

int
Buffered_viewer_instructor::width() const{
	return viewer_->width();
}
int
Buffered_viewer_instructor::height() const{
	return viewer_->height();
}

void
Buffered_viewer_instructor::set_dimension(int width,int height){
	viewer_->resize(width,height);
}


void Buffered_viewer_instructor::init_buffer(){
	set_viewer_parameters();
	load_vertices();
	load_edges();
	load_triangles();
}

void Buffered_viewer_instructor::set_viewer_parameters(){
	viewer_->set_point_size(view_params_.size_vertices);
	viewer_->set_edge_size(view_params_.size_edges);
}


void Buffered_viewer_instructor::load_vertices(){
	vh_to_index_.clear();
	unsigned current = 0;
	std::list<Point> points;
	for(auto vh : mesh_ctxt_.mesh.vertex_range()){
		points.push_back(mesh_ctxt_.mesh.point(vh));
		vh_to_index_[vh] = current++;
	}
	viewer_->load_new_points(points.begin(),points.end());
	load_vertices_colors();
}

void Buffered_viewer_instructor::load_vertices_colors(){
	std::list<Color> colors;
	for(auto vh : mesh_ctxt_.mesh.vertex_range())
		colors.push_back(vertex_color(vh));
	viewer_->load_new_points_colors(colors.begin(),colors.end());

}

Color Buffered_viewer_instructor::vertex_color(Vertex_handle vh) const{
	switch (view_params_.vertex_mode) {
	case View_parameter::V_SIMPLE:
		return Color(.5,.5,.6);
		break;
	case View_parameter::V_PROXY_NUMBER:
		switch ((mesh_ctxt_.mesh)[vh].num_plane_primitives()) {
		case 0:
			return Color(1,0,0);
			break;
		case 1:
			return Color(.1,.1,0.1);
			break;
		case 2:
			return Color(0,0,1);
			break;
		case 3:
			return Color(1,1,0);
			break;
		default:
			return Color(0,1,1);
			break;
		}
		break;
		case View_parameter::V_PROXY_COLOR:
			if(!(mesh_ctxt_.mesh)[vh].empty_plane_primitives()){
				CGAL::Color primitive_color = (*(mesh_ctxt_.mesh)[vh].plane_primitives().begin())->color();
				return Color(
						(unsigned)primitive_color.r() / 255.,
						(unsigned)primitive_color.g() / 255.,
						(unsigned)primitive_color.b() / 255.
				);
			}
			else
				return Color(1,0,0);
			break;
		default:
			return Color(1,0,0);
			break;
	}
	return Color(1,0,0);
}



void Buffered_viewer_instructor::load_edges(){
	std::list<std::tuple<unsigned,unsigned>> edges;
	for(auto eh : mesh_ctxt_.mesh.edge_range()){
		Vertex_handle v0 = mesh_ctxt_.mesh.first_vertex(eh);
		Vertex_handle v1 = mesh_ctxt_.mesh.second_vertex(eh);
		if(!(v0<v1)) std::swap(v0,v1);
		edges.push_back(std::make_tuple(vh_to_index_[v0],vh_to_index_[v1]));
	}
	viewer_->load_new_edges(edges.begin(),edges.end());
	load_edges_colors();
}

void Buffered_viewer_instructor::load_edges_colors(){
	std::list<Color> colors;
	for(auto eh : mesh_ctxt_.mesh.edge_range())
		colors.push_back(edge_color(eh));
	viewer_->load_new_edges_colors(colors.begin(),colors.end());
}

Color Buffered_viewer_instructor::edge_color(Edge_handle eh) const{
	if(view_params_.edge_mode == View_parameter::E_BORDER && is_border_edge(mesh_ctxt_.mesh,eh))
		return Color(0,.1,1);
	else
		return Color(view_params_.light_edges,view_params_.light_edges,view_params_.light_edges);
}

void Buffered_viewer_instructor::load_triangles(){
	if(mesh_ctxt_.mesh.num_vertices()>=4){
		std::list<std::tuple<unsigned,unsigned,unsigned>> triangles;
		for(auto th : mesh_ctxt_.mesh.triangle_range()){
			auto t_it = th.begin();
			Vertex_handle v0 = *(t_it++);
			Vertex_handle v1 = *(t_it++);
			Vertex_handle v2 = *(t_it);
			triangles.push_back(std::make_tuple(vh_to_index_[v0],vh_to_index_[v1],vh_to_index_[v2]));
		}
		viewer_->load_new_triangles(triangles.begin(),triangles.end());
		load_triangles_colors();
	}
}

void Buffered_viewer_instructor::load_triangles_colors(){
	if(mesh_ctxt_.mesh.num_vertices()>=3){
		std::list<Color> colors;
		for(auto th : mesh_ctxt_.mesh.triangle_range())
			colors.push_back(triangle_color(th));
		viewer_->load_new_triangles_colors(colors.begin(),colors.end());
	}
}

Color Buffered_viewer_instructor::triangle_color(Simplex_handle fit) const{
	assert(fit.dimension()==2);

	Vertex_handle vertex;
	CGAL::Color primitive_color;
	typedef Complex::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	Planar_proxy_handle proxy;
	int num_proxy_vertices;

	switch (view_params_.triangle_mode) {

	case View_parameter::T_SIMPLE:
		return Color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
		break;
	case View_parameter::T_PROXY_COLOR:
		proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt_.mesh,fit);
		if(proxy)
			return Color( proxy->color());
		else
			return Color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
		break;
	case View_parameter::T_ONLY_PROXY:
		num_proxy_vertices = std::count_if(
				fit.begin(),
				fit.end(),
				[&](Vertex_handle v){return !mesh_ctxt_.mesh[v].empty_plane_primitives();}
		);
		if( num_proxy_vertices >= 3 ){
			proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt_.mesh,fit);
			if(proxy)
				return Color(proxy->color());
		}
		break;
	default:
		break;
	}
	return Color(1,1,1);
}




void Buffered_viewer_instructor::sceneChanged(){
	init_buffer();
	viewer_->update_GL();
}


void Buffered_viewer_instructor::snap_shot() const{
	viewer_->setSnapshotQuality(95);
	std::cerr<<(QString::number(mesh_ctxt_.mesh.num_vertices())).toStdString()<<endl;
	viewer_->setSnapshotFileName(
			"../snapshot/scene_"+QString::number(mesh_ctxt_.mesh.num_vertices())+".jpg"
	);
	viewer_->saveSnapshot();
}


void Buffered_viewer_instructor::update_view_params(){
	viewer_->set_draw_points(view_params_.vertex_mode);
	viewer_->set_draw_edges(view_params_.edge_mode);
	viewer_->set_draw_triangles(view_params_.triangle_mode);
	load_vertices_colors();
	load_edges_colors();
	load_triangles_colors();
}


void Buffered_viewer_instructor::change_draw_vertices(){
	view_params_.change_vertex_mode();
	viewer_->set_draw_points(view_params_.vertex_mode);
	load_vertices_colors();
}

void Buffered_viewer_instructor::change_draw_edges(){
	view_params_.change_edge_mode();
	viewer_->set_draw_edges(view_params_.edge_mode);
	load_edges_colors();
}

void Buffered_viewer_instructor::change_draw_triangles(){
	view_params_.change_triangle_mode();
	viewer_->set_draw_triangles(view_params_.triangle_mode);
	load_triangles_colors();
}

void Buffered_viewer_instructor::change_draw_primitives(){
	view_params_.draw_primitives=!view_params_.draw_primitives;
}


void Buffered_viewer_instructor::change_draw_primitives_graph(){
	view_params_.draw_graph_primitives=!view_params_.draw_graph_primitives;
}

void Buffered_viewer_instructor::change_draw_primitives_border(){
	view_params_.draw_primitives_border = !view_params_.draw_primitives_border;
}

void Buffered_viewer_instructor::change_draw_primitives_corner(){
	view_params_.change_corner_mode();
}

void Buffered_viewer_instructor::change_draw_best_edge(bool value){
	view_params_.draw_best_edge=value;
	viewer_->update_GL();
}

void Buffered_viewer_instructor::change_draw_new_point(bool value){
	view_params_.draw_new_point=value;
	viewer_->update_GL();
}

void Buffered_viewer_instructor::change_light(){
	view_params_.light =! view_params_.light ;
}









#include "Buffered_viewer_instructor.moc"



