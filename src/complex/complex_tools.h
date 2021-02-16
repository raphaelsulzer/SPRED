/*
 * complex_tools.h
 *
 *  Created on: Feb 25, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef COMPLEX_TOOLS_H_
#define COMPLEX_TOOLS_H_

#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/squared_distance_3.h>
#include "urban_utils.h"

//#include "utils/reorient/Reorient.h"
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////     Mesh     /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///**
// * Given a file off that contains triangles of a manifold, it reorients coherently the faces
// * and modify the off file.
// */
//inline void reorient_faces(std::string name_file){
//	Reorient::reorient(Reorient::OFF_save_visitor(name_file+"reoriented"),name_file);
////	std::stringstream cmd;
////	//	cmd << "meshlabserver -i ../code/data/tmp.off -o ../code/data/tmp.off -s ../code/data/mesh_lab_script.mlx";
////
////	cmd << "meshlabserver -i "<< name_file << " -o "<< name_file << " -s " <<"~/Documents/CodeSVN/urban/code/data/meshlab_reorient.mlx > tmp_.txt";
////
////	//std::string cmd("meshlabserver -i ../code/data/tmp.off -o ../code/data/tmp.off -s ../code/data/mesh_lab_script.mlx");
////	auto r = system (cmd.str().c_str());
////	if (r); // remove warning
//}
//




/**
 * @brief return true iff the complex is a manifold at the edge
 */
template <typename ComplexType>
bool
is_two_manifold(ComplexType& complex, typename ComplexType::Edge_handle edge,bool& is_border){
	Skeleton_blocker_link_complex<ComplexType> link(complex,edge);
	is_border = (link.num_vertices()==1);
	return is_border || (link.num_vertices()==2&& link.num_edges()==0);
}

//template <typename ComplexType>
//bool
//is_two_manifold(
//		const ComplexType& complex, typename ComplexType::Vertex_handle v,
//		const Skeleton_blocker_link_complex<ComplexType>& link_v,
//
//		bool& is_border){
//}



/**
 * Walk through the vertices of the link.
 * If the link is a 1-sphere or a 1-ball, it initializes the ordered vertices.
 * The returned boolean is true if the link is a 1-ball.
 */
template <typename ComplexType>
bool two_manifold_walk(
		const ComplexType& complex,
		const Skeleton_blocker_link_complex<ComplexType>& link,
		boost::optional<std::list<typename ComplexType::Vertex_handle> >& ordered_vertices
){
	ordered_vertices = std::list<typename ComplexType::Vertex_handle>();
	// or vector with link.vector_size()
	std::set<Vertex_handle> already_seen;

	if(link.empty()) {
		//		UDBG("empty link");
		//		UDBGVALUE(link.to_string());

		return false;
	}

	auto first_nv  = *(link.vertex_range().begin());
	auto current_v  = first_nv;

	already_seen.insert(current_v);

	ordered_vertices->push_back(*complex.get_address(link.get_id(current_v)));

	int num_seen = 1;
	do{
		int num_neighbors = link.degree(current_v);

		if(num_neighbors==0 || num_neighbors>2) {
			//			UDBG("0 OR 2 NEIGHBORS");
			ordered_vertices.reset();
			return false;
		}
		if(num_neighbors == 1) {
			// in this case, border we have to start from the first
			// vertex to see if the link is a 1-ball
			return two_manifold_border_walk(complex,link,ordered_vertices,current_v);
		}

		// num_neighbors = 2
		// we now look for a neighbor of nv that has not already been seen
		bool find = false;

		for(auto nv: link.vertex_range(current_v)){
			if(already_seen.find(nv) == already_seen.end()) {
				already_seen.insert(nv);
				current_v = nv;
				ordered_vertices->push_back(*complex.get_address(link.get_id(current_v)));
				find = true;
				break;
			}
		}
		if(find) ++num_seen;
		else{

			// if no new vertex, we find a 1-sphere if we are back
			// at first_nv and we explored all vertices
			bool is_two_manif = (num_seen == link.num_vertices());

			if(!is_two_manif) ordered_vertices.reset();
			return false;
		}
	} while(true);
}



/**
 * Walk through the vertices of the link starting from a degree 1 vertex.
 * It checks if the link is a one ball, and in this case it initializes the ordered vertices.
 * Otherwise it return false.
 */
