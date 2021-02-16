/*
 * Urban_edge_profile.h
 *
 *  Created on: May 6, 2014
 *      Author: dsalinas
 */

#ifndef URBAN_EDGE_PROFILE_H_
#define URBAN_EDGE_PROFILE_H_

#include <algorithm>
#include <type_traits>
#include "urban_utils.h"

#include "gudhi/gudhi_contraction.h"
//#include "geometry/contraction/Edge_profile.h"

#include "interface/Contraction_parameter.h"

#include "geometry/Independent_proxies.h"
#include "geometry/geometry_tools.h"


using namespace Gudhi;
using namespace skbl;

template<typename GeometricSimplifiableComplex> class Urban_edge_profile : public Gudhi::contraction::Edge_profile<GeometricSimplifiableComplex>{

	template<class ComplexType> friend class Urban_edge_profile_factory;
public:

	typedef GeometricSimplifiableComplex Complex;
	typedef typename Complex::GT GT;

	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::Root_vertex_handle Root_vertex_handle;
	typedef typename Complex::Edge_handle Edge_handle;
	typedef typename Complex::Graph_vertex Graph_vertex;
	typedef typename Complex::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;
	typedef typename Complex::Graph_vertex::Error_quadric Error_quadric;
	typedef typename Complex::Graph_edge Graph_edge;
	typedef typename Complex::Point Point;


	////////////////////////////// xxx
	// ces champs sont remplis par l'edge profile factory avec make_profile
	boost::optional<Point> placement_;
	boost::optional<double> cost_;
	boost::optional<double> is_valid_;
	////////////////////////////// xxx





	/**
	 * set of proxies that is the union of proxies of v0 and v1
	 */
	std::set<Planar_proxy_handle> proxies_v0v1_union_;


	/**
	 * note that this list is also the list of vertices in the graph (ie proxies)
	 * that will have a vertex less if the edge is contracted
	 */
	std::set<Planar_proxy_handle> proxy_v0v1_intersection_;

	boost::optional<Error_quadric> quadric_data_edge_;



	Urban_edge_profile( GeometricSimplifiableComplex& complex,Edge_handle edge):
		Gudhi::contraction::Edge_profile<GeometricSimplifiableComplex>(complex,edge)		{
		assert(this->complex().get_address(this->complex()[this->edge_handle()].first()));
		assert(this->complex().get_address(this->complex()[this->edge_handle()].second()));
		assert(this->complex().contains_edge(this->v0_handle(),this->v1_handle()));
		assert(this->v0_handle() != this->v1_handle());

		init();
	}


	const std::set<Planar_proxy_handle>& proxies_union()const{
		return proxies_v0v1_union_;
	}

	const std::set<Planar_proxy_handle>& proxies_intersection()const{
		return proxy_v0v1_intersection_;
	}



	/**
	 * updates the attributes :
	 * proxy_v0v1_intersection
	 * graph_proxy_loosing_vertex_
	 * graph_edges_loosing_crease_
	 */
	void init(){
		//		UDBG("init");
		std::set<Planar_proxy_handle>& proxies_v0 = this->v0().plane_primitives();
		std::set<Planar_proxy_handle>& proxies_v1 = this->v1().plane_primitives();
		proxies_v0v1_union_ = proxies_v0;
		proxies_v0v1_union_.insert(proxies_v1.begin(),proxies_v1.end());

		//		std::set<Planar_proxy_handle> proxy_v0v1_intersection;
		proxy_v0v1_intersection_.clear();
		std::set_intersection(
				proxies_v0.begin(),proxies_v0.end(),
				proxies_v1.begin(),proxies_v1.end(),
				std::inserter(proxy_v0v1_intersection_,proxy_v0v1_intersection_.begin())
		);
	}






	template<typename PrimitiveGraph>
	bool is_structure_preserving(Contraction_parameter& params,PrimitiveGraph& graph,const Point& placement,double scale=0.1) const{
		if(params.check_primitive_vertices)
			if (!preserve_graph_vertices()) {
				if(params.verbose) UDBG("-> Proxy destroyed");
				return false;
			}
		if(params.check_primitive_corner){
			if(!preserve_graph_corner(graph,placement)){
				if(params.verbose) UDBG("-> Corner destroyed");
				return false;
			}
		}

		return true;
	}

private:

	/**
	 * true iff all graph vertices have at least 4 vertices after the collapse
	 */
	bool preserve_graph_vertices() const{

		// the new number of vertices of the proxy is proxy.num_vertices()-1
		// if this number was already less than 4 then it was like that
		// since the beginning of the simplification since we reject collapses
		// when the proxy's vertex number is 4 AND when one proxy disapears
		// (ie when the proxy belongs to both vertices of the profile)
		auto check_new_number_vertices =
				[&](const Planar_proxy_handle proxy){
			return (
					proxy->num_vertices() == 4 &&
					proxy_v0v1_intersection_.find(proxy) != proxy_v0v1_intersection_.end()
					);
		};

		bool res = (
				std::find_if(
						proxy_v0v1_intersection_.begin(),
						proxy_v0v1_intersection_.end(),
						check_new_number_vertices
				) == proxy_v0v1_intersection_.end()
		);
		return res;
	}






	/**
	 * check for every independent triplet f = {P,Q,R} of proxies with
	 * one corner registered for f and such that one vertex of the
	 * edge is the corner vertex, if the placement does not go
	 * further from the intersection of the corner intersection
	 */
	template<typename PrimitiveGraph>
	bool preserve_graph_corner(PrimitiveGraph& graph,
			const boost::optional<Point>& placement,
			double alpha = default_alpha_value) const{
		if(!placement) return false;
		if(proxies_v0v1_union_.size()<3) return true;

		return	graph.check_not_going_further_from_corner(this->v0_handle(),*placement) &&
				graph.check_not_going_further_from_corner(this->v1_handle(),*placement);
	}
};



