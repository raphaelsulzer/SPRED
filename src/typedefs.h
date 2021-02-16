/*
 * typedefs.h
 *
 *  Created on: Feb 17, 2014
 *      Author: dsalinas
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_


#include <CGAL/Cartesian.h>

#include "gudhi/gudhi_complex.h"
//#include "combinatorics/Skeleton_blocker/Skeleton_blocker_simple_traits.h"
//#include "geometry/Skeleton_blocker_geometric_complex.h"

#include "semantic/Semantized_geometric_traits.h"
#include "contraction/quadric/Error_quadric.h"


using namespace Gudhi;
using namespace skbl;


typedef CGAL::Cartesian<double>     Kernel;

struct Gudhi_kernel : public Kernel{
	typedef Kernel::Point_3 Point;
};

//typedef Kernel::FT FT;
//typedef Kernel::Point_2 Point_2;
//typedef Kernel::Point_3 Point;
//typedef Kernel::Vector_3 Vector;


/// needed for semantic, cloud segmentation

typedef quadric::Error_quadric<Gudhi_kernel> Error_quadric;



typedef Semantized_geometric_traits<Gudhi_kernel,Quadric_simple_storage<Error_quadric> > Semantized_traits;


typedef Semantized_traits::Planar_proxy Planar_proxy;
typedef Kernel::Plane_3 Plane_3;
typedef Kernel::Line_3 Line_3;







typedef Skeleton_blocker_geometric_complex< Semantized_traits > Complex;

typedef Complex::Graph_vertex Graph_vertex;
typedef Complex::Graph_edge Graph_edge;
typedef Complex::Vertex_handle Vertex_handle;
typedef Complex::Simplex_handle Simplex_handle;
typedef Complex::Root_vertex_handle Root_vertex_handle;
typedef Complex::Edge_handle Edge_handle;



#endif /* TYPEDEFS_H_ */
