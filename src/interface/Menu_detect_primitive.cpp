/*
 * Menu_detect_primitive.cpp
 *
 *  Created on: 30 janv. 2014
 *      Author: dsalinas
 */

#include "Menu_detect_primitive.h"

using namespace std;

Menu_detect_primitive::Menu_detect_primitive(MainWindow* parent_):parent(parent_)
{
	setupUi(this);
	connectActions(parent_);

	// set nb of vertices in slider and label
	//this->txt_nb_collapses->setNum((int)1);
	//update_nb_collapse(1);
}

void Menu_detect_primitive::connectActions(MainWindow* parent){
	QObject::connect(this->pushButton_detect,SIGNAL(clicked()),this,SLOT(send_detect_primitives()));
	QObject::connect(this->pushButton_regularize,SIGNAL(clicked()),this,SLOT(send_regularize_primitives()));
	QObject::connect(this->pushButton_recompute_graph,SIGNAL(clicked()),parent,SLOT(recompute_graph()));
	QObject::connect(
			this,
			SIGNAL(detect_primitives(const Detection_params&)),
			parent,
			SLOT(detect_primitives(const Detection_params&))
	);

	QObject::connect(
			this,
			SIGNAL(regularize_primitives(const Merge_proxies_params&)),
			parent,
			SLOT(regularize_primitives(const Merge_proxies_params&))
	);
}


Detection_params Menu_detect_primitive::detect_params() const{
	return Detection_params(
			doubleSpinBox_maxdist->value(),
			doubleSpinBox_max_angle->value(),
			doubleSpinBox_min_area->value(),
			comboBox_detect_method->currentText().toStdString());
}

Merge_proxies_params Menu_detect_primitive::merge_params() const{
	return Merge_proxies_params(
			doubleSpinBox_coplanarity->value(),
			doubleSpinBox_max_distance->value(),
			doubleSpinBox_min_area_filter->value(),
			doubleSpinBox_min_area_filter->value()
	);
}




void
Menu_detect_primitive::send_detect_primitives(){
	//	double tolerance_error = doubleSpinBox_tolerance_error->value();
	emit(detect_primitives(detect_params()));
}

void
Menu_detect_primitive::send_regularize_primitives(){
	emit(regularize_primitives(merge_params()));
}


#include "Menu_detect_primitive.moc"




