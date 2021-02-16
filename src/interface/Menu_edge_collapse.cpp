#include "Menu_edge_collapse.h"

using namespace std;
Menu_edge_collapse::Menu_edge_collapse(MainWindow* parent_):parent(parent_),num_collapses(0)
{
	setupUi(this);
	connectActions(parent_);

	// set nb of vertices in slider and label
	//	this->txt_nb_collapses->setNum((int)1);
	//update_nb_collapse(1);		


}

void Menu_edge_collapse::connectActions(MainWindow* parent){
	QObject::connect(this->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slider_value_changed(int)));
	QObject::connect(this->spinBox_nb_remaining_vertices, SIGNAL(valueChanged(int)), this, SLOT(box_value_num_remaining_vertices_changed(int)));
	QObject::connect(this->spinBox_nb_remaining_vertices, SIGNAL(editingFinished()), this, SLOT(box_value_num_remaining_vertices_edited()));


	QObject::connect(this->m_simplificationMethod, SIGNAL(currentIndexChanged(const QString&)), parent, SLOT(update_policy(const QString&)));
	QObject::connect(this->pushButton_collapse, SIGNAL(clicked()), this, SLOT(collapse_edges()));
	QObject::connect(this, SIGNAL(collapse_edges(int)), parent, SLOT(collapse_edges(int)));	

	//connect check boxes
	QObject::connect(this->checkBox_link_condition,SIGNAL(clicked(bool)),parent,SLOT(update_parameters(bool)));
	QObject::connect(this->checkBox_preserve_normals,SIGNAL(clicked(bool)),parent,SLOT(update_parameters(bool)));

	QObject::connect(this->doubleSpinBox_preserve_normals,SIGNAL(valueChanged(double)),parent,SLOT(update_parameters(double)));
	QObject::connect(this->doubleSpinBox_boundary_weight, SIGNAL(valueChanged(double)), parent, SLOT(update_parameters(double)));
	QObject::connect(this->doubleSpinBox_abstraction_weight, SIGNAL(valueChanged(double)), parent, SLOT(update_parameters(double)));
}



void Menu_edge_collapse::update_num_vertices(int new_num_vertices){
	if(num_vertices!=0) num_collapses *=  new_num_vertices/(double)num_vertices;
	num_collapses = (std::max)(1,num_collapses);
	num_vertices = new_num_vertices;
	update_gui_numbers();
}

void Menu_edge_collapse::update_num_collapses(int new_num_collapses){
	num_collapses = new_num_collapses;
	update_gui_numbers();
}

void Menu_edge_collapse::update_gui_numbers(){
	update_slider_value();
	this->txt_nb_vertices->setNum(num_vertices);
	this->txt_nb_collapses->setNum(num_collapses);
	this->spinBox_nb_remaining_vertices->setValue(num_vertices-num_collapses);
}

void Menu_edge_collapse::update_slider_value(){
	int horizontal_slider_position = num_vertices>0?  num_collapses/(double)num_vertices * 100 : 1  ;
	horizontalSlider->setValue(horizontal_slider_position);
}

void Menu_edge_collapse::slider_value_changed(int new_slider_value){
	num_collapses = (horizontalSlider->value()==1)? 1 : horizontalSlider->value() * num_vertices / 100 ;

	this->txt_nb_vertices->setNum(num_vertices);
	this->txt_nb_collapses->setNum(num_collapses);
	this->spinBox_nb_remaining_vertices->setValue(num_vertices-num_collapses);
}


void Menu_edge_collapse::box_value_num_remaining_vertices_changed(int new_value_num_remaining_vertices){
	num_collapses = num_vertices - new_value_num_remaining_vertices;

	// we block horizontalSlider signals otherwise it would modify
	// num_collapse value by rounding
	bool oldState = horizontalSlider->blockSignals(true);
	update_slider_value();
	horizontalSlider->blockSignals(oldState);

	this->txt_nb_vertices->setNum(num_vertices);
	this->txt_nb_collapses->setNum(num_collapses);
}

void Menu_edge_collapse::box_value_num_remaining_vertices_edited(){

}

void Menu_edge_collapse::collapse_edges(){
//	int num_collapses = (horizontalSlider->value()==1)? 1 : horizontalSlider->value() * parent->num_vertices() / 100 ;
	num_collapses = (std::max)(1,num_collapses);
	emit(collapse_edges(num_collapses));
}

#include "Menu_edge_collapse.moc"
