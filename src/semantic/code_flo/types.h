#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <cstring>
#include <list>
#include <map>



#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel KK;

#include <CGAL/IO/Color.h>
#include <CGAL/Kd_tree_rectangle.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Search_traits.h>
#include <CGAL/Point_container.h>
#include <CGAL/Point_2.h>
#include <CGAL/Splitters.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/bounding_box.h>
#include <CGAL/centroid.h>
//#include <CGAL/jet_smooth_point_set.h>
//#include <CGAL/pca_estimate_normals.h> 

//test line-points
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/Segment_3.h>


//convex hull
#include <CGAL/algorithm.h>
#include <CGAL/Convex_hull_traits_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/ch_graham_andrew.h>





//Delaunay 2D + alpha shape 2D
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h> 
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h> 
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Triangulation_conformer_2.h>
typedef CGAL::Polygon_2<KK> Polygon_2;
typedef CGAL::Alpha_shape_vertex_base_2<KK>    Vb2;
typedef CGAL::Alpha_shape_face_base_2<KK>    Fb2;
typedef CGAL::Triangulation_data_structure_2<Vb2,Fb2> Tds2; 
typedef CGAL::Delaunay_triangulation_2<KK,Tds2> Triangulation_2;
typedef CGAL::Alpha_shape_2<Triangulation_2>  Alpha_Shape;
 /*typedef CGAL::Triangulation_vertex_base_2<KK> Vb2t;
typedef CGAL::Delaunay_mesh_face_base_2<KK> Fb2t;
typedef CGAL::Triangulation_data_structure_2<Vb2t, Fb2t> Tds2t;
typedef CGAL::Constrained_Delaunay_triangulation_2<KK, Tds2t, CGAL::Exact_intersections_tag> CDT2;*/
typedef CGAL::Constrained_Delaunay_triangulation_2<KK> CDT2;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT2> Criteria; 
typedef CGAL::Delaunay_mesher_2<CDT2, Criteria> Meshing_engine;



typedef KK::Point_3 Point_d;
typedef KK::Segment_3 Segment_d;
typedef KK::Point_2 Point_2d;

typedef CGAL::Simple_cartesian<double>     KKsc;
typedef KK::FT FT;
typedef CGAL::Search_traits_3<KK> SearchTraits_3;
//typedef CGAL::Search_traits_3<KK> SearchTraits_3sc;
typedef CGAL::Search_traits_2<KK> SearchTraits_2;
typedef CGAL::Orthogonal_k_neighbor_search<SearchTraits_3> Neighbor_search;
//typedef CGAL::Orthogonal_k_neighbor_search<SearchTraits_3sc> Neighbor_search3sc;
typedef CGAL::Orthogonal_k_neighbor_search<SearchTraits_2> Neighbor_search2;
typedef Neighbor_search::Tree Tree;
typedef Neighbor_search2::Tree Tree2;
//typedef Neighbor_search3sc::Tree Tree3sc;
typedef KK::Iso_cuboid_3 bounding_3;
typedef KK::Iso_rectangle_2 bounding_2;


//test-convex hull
typedef CGAL::Convex_hull_traits_3<KK>             Traits;
typedef Traits::Polyhedron_3                      Polyhedron;
typedef Polyhedron::HalfedgeDS                  HalfedgeDS;

//test line-points
typedef KK::Line_3 Line_3;
typedef KK::Line_3 Line;
typedef KK::Plane_3 Plane_3;
typedef KK::Vector_3 Vector_3;

typedef KK::Segment_2 Segment_2d;
typedef KK::Line_2 Line_2;
typedef KK::Vector_2 Vector_2;




typedef CGAL::Simple_cartesian<double>     KKsc;
typedef KKsc::Plane_3 Plane_Flo;
typedef KKsc::Vector_3 Vector_Flo;
typedef KKsc::Point_3 Point_Flo;
typedef KKsc::Segment_3 Segment_Flo;
typedef KKsc::Vector_2 Vector_Flo_2;
typedef KKsc::Line_3 Line_Flo;


#define PARAM_OFSTREAM_TEXTE std::ios::out
#define OFSTREAM_TEXTE(nomvar, nomfic) std::ofstream nomvar (nomfic, PARAM_OFSTREAM_TEXTE) ; nomvar.setf(std::ios::fixed);

//Mesh simplification
#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_and_length.h> 
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/LindstromTurk_placement.h>
#include <CGAL/Surface_mesh_simplification/Detail/Edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Detail/Common.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_profile.h>
#include <CGAL/Unique_hash_map.h>
