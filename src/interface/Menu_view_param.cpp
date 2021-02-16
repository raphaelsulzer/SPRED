/*
 * Menu_view_param.cpp
 *
 *  Created on: Jul 31, 2014
 *      Author: dsalinas
 */


#include "Menu_view_param.h"

#include "urban_utils.h"

Menu_view_param::Menu_view_param(MainWindow* parent):parent_(parent)
{
	setupUi(this);
	connectActions(parent);
}

void
Menu_view_param::connectActions(MainWindow* parent){
	QObject::connect(this->horizontalSlider_theta, SIGNAL(valueChanged(int)), this,
			SLOT(update_theta(int)));
	QObject::connect(this->horizontalSlider_phi, SIGNAL(valueChanged(int)), this,
			SLOT(update_phi(int)));

	QObject::connect(this->checkBox_relative_light, SIGNAL(clicked(bool)), this,
					SLOT(update_relative_light(bool)));



	QObject::connect(this->horizontalSlider_vertex_size, SIGNAL(valueChanged(int)), this,
			SLOT(update_vertex_size(int)));


	QObject::connect(this->horizontalSlider_vertex_style, SIGNAL(valueChanged(int)), this,
			SLOT(update_vertex_style(int)));


	QObject::connect(this->horizontalSlider_edge_size, SIGNAL(valueChanged(int)), this,
				SLOT(update_edge_size(int)));
	QObject::connect(this->horizontalSlider_edge_style, SIGNAL(valueChanged(int)), this,
				SLOT(update_edge_style(int)));
	QObject::connect(this->horizontalSlider_edge_luminosity, SIGNAL(valueChanged(int)), this,
					SLOT(update_edge_luminosity(int)));

	QObject::connect(this->horizontalSlider_triangle_luminosity, SIGNAL(valueChanged(int)), this,
					SLOT(update_triangle_luminosity(int)));




}

void
Menu_view_param::set_view_params(View_parameter* view_params){
	view_params_ = view_params;
}


void
Menu_view_param::update_vertex_size(int new_size){
	view_params_->size_vertices = (double)new_size/5.;
	parent_->update_display();
}

void
Menu_view_param::update_vertex_style(int new_style){
	view_params_->change_vertex_mode(new_style);
	parent_->update_display();
}

void
Menu_view_param::update_edge_size(int new_size){
	view_params_->size_edges = (double)new_size/5.;
	parent_->update_display();
}

void
Menu_view_param::update_edge_style(int new_style){
	view_params_->change_edge_mode (new_style);
	parent_->update_display();
}

void
Menu_view_param::update_edge_luminosity(int new_value){
	view_params_->light_edges = (double)new_value/100.;
	parent_->update_display();
}

void
Menu_view_param::update_triangle_luminosity(int new_value){
	view_params_->light_triangles = (double)new_value/100.;
	parent_->update_display();

}



void
Menu_view_param::update_theta(int new_theta){
	view_params_->theta = (double)new_theta*2*3.1415/100.;
	parent_->update_display();
}

void
Menu_view_param::update_phi(int new_phi){
	view_params_->phi = (double)new_phi*3.1415/100.;
	parent_->update_display();
}

void
Menu_view_param::update_relative_light(bool b){
	view_params_->relative_light = b;
	parent_->update_display();
}



#include "Menu_view_param.moc"
