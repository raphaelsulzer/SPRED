/*
 * Menu_detect_primitives.h
 *
 *  Created on: 30 janv. 2014
 *      Author: dsalinas
 */

#ifndef MENU_DETECT_PRIMITIVES_H_
#define MENU_DETECT_PRIMITIVES_H_

#include <CGAL/Qt/DemosMainWindow.h>
#include <QSlider>
#include <QFileDialog>
#include <string>

#include "semantic/Detection_params.h"
#include "semantic/Merge_proxies_params.h"
#include "interface/MainWindow.h"
#include "interface/ui_Menu_detect_primitive.h"

class QWidget;
class MainWindow;


class Menu_detect_primitive : public QDialog, public Ui::Menu_detect_primitive
{
	Q_OBJECT
private:
	MainWindow* parent;

public:

	Menu_detect_primitive(MainWindow* parent_);

	void connectActions(MainWindow* parent);

	//void get_and_update_best_edge();

	Detection_params detect_params() const;
	Merge_proxies_params merge_params() const;

	public slots:
	void send_detect_primitives();
	void send_regularize_primitives();


	signals:
	void detect_primitives(const Detection_params& params);
	void regularize_primitives(const Merge_proxies_params& params);

};


#endif /* MENU_DETECT_PRIMITIVES_H_ */
