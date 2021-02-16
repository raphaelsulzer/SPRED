#include "MainWindow.h"

#include "urban_utils.h"

//#include "Menu_edge_collapse.h"
//#include "Constrain_strategy.h"
//#include "Collapse_strategy.h"
#include "iocomplex.h"
#include "geometry/geometry_tools.h"
#include "geometry/hausdorff/Hausdorff_distance.h"
#include "geometry/hausdorff/Meshlab_hausdorff_distance.h"
#include "geometry/hausdorff/Distance_point_to_triangle.h"


#include <iostream>
#include <fstream>
#include <ctime>
#include <QInputDialog>
#include <QGLViewer/vec.h>
#include <boost/scoped_ptr.hpp>


#include "gudhi/gudhi_contraction.h"

#include "semantic/Smooth_expansion.h"
#include "semantic/Merge_proxies_params.h"
#include "semantic/Graph_detection_params.h"

#include "complex/reorient.h"

using namespace std;

typedef Gudhi::contraction::Edge_profile<Complex> Profile;

void MainWindow::closeEvent(QCloseEvent *event)
{
	// do some data saves or something else
	exit(0);
}

MainWindow::MainWindow(QWidget* parent) :
						CGAL::Qt::DemosMainWindow(parent),
						viewer_instructor(0),
						menu_collapse(new Menu_edge_collapse(this)),
						menu_detect_primitive(new Menu_detect_primitive(this)),
						menu_view_param(new Menu_view_param(this)),
						collapsor(mesh_),name_file()
{
	setupUi(this);
	//	viewer_instructor = new Viewer_instructor(this,
	viewer_instructor = new Buffered_viewer_instructor(this,
			this->viewer,
			mesh_,
			&best_edge,
			&new_point
	);
	update_parameters();


	menu_detect_primitive->hide();
	menu_view_param->hide();
	this->menu_view_param->set_view_params(&(viewer_instructor->view_params()));
	connectActions();

	this->addRecentFiles(this->menuFile, this->actionQuit);

	//nan detection
	//	feenableexcept(FE_INVALID | FE_OVERFLOW);
}

