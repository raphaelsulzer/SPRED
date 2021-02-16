/*
 * CGAL_contractor.cpp
 *
 *  Created on: Apr 7, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */


#include "urban_utils.h"

#include "CGAL_contractor.h"


#include <iostream>
#include <fstream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
// Adaptor for Polyhedron_3
#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>
// Simplification function
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
// Stop-condition policy
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Polyhedron_items_with_id_3.h>

//#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_and_length.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk.h>

#include "CGAL/Surface_mesh_simplification/Detail/Common.h"
#include "CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h"
#include "CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_profile.h"

#include "typedefs.h"


#include "utils/MClock.h"

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Polyhedron_3<K,CGAL::Polyhedron_items_with_id_3> Surface_mesh;
//typedef CGAL::Polyhedron_3<K> Surface_mesh;
namespace SMS = CGAL::Surface_mesh_simplification ;


namespace cgal_contraction {

template<class ECM_>
struct Num_vertices_stop{

	typedef ECM_ ECM ;

	typedef SMS::Edge_profile<ECM> Profile ;
	typedef typename boost::graph_traits<ECM>::edge_descriptor edge_descriptor ;
	typedef typename boost::graph_traits<ECM>::edges_size_type size_type ;

//	typedef typename halfedge_graph_traits<ECM>::Points Point ;
//	typedef typename Kernel_traits<Point>::Kernel      Kernel ;
//	typedef typename Kernel::FT                        FT ;

	typedef  Gudhi_kernel::Point Point ;
	typedef  Gudhi_kernel Kernel;
	typedef  Kernel::FT                        FT ;

	int num_vertices_wanted_;

	Num_vertices_stop( int num_vertices_wanted):num_vertices_wanted_(num_vertices_wanted)
	{

	}


	bool operator() ( 	FT const &  	current_cost,
			Profile const &  	profile,
			size_type  	initial_count,
			size_type  	current_count
	) 		const{
		assert(num_vertices_wanted_ <= profile.surface().size_of_vertices());
		return profile.surface().size_of_vertices() <= num_vertices_wanted_;
	}
};


void cgal_collapse(std::string name_file,int num_collapses) {
	UDBGVALUE(num_collapses);
	UDBGVALUE(name_file);

	Surface_mesh surface_mesh;
	std::ifstream is(name_file) ; is >> surface_mesh ;


	std::cout << "\nLoad mesh with "<< (surface_mesh.size_of_halfedges()/2) << " edges.\n" ;

	// This is a stop predicate (defines when the algorithm terminates).
	// In this example, the simplification stops when the number of undirected edges
	// left in the surface mesh drops below the specified number (1000)
	MClock clock;
	UDBGVALUE(surface_mesh.size_of_vertices());
	UDBGVALUE(num_collapses);
	UDBGVALUE(surface_mesh.size_of_vertices() - num_collapses);
	Num_vertices_stop<Surface_mesh> stop(surface_mesh.size_of_vertices() - num_collapses);


	int index = 0 ;
	for( Surface_mesh::Halfedge_iterator eb = surface_mesh.halfedges_begin()
			, ee = surface_mesh.halfedges_end()
			; eb != ee
			; ++ eb
	)
		eb->id() = index++;
	index = 0 ;
	for( Surface_mesh::Vertex_iterator vb = surface_mesh.vertices_begin()
			, ve = surface_mesh.vertices_end()
			; vb != ve
			; ++ vb
	)
		vb->id() = index++;


	// This the actual call to the simplification algorithm.
	// The surface mesh and stop conditions are mandatory arguments.
	// The index maps are needed because the vertices and edges
	// of this surface mesh lack an "id()" field.
	int r = SMS::edge_collapse
			(surface_mesh
					,stop
                    ,CGAL::parameters::get_cost (SMS::LindstromTurk_cost<Surface_mesh>())
                                        .get_placement(SMS::LindstromTurk_placement<Surface_mesh>())
	//					,CGAL::vertex_index_map(boost::get(CGAL::vertex_external_index,surface_mesh))
	//	.edge_index_map (boost::get(CGAL::edge_external_index ,surface_mesh))
			);
	clock.end();
	std::cout << "\nFinished...\n" << r << " edges removed.\n"
			<< (surface_mesh.size_of_halfedges()/2) << " final edges.\n" ;
	std::cout <<" in "<<clock<<" s (i.e. "<< num_collapses/clock.num_seconds()  <<"/s)"<<std::endl;

	std::ofstream os( name_file) ;	os << surface_mesh ;
}
}

