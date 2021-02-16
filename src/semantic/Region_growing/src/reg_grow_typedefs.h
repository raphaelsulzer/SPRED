/*
 * reg_grow_typedefs.h
 *  Created on: Dec 3, 2014
 * This file is part of the Gudhi Library. The Gudhi library 
 *    (Geometric Understanding in Higher Dimensions) is a generic C++ 
 *    library for computational topology.
 *
 *    Author(s):       David Salinas
 *
 *    Copyright (C) 2014  INRIA Sophia Antipolis-Méditerranée (France)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#ifndef REG_GROW_TYPEDEFS_H_
#define REG_GROW_TYPEDEFS_H_


#include "CGAL/Plane_3.h"
#include <CGAL/Cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>



namespace regrow {

typedef CGAL::Cartesian<double>     Kernel;
typedef Kernel::Plane_3 Plane_3;
typedef Kernel::Point_3 Point;


}  // namespace regrow


#endif /* REG_GROW_TYPEDEFS_H_ */
