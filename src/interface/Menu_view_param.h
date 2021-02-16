/*
 * Menu_view_param.h
 *
 *  Created on: Jul 31, 2014
 *      Author: dsalinas
 */

#ifndef MENU_VIEW_PARAM_H_
#define MENU_VIEW_PARAM_H_



#include <CGAL/Qt/DemosMainWindow.h>
#include <QSlider>
#include <QFileDialog>
#include <string>
#include "interface/MainWindow.h"
#include "interface/ui_Menu_view_param.h"
#include "interface/View_parameter.h"

class QWidget;
class MainWindow;


class Menu_view_param : public QDialog, public Ui::Menu_view_param
{
	Q_OBJECT
	private:
		MainWindow* parent_;
		View_parameter* view_params_;

	public:
		Menu_view_param(MainWindow* parent_);

		void set_view_params(View_parameter* view_params);

		void connectActions(MainWindow* parent);


		public slots:


		void update_vertex_size(int new_size);
		void update_vertex_style(int new_style);

		void update_edge_size(int new_size);
		void update_edge_style(int new_style);
		void update_edge_luminosity(int new_value);

		void update_triangle_luminosity(int new_value);

		void update_theta(int new_theta);
		void update_phi(int new_theta);

		void update_relative_light(bool);


		signals:

};


#endif /* MENU_VIEW_PARAM_H_ */
