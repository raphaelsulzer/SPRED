/*
 * Urban_valid_contraction.h
 *
 *  Created on: Feb 26, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef URBAN_VALID_CONTRACTION_H_
#define URBAN_VALID_CONTRACTION_H_

#include "urban_utils.h"


#include "gudhi/gudhi_contraction.h"
//#include "geometry/contraction/policies/Valid_contraction_policy.h"
#include "complex/complex_tools.h"
#include "semantic/Primitive_graph.h"
#include "semantic/Enriched_mesh.h"
#include "interface/Contraction_parameter.h"
#include "interface/Mesh_context.h"

using namespace Gudhi;
using namespace Gudhi::contraction;
using namespace skbl;

namespace quadric{

template< typename EdgeProfile>
bool check_link_condition(const EdgeProfile& profile){
	auto edge(profile.edge_handle());
	bool res = profile.complex().link_condition(edge,false);

	// condition1
	// to remove holes -> if border edge, collapse to remove a hole
	//	return res || is_border_edge(profile.complex(),profile.edge_handle());


	//		////condition 2 -> homeom a priori (a verifier)
	bool is_border_e(is_border_edge(profile.complex(),profile.edge_handle()));
	bool is_border_v0 = is_border_vertex(profile.complex(),profile.v0_handle());
	bool is_border_v1 = is_border_vertex(profile.complex(),profile.v1_handle());
	bool border_preserving = is_border_e || ( !is_border_v0 || !is_border_v1);

	return res && border_preserving;
}

/**
 * check if the graph is constant (no edge added),
 * if the number of vertices per primitives is greater than 4,
 * and if the number of creases and corner do not drop to zero.
 */
template< typename EdgeProfile , typename PrimitiveGraph>
bool check_constant_graph(const EdgeProfile& profile, PrimitiveGraph* primitive_graph){
	auto primitives(profile.v0().plane_primitives());
	primitives.insert(profile.v1().plane_primitives().begin(),profile.v1().plane_primitives().end());
	for (auto p = primitives.begin(); p != primitives.end(); ++p) {
		for (auto q = p; ++q != primitives.end(); ) {
			if(!primitive_graph->contains_edge(*p,*q)){
				//				UDBG("!! -------------- Proxies graph non constant -------------- !!");
				return false;
			}
		}
	}


	return true;
}

////todo mettre dans urban edge profile
//template< typename EdgeProfile>
//bool check_primitive_corner(const EdgeProfile& profile,double distance_max_to_corner,double scale = 0.1){
//	bool is_v0_corner = is_corner_vertex(profile.complex(),profile.v0_handle(),distance_max_to_corner);
//	bool is_v1_corner = is_corner_vertex(profile.complex(),profile.v1_handle(),distance_max_to_corner);
//	// if both vertices are corners, then we ask that they share the same set of primitives
//	bool res = (!(is_v0_corner&&is_v1_corner) || has_same_primitives(profile.v0(),profile.v1())
//			|| std::sqrt(squared_distance(profile.p0(),profile.p1()) < scale)
//
//	);
//	if(!res) UDBG("destroy corner");
//	return res;
//	//		return (!(is_v0_corner&&is_v1_corner));
//}


/**
 * checks if the new point is closed enough its primitives
 */
template< typename EdgeProfile>
bool check_distance_primitive(
		const EdgeProfile& profile,
		const boost::optional<typename EdgeProfile::Point>& placement,
		double distance_max_to_primitive_plane,
		bool verbose = false
){
	typedef typename EdgeProfile::Point Point;
	double numerical_tolerance = 1e-5;
	auto primitives(profile.v0().plane_primitives());
	primitives.insert(profile.v1().plane_primitives().begin(),profile.v1().plane_primitives().end());
	for(auto primitive:primitives){
		Point proj(primitive->plane().projection(*placement));
		double distance_proj(std::sqrt(squared_distance(proj,*placement)));
		if(distance_proj>distance_max_to_primitive_plane + numerical_tolerance) {
			if(verbose){
				UDBG("!! -------------- Point too far from primitive -------------- !!");
				UDBGVALUE(distance_proj);
				UDBGVALUE(distance_max_to_primitive_plane);
			}

			return false;
		}
	}
	return true;
}


/**
 * Checks if the projection of the placement point is close to the border polygon
 * of the primitive.
 */
template< typename EdgeProfile,typename Segmentation>
bool check_is_in_neighborhood_primitive(
		const EdgeProfile& profile,
		const boost::optional<typename EdgeProfile::Point>& placement,
		Segmentation* segmentation
){
	if(!placement) return false;
	else{
		auto primitives(profile.v0().plane_primitives());
		primitives.insert(profile.v1().plane_primitives().begin(),profile.v1().plane_primitives().end());
		for (auto p : primitives) {
			if (!segmentation->is_in_offset_polygon(p,*placement)){
				UDBG("!! -------------- projection too far from the proxy's border -------------- !!");
				return false;
			}
		}
		return true;
	}
}


