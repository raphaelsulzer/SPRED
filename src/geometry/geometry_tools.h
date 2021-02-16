/*
 * geometry_tools.h
 *
 *  Created on: Mar 27, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef GEOMETRY_TOOLS_H_
#define GEOMETRY_TOOLS_H_

#include "urban_utils.h"

#include <iostream>
#include <list>
#include <CGAL/Simple_cartesian.h>



template<typename Vector>
double scalar_product_vectors(Vector n1, Vector n2){
	n1 = n1 / std::sqrt( n1.squared_length());
	n2 = n2 / std::sqrt( n2.squared_length());
	double scal_prod(n1*n2);
	assert(std::abs(scal_prod)<1+1e-4);
	if(std::abs(scal_prod)>1) {
		scal_prod>0?scal_prod = 1 :scal_prod = -1 ;
	}
	assert(-1<= scal_prod && scal_prod<=1);
	return scal_prod;
}


template<typename Plane>
double scalar_product_normals(const Plane& p1, const Plane& p2){
	return scalar_product_vectors(p1.orthogonal_vector(),p2.orthogonal_vector());
}


template<typename Plane>
double angle_between_two_planes(const Plane& p1, const Plane& p2){
	double sc_prod = scalar_product_normals(p1,p2);
	return std::acos(sc_prod);
}



template<typename Plane>
double determinant(const Plane& p1, const Plane& p2,const Plane& p3){
	auto n1(p1.orthogonal_vector());
	auto n2(p2.orthogonal_vector());
	auto n3(p3.orthogonal_vector());
	n1 = n1 / std::sqrt( n1.squared_length());
	n2 = n2 / std::sqrt( n2.squared_length());
	n3 = n3 / std::sqrt( n3.squared_length());
	return CGAL::determinant(n1,n2,n3);
}


template<typename Plane_primitive>
boost::optional<typename Plane_primitive::GT::Point> proj_on_three_intersection(const Plane_primitive* p1,const Plane_primitive* p2,const Plane_primitive* p3){
	typedef typename  Plane_primitive::GT GT;
	typedef typename GT::Point Point;
	typedef typename GT::Line_3 Line_3;
	boost::optional<Point> res;

	CGAL::Object ob_temp=intersection(p1->plane(),p2->plane());
	Line_3 line;
	if(!assign(line, ob_temp)) {
//		double p1p2  =	std::abs(scalar_product_normals(p1->plane(),p2->plane()))			,
//				p2p3 = std::abs(scalar_product_normals(p2->plane(),p3->plane())),
//				p3p1 = std::abs(scalar_product_normals(p1->plane(),p3->plane()));
//		UDBG("\n\n ");
//		UDBG("P1 / / P2");
//		UDBGVALUE(p1p2);
//		UDBGVALUE(p2p3);
//		UDBGVALUE(p3p1);
		return res;
	}
	CGAL::Object ob_temp2=intersection(p3->plane(),line);
	Point proj;
	if(!assign(proj, ob_temp2)) {
//		UDBG("\n\n ");
//		UDBG("P3 / / (P1 cap P2)");
//		UDBG("P1 cap P2 (line):");
//		UDBG("P3 :");
//		double p1p2  =	std::abs(scalar_product_normals(p1->plane(),p2->plane()))			,
//				p2p3 = std::abs(scalar_product_normals(p2->plane(),p3->plane())),
//				p3p1 = std::abs(scalar_product_normals(p1->plane(),p3->plane()));
//		UDBGVALUE(p1);
//		UDBGVALUE(p2);
//		UDBGVALUE(p3);
//		UDBGVALUE(p1p2);
//		UDBGVALUE(p2p3);
//		UDBGVALUE(p3p1);

		res.reset();
		return res;
	}
	else{
		res = proj;
		return res;
	}
}







#endif /* GEOMETRY_TOOLS_H_ */
