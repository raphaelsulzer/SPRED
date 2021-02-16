#ifndef MENU_EDGE_COLLAPSE_H
#define MENU_EDGE_COLLAPSE_H

#include <CGAL/Qt/DemosMainWindow.h>
#include <QSlider>
#include <QFileDialog>
#include <string>
#include "interface/MainWindow.h"
#include "interface/ui_Menu_edge_collapse.h"


class QWidget;
class MainWindow;


class Menu_edge_collapse : public QDialog, public Ui::Menu_edge_collapse
{
	Q_OBJECT
private:	
	MainWindow* parent;

	int num_collapses;
	int num_vertices;

public:	

	Menu_edge_collapse(MainWindow* parent_);

	void connectActions(MainWindow* parent);

	//void get_and_update_best_edge();

	public slots:

	/**
	 * to be called when the num of vertices
	 * has changed, for example after some collapses.
	 * updates the interface.
	 */
	void update_num_vertices(int num_vertices);

	/**
	 * called when the number of collapse desired
	 * by the user has changed, updates the interface.
	 */
	void update_num_collapses(int new_num_collapses);


	void update_gui_numbers();

	/**
	 * to be called to update the value of the slider
	 */
	void update_slider_value();

	/**
	 * to be called when the value of this box has
	 * changed
	 */
	void box_value_num_remaining_vertices_changed(int new_value);

	void box_value_num_remaining_vertices_edited();

	/**
	 * to be called when the user has changed the value of the
	 * slider
	 */
	void slider_value_changed(int new_slider_value);

	void collapse_edges();

	signals:
	//	void update_cost(const QString& qstring);
	//	void update_placement(const QString& qstring);
	void collapse_edges(int);
	void show_primitives();
	void show_points();
	void show_heap();
};




#endif