/**
 * to be called after an edge collapse, it updates :4
 * 1 - the proxy's vertices by adding/removing vertices
 * 2 - the graph's corner
 * 3 - it merges the set of v1 proxies to the one of v0
 * (not updating the number of crease edges)
 */
template<typename PrimitiveGraph,typename PrimitiveContainer>
void update_info_after_collapse(
		PrimitiveGraph& graph,
		PrimitiveContainer& v0_proxies,
		PrimitiveContainer& v1_proxies,
		const PrimitiveContainer& v0_cup_v1_proxies,
		typename PrimitiveGraph::Vertex_handle v0_vh,
		typename PrimitiveGraph::Vertex_handle v1_vh,
		const boost::optional<typename PrimitiveGraph::Point> &placement){

	// All proxies of v1 loose v1 handle and gain v0 handle
	for(auto proxy : v1_proxies){
		proxy->add_vertex(v0_vh);
		assert(proxy->contains_vertex(v1_vh));
		proxy->remove_vertex(v1_vh);
	}

	/**
	 * Update the corners after an edge collapse.
	 *
	 * For every independent triplet f = {P,Q,R} of proxies of the
	 * edge :
	 *  - if the triplet was not registered, it registers a new corner
	 *  - it it was, it checks that the placment is not closest from the corner
	 *  intersection and updates this corner if it is the case.
	 *
	 *  In case the corner was registered to v1 it registers it to v0.
	 *
	 *  @remark should not be called if preserve_graph_corner is false or
	 *  if the proxy graph is not constant
	 */
	if(placement){
		if(v0_cup_v1_proxies.size()>=3)
			graph.update_corners_after_collapse(v0_vh,v1_vh,v0_cup_v1_proxies,*placement);
	}

	v0_proxies = v0_cup_v1_proxies;

}



/**
 * return a initialized value iff some triangles passes through v0 or v1
 */
template<typename Complex>
boost::optional<typename Complex::Graph_vertex::Error_quadric>
compute_quadrics_data_edge(
		Complex& mesh,
		typename Complex::Edge_handle edge,
		double data_weight) {
	typedef typename Complex::GT GT;
	typedef typename GT::Triangle_3 Triangle_3;

	boost::optional<Error_quadric> res = Error_quadric();

	auto a(mesh.first_vertex(edge));
	auto b(mesh.second_vertex(edge));

	//		if(a>b) std::swap(a,b);

	int num_triangles_passing_through_ab = 0;

	for (auto ti : mesh.triangle_range(a)) {
		auto vertex_ti = ti.begin();
		auto v1 = *vertex_ti;
		auto v2 = *(++vertex_ti);
		auto v3 = *(++vertex_ti);

		Triangle_3 triangle(mesh.point(v1),
				mesh.point(v2), mesh.point(v3));

		if (!triangle.is_degenerate()) {
			// the quadric is multiplied by the triangle area and the weight
			(*res) += Error_quadric(triangle.supporting_plane(),
					std::sqrt(triangle.squared_area()) * data_weight);
			++num_triangles_passing_through_ab;
		}
	}

	for (auto ti : mesh.triangle_range(b)) {
		auto vertex_ti = ti.begin();
		auto v1 = *vertex_ti;
		auto v2 = *(++vertex_ti);
		auto v3 = *(++vertex_ti);
		if (a != v1 && a != v2 && a != v3) {
			Triangle_3 triangle(mesh.point(v1),
					mesh.point(v2),
					mesh.point(v3));
			// the quadric is multiplied by the triangle area and the weight
			if (!triangle.is_degenerate()) {
				(*res) += Error_quadric(triangle.supporting_plane(),
						std::sqrt(triangle.squared_area()) * data_weight);
				++num_triangles_passing_through_ab;
			}
		}
	}

	if (num_triangles_passing_through_ab == 0){
		UDBGVALUE(num_triangles_passing_through_ab);
		res.reset();
	}

	return res;
}






/**
 * compute the quadric of the sum of the two neighbor when creating an edge profile
 */
template<typename GeometricSimplifiableComplex> class Urban_cached_edge_profile_factory : public contraction::Edge_profile_factory<Urban_edge_profile< GeometricSimplifiableComplex> >{
	Contraction_parameter& params_;
public:
	typedef Urban_edge_profile< GeometricSimplifiableComplex> EdgeProfile;
	typedef typename EdgeProfile::Complex Complex_;
	typedef typename Complex_::Edge_handle Edge_handle_;

	// todo passer un mesh context plutot
	Urban_cached_edge_profile_factory(Contraction_parameter& params):params_(params){
	}

	// todo stocker le mesh context dans le profile
	EdgeProfile make_profile (
			Complex_& complex,
			Edge_handle_ edge) const override{
		auto res = EdgeProfile(complex,edge);
		res.quadric_data_edge_.reset(get_sum_quadrics(res.v0(),res.v1()).quadric());
		return res;
	}
};




#endif /* URBAN_EDGE_PROFILE_H_ */