template <typename ComplexType>
bool two_manifold_border_walk(
		const ComplexType& complex,
		const Skeleton_blocker_link_complex<ComplexType>& link,
		boost::optional<std::list<typename ComplexType::Vertex_handle> >& ordered_vertices,
		typename ComplexType::Vertex_handle degree_1_vertex
){
	bool is_border = false;

	ordered_vertices = std::list<typename ComplexType::Vertex_handle>();

	if(link.empty()) return false;

	assert(link.degree(degree_1_vertex)==1);

	auto current_v  = degree_1_vertex;

	std::set<Vertex_handle> already_seen;
	already_seen.insert(current_v);

	int num_seen = 1;
	do{
		//		UDBGVALUE(current_v);
		int num_neighbors = link.degree(current_v);
		if(num_neighbors==0 || num_neighbors>2) {
			ordered_vertices.reset();
			return false;
		}

		if(num_neighbors == 1 && current_v != degree_1_vertex) {
			// in this case, border if we have seen all vertices
			is_border = (already_seen.size() == link.num_vertices());
			if(!is_border){
				//				UDBG("not border in");
				//				UDBGVALUE(already_seen.size());
				//				UDBGVALUE(link.num_vertices());
				//				UDBGVALUE(link.to_string());
			}
			if(!is_border) ordered_vertices.reset();
			return is_border;
		}

		// num_neighbors = 2
		// we now look for a neighbor of nv that has not already been seen
		bool find = false;
		for(auto nv: link.vertex_range(current_v)){
			if(already_seen.find(nv) == already_seen.end()) {
				already_seen.insert(nv);
				current_v = nv;
				ordered_vertices->push_back(*complex.get_address(link.get_id(current_v)));
				find = true;
			}
		}
		if(find) ++num_seen;
		else {
			ordered_vertices.reset();
			//			UDBG("not find");
			return false;
		}

	} while(true);
	return is_border;
}


/**
 * @brief return true iff the complex is a 2-manifold at the vertex
 * It checks if the link is either
 *   - a 1-sphere  (a1 a2) (a2 a3) ... (an-1 a1)
 *   - or a 1-ball (a sequence of consecutive disjoint segment such as
 *   (a1 a2) (a2 a3) ... (an-1 an)
 * is_border is true if we are in the second case
 *
 * It first looks at the degree of vertices.
 * If all of them are 2, it looks for the first case and
 * if one of them is 1 it looks for the second one.
 *
 * todo a tester et remplacer is_border par ce test
 */
template <typename ComplexType>
bool
is_two_manifold(const ComplexType& complex, typename ComplexType::Vertex_handle v,bool& is_border){
	is_border = false;
	Skeleton_blocker_link_complex<ComplexType> link(complex,v);

	boost::optional<std::list<typename ComplexType::Vertex_handle> > ordered_vertices;

	is_border = two_manifold_walk(complex,link,ordered_vertices);
	//bug vs with optional then !!
	return !!ordered_vertices;
}


/**
 * @brief return true iff the complex is a 2-manifold border at the vertex
 * It checks if the link is a 1-ball (a sequence of consecutive disjoint segment
 * such as (a1 a2) (a2 a3) ... (an-1 an)
 */
