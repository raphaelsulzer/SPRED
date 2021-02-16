/*
 * QP_solver.h
 *
 *  Created on: Mar 25, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef QP_SOLVER_H_
#define QP_SOLVER_H_

#include "contraction/quadric/Error_quadric.h"

/// qp solver///////////////////////////
#include <iostream>
#include <cassert>
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
// choose exact integral type
//#ifdef CGAL_USE_GMP
//#include <CGAL/Gmpz.h>
//typedef CGAL::Gmpz ET;
//#else
#include <CGAL/MP_Float.h>
typedef CGAL::MP_Float ET;
//#endif
// program and solution types

////////////////////////////////////////


/**
 * Solves the quadractic problem min (v^T quad v) _ { A v \leq b }
 * where A v \leq b is given by the intersection of half spaces
 * defined by the planes.
 * The planes are reoriented such that interior_point is in their
 * negative side.
 */
template <typename GT , typename STLContainer>
boost::optional<typename GT::Point_3> min_cost_qp_solver(
		const contraction::Error_quadric<GT> & quad,
		STLContainer planes,
		bool verbose = false
){
	typedef CGAL::Quadratic_program<double> Program;
	typedef CGAL::Quadratic_program_solution<ET> Solution;
	typedef typename GT::Point_3 Point_3;
	Program qp (CGAL::SMALLER, true, 0, false, 0);
	// now set the non-default entries:
	const int X = 0;
	const int Y = 1;
	const int Z = 2;



	//	UDBG("insertions contraintes ");
	qp.set_a(X, 0, 1); qp.set_a(Y, 0, 1.0); qp.set_b(0, 70000.0); // x + y <= 7000
	qp.set_u(Y, true, 70000.0); // y <= 7000

	//	UDBGMSG("quadric",quad);


	qp.set_d(X, X,2*quad(0)); // coeff[0] * x^2
	qp.set_d(Y, Y,2*quad(4)); // coeff[4] * y^2
	qp.set_d(Z, Z,2*quad(7)); // coeff[7] * z^2

	qp.set_d(Y, X,2*quad(1)); //2coeff[1] * x * y
	qp.set_d(Z, X,2*quad(2)); //2coeff[2] * x * z
	qp.set_d(Z, Y,2*quad(5)); //2coeff[5] * y * z

	qp.set_c(X,   2*quad(3)); // coeff[3] * x
	qp.set_c(Y,   2*quad(6)); // coeff[6] * y
	qp.set_c(Z,   2*quad(8)); // coeff[8] * z

	qp.set_c0(      quad(9)); // coeff[9]


	// constrains
	int i = 0;
	for(auto& plane : planes){
		qp.set_a(X, i, plane.a() ); qp.set_a(Y, i, plane.b()); qp.set_a(Z, i, plane.c()); qp.set_b(i, -plane.d()); // aX + bY + cZ + d <= 0  <=> aX + bY + cZ <= -d
		++i;
	}


	double large_value = 1e5;
	qp.set_u(X, true, large_value); // y <= large_value
	qp.set_l(X, true, -large_value); // y >= -large_value
	qp.set_u(Y, true, large_value); // y <= large_value
	qp.set_l(Y, true, -large_value); // y >= -large_value
	qp.set_u(Z, true, large_value); // y <= large_value
	qp.set_l(Z, true, -large_value); // y >= -large_value
	//

	// print the program in MPS format




	//	UDBG("contraintes rentrees");

	// solve the program, using ET as the exact type
	Solution s = CGAL::solve_quadratic_program(qp, ET());

	//	assert (s.solves_quadratic_program(qp));
	//	assert(qp.is_nonnegative());



	if(verbose){
		CGAL::print_nonnegative_quadratic_program(std::cout, qp, "qp");
		UDBGVALUE(quad);
		UDBGVALUE(s.solves_quadratic_program(qp));
		UDBGVALUE(s.solves_nonnegative_quadratic_program(qp));
		UDBGVALUE(qp.is_nonnegative());
	}

	//	UDBG("pb resolu");

	auto s_it = s.variable_values_begin();
	double x = to_double(*s_it) , y = to_double(*(++s_it)), z = to_double(*(++s_it));
	Point_3 pt_res(x,y,z);
	return Point_3(x,y,z);
}





#endif /* QP_SOLVER_H_ */
