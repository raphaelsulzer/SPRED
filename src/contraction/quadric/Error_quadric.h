/*
 * Error_quadric.h
 *
 *  Created on: 24 janv. 2014
 *      Author: dsalinas
 */

#ifndef ERROR_QUADRIC_H_
#define ERROR_QUADRIC_H_

#include <vector>
#include <utility>
#include <CGAL/Linear_algebraCd.h>
#include <boost/optional/optional.hpp>
#include <CGAL/Origin.h>
#include <Eigen/Eigenvalues>

#include "urban_utils.h"

//xxx passer une traits plutot...
#include "typedefs.h"



using namespace Gudhi;
using namespace skbl;

namespace quadric{

/**
 * Solver is used to find the position of the optimal vertex
 * see below for examples
 */
template <typename GT_> class Error_quadric{
private :
	typedef GT_ GT;
	typedef typename GT::FT FT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Line_3 Line_3;

	std::vector<double> coeff;


	typedef CGAL::Linear_algebraCd<FT> Linear_algebraCd;
	typedef typename Linear_algebraCd::Matrix Matrix;
	typedef typename Linear_algebraCd::Vector Vector;


public :
	typedef typename GT::Point_3 Point;
	Error_quadric():coeff(10,0){
	}


	/**
	 * Quadric corresponding to the L2 distance to the plane
	 *
	 * According to the notation of Garland Heckbert, they
	 * denote a quadric symetric matrix as :
	 * Q = [ q11 q12 q13 q14]
	 *     [ q12 q22 q23 q24]
	 *     [ q13 q23 q33 q34]
	 *     [ q14 q24 q34 q44]
	 *
	 * which is represented by a length 10 vector coeff whose elements
	 * are denoted ci for clarity with :
	 * Q = [ c0  c1  c2  c3 ]
	 *     [ c1  c4  c5  c6 ]
	 *     [ c2  c5  c7  c8 ]
	 *     [ c3  c6  c8  c9 ]
	 *
	 *
	 * The quadric can be weighted (preferably with an area).
	 */
	Error_quadric(const Plane_3 & plane , double weight = 1 ):coeff(10){
		double a= plane.a(),
				b=plane.b(),
				c=plane.c(),
				d=plane.d();
		// Garland Heckbert, a^2+b^2+c^2 should be equal to 1
		// equivalent to having a normalized normal
		double norm_abc = sqrt(a*a+b*b+c*c);
		a/=norm_abc;
		b/=norm_abc;
		c/=norm_abc;
		d/=norm_abc;
		coeff[0] = a*a ;
		coeff[1] = a*b ;
		coeff[2] = a*c ;
		coeff[3] = a*d ;
		coeff[4] = b*b ;
		coeff[5] = b*c ;
		coeff[6] = b*d ;
		coeff[7] = c*c ;
		coeff[8] = c*d ;
		coeff[9] = d*d ;

		assert(std::abs(this->cost(Point(CGAL::ORIGIN)) - squared_distance(plane,Point(CGAL::ORIGIN)))<1e-4);
		(*this)*= weight;
	}

	/**
	 * Quadric corresponding to the L2 distance to a point
	 */
	Error_quadric(const Point & point,double weight = 1):coeff(10){
		coeff[0] = 1 ;
		coeff[1] = 0 ;
		coeff[2] = 0 ;
		coeff[3] = -point.x() ;
		coeff[4] = 1 ;
		coeff[5] = 0 ;
		coeff[6] = -point.y() ;
		coeff[7] = 1 ;
		coeff[8] = -point.z() ;
		coeff[9] = point.x()*point.x() + point.y()*point.y() + point.z()*point.z() ;

		assert(std::abs(this->cost(Point(CGAL::ORIGIN))-squared_distance(point,Point(CGAL::ORIGIN))) < 1e-4);

		(*this)*= weight;
	}


	//	/**
	//	 * Quadric corresponding to a constant value
	//	 */
	//	Error_quadric(double cte):coeff(10),weight_(1){
	//		for(int i = 0 ; i<9; ++i) coeff[i] = 0 ;
	//		coeff[9] = cte ;
	//	}



	/**
	 * Quadric corresponding to the L2 distance to a line
	 */
	Error_quadric(const Line_3 & line,double weight = 1):coeff(10){
		auto line_vector(line.to_vector());
		line_vector = line_vector / std::sqrt( line_vector.squared_length() );
		const double u1 = line_vector.x() , u2 = line_vector.y() , u3 = line_vector.z();
		Point line_point(line.point(0));
		const double b1 = line_point.x() , b2 = line_point.y() , b3 = line_point.z();

		// the quadric to the squared-distance to the line is
		// [u3^2+u2^2,       -u1*u2,       -u1*u3,   -(-u1*b3+u3*b1)*u3-(u2*b1-u1*b2)*u2 ]
		// [-u1*u2 ,         u3^2+u1^2 ,   -u2*u3,  -(u3*b2-u2*b3)*u3+(u2*b1-u1*b2)*u1   ]
		// [-u1*u3 ,        -u2*u3 ,    u1^2+u2^2 ,  (u3*b2-u2*b3)*u2+(-u1*b3+u3*b1)*u1  ]
		// [-(-u1*b3+u3*b1)*u3-(u2*b1-u1*b2)*u2,  -(u3*b2-u2*b3)*u3+(u2*b1-u1*b2)*u1,  (u3*b2-u2*b3)*u2+(-u1*b3+u3*b1)*u1,  (u3*b2-u2*b3)^2+(u2*b1-u1*b2)^2+(-u1*b3+u3*b1)^2 ]

		coeff[0] = u3*u3+u2*u2;
		coeff[1] = -u1*u2;
		coeff[2] = -u1*u3;
		coeff[3] = -(-u1*b3+u3*b1)*u3-(u2*b1-u1*b2)*u2;
		coeff[4] = u3*u3+u1*u1;
		coeff[5] = -u2*u3;
		coeff[6] = -(u3*b2-u2*b3)*u3+(u2*b1-u1*b2)*u1;
		coeff[7] = u1*u1+u2*u2;
		coeff[8] = (u3*b2-u2*b3)*u2+(-u1*b3+u3*b1)*u1;


		//coeff[9] = (u3*b2-u2*b3)^2+(u2*b1-u1*b2)^2+(-u1*b3+u3*b1)^2 ;
		double  c91 = (u3*b2-u2*b3),
				c92 = (u2*b1-u1*b2),
				c93 = (-u1*b3+u3*b1);
		coeff[9] = c91*c91+c92*c92+c93*c93 ;

		// check the quadric by asserting that the distance from the origin to the line
		// is the same whether we compute it by quadric or by CGAL
		assert( std::abs(squared_distance(Point(CGAL::ORIGIN),line) -  this->cost(Point(CGAL::ORIGIN)))/(this->cost(Point(CGAL::ORIGIN))) < 1e-4);

		(*this)*= weight;
	}

	void clear(){
		for(auto& x:coeff){
			x=0;
		}
	}

	//	Error_quadric& operator=(const Error_quadric& other ){
	//		this->coeff = other.coeff;
	//		this->
	//		return *this;
	//	}


	Error_quadric & operator*=(const double scalar){
		for(auto& x : coeff)
			x *= scalar;
		return *this;
	}

	Error_quadric& operator+=(const Error_quadric& other){
		auto x = coeff.begin();
		for(auto y = other.coeff.cbegin();
				y!= other.coeff.cend();
				++y,++x){
			*x += *y;
		}
		return *this;
	}

	Error_quadric& operator-=(const Error_quadric& other){
		auto x = coeff.begin();
		for(auto y = other.coeff.cbegin();
				y!= other.coeff.cend();
				++y,++x){
			*x -= *y;
		}
		return *this;
	}

	double operator()(int i) const{
		assert(0<=i && i <=9);
		return coeff[i];
	}


	double& operator()(int i,int j){
		assert(0<=i && i <=3 && 0<=j && j<=3);
		if (i>j) std::swap(i,j);
		// therefore i<=j here
		if(i==0)  return coeff[j];	 // j=0,1,2,3
		if (i==1) return coeff[j+3]; // j=1,2,3
		if (i==2) return coeff[j+5]; // j=2,3
		if (i==3) return coeff[j+6]; // j=3
		return coeff[0];
	}

	const double& operator()(int i,int j) const{
		assert(0<=i && i <=3 && 0<=j && j<=3);
		if (i>j) std::swap(i,j);
		// therefore i<=j here
		if(i==0)  return coeff[j];	 // j=0,1,2,3
		if (i==1) return coeff[j+3]; // j=1,2,3
		if (i==2) return coeff[j+5]; // j=2,3
		if (i==3) return coeff[j+6]; // j=3
		return coeff[0];
	}


	/**
	 * @return The quadric quost defined by the scalar product v^T Q v where Q is the quadratic form of Garland/Heckbert
	 */
	inline double cost(const Point& point) const{
		//		const double v1 = point.x(),
		//				v2 = point.y(),
		//				v3 = point.z();
		double cost =
				coeff[0]*point.x()*point.x()+coeff[4]*point.y()*point.y()+coeff[7]*point.z()*point.z()
				+2*(coeff[1]*point.x()*point.y()+coeff[5]*point.y()*point.z()+coeff[2]*point.z()*point.x())
				+2*(coeff[3]*point.x()+coeff[6]*point.y()+coeff[8]*point.z())
				+coeff[9];
		if(cost<0) return 0;
		else {
			return cost;
		}
	}

	inline double grad_determinant() const{
		return
				coeff[0] * coeff[4] * coeff[7]
				                            -  coeff[0] * coeff[5] * coeff[5]
				                                                           -  coeff[1] * coeff[1] * coeff[7]
				                                                                                          +2*coeff[1] * coeff[5] * coeff[2]
				                                                                                                                         -  coeff[4] * coeff[2] * coeff[2];
		//		q0*q4*q7-q0*q5^2-q1^2*q7+2*q1*q5*q2-q4*q2^2
	}


	/**
	 * Modidy pt_res such that it minimizes the gradient of the quadric.
	 * Det must be passed with the determinant value of the gradient (should be non zero).
	 */
	Point solve_linear_gradient(double det = grad_determinant()) const{
		//		double x = ;
		//		double y = coeff[0]*coeff[5]*coeff[8]-coeff[0]*coeff[7]*coeff[6]-coeff[5]*coeff[2]*coeff[3]-coeff[1]*coeff[2]*coeff[8]+coeff[6]*coeff[2]*coeff[2]+coeff[1]*coeff[3]*coeff[7];
		//		double z = -coeff[8]*coeff[0]*coeff[4]+coeff[8]*coeff[1]*coeff[1]+coeff[2]*coeff[3]*coeff[4]+coeff[5]*coeff[0]*coeff[6]-coeff[5]*coeff[1]*coeff[3]-coeff[1]*coeff[2]*coeff[6];
		return Point(
				(-coeff[1]*coeff[5]*coeff[8]+coeff[1]*coeff[7]*coeff[6]+coeff[2]*coeff[8]*coeff[4]-coeff[2]*coeff[5]*coeff[6]-coeff[3]*coeff[4]*coeff[7]+coeff[3]*coeff[5]*coeff[5])/ det,
				(coeff[0]*coeff[5]*coeff[8]-coeff[0]*coeff[7]*coeff[6]-coeff[5]*coeff[2]*coeff[3]-coeff[1]*coeff[2]*coeff[8]+coeff[6]*coeff[2]*coeff[2]+coeff[1]*coeff[3]*coeff[7])/det,
				(-coeff[8]*coeff[0]*coeff[4]+coeff[8]*coeff[1]*coeff[1]+coeff[2]*coeff[3]*coeff[4]+coeff[5]*coeff[0]*coeff[6]-coeff[5]*coeff[1]*coeff[3]-coeff[1]*coeff[2]*coeff[6])/det
		);
	}



	enum Intersection_type { CORNER, LINE, PLANE};

	/**
	 * direct_computation : to allow direct computation, faster but less accurate
	 */
	boost::optional<Point> solve_svd(const Point& center_cell,Intersection_type& intersection_type,double svd_thresh,bool direct_computation) const{

		Eigen::Matrix3f A;
		A(0,0) = coeff[0];
		A(1,0) = coeff[1];
		A(2,0) = coeff[2];
		A(1,1) = coeff[4];
		A(2,1) = coeff[5];
		A(2,2) = coeff[7];

		//xxx should be useless to remove when good type founded
		A(0,1) = coeff[1];
		A(0,2) = coeff[2];
		A(1,2) = coeff[5];
		//xxx

		Eigen::Vector3f b;
		b(0) = -coeff[3];
		b(1) = -coeff[6];
		b(2) = -coeff[8];


		Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> es;

		direct_computation?es.computeDirect(A):es.compute(A);
//		es.compute(A);

		Eigen::Matrix3f X = es.eigenvectors();

		Eigen::Matrix3f diag_inv = es.eigenvalues().asDiagonal();

		int num_zero_eigen_values = 0;

	//	if(diag_inv(2,2)<1e-10) UDBGVALUE(diag_inv(2,2));

		// xxx pourquoi cela arrive??
//		assert(diag_inv(2,2)>=1e-10);

		for(int i=0;i<2;++i){
			if(diag_inv(i,i)/diag_inv(2,2) <= svd_thresh){
				++num_zero_eigen_values;
				diag_inv(i,i)=0;
			}
			else diag_inv(i,i)=1/(double)diag_inv(i,i);
		}
		diag_inv(2,2)=1/(double)diag_inv(2,2);

		Eigen::Matrix3f a_inv_thresh = X * diag_inv * X.transpose();

		Eigen::Vector3f center_cell_vector;
		for(int i=0;i<3;++i){
			center_cell_vector(i)=center_cell[i];
		}

		//		UDBGVALUE(center_cell_vector);

		//		UDBGVALUE(A*a_inv_thresh);

		Eigen::Vector3f res_vector = center_cell_vector + a_inv_thresh*(b-A*center_cell_vector);


		switch (num_zero_eigen_values) {
		case 0:
			intersection_type = CORNER;
			break;
		case 1:
			intersection_type = LINE;
			break;
		case 2:
			intersection_type = PLANE;
			break;
		default:
			break;
		}
		return Point(res_vector(0),res_vector(1),res_vector(2));

	}


	double squared_norm() const{
		double res = 0;
		for(int i = 0 ; i<4 ; ++i)
			for(int j = 0 ; j<4 ; ++j)
				res+=(*this)(i,j)*(*this)(i,j);
		return res;
	}


	/**
	 * returns the point that minimizes the quadric if it is inversible and
	 * a point not too far from the star_centroid with the method of Lindstrom with
	 * eigen decomposition otherwise.
	 * direct_computation : to allow direct computation, faster but less accurate.
	 */
	boost::optional<Point> min_cost_svd(const Point& star_centroid,Intersection_type& intersection_type,double svd_threshold,bool direct_computation ) const{
		//		const double min_determinant = 1e-4 * scale*scale;
		return solve_svd(star_centroid,intersection_type,svd_threshold,direct_computation);
	}


	/**
	 * returns the point that minimizes the quadric if it is inversible and
	 * a point not too far from the star_centroid with the method of Lindstrom with
	 * eigen decomposition otherwise.
	 */
	boost::optional<Point> min_cost_svd(const Point& star_centroid,double svd_threshold,bool direct_computation) const{
		//		const double min_determinant = 1e-4 * scale*scale;
		Intersection_type intersection_type;
		return solve_svd(star_centroid,intersection_type,svd_threshold,direct_computation);
	}




	/**
	 * returns the point that minimizes the quadric.
	 * It inverses a matrix and check before if its determinant is higher that a threshold multiplied by scale^2.
	 * (scale should be a surface area)
	 * If the determinant is lower than this value the returned value is uninitialized.
	 *
	 */
	boost::optional<Point> min_cost(double scale=1) const{
		//		const double min_determinant = 1e-4 * scale*scale;
		const double min_determinant = 1e-5;

		boost::optional<Point> pt_res;


		double determinant = grad_determinant() ;


		if (determinant<0) determinant*= -1 ;
		// xxx abs fait un arondi??
		// determinant = std::abs(determinant);
		if (determinant>min_determinant) {
			pt_res = solve_linear_gradient(determinant);
		}
		else {
//			UDBGMSG("determinant too low:",determinant);
		}
		return pt_res;
	}



	friend std::ostream& operator<< (std::ostream& stream, const Error_quadric& quadric) {
		stream << "\n[ "<<quadric.coeff[0]<<","<<quadric.coeff[1]<<","<<quadric.coeff[2]<<","<<quadric.coeff[3]<<";\n";
		stream <<    " "<<quadric.coeff[1]<<","<<quadric.coeff[4]<<","<<quadric.coeff[5]<<","<<quadric.coeff[6]<<";\n";
		stream <<    " "<<quadric.coeff[2]<<","<<quadric.coeff[5]<<","<<quadric.coeff[7]<<","<<quadric.coeff[8]<<";\n";
		stream <<    " "<<quadric.coeff[3]<<","<<quadric.coeff[6]<<","<<quadric.coeff[8]<<","<<quadric.coeff[9]<<"]";
		return stream;
	}


};



}

#endif /* ERROR_QUADRIC_H_ */