template <typename ComplexType>
bool
is_two_manifold_border_vertex(
		const ComplexType& complex,
		typename ComplexType::Vertex_handle v
){
	bool is_border = false;
	Skeleton_blocker_link_complex<ComplexType> link(complex,v);


	boost::optional<std::list<typename ComplexType::Vertex_handle> > ordered_vertices;

	//	// we look for a vertex of degree one
	auto first_nv  = *(link.vertex_range().begin());
	if(link.degree(first_nv)!=1){
		for(auto v : link.vertex_range()){
			if(link.degree(v)>3) return false;
			if(link.degree(v)==1) {
				first_nv = v;
				break;
			}
		}
		if(link.degree(first_nv)!=1) {
			//			UDBGVALUE(link.degree(first_nv));
			return false;
		}
	}

	return two_manifold_border_walk(complex,link,ordered_vertices,first_nv);

	//	if(link.empty()) return false;
	//
	//	// we look for a vertex of degree one
	//	auto first_nv  = *(link.vertex_range().begin());
	//	if(link.degree(first_nv)!=1){
	//		for(auto v : link.vertex_range()){
	//			if(link.degree(v)>3) return false;
	//			if(link.degree(v)==1) {
	//				first_nv = v;
	//				break;
	//			}
	//		}
	//		if(link.degree(first_nv)!=1) {
	//			//			UDBGVALUE(link.degree(first_nv));
	//			return false;
	//		}
	//	}
	//
	//	auto current_v  = first_nv;
	//
	//	std::set<Vertex_handle> already_seen;
	//	already_seen.insert(first_nv);
	//
	//	int num_seen = 1;
	//	do{
	//		//		UDBGVALUE(current_v);
	//		int num_neighbors = link.degree(current_v);
	//		if(num_neighbors==0 || num_neighbors>2) return false;
	//
	//		if(num_neighbors == 1 && current_v != first_nv) {
	//			// in this case, border if we have seen all vertices
	//			is_border = (already_seen.size() == link.num_vertices());
	//			if(!is_border){
	//				//				UDBG("not border in");
	//				//				UDBGVALUE(already_seen.size());
	//				//				UDBGVALUE(link.num_vertices());
	//				//				UDBGVALUE(link.to_string());
	//			}
	//			return is_border;
	//		}
	//
	//		// num_neighbors = 2
	//		// we now look for a neighbor of nv that has not already been seen
	//		bool find = false;
	//		for(auto nv: link.vertex_range(current_v)){
	//			if(already_seen.find(nv) == already_seen.end()) {
	//				already_seen.insert(nv);
	//				current_v = nv;
	//				find = true;
	//			}
	//		}
	//		if(find) ++num_seen;
	//		else {
	//			//			UDBG("not find");
	//			return false;
	//		}
	//
	//	} while(true);
}

/**
 * @brief return true iff the complex is a 2-manifold
 */
template <typename ComplexType>
bool
is_two_manifold(ComplexType& complex){
	bool is_border;
	for(auto v: complex.vertex_range()){
		if (complex.degree(v)!=0 && !is_two_manifold(complex,v,is_border)) return false;
	}
	return true;
}



/**
 * @brief remove pending vertices, those whose link is a point
 * return the number of remove vertices
 */
template <typename ComplexType>
int
remove_pending_vertices(ComplexType& complex){
	typedef typename ComplexType::Vertex_handle Vertex_handle;

	int removed_vertices = 0;
	std::list<Vertex_handle> pending_vertices;
	do{
		pending_vertices.clear();
		for(auto vh: complex.vertex_range()){
			if(complex.degree(vh)==1) {
				pending_vertices.push_back(vh);
				continue;
			}
			Skeleton_blocker_link_complex<ComplexType> link_vh(complex,vh);
			if (link_vh.num_vertices()==0 || (link_vh.num_edges()==1 && link_vh.num_vertices()==2)) pending_vertices.push_back(vh);
		}

		std::for_each(pending_vertices.begin(), pending_vertices.end(),
				[&](Vertex_handle vh){complex.remove_star(vh);}
		);
		removed_vertices+= pending_vertices.size();
	} while (pending_vertices.size()!= 0);
	return removed_vertices;

}


/**
 * @brief Returns a initialized value iff the edge passed as argument
 * is adjacent to exactely one triangle.
 * In this case, the value returned is a vertex that belongs to a unique triangle passing
 * through the edge.
 */
template <typename ComplexType>
boost::optional<typename ComplexType::Vertex_handle>
is_border_edge(ComplexType& complex, typename ComplexType::Edge_handle edge){
	typedef typename ComplexType::Vertex_handle Vertex_handle;
	typedef typename ComplexType::Simplex_handle Simplex_handle;

	boost::optional<Vertex_handle> res;
	Simplex_handle edge_simplex(complex.get_vertices(edge));
	//todo ajouter un template pour indiquer lq le calcul du link doit s'arêter
	// par ex ici il s'arete si plus d'un sommet ou si on trouve une arete
	Skeleton_blocker_link_complex<ComplexType> link(complex,edge_simplex);
	if (link.num_vertices()!=1)
		return res;
	else{
		return *complex.get_address(link.get_id( *link.vertex_range().begin()));
		//		auto id_simplex = link.get_id( *link.triangle_range().begin());
		//		return complex.get_simplex_address(id_simplex);
	}
}