void MainWindow::connectActions()
{
	QObject::connect(this->viewer, SIGNAL(click(const Point&)), this,SLOT(click(const Point&)));

	connect(this, SIGNAL(openRecentFile(QString)), this, SLOT(open(QString)));

	QObject::connect(this->actionLoad_New_File, SIGNAL(triggered()), this,
			SLOT(off_file_open()));


	QObject::connect(this->actionLoad_segmentation, SIGNAL(triggered()), this,
			SLOT(proxy_file_open()));

	QObject::connect(this->actionSave_segmentation, SIGNAL(triggered()), this,
			SLOT(proxy_file_save()));

	QObject::connect(this->actionSave, SIGNAL(triggered()), this,
			SLOT(off_file_save()));

	// connect render option 
	QObject::connect(this, SIGNAL(sceneChanged()), this->viewer_instructor,
			SLOT(sceneChanged()));
	QObject::connect(this->actionVertices, SIGNAL(triggered()),
			this->viewer_instructor, SLOT(change_draw_vertices()));
	QObject::connect(this->actionEdges, SIGNAL(triggered()),
			this->viewer_instructor, SLOT(change_draw_edges()));
	QObject::connect(this->actionTriangles, SIGNAL(triggered()),
			this->viewer_instructor, SLOT(change_draw_triangles()));
	QObject::connect(this->actionPlanar_primitives, SIGNAL(triggered()),
			this->viewer_instructor, SLOT(change_draw_primitives()));
	QObject::connect(this->actionLight, SIGNAL(triggered()),
			this->viewer_instructor, SLOT(change_light()));
	QObject::connect(this->actionMore, SIGNAL(triggered()), this,
			SLOT(show_menu_view_param()));

	// connect simplification menu
	QObject::connect(this->actionEdge_collapse, SIGNAL(triggered()), this,
			SLOT(simplification_collapse_menu()));
	QObject::connect(this->actionProject_on_primitives, SIGNAL(triggered()),
			this, SLOT(simplification_project_on_primitives()));
	QObject::connect(this, SIGNAL(draw_best_edge(bool)),
			this->viewer_instructor, SLOT(change_draw_best_edge(bool)));
	QObject::connect(this, SIGNAL(draw_new_point(bool)),
			this->viewer_instructor, SLOT(change_draw_new_point(bool)));

	QObject::connect(this->actionPlanes, SIGNAL(triggered()), this,
			SLOT(detection_compute_planes()));
	QObject::connect(this->actionPrimitives_expansion,SIGNAL(triggered()),
			this,SLOT(expand_primitives()));


	QObject::connect(this->actionGraph_detection,SIGNAL(triggered()),
			this,SLOT(graph_detection()));



	QObject::connect(this->actionPrimitives_perturbation, SIGNAL(triggered()), this,
			SLOT(primitives_perturbation()));

	QObject::connect(this->actionPromptPrimitives, SIGNAL(triggered()), this,
			SLOT(detection_prompt_planes()));
	QObject::connect(this->actionPrompt_detection_params, SIGNAL(triggered()), this,
			SLOT(prompt_detection_params()));
	QObject::connect(this->actionPrompt_contraction_params, SIGNAL(triggered()), this,
			SLOT(prompt_contr_params()));

	QObject::connect(this->actionPrompt_complex, SIGNAL(triggered()), this,
			SLOT(show_complex()));

	QObject::connect(this->menu_detect_primitive->doubleSpinBox_max_graph_length, SIGNAL(valueChanged(double)),
			this, SLOT(update_parameters(double)));

	QObject::connect(this->menu_detect_primitive->pushButton_small_proxies, SIGNAL(clicked()),
			this, SLOT(filter_small_primitives()));


	QObject::connect(this->actionNoise, SIGNAL(triggered()), this,
			SLOT(apply_noise()));

	QObject::connect(this->actionNormalise, SIGNAL(triggered()), this,
			SLOT(apply_normalise()));

	QObject::connect(this->actionMesh_stats, SIGNAL(triggered()), this,
			SLOT(apply_stats()));

	QObject::connect(this->actionKeep_only_one_CC, SIGNAL(triggered()), this,
			SLOT(keep_only_main_CC()));


}

void MainWindow::off_file_open()
{
    QString fileName;
#ifdef WIN32
    fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh File"),
            "", tr("off files (*.off)"));
#else
    fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh File"),
            "/user/dsalinas/home/Documents/Recherche/Code/data/", tr("off files (*.off)"));
#endif

	if (!fileName.isEmpty())
	{
		open_off(fileName);
	}
}

void MainWindow::off_file_save()
{
	std::cout << "Please indicate an .off file\n";

    QString fileName;
#ifdef WIN32
    fileName = QFileDialog::getSaveFileName(this, tr("Save Mesh File"),
            "", tr("off files (*.off)"));
#else
    fileName = QFileDialog::getSaveFileName(this, tr("Save Mesh File"),
            "/user/dsalinas/home/Documents/Recherche/Code/data/", tr("off files (*.off)"));
#endif

	cerr << fileName.toStdString() << endl;

	bool saved = save_off_file(fileName.toStdString(), mesh_.mesh);


	if (!saved)
		cerr << "Can't write in file " << fileName.toStdString() << endl;
	else{
		reorient<Complex>(fileName.toStdString(),fileName.toStdString());
//		reorient_faces(fileName.toStdString());
	}

	if(!mesh_.segmentation.is_empty()){
		string colored_file_name(fileName.toStdString());
		colored_file_name.insert(colored_file_name.size()-4,string("_colored"));
		bool saved2 = save_segmented_off_file(colored_file_name,mesh_);
		if (!saved2)
			cerr << "Can't write in file " << fileName.toStdString() << endl;
		else{
//			reorient_faces(fileName.toStdString());
		}
	}
}

void MainWindow::proxy_file_open()
{


    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Proxy File"),
            "", tr("seg files (*.seg)"));

	if (!fileName.isEmpty())
	{
		open_proxy_file(fileName);
	}
}


