/*
 * Corners_clusters.h
 *
 *  Created on: Jun 5, 2014
 *      Author: dsalinas
 */

#ifndef CORNERS_CLUSTERS_H_
#define CORNERS_CLUSTERS_H_


#include <cassert>
#include <set>
#include <memory>
#include <map>

#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/optional.hpp>

#include "Plane_primitive.h"
#include "geometry/geometry_tools.h"



const double alpha_corner_clusters = 0.2;



/**
 * Every independent triangle in the graph is associated to
 * a point.
 * For maximal triangle, this point would be the intersection.
 * Non maximal triangles of a same clique share the same point.
 * The triangle whose intersection is taken is chosen when the first
 * request to the intersection is made. For this request, one must
 * provide a point and the intersection that is chosen is the closest
 * to this point.
 */
template<typename ProxyGraph,typename Mesh> class Corners_clusters{

public:

	typedef typename Mesh::Vertex_handle Vertex_handle;
	typedef typename ProxyGraph::Point Point;
	typedef typename ProxyGraph::Plane_primitive_ Plane_primitive_;
	typedef typename ProxyGraph::Graph Graph;

	/**
	 * For every triplet of independent proxies f={P,Q,R},
	 * we store :
	 *  - its containing clique
	 *  - three optional that are initialized whenever a vertex has these three proxies :
	 *  	- optional<Vertex_handle> current_vertex : the closest vertex from the vertices having these three proxies
	 *  	- optional<Point> the closest intersection from the triangle of clique to current_vertex
	 *  	(once this is initialized it is not modified anymore
	 *  	- optional<double> the distance between the intersection choosen and the current vertex
	 */
	typedef typename std::vector<Plane_primitive_*> Clique;
	struct Corner_info{

		friend class Corners_clusters;
	private:
		Clique containing_clique;
		boost::optional<Vertex_handle> current_vertex;
		boost::optional<double> dist_intersection_to_closest_vertex;
		boost::optional<Point> chosen_intersection;

	public:
		Corner_info(const Clique& containing_clique_):containing_clique(containing_clique_)	{
			assert(containing_clique.size()>=3);
		}

		/**
		 * update a corner when a vertex current_ placed at placement_ is found
		 * If the vertex become closer then the vertex corner is updated.
		 * If the vertex was not initialized then it initializes it.
		 *
		 * Returns true iff the corner was modified
		 */
		bool update(Vertex_handle new_vertex_,Point new_point_){
			if(!chosen_intersection) {
				return set_intersection_point(new_vertex_,new_point_);
			}
			else{
				if(squared_distance(new_point_,*chosen_intersection)<=*dist_intersection_to_closest_vertex){
					*current_vertex = new_vertex_;
					*dist_intersection_to_closest_vertex = squared_distance(new_point_,*chosen_intersection);
					return true;
				}
				else return false;
			}
		}

		Vertex_handle vertex() const{
			return current_vertex;
		}

		/**
		 * return the intersection associated to the a corner
		 * If not initialized it is initialized once for all (the intersectino point is fixed).
		 */
		const Point& intersection_point(Vertex_handle witness_vh,const Point& witness_point) const {
			if(!chosen_intersection) set_intersection_point(witness_vh,witness_point);
			assert(chosen_intersection);
			return *chosen_intersection;
		}

		/**
		 * return the intersection associated to the a corner
		 * it needs to have been initialized before
		 */
		const Point& intersection_point() const {
			assert(chosen_intersection);
			return *chosen_intersection;
		}

		double current_distance() const{
			//			if(!chosen_intersection) set_intersection_point(witness_vh,witness_point);
			assert(chosen_intersection);
			return *dist_intersection_to_closest_vertex;
		}

		bool check_not_going_further(Vertex_handle vh,const Point& placement) const{
			return squared_distance(placement,*chosen_intersection)<= *dist_intersection_to_closest_vertex;
		}

		/**
		 * returns true iff a corner vertex is associated to the clique
		 */
		bool is_registered() const{
			// bug boost optional : cannot convert from 'const boost::optional<Viewer_instructor::Point>' to 'const bool'
			// weird isnit
			// return chosen_intersection;
			return !!chosen_intersection; 
		}

	private:
		/**
		 * looks for all intersection of triangles of containing_clique
		 * and picks the one closest to witness to initialize both
		 * dist_intersection_to_closest_vertex and chosen_intersection
		 *
		 * returns true iff the intersection was initialized ie if at least
		 * one independent triangle passes through the vertex
		 */
		bool set_intersection_point(Vertex_handle witness_vh,const Point& witness_point){
			assert(!dist_intersection_to_closest_vertex);
			assert(!chosen_intersection);

			dist_intersection_to_closest_vertex.reset(1e10);
			//			boost::optional<Point> chosen_intersection;

			for(auto p = containing_clique.begin(); p!= containing_clique.end() ; ++p){
				for(auto q = p ; (++q)!= containing_clique.end() ; ){
					for(auto r = q ; (++r)!= containing_clique.end() ; ){
						if(is_alpha_independent_proxies(*p,*q,*r,alpha_corner_clusters)){
							assert(proj_on_three_intersection(*p,*q,*r));
							auto inter(proj_on_three_intersection(*p,*q,*r));
							double current_dist = squared_distance(witness_point,*inter);
							if(current_dist < *dist_intersection_to_closest_vertex){
								dist_intersection_to_closest_vertex.reset(current_dist);
								current_vertex.reset(witness_vh);
								chosen_intersection.reset(*inter);
							}
						}
					}
				}
			}

			// if no independent triangle passes through the vertex
			if(!current_vertex) {
				dist_intersection_to_closest_vertex.reset();
				return false;
			} else return true;
		}

	};

private:
	ProxyGraph& proxy_graph_;

	typedef typename std::shared_ptr<Corner_info> Ptr;
	typedef typename std::set<const Plane_primitive_*> Triangle;
	typedef typename std::map<Triangle , Ptr > Triangle_to_corner_map;

	Triangle_to_corner_map triangle_to_corner_;
	std::map<Vertex_handle,Corner_info* > vertex_to_corner_;


public:

	typedef typename std::map<Vertex_handle,Corner_info* >::const_iterator Corner_const_iterator;



	Corners_clusters(ProxyGraph& graph):proxy_graph_(graph){
	}


private:
	/**
	 * updates the map of triangle->corner and the map vertex->corner
	 * when a candidate vertex vh have the same proxy as the corner
	 */
	inline void update_corner_info(Corner_info* corner,Vertex_handle& vh,const Point& point){
		boost::optional<Vertex_handle> corner_vertex = corner->current_vertex;
		bool vertex_updated = corner->update(vh,point);
		if(vertex_updated){
			// the vertex has changed, we update the map
			if(corner_vertex){
				vertex_to_corner_.erase(*corner_vertex);
			}
			vertex_to_corner_.insert(std::make_pair(vh,corner));
		}
	}

public:

	// visitor for exploring cliques
	// just call register_clique
	struct clique_visitor{
		clique_visitor(Corners_clusters& corners_clusters):corners_clusters_(corners_clusters){}
		template <typename Clique,typename Graph>
		void clique(const Clique& c, const Graph& g){
			corners_clusters_.register_clique<Clique>(c);
		}
	private:
		Corners_clusters& corners_clusters_;
	};

	/** Compute for each triangle in the graph a point
	 *	corresponding to its intersection if it is a maximal
	 *  clique and to a intersection of a triangle of
	 *  its maximal clique otherwise.
	 */
	void init(Mesh& mesh){
		clear();

		// compute map Triangle -> Corner_info*
		boost::bron_kerbosch_all_cliques(proxy_graph_.graph, clique_visitor(*this));

		add_initial_corners(mesh);
	}


private:
	void add_initial_corners(Mesh& mesh){
		// go through every vertex, if three proxies, register the vertex into Corner_info
		// we choose the closest vertex and we register also a map vertex -> Corner_info*
		// the alpha parameter to filtrate dependant triangles
		for( auto vi : mesh.vertex_range())
		{
			if(mesh[vi].plane_primitives().size()>=3) {
				for(auto p1 = mesh[vi].plane_primitives().cbegin(); p1 != mesh[vi].plane_primitives().cend(); ++p1 ){
					for(auto p2 = p1; ++p2 != mesh[vi].plane_primitives().cend();  ){
						for(auto p3 = p2 ; ++p3 != mesh[vi].plane_primitives().cend();  ){
							//							std::cerr<< "looks for relation "<<*p1<<","<<*p2<<","<<*p3<<std::endl;
							if(is_alpha_independent_proxies(*p1,*p2,*p3,alpha_corner_clusters)){
								auto corner_info = corner(*p1,*p2,*p3);
								// p1 p2 p3 is a clique and should be here
								// unless an edge had been removed from the graph previously
								if(corner_info)
									update_corner_info(corner_info,vi,mesh.point(vi));
							}
						}
					}
				}
			}
		}
	}


public:


	void clear(){
		triangle_to_corner_.clear();
		vertex_to_corner_.clear();
	}

	/**
	 * it returns a point if the triangle p0p1p2 is independent
	 * (and eventually updates the corner with this point see bellow)
	 */
	const Point& triangle_to_point(
			const Plane_primitive_* p0,const Plane_primitive_* p1,const Plane_primitive_ * p2,
			Vertex_handle witness_vh,const Point& witness_point)
	const{
		auto corner_it = triangle_to_corner_.find(make_triangle(p0,p1,p2));
		assert(corner_it != triangle_to_corner_.end());
		return corner_it->second->intersection_point(witness_vh,witness_point);
	}

	/**
	 * it returns a corner passing through the vertex if any
	 */
	boost::optional<const Corner_info&> corner(Vertex_handle vh)const{
		boost::optional<const Corner_info&> res;
		auto it = vertex_to_corner_.find(vh);
		if(it == vertex_to_corner_.end()) return res;
		else{
			return *(it->second);
		}
	}


	//	bool contains_corner(const Plane_primitive_* p0,const Plane_primitive_* p1,const Plane_primitive_ * p2) const;

	Corner_const_iterator begin() const{
		return vertex_to_corner_.begin();
	}

	Corner_const_iterator end() const{
		return vertex_to_corner_.end();
	}

	/**
	 * It updates the corner passing through vh (we suppose they is only one
	 * corner passing through a vertex).
	 * If vh is closer from a corner after moving than its current corner vertex,
	 * it updates the corresponding corner (its vertex and its distance)
	 */
	void update_corner(Vertex_handle vh,const Point& placement){
		auto corner_it = vertex_to_corner_.find(vh);
		if(corner_it != vertex_to_corner_.end()){
			assert(corner_it->second->is_registered());
			update_corner_info(corner_it->second,vh,placement);
		}
	}


	/**
	 * to be called when corner infos need to be updated after a collapse
	 * v0v1->placement
	 * The corner of v1 (if any) will points to v0 instead.
	 *
	 * New corners (created by new triplets in the proxies union) will be added.
	 * Also, corners that are now closer to v0 are updated.
	 */
	void update_after_collapse(Vertex_handle v0,Vertex_handle v1,
			const std::set<Plane_primitive_*>& proxies_v0v1_union,
			const Point& placement)
	{
		// the corners of v1 are now represented with v0
		auto corner_it_v1 = vertex_to_corner_.find(v1);
		if(corner_it_v1 != vertex_to_corner_.end()){
			assert((*corner_it_v1).second->current_vertex == v1);
			(*corner_it_v1).second->current_vertex = v0;
		}
		update_corner(v0,placement);

		if (proxies_v0v1_union.size()>=3){
			update_new_corner_after_collapse(v0,proxies_v0v1_union,placement);
		}
	}

	/**
	 * after v0v1->v
	 * goes through all proxies triangles passing through v
	 * and update the corner status for each of them
	 */
	void update_new_corner_after_collapse(
			Vertex_handle v0,
			const std::set<Plane_primitive_*>& proxies_v0v1_union,
			const Point& placement
	){

		for(auto p1 = proxies_v0v1_union.cbegin(); p1 != proxies_v0v1_union.cend(); ++p1 ){
			for(auto p2 = p1; ++p2 != proxies_v0v1_union.cend();  ){
				for(auto p3 = p2 ; ++p3 != proxies_v0v1_union.cend();  ){
					if(is_alpha_independent_proxies(*p1,*p2,*p3,alpha_corner_clusters)){
						auto corner_info = corner(*p1,*p2,*p3);
						// p1 p2 p3 is a clique and should be here
						// unless the graph is not constant which is not currently well supported
						if(corner_info){
							update_corner_info(corner_info,v0,placement);
						}
						else{
							UDBG("corner added that was a clique -> is the proxy graph really constant?");
						}
					}
				}
			}
		}


	}

	/**
	 * check that a vertex is not going further from one of its corner vertices
	 * if no vertex passes through the vertex then we return true
	 */
	bool check_not_going_further_from_corner(Vertex_handle vh,const Point& placement) const{
		auto corner_it = vertex_to_corner_.find(vh);
		if(corner_it != vertex_to_corner_.end()){
			assert(corner_it->second->is_registered());
			return corner_it->second->check_not_going_further(vh,placement);
		}
		return true;
	}

private:
	/**
	 * it returns a corner passing through the vertex
	 */
	Corner_info* corner(Plane_primitive_* p1,Plane_primitive_* p2,Plane_primitive_* p3)
	const{
		auto corner_it = triangle_to_corner_.find(make_triangle(p1,p2,p3));
		if(corner_it == triangle_to_corner_.end()) return 0;
		else{
			return (corner_it->second.get());
		}
	}


	/**
	 * To each clique, we associate a common intersection.
	 */
	template <typename BoostClique>
	void register_clique(const BoostClique& c){
		// we just register cliques that are at least triangles
		if(c.size()<3) return;

		Clique clique(c.size(),0);
		for(int i = 0 ; i<c.size(); ++i)
			clique[i] = (proxy_graph_.graph[c[i]].primitive);

		assert(clique.size()==c.size());

		// create corner pointer and register it
		// to all triangles contained in the clique
		Ptr corner_ptr(new Corner_info(clique));

		for(int i=0;i< c.size();++i)
			for(int j=i+1;j< c.size();++j)
				for(int k=j+1;k<c.size();++k){
					add_triangle_to_corner_relation(
							proxy_graph_.graph[c[i]].primitive,
							proxy_graph_.graph[c[j]].primitive,
							proxy_graph_.graph[c[k]].primitive,
							corner_ptr
					);
				}
	}

	inline Triangle make_triangle(
			const Plane_primitive_* p0,const Plane_primitive_* p1,const Plane_primitive_ * p2)
	const{
		Triangle res;
		res.insert(p0);
		res.insert(p1);
		res.insert(p2);
		assert(res.size()==3);
		return res;
	}

	void add_triangle_to_corner_relation(
			const Plane_primitive_* p0,const Plane_primitive_* p1,const Plane_primitive_ *p2,
			Ptr corner){
		triangle_to_corner_.insert(make_pair(make_triangle(p0,p1,p2),corner));
	}

};




#endif /* CORNERS_CLUSTERS_H_ */