/**
 * @brief Returns true iff the vertex passed as argument
 * is adjacent to at least one border edge
 *
 * todo faster compute the link and test if a vertex has degree 1
 */
template <typename ComplexType>
bool
is_border_vertex(ComplexType& complex, typename ComplexType::Vertex_handle u){
	Skeleton_blocker_link_complex<ComplexType> link(complex,u);

	for(auto nu : link.vertex_range()){
		if(link.degree(nu)==1) return true;
	}
	return false;

	//	for(auto v : complex.vertex_range(u)){
	//		auto uv(complex[std::make_pair(u,v)]);
	//				// the edge must be in the complex
	//				assert(uv);
	//				if (is_border_edge<ComplexType>(complex,*uv))
	//					return true;
	//	}
	//	return false;
}





/**
 * @brief Returns true iff no triangle passes through the edge.
 */
template <typename ComplexType>
bool
no_triangle_through_edge(ComplexType& complex, typename ComplexType::Edge_handle edge){
	typedef typename ComplexType::Simplex_handle Simplex_handle;
	boost::optional<Simplex_handle> res;
	Simplex_handle edge_simplex(complex.get_vertices(edge));
	Skeleton_blocker_link_complex<ComplexType> link(complex,edge_simplex);
	// there is no triangle through the edge iff there is no vertex in the link
	return (link.num_vertices()==0);
}


/**
 * gives a list of triangles around 'a' and 'b'.
 * Triangles that passes through 'ab' are the first elements in the list.
 */
template<typename Complex>
std::list<typename Complex::Simplex_handle> star_triangles(
		Complex& mesh,
		typename Complex::Vertex_handle a,
		typename Complex::Vertex_handle b){
	std::list<typename Complex::Simplex_handle> res;

	for(auto t_a : mesh.triangle_range(a)){
		auto v_it = t_a.begin();
		if(!t_a.contains(b))
			res.push_back(Simplex_handle(*(v_it++),*(v_it++),*(v_it++)));
		else
			res.push_front(Simplex_handle(*(v_it++),*(v_it++),*(v_it++)));
	}

	for(auto t_b : mesh.triangle_range(b)){

		if(!t_b.contains(a)){
			auto v_it = t_b.begin();
			res.push_back(Simplex_handle(*(v_it++),*(v_it++),*(v_it++)));
		}
	}
	return res;
}



/**
 * @return the 'offset'-ring neighbor of 'vertex'
 * specialized for every datastructure
 *
 * todo remove the nexttobeseen!!!
 */
template<class ComplexType>
typename std::set< typename ComplexType::Vertex_handle>
neighbors(ComplexType& complex,typename ComplexType::Vertex_handle vertex,int offset=0){
	int current_offset = 0;

	std::set<typename ComplexType::Vertex_handle> seen;
	std::list<typename ComplexType::Vertex_handle> tobeseen;
	std::list<typename ComplexType::Vertex_handle> nexttobeseen;


	tobeseen.push_back(vertex);

	while(current_offset++ <= offset){
		while(!tobeseen.empty()){
			auto v = tobeseen.back();
			tobeseen.pop_back();
			seen.insert(v);
			if(current_offset<=offset){
				for(auto nv : complex.vertex_range(v)){
					if(seen.find(nv)==seen.end()){
						nexttobeseen.push_back(nv);
					}
				}
			}
		}
		tobeseen = nexttobeseen;
	}
	return seen;
}


/**
 * @brief Returns the first blocker passing through the edge a b if it exists
 * and NULL otherwise
 */
template<class ComplexType>
typename ComplexType::Simplex_handle*
get_blocker_through_edge(ComplexType& complex,typename ComplexType::Vertex_handle a,typename ComplexType::Vertex_handle b){
	for (auto blocker_ab : complex.blocker_range(a)){
		if (blocker_ab->contains(b)){
			return blocker_ab;
		}
	}
	return NULL;
}


/**
 * @returns true iff for all edge of the blocker, the edge is a border edge.
 */