void MainWindow::proxy_file_save(){
	std::cout << "Please indicate an .seg file\n";
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save Mesh File"),
            "", tr("seg files (*.seg)"));
	save_proxy_in_file(fileName.toStdString());
}




void MainWindow::open_off(const QString& fileName)
{
	// we just get the name of the file toto for /etc/blabla/toto.off
	auto file_str = fileName.toStdString();
	name_file =  file_str.substr(file_str.rfind("/")+1);
	name_file = name_file.substr(0,name_file.size()-4);
	UDBGVALUE(name_file);


	QApplication::setOverrideCursor(Qt::WaitCursor);
	clear_graph_segmentation();

	bool loaded = read_off_two_dimensional_file(fileName.toStdString(), mesh_.mesh);
	if (!loaded) return;

	std::cerr << "Loaded mesh has " << mesh_.mesh.num_vertices() << " vertices\n";

	this->addToRecentFiles(fileName);
	QApplication::restoreOverrideCursor();
	this->viewer_instructor->initialize_bounding_box();
	this->viewer_instructor->show_entire_scene();

	menu_collapse->hide();
	menu_collapse->update_num_collapses(1);
	menu_collapse->update_num_vertices(mesh_.mesh.num_vertices());
	update_policy(menu_collapse->m_simplificationMethod->currentText());
	update_parameters();
	//	update_boundary_weight(menu_collapse->doubleSpinBox_boundary_weight->value());
	//	update_proxy_plane_weight(menu_collapse->doubleSpinBox_proxy_plane_weight->value());
	//	update();

	mesh_.set_initial_points();

	menu_detect_primitive->hide();
}


void MainWindow::open_proxy_file(const QString& fileName)
{
	if(mesh_.mesh.empty()) return;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	clear_graph_segmentation();


	std::ifstream file_planes(fileName.toStdString());
	if(file_planes.is_open()){
		file_planes>>mesh_.segmentation;
	}
	else{
		std::cerr << "Error while opening file "<<fileName.toStdString()<<"\n";
		return;
	}
	file_planes.close();

	recompute_graph();

	update();

	std::cerr << "Loaded segmentation with " << mesh_.segmentation.size() << " planes\n";

	this->addToRecentFiles(fileName);
	QApplication::restoreOverrideCursor();
}


void
MainWindow::open(QString fileName){
	std::string file_name = fileName.toStdString();

	auto off_pos = file_name.find(".off");
	auto seg_pos = file_name.find(".seg");
	auto dat_pos = file_name.find(".dat");

	if(off_pos != std::string::npos){
		open_off(fileName);
	}
	if(seg_pos != std::string::npos){
		open_proxy_file(fileName);
	}
	if(dat_pos != std::string::npos){
		load_camera(file_name);
	}

}

void
MainWindow::click(const Point& point){

	typedef Complex::Edge_handle Edge_handle;
	typedef GT::Segment_3 Segment_3;


	typedef Complex::Graph_vertex::Planar_proxies Planar_proxies;

	if(!mesh_.mesh.empty() && mesh_.mesh.num_edges()>=1){

		double dist_min = 1e10;
		Edge_handle e_min = * mesh_.mesh.edge_range().begin();

		for (auto e : mesh_.mesh.edge_range()){
			double dist_edge_click = std::sqrt(squared_distance(point,
					Segment_3(
							mesh_.mesh.point(mesh_.mesh.first_vertex(e)),
							mesh_.mesh.point(mesh_.mesh.second_vertex(e)) ))
			);
			if(dist_edge_click < dist_min){
				dist_min  = dist_edge_click ;
				e_min = e;
			}
		}

		viewer_instructor->selected_edge = std::pair<Point,Point>(
				mesh_.mesh.point(mesh_.mesh.first_vertex(e_min)),
				mesh_.mesh.point(mesh_.mesh.second_vertex(e_min))
		) ;


		double edge_length =
				std::sqrt(squared_distance(mesh_.mesh.point(mesh_.mesh.first_vertex(e_min)),mesh_.mesh.point(mesh_.mesh.second_vertex(e_min)) ));
		UDBGVALUE(edge_length);


		auto placement_and_cost = collapsor.get_placement_and_cost(e_min);

		if(placement_and_cost){
			auto placement = placement_and_cost->first;
			auto cost = placement_and_cost->second;

			if(cost) UDBGVALUE(std::sqrt(*cost));
			else UDBG("Cant compute cost");

			if (placement){
				viewer_instructor->placement_selected_edge = *placement;

				Planar_proxies proxies = mesh_.mesh[mesh_.mesh.first_vertex(e_min)].plane_primitives();
				proxies.insert(mesh_.mesh[mesh_.mesh.second_vertex(e_min)].plane_primitives().begin(),mesh_.mesh[mesh_.mesh.second_vertex(e_min)].plane_primitives().end());
				if(!proxies.empty()) {
					double dist_max = 0;
					for(auto proxy : proxies){
						UDBGVALUE(proxy->num_vertices());
						dist_max = (std::max)(dist_max,squared_distance(*placement,proxy->plane()));
					}
					dist_max = std::sqrt(dist_max);
					UDBGMSG("Distance maximum placement with proxies:",dist_max);
				}
			}
			else UDBG("Cant compute placement");
		}
	}

}


