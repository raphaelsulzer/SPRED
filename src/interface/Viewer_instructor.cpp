/*
 * Viewer_instructor.cpp
 *
 *  Created on: Mar 11, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#include "Viewer_instructor.h"

#include "urban_utils.h"

#include "complex/complex_tools.h"
#include "geometry/geometry_tools.h"
#include <vector>
#include <CGAL/bounding_box.h>
#include <QGLViewer/vec.h>

typedef Kernel::Iso_cuboid_3 bounding_3;


Viewer_instructor::Viewer_instructor(QWidget* parent_,
		Viewer* viewer_,
		Mesh_context& mesh_ctxt_,
		//		Complex *mesh_,
		//		Enriched_mesh<Complex>* segmentation_,
		//		//		Collapsor* collapsor_,
		//		Primitive_graph_ * primitive_graph_,
		boost::optional< std::pair<Point,Point> > *best_edge_,
		boost::optional< Point > * new_point_
)
: QWidget(parent_),viewer(viewer_),
  view_params_(),
  mesh_ctxt(mesh_ctxt_),
  //  mesh(mesh_),segmentation(segmentation_), primitives_graph(primitive_graph_),
  best_edge(best_edge_),new_point(new_point_)

{
	//	QObject::connect(this, SIGNAL(click(const Point&)), parent_, SLOT(click(const Point&)));

	//	view_params_.draw_vertices = true;
	//	view_params_.draw_edges = false;
	//	view_params_.draw_border_edges = false;
	//	view_params_.draw_triangles = false;
	//	view_params_.light = true;
	//	view_params_.draw_primitives = true;
	//	view_params_.draw_graph_primitives = false;
	//	view_params_.draw_primitives_border = false;
	//	view_params_.draw_primitives_corner = false;
	//	view_params_.draw_best_edge=true;
	//	view_params_.draw_new_point=true;
}



void Viewer_instructor::initialize_bounding_box(){
	//TODO il doit y avoir une fonction pour faire ca direct!!
	std::list < Point > points_temp2;
	for( auto vi: mesh_ctxt.mesh.vertex_range())
	{
		Point& ptrt(mesh_ctxt.mesh.point(vi));
		points_temp2.push_back(ptrt);
	}

	bounding_3 bb = CGAL::bounding_box(points_temp2.begin(), points_temp2.end());
	cout << "Bounding box: "<< bb<<endl;

	//xxx
	//this->camera()->setSceneBoundingBox(qglviewer::Vec(bb.xmin(), bb.ymin(), bb.zmin()),
	viewer->set_bounding_box(bb);

	viewer->init_scene();

}

void Viewer_instructor::show_entire_scene(){
	viewer->show_entire_scene();
}

qglviewer::Camera*
Viewer_instructor::camera() const{
	return viewer->camera();
}

int
Viewer_instructor::width() const{
	return viewer->width();
}
int
Viewer_instructor::height() const{
	return viewer->height();
}

void
Viewer_instructor::set_dimension(int width,int height){
	viewer->resize(width,height);
}


void Viewer_instructor::sceneChanged(){
	//xxx
	//	this->updateGL();
	viewer->update_GL();
}


void Viewer_instructor::snap_shot() const{
	viewer->setSnapshotQuality(95);
	std::cerr<<(QString::number(mesh_ctxt.mesh.num_vertices())).toStdString()<<endl;
	viewer->setSnapshotFileName(
			"../snapshot/scene_"+QString::number(mesh_ctxt.mesh.num_vertices())+".jpg"
	);
	viewer->saveSnapshot();
}

void Viewer_instructor::change_draw_vertices(){
	view_params_.change_vertex_mode();
}

void Viewer_instructor::change_draw_edges(){
	view_params_.change_edge_mode();
}

void Viewer_instructor::change_draw_triangles(){
	view_params_.change_triangle_mode();
}

void Viewer_instructor::change_draw_primitives(){
	view_params_.draw_primitives=!view_params_.draw_primitives;
}


void Viewer_instructor::change_draw_primitives_graph(){
	view_params_.draw_graph_primitives=!view_params_.draw_graph_primitives;
}

void Viewer_instructor::change_draw_primitives_border(){
	view_params_.draw_primitives_border = !view_params_.draw_primitives_border;
}

void Viewer_instructor::change_draw_primitives_corner(){
	view_params_.change_corner_mode();
}

void Viewer_instructor::change_draw_best_edge(bool value){
	view_params_.draw_best_edge=value;
	viewer->update_GL();
}

void Viewer_instructor::change_draw_new_point(bool value){
	view_params_.draw_new_point=value;
	viewer->update_GL();
}

void Viewer_instructor::change_light(){
	view_params_.light =! view_params_.light ;
}



void Viewer_instructor::give_instructions()
{
	if(view_params_.relative_light)
		viewer->set_light_direction();
	else
		viewer->set_light_direction(view_params_.theta,view_params_.phi);

	viewer->set_light();

	// define material
	if (mesh_ctxt.mesh.num_vertices()!=0){
		if (view_params_.edge_mode) gl_draw_edges(0.,0.,0.,view_params_.size_edges/2.);
		if (view_params_.triangle_mode)	gl_draw_triangles(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
		if (view_params_.vertex_mode) gl_draw_points(1,0,0,view_params_.size_vertices);

		if (view_params_.draw_best_edge && *best_edge)
			gl_draw_edge((*best_edge)->first,(*best_edge)->second,0,255,0,view_params_.size_edges*1.5);
		if (view_params_.draw_new_point && *new_point) gl_draw_point(**new_point,0,255,255,view_params_.size_vertices);

		if (view_params_.draw_new_point&& placement_selected_edge) gl_draw_point(*placement_selected_edge,0,255,255,view_params_.size_vertices*6);
		if (view_params_.draw_best_edge && selected_edge)	gl_draw_edge((selected_edge)->first,(selected_edge)->second,0,255,0,view_params_.size_edges);

		if (view_params_.draw_graph_primitives) gl_draw_primitive_graph();
		if (view_params_.draw_primitives){
			gl_draw_primitives();
		}
		if (view_params_.draw_primitives_border) {
			gl_draw_primitives_offset_border();
			gl_draw_primitives_border_edges();
		}
		if (view_params_.corner_mode){
			gl_draw_primitives_corners();
		}
	}
}


void Viewer_instructor::gl_draw_point(Point const & p,float red,float green,float blue,double size){
	viewer->draw_point(p,red,green,blue,size);
}

void Viewer_instructor::gl_draw_points(float red,float green,float blue,double size){
	viewer->begin_draw_points(red,green,blue,size);

	for( auto vi : mesh_ctxt.mesh.vertex_range())
	{
		viewer->set_size_point(size);
		set_color_vertex(vi);
		viewer->draw_points(mesh_ctxt.mesh.point(vi));
	}
	viewer->end_draw_points();
}

void Viewer_instructor::gl_draw_edge(const Point& a,const Point& b,float red,float green,float blue,double size){
	viewer->draw_edge(a,b,red,green,blue,size);
}

void Viewer_instructor::gl_draw_edges(float red,float green,float blue,double size){
	viewer->begin_draw_edges(red,green,blue,size,false);

	for(auto edge : mesh_ctxt.mesh.edge_range()){
		set_color_edge(edge);
		Point& a = mesh_ctxt.mesh.point(mesh_ctxt.mesh.first_vertex(edge));
		Point& b = mesh_ctxt.mesh.point(mesh_ctxt.mesh.second_vertex(edge)) ;
		viewer->draw_edges(a,b);
	}

	viewer->end_draw_edges();
}

void Viewer_instructor::gl_draw_triangles(float red,float green,float blue)
{
	const double size_triangles = 1.0;
	typedef Complex::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	Planar_proxy_handle proxy = 0;
	int num_proxy_vertices;

	viewer->begin_draw_triangles(red,green,blue,size_triangles,view_params_.light);

	for(const auto& fit : mesh_ctxt.mesh.triangle_range()) {
		bool draw_triangle = true;
		switch (view_params_.triangle_mode) {

		case View_parameter::T_SIMPLE:
			viewer->set_color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
			break;
		case View_parameter::T_PROXY_COLOR:
			proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt.mesh,fit);
			if(proxy)
				viewer->set_color( proxy->color());
			else
				viewer->set_color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
			break;
		case View_parameter::T_ONLY_PROXY:
			num_proxy_vertices = std::count_if(
					fit.begin(),
					fit.end(),
					[&](Vertex_handle v){return !mesh_ctxt.mesh[v].empty_plane_primitives();}
			);
			if( num_proxy_vertices >= 3 ){
				proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt.mesh,fit);
				if(proxy){
					viewer->set_color( proxy->color());
				}
			}
			else
				draw_triangle = false;
			break;
		default:
			draw_triangle = false;
			break;
		}

		if(draw_triangle){
			auto fit_it  = fit.begin();

			if(view_params_.triangle_mode != View_parameter::T_ONLY_PROXY || !proxy){
				Point& p1 = mesh_ctxt.mesh.point(*fit_it);
				Point& p2 = mesh_ctxt.mesh.point(*(++fit_it));
				Point& p3 = mesh_ctxt.mesh.point(*(++fit_it));
				Vector triangle_normal = CGAL::unit_normal(p1,p2,p3);
				viewer->draw_triangles(p1,p2,p3,triangle_normal);
			}
			else{
				Point p1 = proxy->expand(proxy->project(mesh_ctxt.mesh.point(*fit_it)));
				Point p2 = proxy->expand(proxy->project(mesh_ctxt.mesh.point(*(++fit_it))));
				Point p3 = proxy->expand(proxy->project(mesh_ctxt.mesh.point(*(++fit_it))));
				Vector triangle_normal = CGAL::unit_normal(p1,p2,p3);
				viewer->draw_triangles(p1,p2,p3,triangle_normal);
			}

		}
	}
	viewer->end_draw_triangles();
}


void
Viewer_instructor::set_color_vertex(Vertex_handle vh){
	//	V_SIMPLE,V_PROXY_NUMBER,V_PROXY_COLOR
	switch (view_params_.vertex_mode) {
	case View_parameter::V_SIMPLE:
		viewer->set_color(.5,.5,.6);
		//		viewer->set_color(1,0,0);
		break;
	case View_parameter::V_PROXY_NUMBER:
		switch ((mesh_ctxt.mesh)[vh].num_plane_primitives()) {
		case 0:
			viewer->set_color(1,0,0);
			break;
		case 1:
			viewer->set_color(.1,.1,0.1);
			break;
		case 2:
			viewer->set_color(0,0,1);
			break;
		case 3:
			viewer->set_size_point(view_params_.size_vertices);
			viewer->set_color(1,1,0);
			break;
		default:
			viewer->set_size_point(view_params_.size_vertices);
			viewer->set_color(0,1,1);
			break;
		}
		break;
		case View_parameter::V_PROXY_COLOR:
			if(!(mesh_ctxt.mesh)[vh].empty_plane_primitives()){
				auto primitive_color = (*(mesh_ctxt.mesh)[vh].plane_primitives().begin())->color();
				viewer->set_color(primitive_color);
			}
			else{
				viewer->set_color(1,0,0);
			}
			break;
		default:
			break;
	}
}

void
Viewer_instructor::set_color_edge(Edge_handle eh){
	if(view_params_.edge_mode == View_parameter::E_BORDER && is_border_edge(mesh_ctxt.mesh,eh))
		viewer->set_color(0,.1,1);
	else viewer->set_color(view_params_.light_edges,view_params_.light_edges,view_params_.light_edges);

}




bool
Viewer_instructor::set_color_triangle(const Simplex_handle& triangle){
	Vertex_handle vertex;
	CGAL::Color primitive_color;
	typedef Complex::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	Planar_proxy_handle proxy;
	int num_proxy_vertices;

	switch (view_params_.triangle_mode) {
	case View_parameter::T_SIMPLE:
		viewer->set_color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
		return true;
		//	case View_parameter::T_PROXY_COLOR_SIMPLE:
		//		vertex = *(triangle.begin());
		//		if(!(*mesh)[vertex].empty_plane_primitives()){
		//			primitive_color = (*(*mesh)[vertex].primitives().begin())->color();
		//			viewer->set_color(primitive_color);
		//		}
		//		else {
		//			viewer->set_color(0.5,0.5,0.5);
		//		}
		//
		//		break;
	case View_parameter::T_PROXY_COLOR:
		proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt.mesh,triangle);
		if(proxy)
			viewer->set_color( proxy->color());
		else
			viewer->set_color(view_params_.light_triangles,view_params_.light_triangles,view_params_.light_triangles);
		return true;

	case View_parameter::T_ONLY_PROXY:
		//		proxy = get_majority_proxy_from_triangle<Complex>(*mesh,triangle);
		num_proxy_vertices = std::count_if(
				triangle.begin(),
				triangle.end(),
				[&](Vertex_handle v){return !mesh_ctxt.mesh[v].empty_plane_primitives();}
		);
		if( num_proxy_vertices == 3 ){
			proxy = get_best_proxy_for_triangle_normal<Complex>(mesh_ctxt.mesh,triangle);
			if(proxy){
				viewer->set_color( proxy->color());
				return true;
			}
		}
		return false;
		break;
	default:
		return false;
		break;
	}
}



void
Viewer_instructor::gl_draw_primitive_graph(){
	typedef Primitive_graph_::Graph_vertex_iterator Graph_vertex_iterator;
	typedef Primitive_graph_::Graph_edge_iterator Graph_edge_iterator;

	Graph_vertex_iterator vi, vi_end;
	Graph_edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end) = mesh_ctxt.primitive_graph.edges(); ei != ei_end; ++ei){
		auto v1 = mesh_ctxt.primitive_graph.source(*ei);
		auto v2 = mesh_ctxt.primitive_graph.target(*ei);
		if(mesh_ctxt.primitive_graph.is_active(v1) && mesh_ctxt.primitive_graph.is_active(v2)){
			Point p1(mesh_ctxt.primitive_graph.primitive(v1)->centroid());
			Point p2(mesh_ctxt.primitive_graph.primitive(v2)->centroid());
			gl_draw_edge(p1,p2,0.8,0.8,0.9,2.0);
		}
	}

	for (boost::tie(vi, vi_end) = mesh_ctxt.primitive_graph.vertices(); vi != vi_end; ++vi){
		if(mesh_ctxt.primitive_graph.is_active(*vi)){
			auto pt = mesh_ctxt.primitive_graph.primitive(*vi);
			gl_draw_point(pt->centroid(),pt->color().r()/(double)255,pt->color().g()/(double)255,pt->color().b()/(double)255,10);
		}
	}
}


void
Viewer_instructor::gl_draw_primitives_offset_border(){
	// todo xxx berk clean up
	for(auto primitive = mesh_ctxt.segmentation.planes.begin() ; primitive != mesh_ctxt.segmentation.planes.end(); ++primitive){
		auto primitive_pt = &(*primitive);
		auto list_border_vertices = mesh_ctxt.segmentation.get_border_from_proxy(primitive_pt)->border_polygon_vertices();
		if(list_border_vertices.size()>=3){
			//			if(list_border_vertices.front() == list_border_vertices.back()){
			auto color = primitive_pt->color();
			double r = color.r()/255.,
					g = color.g()/255.,
					b = color.b()/255.;

			viewer->begin_draw_edges(r,g,b,2.0,false);
			auto vi = list_border_vertices.begin();

			const Point &first_point(mesh_ctxt.mesh.point(* list_border_vertices.begin()));
			const Point &last_point(mesh_ctxt.mesh.point((* (--list_border_vertices.end()))));
			viewer->draw_edges(first_point,last_point);
			auto next_vi = vi; ++next_vi;
			do {
				viewer->draw_edges(mesh_ctxt.mesh.point(*vi),mesh_ctxt.mesh.point(*next_vi));
				++vi;
				++next_vi;
			} while (next_vi!=list_border_vertices.end());
			viewer->end_draw_edges();
		}
	}
}


void
Viewer_instructor::gl_draw_primitives_border_edges(){
	// xxx berk faire mieux
	for(auto primitive = mesh_ctxt.segmentation.planes.begin() ; primitive != mesh_ctxt.segmentation.planes.end(); ++primitive){
		auto primitive_pt = &(*primitive);


		auto proxy_border( mesh_ctxt.segmentation.get_border_from_proxy(primitive_pt) );

		//		if(proxy_border->num_border_edges()>=3){
		//			auto color = primitive_pt->color();
		//			double r = color.r()/255.,
		//					g = color.g()/255.,
		//					b = color.b()/255.;
		//
		//			viewer->begin_draw_edges(r,g,b,4.0,false);
		//			for(auto e = proxy_border->begin_border_edges();  e!= proxy_border->end_border_edges(); ++e){
		//				viewer->draw_edges( (*mesh).point( (*mesh).first_vertex(*e) ),(*mesh).point( (*mesh).second_vertex(*e)));
		//			}
		//
		//			viewer->end_draw_edges();
		//		}

		if(proxy_border->num_border_edges()>=3){
			auto color = primitive_pt->color();
			double r = color.r()/255.,
					g = color.g()/255.,
					b = color.b()/255.;

			viewer->begin_draw_points(r,g,b,4.0);
			for(auto point = proxy_border->begin_border_edges_points();  point!= proxy_border->end_border_edges_points(); ++point){
				viewer->draw_points(*point);
			}

			viewer->end_draw_points();
		}


		//		auto list_border_edges = mesh_ctxt.segmentation.proxy_to_border[primitive_pt]->border_edges_;
		//		if(list_border_edges.size()>=3){
		//			auto color = primitive_pt->color();
		//			double r = color.r()/255.,
		//					g = color.g()/255.,
		//					b = color.b()/255.;
		//
		//			viewer->begin_draw_edges(r,g,b,4.0,false);
		//			for(auto e : list_border_edges){
		//				viewer->draw_edges( (*mesh).point( (*mesh).first_vertex(e) ),(*mesh).point( (*mesh).second_vertex(e)));
		//			}
		//
		//			viewer->end_draw_edges();
		//		}
	}
}

void
Viewer_instructor::gl_draw_primitives(){

	viewer->begin_draw_triangles(0.,0.,0.,2.0,true);

	for(auto primitive = mesh_ctxt.segmentation.planes.begin() ; primitive != mesh_ctxt.segmentation.planes.end(); ++primitive){
		auto primitive_pt = &(*primitive);


		std::list < Point_2 > points_temp;
		for( auto vh =  primitive_pt->begin_initial_vertices() ;
				vh != primitive_pt->end_initial_vertices();
				++vh){
			points_temp.push_back(primitive_pt->project(mesh_ctxt.mesh.point(*vh)));
		}

		auto bbox = CGAL::bounding_box(points_temp.begin(), points_temp.end());

		Point_2 bbox1(bbox.xmin(), bbox.ymin());
		Point_2 bbox2(bbox.xmin(),bbox.ymax());
		Point_2 bbox3(bbox.xmax(),bbox.ymax());
		Point_2 bbox4(bbox.xmax(),bbox.ymin());

		Point p1(primitive_pt->expand(bbox1));
		Point p2(primitive_pt->expand(bbox2));
		Point p3(primitive_pt->expand(bbox3));
		Point p4(primitive_pt->expand(bbox4));

		viewer->set_color(primitive_pt->color());

		Vector triangle_normal = CGAL::unit_normal(p1,p2,p3);
		viewer->draw_triangles(p1,p2,p3,triangle_normal);
		viewer->draw_triangles(p1,p3,p4,triangle_normal);
	}
	viewer->end_draw_triangles();

}


void
Viewer_instructor::gl_draw_primitives_corners(){
	//	viewer->begin_draw_points(1,1,0,7.0);
	//	int num_corners = 0;
	//	for(auto vi : mesh_ctxt.mesh.vertex_range()){
	//		auto vertex((mesh_ctxt.mesh)[vi]);
	//		if(vertex.is_corner()){
	//			viewer->draw_points(vertex.corner_position());
	//			++num_corners;
	//		}
	//	}
	//	viewer->end_draw_points();


	if(view_params_.corner_mode==View_parameter::C_SIMPLE){
		viewer->begin_draw_points(1,1,0,7.0);
		for(auto corner = mesh_ctxt.primitive_graph.begin_corners();
				corner != mesh_ctxt.primitive_graph.end_corners();
				++corner){

			viewer->draw_points(mesh_ctxt.mesh.point(corner->first));
		}
		viewer->end_draw_points();
	}

	if(view_params_.corner_mode==View_parameter::C_LINKED){

		viewer->begin_draw_edges(1.,1.,0.,1);
		for(auto corner = mesh_ctxt.primitive_graph.begin_corners();
				corner != mesh_ctxt.primitive_graph.end_corners();
				++corner){
			viewer->draw_edges((corner->second->intersection_point()),mesh_ctxt.mesh.point(corner->first));
		}
		viewer->end_draw_edges();


		viewer->begin_draw_points(1,1,0,7.0);
		for(auto corner = mesh_ctxt.primitive_graph.begin_corners();
				corner != mesh_ctxt.primitive_graph.end_corners();
				++corner){

			viewer->draw_points(mesh_ctxt.mesh.point(corner->first));
		}
		viewer->end_draw_points();
	}

	if(view_params_.corner_mode==View_parameter::C_DUAL){
		viewer->begin_draw_edges(0,1,0,5,false);
		typedef Mesh_context::Dual_graph::Dual_graph_edge_iterator Dual_graph_edge_iterator;
		Dual_graph_edge_iterator e,e_end;
		for(boost::tie(e,e_end) = mesh_ctxt.dual_graph.edges_iterator();
				e!=e_end;
				++e){
			viewer->draw_edges(
					mesh_ctxt.dual_graph.first_vertex(*e).point,
					mesh_ctxt.dual_graph.second_vertex(*e).point
			);
		}
		viewer->end_draw_edges();


		viewer->begin_draw_points(0,0,1,15);
		typedef Mesh_context::Dual_graph::Dual_graph_vertex_iterator Dual_graph_vertex_iterator;
		Dual_graph_vertex_iterator v,v_end;
		for(boost::tie(v,v_end) = mesh_ctxt.dual_graph.vertices_iterator();
				v!=v_end;
				++v){
			auto dual_vertex = mesh_ctxt.dual_graph.vertex(*v);
			if(dual_vertex.clique.size()==3)
				viewer->set_color(0,0,1);
			if(dual_vertex.clique.size()==4)
				viewer->set_color(1,0,0);
			if(dual_vertex.clique.size()>=5)
				viewer->set_color(0,1,0);
			viewer->draw_points(dual_vertex.point);
		}
		viewer->end_draw_points();

	}
}



#include "Viewer_instructor.moc"