template<class ComplexType>
bool
is_blocker_hole(ComplexType& complex,typename ComplexType::Simplex_handle* blocker){
	for (auto x  = blocker->begin() ; x!= blocker->end(); ++x){
		auto y = x;
		for(++y ; y!= blocker->end(); ++y){
			auto xy(complex[std::make_pair(*x,*y)]);
			assert(xy);
			if (!is_border_edge(complex,*xy)){
				return false;
			}
		}
	}
	return true;
}



/**
 * @brief Remove all the blockers of an edge a b if all them are "removable" or keep
 * the blockers the same otherwise (a blocker is "removable" if the it is popable or a simple hole).
 * @returns true iff the set of blockers passing through ab is empty at the end.
 */
template<typename ComplexType>
bool
filter_blockers(ComplexType& complex,typename ComplexType::Vertex_handle a,typename ComplexType::Vertex_handle b){
	typedef typename ComplexType::Simplex_handle Simplex_handle;
	std::list<Simplex_handle*> blockers_removed;
	Simplex_handle* blocker_ab;
	while((blocker_ab = get_blocker_through_edge(complex,a,b)) && blocker_ab){
		if(!is_blocker_hole(complex,blocker_ab)&&!complex.is_popable_blocker(blocker_ab))
			break;
		else{
			complex.remove_blocker(blocker_ab);
			blockers_removed.push_back(blocker_ab);
		}
	}
	// if there is still some blockers attached to ab then
	// we need to readd the blockers we removed in order to keep
	// the blocker set unchanged.
	if (get_blocker_through_edge(complex,a,b)){
		for(auto b : blockers_removed)
			complex.add_blocker(*b);
		return false;
	}
	else return true;

}




/**
 * @return the point corresponding to the centroid of the star of the edge
 */
template<typename ComplexType>
typename ComplexType::Point
star_centroid(ComplexType& complex,typename ComplexType::Edge_handle v0v1){
	typedef typename ComplexType::Vertex_handle Vertex_handle;
	typedef typename ComplexType::Point Point;


	auto v0 = complex.first_vertex(v0v1);
	auto v1 = complex.second_vertex(v0v1);
	std::set<Vertex_handle> star_vertices;
	// get the star vertices

	star_vertices.insert(complex.vertex_range(v0).begin(),complex.vertex_range(v0).end());
	star_vertices.insert(complex.vertex_range(v1).begin(),complex.vertex_range(v1).end());

	std::vector<Point> points_of_star;
	for(auto v : star_vertices){
		points_of_star.push_back(complex.point(v));
	}

	return CGAL::centroid(points_of_star.begin(),points_of_star.end());
}



/**
 * @return the maximum distance from vertices of the link of an edge v0v1
 *  to the plane approximating these vertices.
 */
template<typename ComplexType>
double
max_squared_distance_star_plane(ComplexType& complex,typename ComplexType::Edge_handle v0v1){
	typedef typename ComplexType::Vertex_handle Vertex_handle;
	typedef typename ComplexType::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename ComplexType::Point Point;


	auto v0 = complex.first_vertex(v0v1);
	auto v1 = complex.second_vertex(v0v1);
	std::set<Vertex_handle> star_vertices;
	// get the star vertices

	Skeleton_blocker_link_complex<Complex> link_v0(complex,v0);
	Skeleton_blocker_link_complex<Complex> link_v1(complex,v1);

	for (auto v : link_v0.vertex_range()){
		auto v_complex(complex.get_address(link_v0.get_id(v)));
		star_vertices.insert(*v_complex);
	}
	for (auto v : link_v1.vertex_range()){
		auto v_complex(complex.get_address(link_v1.get_id(v)));
		star_vertices.insert(*v_complex);
	}

	// xxx todo std::vector<Point> point_for_least_squares;
	std::vector<Point> point_for_least_squares;
	for(auto v : star_vertices){
		point_for_least_squares.push_back(complex.point(v));
	}

	Plane_3 optimal_plane;
	CGAL::linear_least_squares_fitting_3(point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());

	double dist_max = 0 ;
	for (auto p : point_for_least_squares  ){
		dist_max = std::max( CGAL::squared_distance( p , optimal_plane )  , dist_max );
	}

	return dist_max;
}

/**
 * @return true if the star of the edge is coplanar
 */