void MainWindow::update_display(){
	emit (sceneChanged());
}

void MainWindow::update()
{
	collapsor.need_update();
	emit (sceneChanged());
}

void MainWindow::detection_compute_planes()
{
	cerr << "Detect planar primitives\n";
	menu_detect_primitive->show();
}

void MainWindow::primitives_perturbation(){
	bool ok;
	double percent = QInputDialog::getDouble(this, tr("Percent of removed proxies"),
			tr("Percent:"), 0, 0, 100, 1, &ok);
	srand(time(NULL));
	if (ok)
	{
		mesh_.primitives_perturbation(percent);
	}
	update();
}



void MainWindow::show_menu_view_param(){
	menu_view_param->show();
}

std::string get_segmentation_path(const std::string& name_file){
	//add params
    std::string segmentation_path;
	segmentation_path = "";
	return segmentation_path+name_file;
}

void MainWindow::detect_primitives(const Detection_params& detection_params)
{
	UDBG("--- detect proxies ---");
	mesh_.segmentation.compute_planes(
			get_segmentation_path(name_file),
			detection_params
	);

	UDBGMSG("nb of detected proxies:",mesh_.segmentation.size());

	//todo graph_detect_params
	recompute_graph();

	update();
}

void MainWindow::regularize_primitives(const Merge_proxies_params& merge_params)
{
	mesh_.merge_proxies(merge_params);
	save_proxy_in_file(get_segmentation_path(name_file)+".seg");
	update();
}

void MainWindow::save_proxy_in_file(const std::string& proxy_name_file){
	if(!mesh_.mesh.empty()){
		ofstream file_segmentation;
		file_segmentation.open (proxy_name_file);
		if(file_segmentation)
			file_segmentation << mesh_.segmentation;
		else
			std::cerr << "Cant load file "<<proxy_name_file<<endl;
		file_segmentation.close();
	}

}


void MainWindow::expand_primitives(){
	bool ok;
	double max_dist = QInputDialog::getDouble(this, tr("Maximum distance for expansion"),
			tr("Max dist"), 0, 0, 10000, 4, &ok);
	if (ok)
	{
		UDBG("Expand primitives");
		int num_assigned_regions = mesh_.expand_proxies(max_dist);
		UDBGVALUE(num_assigned_regions);

		if(num_assigned_regions != 0 ) update();
	}
}

void MainWindow::graph_detection(){
	bool ok;
	double max_dist = QInputDialog::getDouble(this, tr("Maximum distance for edges"),
			tr("Max dist"), 0, mesh_.params.distance_max_graph_edges, 10000, 4, &ok);
	if (ok)
	{
		UDBG("Compute graph");
		mesh_.update_graphs(max_dist,true);
		update();
	}
}

void MainWindow::recompute_graph(){
	mesh_.update_graphs(mesh_.params.distance_max_graph_edges);
}


