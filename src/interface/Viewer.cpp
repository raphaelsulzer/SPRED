#include "Viewer.h"
#include "complex/complex_tools.h"
#include <vector>
#include <CGAL/bounding_box.h>
#include <QGLViewer/vec.h>
#include <QGLViewer/camera.h>

typedef Kernel::Iso_cuboid_3 bounding_3;


void Viewer::show_entire_scene(){
	this->showEntireScene();
}

void Viewer::draw(){
//
//
////	set_light();
	instructor->give_instructions();
	this->drawLight(GL_LIGHT0,10.f);
}

void Viewer::set_bounding_box(const bounding_3 & bbox){
	std::cout << "Bounding box: "<< bbox<<endl;
	this->camera()->setSceneBoundingBox(
			qglviewer::Vec(bbox.xmin(), bbox.ymin(), bbox.zmin()),
			qglviewer::Vec(bbox.xmax(),bbox.ymax(),bbox.zmax())
	);
}

void Viewer::update_GL(){
	this->updateGL();
}
void Viewer::init_scene(){

	this->setBackgroundColor(Qt::white);
	::glEnable(GL_LINE_SMOOTH);

	init_light();

	//	// CGAL settings ///////////////////////////
	//	float difLight0[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	//	glLightfv(GL_LIGHT0, GL_DIFFUSE, difLight0);
	//	float specLight0[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	//	glLightfv(GL_LIGHT0, GL_SPECULAR, specLight0);
	//
	//	float specLight1[4] = {10.0f, 10.0f,10.0f,10.0f};
	//
	//	glMaterialfv(GL_FRONT, GL_SHININESS, specLight1);
	//	// define material
	//	float	ambient[]  =   { 0.25f,	0.20725f,0.20725f,0.922f };
	//	::glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,   ambient);
	//	float	diffuse[]  =   { 1.0f,0.829f,0.829f,0.922f };
	//	::glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,   diffuse);
	//	float	specular[]  = {  0.8f,0.8f,0.8f,0.8f };
	//	::glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,  specular);
	//	float	emission[]  = {  0.3f,0.3f,0.3f,1.0f };
	//	::glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION,  emission);
	//	float shininess[] = {  20.264f };
	//	::glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	//	// anti-aliasing (if the OpenGL driver permits that)
	//	::glEnable(GL_LINE_SMOOTH);
	//	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//	// CGAL settings ///////////////////////////


	//	//////////////////////////////////////////////
	//	// stack overflow settings
	//	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//
	//	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//	GLfloat mat_shininess[] = { 50.0 };
	//	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	//
	//	GLfloat mat_color1[] = { 0.0, 0.0, 1.0, 1.0 };
	//	GLfloat mat_color2[] = { 0.0, 1.0, 0.0, 1.0 };
	//
	//	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_color1);
	//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color2);
	//
	//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//	glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular);
	//	glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess);
	//
	//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_LIGHT0);
	//	glEnable(GL_DEPTH_TEST);
	//	//////////////////////////////////////////////


	//	// QGL light ///////////////////////////////////////
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	//
	//	// Light0 is the default ambient light
	//	glEnable(GL_LIGHT0);
	//
	//	// Light1 is a spot light
	//	glEnable(GL_LIGHT1);
	//	const GLfloat light_ambient[4]  = {0.8f, 0.2f, 0.2f, 1.0};
	//	const GLfloat light_diffuse[4]  = {1.0, 0.4f, 0.4f, 1.0};
	//	const GLfloat light_specular[4] = {1.0, 0.0, 0.0, 1.0};
	//
	//	glLightf( GL_LIGHT1, GL_SPOT_EXPONENT,  3.0);
	//	glLightf( GL_LIGHT1, GL_SPOT_CUTOFF,    20.0);
	//	glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5);
	//	glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0);
	//	glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1.5);
	//	glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
	//	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	//	glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
	//
	//	// Light2 is a classical directionnal light
	//	glEnable(GL_LIGHT2);
	//	const GLfloat light_ambient2[4]  = {0.2f, 0.2f, 2.0, 1.0};
	//	const GLfloat light_diffuse2[4]  = {0.8f, 0.8f, 1.0, 1.0};
	//	const GLfloat light_specular2[4] = {0.0, 0.0, 1.0, 1.0};
	//
	//	glLightfv(GL_LIGHT2, GL_AMBIENT,  light_ambient2);
	//	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
	//	glLightfv(GL_LIGHT2, GL_DIFFUSE,  light_diffuse2);
	//
	//
	//	::glEnable(GL_LINE_SMOOTH);
	//	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//	//////////////////////////////////////////////////
}