template<typename EdgeProfile>
double
is_star_almost_coplanar(const EdgeProfile& profile,double coplanarity_tolerance){
	double dist_max  =  max_squared_distance_star_plane( profile.complex() , profile.edge_handle() );
	auto length = squared_distance(profile.p0(),profile.p1());
	return dist_max < length * coplanarity_tolerance *coplanarity_tolerance;
}

/**
 * @return True if the neighborhood of the edge is non degenerate.
 * Non degenerate can mean not low quadric determinant, or non planar star for instance.
 */
template<typename EdgeProfile,typename ErrorQuadric>
bool
is_neighborhood_degenerate(const EdgeProfile& profile,const ErrorQuadric& quadric){
	//	auto length = squared_distance(profile.p0(),profile.p1());
	//	double tolerance = 1e-4;
	//	double dist_max  =  max_squared_distance_star_plane( profile.complex() , profile.edge_handle() );
	//	return (dist_max < length * tolerance *tolerance) || (quadric.determinant()<1e-4 * length*length);

	//	auto length = squared_distance(profile.p0(),profile.p1());
	return (quadric.grad_determinant()<1e-2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////     Proxies     /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * return true iff it manages to extract a polygon whose edges are all border edges with a simple strategy.
 * This strategy picks first a vertex v which belongs to the convex hull of the complex.
 * Then it iteratively looks for a next vertex 'next_v' in the neighbor of the current vertex 'current_v' (starting from v)
 * such that (next_v,current_v) is a border edge of the complex whose has not been seen before.
 *
 *
 * todo repeat for all connected components
 */
template<typename ComplexType>
bool extract_border(
		const ComplexType& complex,
		std::list<typename ComplexType::Root_vertex_handle>& root_border_vertices
){
	typedef typename ComplexType::Vertex_handle Vertex_handle;

	typedef typename ComplexType::Simplex_handle Simplex_handle;


	std::list<typename ComplexType::Vertex_handle>	border_vertices;

	// we look for the vertex with minimum x field, it ensures that the associated
	// point belongs to the border
	double x_min = complex.point(* complex.vertex_range().begin()).x();
	Vertex_handle v_x_min = *complex.vertex_range().begin();
	for (auto vi : complex.vertex_range()){
		if ( complex.point(vi).x() < x_min){
			x_min = complex.point(vi).x();
			v_x_min = vi;
		}
	}

	// we iteratively pick a neighbor such that the current vertex plus the neighbor
	// forms a boundary edge
	Vertex_handle old_v = v_x_min;
	border_vertices.push_back(v_x_min);
	std::set<Vertex_handle> vertices_seen;
	vertices_seen.insert(v_x_min);

	do{
		Simplex_handle v_neighbor;
		complex.add_neighbours(border_vertices.back(),v_neighbor);
		bool found_unseen_vertex = false;
		for (auto neighbor : v_neighbor){
			// check if already seen
			if(vertices_seen.find(neighbor) == vertices_seen.end()){
				typename ComplexType::Edge_handle edge(*complex[std::make_pair(border_vertices.back(),neighbor)]);
				if (is_border_edge(complex,edge)){
					old_v = border_vertices.back();
					vertices_seen.insert(neighbor);
					border_vertices.push_back(neighbor);
					found_unseen_vertex = true;
					break;
				}
			}
		}
		if (!found_unseen_vertex) {
			break;
		}
	} while (true);

	bool succeeded = false;
	Simplex_handle v_neighbor;
	complex.add_neighbours(border_vertices.back(),v_neighbor);
	// the strategy suceeds if it ends near v_x_min and found
	// a polygon with more than 3 vertices
	if (border_vertices.size()>=3){
		for (auto neighbor : v_neighbor){
			if (neighbor == v_x_min){
				succeeded = true;
			}
		}
	}

	root_border_vertices.clear();
	for (auto vi : border_vertices)
		root_border_vertices.push_back(complex.get_id(vi));

	return succeeded;
}







/**
 * return the maximum squared distance between a point and its primitive
 */
template<typename ComplexType>
double max_squared_distance_to_proxy_plane(const ComplexType& complex){
	double max_distance = 0;
	for (auto vi : complex.vertex_range()){
		for(auto primitive : complex[vi].plane_primitives())
			max_distance = std::max( max_distance , squared_distance(complex.point(vi), primitive->plane()) );
	}
	return max_distance;
}


// xxx Mesh should be const
/**
 * detect if vertex is a corner ie if it has at least a triplet of proxies
 * such that their 3-intersection is at a distance lower than max_distance from the vertex.
 * If it is the case, it return the point of this three intersection
 */
template<typename Mesh>
boost::optional<typename Mesh::Point> is_corner_vertex( Mesh& mesh, typename Mesh::Vertex_handle vertex,double max_distance){
	boost::optional<typename Mesh::Point> res;
	auto graph_vertex = mesh[vertex];
	auto proxies(graph_vertex.plane_primitives());
	if(proxies.size()<3) return res;
	else{

		for(auto p1 = graph_vertex.plane_primitives().cbegin(); p1 != graph_vertex.plane_primitives().cend(); ++p1 ){
			for(auto p2 = p1; ++p2 != graph_vertex.plane_primitives().cend();  ){
				for(auto p3 = p2 ; ++p3 != graph_vertex.plane_primitives().cend();  ){
					// todo faire comme LT rajouter que des plans independants
					boost::optional<Gudhi_kernel::Point> proj = proj_on_three_intersection<Planar_proxy>(*p1,*p2,*p3);
					//		return (graph_vertex.num_plane_primitives()>=3 && proj);
					if(proj && (squared_distance(*proj,mesh.point(vertex))<max_distance*max_distance) )
						return *proj;
				}
			}
		}
		return res;
	}
}


// xxx Mesh should be const
template<typename Mesh>
int num_corner_vertices( Mesh& mesh, double max_distance,bool verbose = false){
	int res = 0;
	for (auto vi : mesh.vertex_range()){
		if (is_corner_vertex(mesh,vi,max_distance))
			++res;
	}
	return res;
}


template<typename Semantized_geometric_vertex>
bool is_primitives_included(const Semantized_geometric_vertex& v0,const Semantized_geometric_vertex& v1){
	if( v0.plane_primitives().size() >  v1.plane_primitives().size())
		return false;
	for (auto primitive: v0.plane_primitives()){
		if(v1.plane_primitives().find(primitive)  == v1.primitives().end())
			return false;
	}
	return true;
}


/**
 * keep only the vertices which have proxies attached.
 */
template<typename Mesh>
void keep_only_proxy_vertices(Mesh & complex){
	for(auto v : complex.vertex_range()){
		if (complex[v].empty_plane_primitives())
			complex.remove_star(v);
	}
}



/**
 * return proxies(v0) \cup proxies(v1) for an edge v0v1
 */
template<typename Mesh>
typename Mesh::Graph_vertex::Proxies
union_proxies(const Mesh& complex,typename Mesh::Edge_handle edge){
	typename Mesh::Graph_vertex::Proxies res = complex[complex.first_vertex(edge)].plane_primitives();
	res.insert(complex[complex.second_vertex(edge)].plane_primitives().begin(),complex[complex.second_vertex(edge)].plane_primitives().end());
	return res;
}



template<typename Semantized_geometric_vertex>
bool has_same_primitives(const Semantized_geometric_vertex& v0,const Semantized_geometric_vertex& v1){
	return is_primitives_included(v0,v1) || is_primitives_included(v1,v0);
	//	if( v0.primitives_.size() !=  v1.primitives_.size())
	//		 return false;
	//	for (auto primitive: v0.primitives_){
	//		if(v1.primitives_.find(primitive)  == v1.primitives_.end())
	//			return false;
	//	}
	//	return true;
}



template<typename Mesh>
typename Mesh::Graph_vertex::Planar_proxy_handle
get_majority_proxy_from_triangle(const Mesh & mesh,const typename Mesh::Simplex_handle& triangle){
	typedef typename Mesh::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	std::map<Planar_proxy_handle,int> num_proxy;
	for(auto vh : triangle){
		for(auto proxy : mesh[vh].plane_primitives()){
			++num_proxy[proxy];
		}
	}
	if(num_proxy.empty()) return 0;
	else{
		auto pr = std::max_element(num_proxy.begin(), num_proxy.end(),
				[](const std::pair<Planar_proxy_handle,int>& p1, const std::pair<Planar_proxy_handle,int>& p2) {
			return p1.second < p2.second; });
		return pr->first;
	}
}



/**
 * return abs((n1|n2)) where ni = vi/||vi||
 */
template<typename Vector>
double compute_normalised_sc_prod(const Vector& v1,const Vector& v2){
	if(v1.squared_length()==0 || v2.squared_length()==0 )
		return 0;
	Vector n1(v1);
	n1 = n1 / std::sqrt(n1.squared_length());
	Vector n2(v2);
	n2 = n2 / std::sqrt(n2.squared_length());
	return std::abs(n1*n2);
}

/**
 * Among the proxies that passes through vertices of the triangle, compute the one
 * such its normal is the closest from the normal of the triangle.
 */
template<typename Mesh>
typename Mesh::Graph_vertex::Planar_proxy_handle
get_best_proxy_for_triangle_normal(const Mesh & mesh,const typename Mesh::Simplex_handle& triangle){
	typedef typename Mesh::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	std::set<Planar_proxy_handle> triangle_proxies;
	for(auto vh : triangle){
		triangle_proxies.insert(mesh[vh].plane_primitives().begin(),mesh[vh].plane_primitives().end());
	}
	if(triangle_proxies.empty()) return 0;

	typedef typename Mesh::GT GT;
	typedef typename GT::Triangle_3 Triangle_3;
	Planar_proxy_handle best_proxy = 0;


	auto it3 = triangle.begin();
	auto it1 = it3++;
	auto it2 = it3++;
	Triangle_3 tri_coord(mesh.point(*it1),mesh.point(*it2),mesh.point(*it3));

	auto normal_tri = tri_coord.supporting_plane().orthogonal_vector();
	auto pr = std::max_element(triangle_proxies.begin(), triangle_proxies.end(),
			[&](const Planar_proxy_handle& p1, const Planar_proxy_handle& p2) {
		return std::abs(compute_normalised_sc_prod(p1->plane().orthogonal_vector(),normal_tri)) <
				std::abs(compute_normalised_sc_prod(p2->plane().orthogonal_vector(),normal_tri));
	});

	if(!(*pr)->contains_vertex(*it1) || !(*pr)->contains_vertex(*it2) || !(*pr)->contains_vertex(*it3))
		return 0;
	else
		return *pr;
}



/**
 * the proxy p2 is merged into the proxy p1.
 * Its vertices are redirected to the one of p1.
 */
template<typename Mesh>
void merge_plane_primitive(Mesh& mesh,
		typename Mesh::Graph_vertex::Planar_proxy_handle p1,
		typename Mesh::Graph_vertex::Planar_proxy_handle p2){
	//remove p2 from the proxies of its vertices
	std::for_each(p2->begin_initial_vertices(),p2->end_initial_vertices(),
			[&](Vertex_handle vh){
		assert(mesh[vh].contains_primitive(p2));
		mesh[vh].remove_plane_primitive(p2);
		assert(!mesh[vh].contains_plane_primitive(p2));
		mesh[vh].add_plane_primitive(p1);
		assert(mesh[vh].contains_plane_primitive(p1));
	}
	);

	//add vertices of p2 to the vertices of the current proxy
	p1->insert_vertices(p2->begin_initial_vertices(),p2->end_initial_vertices());
	//	p1->initial_vertices_.insert(p2->begin_initial_vertices(),p2->end_initial_vertices());
	//	p1->current_vertices_.insert(p2->begin_initial_vertices(),p2->end_initial_vertices());

	// add area
	p1->area() += p2->area();

	typedef typename Mesh::GT::Point_3 Point_3;
	// recompute proxy info
	std::list<Point_3> points;
	std::for_each(
			p1->begin_initial_vertices(),
			p1->end_initial_vertices(),
			[&](Vertex_handle vh){
		points.push_back(mesh.point(vh));}
	);
	p1->recompute_proxy_parameters_after_merge(points);
}




/**
 * return the sum of distances from vertices to their proxy plane
 */
template<typename Mesh>
double sum_avg_distance_to_proxies(Mesh& mesh){
	//remove p2 from the proxies of its vertices
	double sum_dist = 0;
	for(auto vh : mesh.vertex_range())
		for(auto proxy : mesh[vh].plane_primitives())
			sum_dist += proxy->distance(mesh.point(vh)) / mesh[vh].num_plane_primitives();
	return sum_dist;
}









#endif /* COMPLEX_TOOLS_H_ */