void MainWindow::filter_small_primitives(){
	mesh_.filter_proxies(menu_detect_primitive->doubleSpinBox_min_area_filter->value());
	update();
}




void MainWindow::show_complex()
{
	PRINT(mesh_.mesh.to_string());
}



void MainWindow::detection_prompt_planes()
{

	cerr << "Planar primitives : \n";

	cerr << mesh_.segmentation<<endl;
}


void MainWindow::prompt_detection_params()
{
	cerr << "Parameters planar primitives detection : \n";
	cerr << "\n\n"<<menu_detect_primitive->detect_params()<<endl;
	cerr << "\n"<<menu_detect_primitive->merge_params()<<endl;
}


void MainWindow::prompt_contr_params()
{
	cerr << "Parameters planar primitives detection : \n";
	cerr << "\n\n"<<mesh_.params<<endl;
}



std::pair<Gudhi_kernel::FT, Gudhi_kernel::Point>
projection(Gudhi_kernel::Point & pt, Gudhi_kernel::Plane_3& plane)
{
	auto proj = plane.projection(pt);
	auto res = std::make_pair(squared_distance(proj, pt), proj);
	return res;
}

void MainWindow::simplification_project_on_primitives()
{

	for (auto x : mesh_.mesh.vertex_range())
	{
		std::pair<FT, Point> dist_proj(1e10,Point(-1,-1,-1));
		for(auto proxy : mesh_.mesh[x].plane_primitives()){
			auto pl(proxy->plane());
			std::pair<FT, Point> current_dist_proj = projection(mesh_.mesh.point(x), pl);
			if(current_dist_proj.first < dist_proj.first){
				dist_proj = current_dist_proj;
			}
		}
		if(dist_proj.first<1e10){
			cerr << "distance to projection1 : " << dist_proj.first << endl;
			mesh_.mesh.point(x) = Point(dist_proj.second.x(), dist_proj.second.y(),dist_proj.second.z());

		}

		//
		//		auto planes_index = mesh_.mesh[x].primitives();
		//		//if (x->adjacent_planes().size()==1){
		//
		//		if (planes_index.size() >= 1)
		//		{
		//			auto plane((**(planes_index.cbegin())).plane());
		//			std::pair<FT, Point> dist_proj = projection(mesh_.mesh.point(x), plane);
		//			if (dist_proj.first > 2)
		//				cerr << "distance to projection1 : " << dist_proj.first << endl;
		//			mesh_.mesh.point(x) = Point(dist_proj.second.x(), dist_proj.second.y(),
		//					dist_proj.second.z());
		//		}
	}
	update();
}

void MainWindow::simplification_collapse_menu()
{
	menu_collapse->show();
}

void MainWindow::apply_noise()
{

	bool ok;
	double amplitude = QInputDialog::getDouble(this, tr("Uniform noise"),
			tr("Amplitude:"), 0, 0, 10000, 4, &ok);
	srand(time(NULL));
	if (ok)
	{
		for (auto v : mesh_.mesh.vertex_range())
		{
			auto point = mesh_.mesh.point(v);
			double nx = point.x() + ((double)rand()/(RAND_MAX)- .5) * amplitude;
			double ny =point.y() + ((double)rand()/(RAND_MAX)  - .5) * amplitude;
			double nz = point.z() + ((double)rand()/(RAND_MAX) - .5) * amplitude;
			mesh_.mesh.point(v) = Point(nx, ny, nz);
		}

	}
	update();
}