template< typename EdgeProfile> class Urban_valid_contraction : public Valid_contraction_policy<EdgeProfile>{

private:
	typedef typename EdgeProfile::Complex Complex;
	typedef typename EdgeProfile::Point Point;

	Mesh_context& mesh_context_;

	//	Enriched_mesh* segmentation_;
	//	typedef Primitive_graph<Complex> Primitive_graph_;
	//	Primitive_graph_* primitive_graph_;
	//	const Contraction_parameter& parameters_;

public:
	typedef typename EdgeProfile::Vertex_handle Vertex_handle;

	typedef typename EdgeProfile::Edge_handle Edge_handle;
	//typedef typename EdgeProfile::Edge_handle Edge_handle;

	Urban_valid_contraction(Mesh_context& mesh_context):mesh_context_(mesh_context)	{
	}


private:

	//the condition is just checked for manifold vertex
	double compute_max_angle(const Complex& complex,const std::list<Vertex_handle>& star_ring_vertices,const Point& placement) const{
		double min_sc = 1;

		// assume the list is [v1 v2 v3]
		// we want the pairs [v1v2 v2v3] [v2v3 v3v1] to compare normals
		// to do so we have one iterator it
		// the last case [v2v3 v3v1] happens when it = ordered_vertices.end()-1
		if(star_ring_vertices.size()>=3){
			for(auto it1 = star_ring_vertices.begin(); it1 != star_ring_vertices.end(); ++it1){
				auto it2 = it1;
				auto it3 = it1;
				std::advance (it2,1);
				if(it2==star_ring_vertices.end()){
					it2 = star_ring_vertices.begin();
					it3 = ++(star_ring_vertices.begin());
				}
				else{
					std::advance (it3,2);
					if(it3==star_ring_vertices.end()){
						it3 = star_ring_vertices.begin();
					}
				}

				if(!CGAL::collinear(placement,complex.point(*it1),complex.point(*it2))
				&& !CGAL::collinear(placement,complex.point(*it2),complex.point(*it3))){

					// the vertices are *it *it2 and *it3

					auto left_triangle(CGAL::normal(placement,complex.point(*it1),complex.point(*it2)));
					auto right_triangle(CGAL::normal(placement,complex.point(*it2),complex.point(*it3)));
					left_triangle=left_triangle/std::sqrt(left_triangle.squared_length());
					right_triangle=right_triangle/std::sqrt(right_triangle.squared_length());
					double sc_prod = left_triangle * right_triangle;
					min_sc = (std::min)(min_sc,sc_prod);
				}
			}
		}


		if(std::abs(min_sc)>1) min_sc = min_sc>0? 1 : -1;
		assert(std::abs(min_sc)<=1);
		return std::acos(min_sc);
	}


public:
	// Given triangles 'p0,p1,p2' and 'p0,p2,p3', both shared along edge 'v0-v2',
	// determine if they are geometrically valid: that is, the ratio of their
	// respective areas is no greater than a max value and the internal
	// dihedral angle formed by their supporting planes is no greater than
	// a given threshold
	// reste à tester le ratio des aires



	/**
	 * return a list of the common vertices in l0 and l1
	 */
	std::list<Vertex_handle> common_vertices(const std::list<Vertex_handle>& l0,const std::list<Vertex_handle> &l1) const{
		std::list<Vertex_handle> res;
		std::set<Vertex_handle> l0_set;
		l0_set.insert(l0.begin(),l0.end());
		for(auto v : l1)
			if (l0_set.find(v)!=l0_set.end()) res.push_back(v);
		return res;
	}


	/**
	 * Assume that p and v are adjacent.
	 * Swap vertices such that p become the first one
	 * and v the last one
	 * and st the (cyclic) order is kept
	 *
	 * starting from [ ... pv ... ] or [ ... vp ... ]
	 * then we swap until  [p ... v] or [ p v ... ]
	 * if snd case we reverse [ ... v p ] and swap p first [ p ... v ]
	 *
	 *
	 *todo do in constant time with splice
	 *
	 */
	void revert(std::list<Vertex_handle>& list,Vertex_handle p,Vertex_handle v) const{
		int occ = 0;
		while(list.front() != p && occ <= list.size()){
			list.push_back(list.front());
			list.pop_front();
			++occ;
		}
		assert(occ<=list.size());

		if(list.back() != v){
			//case [ p v ... ] -> we reverse [ ... v p ] and swap p first [ p ... v ]
			list.reverse();
			list.push_front(list.back());
			list.pop_back();
		}
	}


	/**
	 * if v0 and v1 are 2-manifold and not on the border, it returns
	 * a list of ordered vertices of the sphere of the star
	 */
	boost::optional<std::list<Vertex_handle> > star_ring_vertices(const Complex& complex,Vertex_handle v0,Vertex_handle v1) const{
		Skeleton_blocker_link_complex<Complex> link_v0(complex,v0);
		Skeleton_blocker_link_complex<Complex> link_v1(complex,v1);

		boost::optional<std::list<typename Complex::Vertex_handle> > l0;

		if(complex.link_condition(v0,v1)){
			boost::optional<std::list<typename Complex::Vertex_handle> > l1;
			bool is_border_v0 = two_manifold_walk(complex,link_v0,l0);
			bool is_border_v1 = two_manifold_walk(complex,link_v1,l1);

			if(l0 && l1){
				// fow now we dont deal with border manif vertices
				if(!is_border_v0&&!is_border_v1){

					if(l0->size()>=3 && l1->size()>=3 ){
						// assume the list is [v1 v2 v3]
						// we want the pairs [v1v2 v2v3] [v2v3 v3v1] to compare normals
						// to do so we have one iterator it
						// the last case [v2v3 v3v1] happens when it = ordered_vertices.end()-1
						auto pq  = common_vertices(*l0,*l1);
						if(pq.size()!=2){
							l0.reset();
							return l0;
						}

						// since manifold two vertices
						pq.remove(v0);
						pq.remove(v1);
						assert(pq.size()==2);
						Vertex_handle p = pq.front();
						Vertex_handle q = pq.back();

						revert(*l0,p,v1);
						revert(*l1,q,v0);

						// now l0 is [p ... x q v1]
						// now l1 is [q ... y p v0]
						assert(l0->back()==v1);
						assert(l1->back()==v0);


						l0->pop_back();
						l1->pop_back();

						// now l0 is [p ... x q ]
						// now l1 is [q ... y p ]
						assert(l0->back() == q);
						assert(l1->back() == p);

						l0->pop_back();
						l1->pop_back();
						// now l0 is [p ... x ]
						// now l1 is [q ... y ]

						// merge lists
						l0->splice(l0->begin(),*l0,l1->begin(),l1->end());

						// now l0 is [p ... x q .. y ]

						return l0;
					}
				}
			}
		}else{
		}
		return l0;
	}

	/**
	 * Checks if performing the edge contraction deviates too much the normal.
	 * @return True iff the dihedral angles of the triangles obtained after the contraction
	 * are not too large
	 */
	bool check_normal_deviation(const EdgeProfile& profile,const boost::optional<Point>& placement,double upper_bound_dihedral_angle,bool verbose = false)
	const{
		if(!placement) return false;
		else{
			auto ring_vertices = star_ring_vertices(profile.complex(),profile.v0_handle(),profile.v1_handle());
			if(ring_vertices){


////				// if normals are already messed up, then we hope that future collapses will
////				// undo the damage
//				double max_angle_around_v0 = compute_max_angle(profile.complex(),*ring_vertices,profile.p0());
//				double max_angle_around_v1 = compute_max_angle(profile.complex(),*ring_vertices,profile.p1());
//				if(std::max(max_angle_around_v0,max_angle_around_v1)>upper_bound_dihedral_angle)
//					return true;

				double max_angle = compute_max_angle(profile.complex(),*ring_vertices,*placement);
				//					std::max(
				//							compute_max_dihedral_angle_between_old_and_new_triangle(profile.complex(),profile.v0_handle(),*placement),
				//							compute_max_dihedral_angle_between_old_and_new_triangle(profile.complex(),profile.v1_handle(),*placement)
				//					);

				if (max_angle <= upper_bound_dihedral_angle){
					return true;
				}
				else{
					if(verbose){
						UDBG("!! -------------- normal deviation too high -------------- !!");
						UDBGVALUE(max_angle);
					}
					return false;
				}
			}
			else {
				return true;
			}
		}
	}




	bool operator()(const EdgeProfile& profile,const boost::optional<Point>& placement) const override{

		if(mesh_context_.params.check_preserve_normals &&
				!check_normal_deviation(
						profile,placement,
						mesh_context_.params.bound_preserve_normals,
						mesh_context_.params.verbose
				)
		)
			return false;
		if(mesh_context_.params.link_condition && !check_link_condition(profile)){
			if(mesh_context_.params.verbose) UDBG("link condition not valid");
			return false;
		}
		if(mesh_context_.params.constant_graph && !check_constant_graph(profile,&mesh_context_.primitive_graph)){
			if(mesh_context_.params.verbose) UDBG("proxy graph not constant");
			return false;
		}

		if(mesh_context_.params.check_distance_primitive && !check_distance_primitive(profile,placement,mesh_context_.params.distance_max_to_primitive_plane)){
			if(mesh_context_.params.verbose) UDBG("distance proxy invalid");
			return false;
		}
		if(mesh_context_.params.check_distance_primitive_border && !check_is_in_neighborhood_primitive(profile,placement,&mesh_context_.segmentation))
			return false;
		if(!placement){
			UDBG("Placement not initialized");
			return false;
		}
		if(!profile.is_structure_preserving(mesh_context_.params,mesh_context_.primitive_graph,*placement)){
			if(mesh_context_.params.verbose) UDBG("destroy structure");
			return false;
		}

		return true;
	}
};

}
#endif /* URBAN_VALID_CONTRACTION_H_ */