void
Viewer::init_light(){
	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}




void
Viewer::set_light_direction(double theta_, double phi_){
	theta = theta_;
	phi = phi_;
}

void
Viewer::set_light_direction(){
	theta = -1;
	phi = -1;

//	const GLfloat pos[4] = {0.,0.,1.,0.};
//	glLightfv(GL_LIGHT0, GL_POSITION, pos);

}


void
Viewer::set_light(){

	if(theta>=0 && phi >=0){
		const GLfloat pos[4] = {(float)(sin(phi)*cos(theta)),(float)(sin(phi)*sin(theta)),(float)(cos(phi)),0.};
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
	}
}


void
Viewer::postSelection(const QPoint& point)
{
	bool found;

	auto vec = this->camera()->pointUnderPixel(point,found);
	UDBGVALUE(found);

	if(found){
		Point position(vec[0],vec[1],vec[2]);
		emit(click(position));
	}

}

void Viewer::set_size_point(double size_points){
	::glPointSize(size_points);
}

void Viewer::draw_point(const Point& p,double red,double green,double blue,double size_points){
	::glColor3f(red,green,blue);
	::glDisable(GL_LIGHTING);
	::glEnable(GL_POINT_SMOOTH);
	::glPointSize(size_points);
	::glBegin(GL_POINTS);
	::glVertex3d(p.x(), p.y(), p.z());
	::glEnd();
	::glDisable(GL_POINT_SMOOTH);
}


void Viewer::set_color(const CGAL::Color & primitive_color){
	::glColor3ub(primitive_color.r(),primitive_color.g(),primitive_color.b());
}

void Viewer::set_color(double r,double g,double b){
	::glColor3f(r,g,b);
}

void Viewer::begin_draw_points(double r,double g,double b,double size,bool light){
	set_color(r,g,b);
	light?glEnable(GL_LIGHTING):glDisable(GL_LIGHTING);
	::glEnable(GL_POINT_SMOOTH);
	::glPointSize(size);
	::glBegin(GL_POINTS);
}


void Viewer::draw_points(const Point & point){
	::glVertex3d(point.x(),point.y(),point.z());
}

void Viewer::end_draw_points(){
	::glEnd();
	::glDisable(GL_POINT_SMOOTH);
}

void Viewer::draw_edge(const Point &a,const Point &b,double red,double green,double blue,double size){
	::glColor3f(red,green,blue);
	::glPointSize(3.0);
	::glLineWidth(size);
	::glBegin(GL_LINES);
	::glVertex3f(a.x(),a.y(),a.z());
	::glVertex3f(b.x(),b.y(),b.z());
	::glEnd();
}

void Viewer::begin_draw_edges(double red,double green,double blue,double size,bool light){
	::glLineWidth(size);
	::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	::glEnable(GL_POLYGON_OFFSET_LINE);
	::glPolygonOffset(3.0f,-3.0f);
	light?glEnable(GL_LIGHTING):glDisable(GL_LIGHTING);
	::glBegin(GL_LINES);
	::glColor3f(red,green,blue);
}

void Viewer::draw_edges(const Point &a,const Point &b){
	::glVertex3f(a.x(),a.y(),a.z());
	::glVertex3f(b.x(),b.y(),b.z());
}

void Viewer::end_draw_edges(){
	::glEnd();
}

void Viewer::begin_draw_triangles(double red,double green,double blue,double size,bool light,bool transparent){
	if(transparent){
		::glEnable (GL_BLEND);
		::glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	::glEnable(GL_POLYGON_OFFSET_FILL);
	::glPolygonOffset(3.0f,-3.0f);
	light?glEnable(GL_LIGHTING):glDisable(GL_LIGHTING);
	::glBegin(GL_TRIANGLES);
}


void Viewer::draw_triangles(const Point& p1,const Point& p2,const Point& p3,Vector triangle_normal){
	::glNormal3d(triangle_normal.x(),triangle_normal.y(),triangle_normal.z());
	::glVertex3d(p1.x(),p1.y(),p1.z());
	::glVertex3d(p2.x(),p2.y(),p2.z());
	::glVertex3d(p3.x(),p3.y(),p3.z());
}


void Viewer::draw_triangles(const  vector<Point*>& points,Vector triangle_normal){
	::glNormal3d(triangle_normal.x(),triangle_normal.y(),triangle_normal.z());
	::glVertex3d(points[0]->x(),points[0]->y(),points[0]->z());
	::glVertex3d(points[1]->x(),points[1]->y(),points[1]->z());
	::glVertex3d(points[2]->x(),points[2]->y(),points[2]->z());
}

void Viewer::end_draw_triangles(){
	::glEnd();
}

#include "Viewer.moc"