void MainWindow::apply_normalise(){
	double max_norm = -1;
	double center[3];
	for(int i = 0 ; i<3 ; ++i) center[i] = 0;

	for (auto v : mesh_.mesh.vertex_range()){
		max_norm = max(max_norm,sqrt(CGAL::squared_distance(mesh_.mesh.point(v),Point(CGAL::ORIGIN))));
		center[0] += mesh_.mesh.point(v).x();
		center[1] += mesh_.mesh.point(v).y();
		center[2] += mesh_.mesh.point(v).z();
	}

	for(int i = 0 ; i<3 ; ++i) center[i] /= mesh_.mesh.num_vertices();

	for (auto v : mesh_.mesh.vertex_range())
	{
		auto point = mesh_.mesh.point(v);
		double nx = (point.x()-center[0]) / max_norm ,
				ny = (point.y()-center[1]) / max_norm ,
				nz = (point.z()-center[2]) / max_norm;
		mesh_.mesh.point(v) = Point(nx, ny, nz);
	}



	//	std::list<Point> points;
	//	for (auto v : mesh_.mesh.vertex_range())
	//		points.push_back(mesh_.mesh.point(v));
	//	auto center = CGAL::barycenter(points.begin(),points.end());

	this->viewer_instructor->initialize_bounding_box();
	this->viewer_instructor->show_entire_scene();
	update();
}



void MainWindow::apply_stats(){
	std::cout << "++++++ Mesh stats +++++++"<< std::endl;
	std::cout << "Num vertices : " << mesh_.mesh.num_vertices()<<std::endl;
	std::cout << "Num edges : " << mesh_.mesh.num_edges()<<std::endl;
	std::cout << "Num connected components : " << mesh_.mesh.num_connected_components()<<std::endl;

	int num_proxy_vertices = std::count_if(
			mesh_.mesh.vertex_range().begin(),
			mesh_.mesh.vertex_range().end(),
			[&](Vertex_handle v){ return !mesh_.mesh[v].empty_plane_primitives();}
	);
	std::cout << "Num proxy vertices : " << num_proxy_vertices<<std::endl;
	std::cout << "Num non proxy vertices : " << mesh_.mesh.num_vertices()-num_proxy_vertices<<std::endl;


	std::cout << "Average distance to proxy : " <<
			sum_avg_distance_to_proxies(mesh_.mesh) /mesh_.mesh.num_vertices()<<std::endl;

	double avg_length = 0;
	for(auto e : mesh_.mesh.edge_range()){
		avg_length += std::sqrt(CGAL::squared_distance(mesh_.mesh.point(mesh_.mesh.first_vertex(e)),mesh_.mesh.point(mesh_.mesh.second_vertex(e))));
	}
	std::cout << "Average edge length: " <<
				avg_length / (std::max)(mesh_.mesh.num_edges(),1)<<std::endl;

	std::cout << "+++++++++++++++++++++++++"<< std::endl;


}

void MainWindow::keep_only_main_CC(){
}
/// Update parameters //////////////////////////////////////////////////////////////

void MainWindow::update_parameters()
{
	mesh_.params.abstraction_weight = menu_collapse->doubleSpinBox_abstraction_weight->value();
	mesh_.params.boundary_weight = menu_collapse->doubleSpinBox_boundary_weight->value();
	mesh_.params.bound_preserve_normals = menu_collapse->doubleSpinBox_preserve_normals->value() /180. * 3.14;
	mesh_.params.distance_max_graph_edges = menu_detect_primitive->doubleSpinBox_max_graph_length->value();
	mesh_.params.link_condition = menu_collapse->checkBox_link_condition->isChecked();

	//todo do a structure preserving param
	mesh_.params.constant_graph = mesh_.params.check_primitive_vertices;
	mesh_.params.check_primitive_corner = mesh_.params.check_primitive_vertices;
	mesh_.params.check_preserve_normals = menu_collapse->checkBox_preserve_normals->isChecked();
	mesh_.params.check_primitive_edges= false;
	update();
}

void MainWindow::update_parameters(bool)
{
	update_parameters();
}

void MainWindow::update_parameters(double)
{
	update_parameters();
}

void MainWindow::update_policy(const QString& qstring)
{
	cerr << "Policy: " << qstring.toStdString() << endl;
	collapsor.set_policy(qstring.toStdString());
	update();
}

/// Update parameters //////////////////////////////////////////////////////////////

void MainWindow::clear_graph_segmentation()
{
	mesh_.segmentation.clear();
	mesh_.primitive_graph.clear();
	mesh_.dual_graph.clear();
}

