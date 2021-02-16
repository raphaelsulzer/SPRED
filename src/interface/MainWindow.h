#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include <string>
#include <CGAL/Qt/DemosMainWindow.h>
#include <QFileDialog>
#include <QSlider>
#include <QKeyEvent>

#include "typedefs.h"
#include "interface/Mesh_context.h"

#include "interface/Viewer_instructor.h"

#include "interface/Buffered_viewer_instructor.h"

#include "interface/Contractor_interface.h"
#include "interface/ui_MainWindow.h"
#include "interface/ui_Menu_edge_collapse.h"

#include "interface/ui_Menu_view_param.h"
#include "interface/Menu_view_param.h"

#include "interface/Menu_edge_collapse.h"
#include "interface/ui_Menu_detect_primitive.h"
#include "interface/Menu_detect_primitive.h"
#include "interface/Contraction_parameter.h"

#include "semantic/Enriched_mesh.h"
#include "semantic/Primitive_graph.h"

#include "contraction/Stats.h"

#include "semantic/Dual_primitive_graph.h"



//#ifndef NDEBUG
//#include <fenv.h>
//feenableexcept(FE_INVALID | FE_OVERFLOW);
//#endif



class QWidget;
class Menu_edge_collapse;
class Menu_detect_primitive;
class Menu_view_param;



class MainWindow : public CGAL::Qt::DemosMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:

	typedef Complex::Point Point;
	typedef Complex::GT GT;
	typedef GT::FT FT;

	// model
	Mesh_context mesh_;



	Dual_primitive_graph<Complex> dual_;


	//view
	//Viewer_instructor* viewer_instructor;
	Buffered_viewer_instructor* viewer_instructor;
	Menu_edge_collapse* menu_collapse ;
	Menu_detect_primitive* menu_detect_primitive;
	Menu_view_param* menu_view_param;


	// 'best_edge'  is uninitialized if the collapse menu is uninitialized
	boost::optional< pair<Point,Point> > best_edge;
	// 'new_point' is uninitialized if either 
	// - the collapse menu is uninitialized OR
	// - the collapse strategy could not place the new point
	boost::optional< Point > new_point;

	Contractor_interface collapsor;

	std::string name_file;

protected:
	void closeEvent(QCloseEvent *event);
public:
	MainWindow(QWidget* parent = 0);
	void connectActions();

	void clear_graph_segmentation();

	void update_best_edge();

	int num_vertices();

private:
void save_camera();
void load_camera(const std::string & name_file);
public:

	public slots:


	/**
	 * open a file chooser to choose an off to load
	 */
	void off_file_open();

	/**
	 * open a file chooser to choose an off to save
	 */
	void off_file_save();

	/**
	 * load an off file given its path
	 */
	void open_off(const QString& fileName);

	/**
	 * open a file chooser to choose a path to a .seg
	 * file to open
	 */
	void proxy_file_open();

	void proxy_file_save();

	/**
	 * open a .seg file given a path
	 */
	void open_proxy_file(const QString& fileName);

	void save_proxy_in_file(const std::string& proxy_name_file);

	/**
	 * Given the extension of the file :
	 * open a .seg file if the file ends by ".seg"
	 * open a .off file if the file ends by ".off"
	 * does nothing otherwise
	 *
	 * launch when file is drag & drop on the window or
	 * when recent file load is clicked
	 */
	void open( QString fileName);



	void click(const Point& point);

	void update_display();

	/**
	 * updates the heap of the collapse algorithm
	 */
	void update();



	void detection_compute_planes();
	void primitives_perturbation();
	void show_menu_view_param();

	void detect_primitives(const Detection_params& detection_params);
	void regularize_primitives(const Merge_proxies_params& merge_params);
	void recompute_graph();

	void expand_primitives();
	void graph_detection();

	void detection_prompt_planes();
	void prompt_detection_params();
	void prompt_contr_params();

	void filter_small_primitives();


	void simplification_project_on_primitives();
	void simplification_collapse_menu();

	void apply_noise();

	void apply_normalise();


	void apply_stats();

	void keep_only_main_CC();


	/**
	 * reread all parameters and tells to the contractor to update the heap
	 */
	void update_parameters();
	void update_parameters(bool);
	void update_parameters(double);

	void update_policy(const QString& qstring);
	void collapse_edges(int);
	void show_complex();

	protected:
	void keyPressEvent(QKeyEvent * e);




	signals:
	void sceneChanged();
	void draw_best_edge(bool);
	void draw_new_point(bool);
	//void update_best_edge(pair< pair<Point,Point>, Point>);
	void update_points();

};

#endif
