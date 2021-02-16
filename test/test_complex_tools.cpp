/*
 * test_complex_tools.cpp
 *
 *  Created on: Feb 26, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#include <CGAL/centroid.h>
#include <Eigen/Eigenvalues>

#include "typedefs.h"
#include "complex/complex_tools.h"
#include "iocomplex.h"

#include "contraction/quadric/Error_quadric.h"

#include "contraction/QP_solver.h"



using namespace std;



void test_filter_blockers(){
	cerr<< "test_filter_blockers \n";
	Complex complex;
	std::string file_name("../code/data/test2.off");
	bool loaded = read_off_two_dimensional_file(file_name,complex);
	if(!loaded) return;
	PRINT(complex.to_string());

	filter_blockers<Complex>(complex,Vertex_handle(3),Vertex_handle(4));

	PRINT(complex.to_string());

	assert(complex.num_blockers()==0);

}


void test_qp(){

	typedef typename Gudhi_kernel::Plane_3 Plane_3;
	typedef typename Gudhi_kernel::Vector_3 Vector_3;
	typedef typename Gudhi_kernel::Point Point;

	typedef typename contraction::Error_quadric<Gudhi_kernel> Error_quadric;
	UDBG("Test qp");

	Point p0( 0.8 , 0. , 0.);
	Point p1( 0. , 1.2 , 0.);
	Point p2(-1. ,-1.5 , 0.);
	Point p3( 0. , 0.2 , 1.);

	Point interior_point(0,0,0.5);

	Error_quadric Q0(Plane_3(p0,p1,p3));
	Error_quadric Q1(Plane_3(p0,p2,p3));
	Error_quadric Q2(Plane_3(p2,p1,p3));
	Error_quadric Q = Q0; Q+=Q1; Q+= Q2;

	std::list<Plane_3> planes;
	planes.push_back(Plane_3(p0,p1,p2));

	planes.push_back(Plane_3(Point(0,0,0.8), Vector_3(0,0,1) ));

	for(auto& plane : planes){
		if(plane.has_on_positive_side(interior_point))
			plane = plane.opposite() ;
	}
	UDBGVALUE(*min_cost_qp_solver(Q,planes));


	UDBGVALUE(*min_cost_qp_solver(Q,std::list<Plane_3>()));

	UDBGVALUE(*Q.min_cost());
}


template<typename ErrorQuadric>
boost::optional<typename ErrorQuadric::Point> min_svd_cost(const ErrorQuadric & quad,const typename ErrorQuadric::Point& center_cell){
	typedef typename ErrorQuadric::Point Point;
	const double epsilon_tolerance = 1e-3;

	boost::optional<typename ErrorQuadric::Point> res;

	Eigen::Matrix3f A;
	A(0,0) = quad(0);
	A(0,1) = quad(1);
	A(0,2) = quad(2);
	A(1,1) = quad(4);
	A(1,2) = quad(5);
	A(2,2) = quad(7);
	//xxx should be useless to remove when good type founded
	A(1,0) = quad(1);
	A(2,0) = quad(2);
	A(2,1) = quad(5);

	Eigen::Vector3f b;
	b(0) = -quad(3);
	b(1) = -quad(6);
	b(2) = -quad(8);

	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> es;
	es.compute(A);

	Eigen::Matrix3f X = es.eigenvectors();

	Eigen::Matrix3f diag_inv = es.eigenvalues().asDiagonal();
	for(int i=0;i<2;++i){
		if(diag_inv(i,i) <= diag_inv(2,2) * epsilon_tolerance) diag_inv(i,i)=0;
		else diag_inv(i,i)=1/diag_inv(i,i);
	}
	diag_inv(2,2)=1/diag_inv(2,2);


	Eigen::Matrix3f a_inv = X * diag_inv * X.transpose();

	Eigen::Vector3f center_cell_vector;
	for(int i=0;i<3;++i){
		center_cell_vector(i)=center_cell[i];
	}
	Eigen::Vector3f res_vector = center_cell_vector + a_inv*(b-A*center_cell_vector);

	res = Point(res_vector(0),res_vector(1),res_vector(2));

	return res;

}

void test_svd(){
	typedef typename Gudhi_kernel::Plane_3 Plane_3;
	typedef typename Gudhi_kernel::Point Point;

	typedef typename contraction::Error_quadric<Gudhi_kernel> Error_quadric;

	std::vector<Point> points;
	points.push_back(Point( 2. , 1. , 1.));
	points.push_back(Point(.500,1.866, 1.));
	points.push_back(Point(.5,.134 , 1.));
	points.push_back(Point( 1. , 1. , 1.01));

	auto centroid = CGAL::centroid(points.begin(),points.end());

	Error_quadric Q0(Plane_3(points[0],points[1],points[3]));
	Error_quadric Q1(Plane_3(points[0],points[2],points[3]));
	Error_quadric Q2(Plane_3(points[2],points[1],points[3]));
	Error_quadric Q = Q0; Q+=Q1; Q+= Q2;

	UDBGVALUE(centroid);

	auto res = min_svd_cost(Q,centroid);


	UDBGVALUE(*Q.min_cost());

	// should be 1 1 1
	if(res) UDBGVALUE(*res);


}

int main(){
//	test_filter_blockers();
//	test_qp();
	test_svd();
}