void MainWindow::update_best_edge()
{
	/* bug here vs12
	auto best_edge_pair(collapsor.get_best_edge());
	if (!best_edge_pair)
	{
		best_edge.reset();
		new_point.reset();
	}
	else
	{
		auto edge_simplex_it =
				(mesh_.mesh.get_vertices(best_edge_pair->first)).begin();
		Point p1 = mesh_.mesh.point(*edge_simplex_it);
		Point p2 = mesh_.mesh.point(*(++edge_simplex_it));
		best_edge = make_pair(p1, p2);
		if (best_edge_pair->second)
			new_point = *best_edge_pair->second;
		else
			new_point.reset();
	}
	*/
}

int MainWindow::num_vertices()
{
	return mesh_.mesh.num_vertices();
}

void MainWindow::collapse_edges(int num_contractions)
{
	if (mesh_.mesh.num_vertices() > 1)
	{
		UDBGMSG("Policy:",menu_collapse->m_simplificationMethod->currentText().toStdString());
		UDBGVALUE(mesh_.params.abstraction_weight);
		collapsor.contract_edges(num_contractions);
		update_best_edge();
		menu_collapse->update_num_vertices(num_vertices());
		emit (sceneChanged());
	}
}

void
MainWindow::save_camera(){
	std::ofstream out_file;
	UDBG("Save camera");
	out_file.open ("cam.dat");
	if(!out_file.is_open()) {
		std::cerr << "Unable to open file cam.dat" << std::endl;
		return;
	}
	out_file << viewer_instructor->camera()->position()<<endl;
	out_file << viewer_instructor->camera()->viewDirection()<<endl;
	out_file << viewer_instructor->camera()->orientation()<<endl;


	out_file << viewer_instructor->width()<<endl;
	out_file << viewer_instructor->height()<<endl;
	out_file << this->width()<<endl;
	out_file << this->height()<<endl;

	out_file << viewer_instructor->view_params() <<endl;
	out_file.close();
}

void
MainWindow::load_camera(const std::string & name_file){

	UDBG("Load camera");
	std::ifstream in_file(name_file,std::ios::in);
	if(!in_file.is_open()) {
		std::cerr << "Unable to open file " << "cam.dat" << std::endl;
		return;
	}
	double  xyz_position[3] , xyz_direction[3] ,xyzw_orientation[4],height,width,height_mw,width_mw;
	for(int i = 0; i<3; ++i)
		in_file >> xyz_position[i];
	for(int i = 0; i<3; ++i)
		in_file >> xyz_direction[i];
	for(int i = 0; i<4; ++i)
		in_file >> xyzw_orientation[i];
	in_file >> width;
	in_file >> height;
	in_file >> width_mw;
	in_file >> height_mw;

	viewer_instructor->camera()->setPosition(qglviewer::Vec(xyz_position[0],xyz_position[1],xyz_position[2]));
	viewer_instructor->camera()->setViewDirection(qglviewer::Vec(xyz_direction[0],xyz_direction[1],xyz_direction[2]));
	viewer_instructor->camera()->setOrientation(qglviewer::Quaternion(xyzw_orientation[0],xyzw_orientation[1],xyzw_orientation[2],xyzw_orientation[3]));
	this->resize(width_mw,height_mw);
	viewer_instructor->set_dimension(width,height);

	in_file>>viewer_instructor->view_params();
	viewer_instructor->update_view_params();
}



void MainWindow::keyPressEvent(QKeyEvent * event)
{
	std::ofstream in_file;
	std::ifstream out_file ;
	std::istringstream iss;

	clock_t start;
	// treated differently because heavy initilization
	if(event->key() == Qt::Key_W){
		double dist_triangles_to_points,dist_points_to_triangles,dist_hausdorff_max,dist_hausdorff_mean;
		UDBG("Distance computation");
		Meshlab_hausdorff_distance<Complex> meshlab_dist(mesh_.mesh);
		Distance_point_to_triangle<Complex> point_to_triangles_dist(mesh_.initial_points);
		meshlab_dist.compute_distance(mesh_.mesh);
		point_to_triangles_dist.compute_distance(mesh_.mesh);
		dist_triangles_to_points = meshlab_dist.max_s_to_o();
		start = clock();
		//		dist_triangles_to_points = distance_hausdorff(mesh,initial_points,1e-5);
		dist_points_to_triangles = point_to_triangles_dist.max_o_to_s();
		dist_hausdorff_max = (std::max)(dist_triangles_to_points,dist_points_to_triangles);
		dist_hausdorff_mean= (std::max)(meshlab_dist.mean_s_to_o(),meshlab_dist.mean_o_to_s());

		UDBGVALUE(point_to_triangles_dist.mean_o_to_s());
		UDBGVALUE(meshlab_dist.mean_o_to_s());
		UDBGVALUE(meshlab_dist.mean_s_to_o());
		UDBGVALUE(dist_triangles_to_points);
		UDBGVALUE(dist_points_to_triangles);
		UDBGVALUE(dist_hausdorff_max);
		UDBGVALUE(dist_hausdorff_mean);

		//		double dist_triangles_to_points,dist_points_to_triangles,dist_hausdorff_max,dist_hausdorff_mean;
				//		UDBG("Distance computation");
				//		Meshlab_hausdorff_distance<Complex> meshlab_dist(mesh_.mesh);
				//		Distance_point_to_triangle<Complex> point_to_triangles_dist(mesh_.initial_points);
				//		meshlab_dist.compute_distance_s_to_o(mesh_.mesh);
				//		point_to_triangles_dist.compute_distance(mesh_.mesh);
				//		dist_triangles_to_points = meshlab_dist.max_s_to_o();
				//		start = clock();
				//		//		dist_triangles_to_points = distance_hausdorff(mesh,initial_points,1e-5);
				//		dist_points_to_triangles = point_to_triangles_dist.max_o_to_s();
				//		dist_hausdorff_max = std::max(dist_triangles_to_points,dist_points_to_triangles);
				//		dist_hausdorff_mean= std::max(meshlab_dist.mean_s_to_o(),point_to_triangles_dist.mean_o_to_s());
				//		UDBGVALUE(dist_triangles_to_points);
				//		UDBGVALUE(dist_points_to_triangles);
				//		UDBGVALUE(dist_hausdorff_max);
				//		UDBGVALUE(dist_hausdorff_mean);
				//		std::cerr << "Distance computation took "<< ( (float)(clock()-start))/CLOCKS_PER_SEC << " seconds\n";
	}


	switch (event->key())
	{
	case Qt::Key_0:
		UDBG("0");

		break;
	case Qt::Key_Equal:
		UDBG("Snapshot");
		viewer_instructor->snap_shot();
		break;

	case Qt::Key_O:
		UDBG("Save camera");
		save_camera();
		break;
	case Qt::Key_I:
		UDBG("Load camera");
		load_camera("cam.dat");
		viewer_instructor->sceneChanged();
		break;

	case Qt::Key_V:
		viewer_instructor->change_draw_vertices();
		break;
	case Qt::Key_E:
		viewer_instructor->change_draw_edges();
		break;
	case Qt::Key_B:
		menu_view_param->show();
		break;
	case Qt::Key_T:
		viewer_instructor->change_draw_triangles();
		break;
	case Qt::Key_D:
		viewer_instructor->change_draw_primitives();
		break;
	case Qt::Key_J:
		viewer_instructor->change_draw_primitives_graph();
		break;
	case Qt::Key_P:
		viewer_instructor->change_draw_primitives();
		break;
	case Qt::Key_M:
		viewer_instructor->change_draw_primitives_corner();
		break;
	case Qt::Key_L:
		viewer_instructor->change_light();
		break;
	case Qt::Key_Semicolon:
		UDBGVALUE(is_two_manifold(mesh_.mesh));

		keep_only_proxy_vertices(mesh_.mesh);

		break;

	case Qt::Key_Colon:

		mesh_.params.verbose = !mesh_.params.verbose;

		mesh_.filter_smallest_proxy();

		update();

		break;

	case Qt::Key_Percent:
		UDBG("percent");
		UDBG(remove_pending_vertices(mesh_.mesh));
		break;
	default:
		break;
	}
}

#include "MainWindow.moc"

