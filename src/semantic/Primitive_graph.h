/*
 * Primitive_graph.h
 *
 *  Created on: 31 janv. 2014
 *      Author: dsalinas
 */

#ifndef PRIMITIVE_GRAPH_H_
#define PRIMITIVE_GRAPH_H_


#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>

#include "complex/complex_tools.h"
#include "interface/Viewer.h"
#include "semantic/Plane_primitive.h"
#include "semantic/Enriched_mesh.h"
#include "semantic/Corners_clusters.h"
#include <boost/graph/adjacency_list.hpp>
#include "utils/Map_with_pointers.h"








// tell at which angle two proxies are considered equals
// the higher, the less corner (and more accurate)

//class Viewer;

template<typename Complex> class Enriched_mesh;




template<typename Mesh> class Pairwise_proxy_distances{
public:
	typedef typename Mesh::Vertex_handle Vertex_handle;
	typedef typename Mesh::GT GT;
	typedef typename GT::Point_3 Point_3;

	typedef Plane_primitive<GT,typename Mesh::Vertex_handle> Plane_primitive_;

	typedef CGAL::Search_traits_3<GT> TreeTraits;
	typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
	typedef typename Neighbor_search::Tree Tree;

	typedef typename std::unique_ptr<Tree> Ptr;

	Pairwise_proxy_distances(const Mesh& mesh,const Enriched_mesh<Mesh> & enriched_mesh):
		mesh_(mesh),enriched_mesh_(enriched_mesh)
	{
		for(auto p1 = enriched_mesh.begin() ; p1 != enriched_mesh.end(); ++p1){
			assert(p1->num_initial_vertices()!=0);
			const Plane_primitive_* p1_pt = &(*p1);
			std::list<Point_3> points;
			for(auto vh = p1->begin_initial_vertices(); vh != p1->end_initial_vertices(); ++vh)
				points.push_back(mesh.point(*vh));

			proxy_to_tree_.insert(
					std::make_pair(
							p1_pt,
							Ptr(new Tree(points.begin(),points.end()))
					)
			);

		}
	}

	/**
	 * return the distance between two proxies, that is the smallest
	 * distance between points of p1 and p2
	 */
	double distance(const Plane_primitive_* p1,const Plane_primitive_* p2) const{
		assert(proxy_to_tree_.find(p2) != proxy_to_tree_.end());
		const Tree& p2_tree(*(proxy_to_tree_.find(p2)->second));

		// look for the closest vertex of p1 from p2
		auto v_min =
				std::max_element(
						p1->begin_initial_vertices(),
						p1->end_initial_vertices(),
						[&](const Vertex_handle& v1,const Vertex_handle & v2){
			Neighbor_search search_v1(p2_tree, mesh_.point(v1),1);
			Neighbor_search search_v2(p2_tree, mesh_.point(v2),1);
			auto it1 = search_v1.begin();
			auto it2 = search_v2.begin();
			bool res = it1->second>it2->second;
			return res;
		});
		return std::sqrt((Neighbor_search(p2_tree, mesh_.point(*v_min),1).begin())->second);
	}

private:
	const Mesh& mesh_;
	const Enriched_mesh<Mesh> & enriched_mesh_;
	std::map<const Plane_primitive_*, Ptr> proxy_to_tree_;

};


const double alpha_corner = 0.2;



template <typename Mesh>
class Primitive_graph {

	template<class T,class M> friend class Corners_clusters;


	template<class M> friend class Dual_primitive_graph;


public:
	typedef typename Mesh::GT GT;
	typedef Plane_primitive<GT,typename Mesh::Vertex_handle> Plane_primitive_;
	typedef typename Mesh::Point Point;
	typedef typename Mesh::Vertex_handle Vertex_handle;
	typedef typename Mesh::Edge_handle Edge_handle;

	typedef Enriched_mesh<Mesh> Enriched_mesh_;


private:

	/**
	 * A graph vertex can be deactivated for instance when removing a vertex
	 * or performing a collapse. Then, it should not be accessed.
	 */
	struct Graph_vertex{
		Plane_primitive_* primitive;
		bool is_active;

		Graph_vertex():primitive(0),is_active(true){}
	};

	struct Graph_edge{		
		int num_crease_edges_;
	};


	typedef boost::adjacency_list
			< boost::setS,
			boost::vecS,
			boost::undirectedS,
			Graph_vertex,
			Graph_edge
			> Graph;
	Graph graph;


public:
	typedef typename Graph::vertex_descriptor Graph_vertex_descriptor;
	typedef typename Graph::edge_descriptor Graph_edge_descriptor;
	typedef typename Graph::vertex_iterator Graph_vertex_iterator;
	typedef typename Graph::edge_iterator Graph_edge_iterator;


private:
	std::map<const Plane_primitive_*,Graph_vertex_descriptor> primitive_to_vertex;

	Corners_clusters<Primitive_graph,Mesh> corner_cluster;

	typedef typename Corners_clusters<Primitive_graph,Mesh>::Corner_const_iterator Corner_const_iterator;

	typedef typename Corners_clusters<Primitive_graph,Mesh>::Corner_info Corner_info;


public:
	Primitive_graph():corner_cluster(*this){
	}

private:

	void compute_vertices(Enriched_mesh_& enriched_mesh){
		for (auto plane = enriched_mesh.planes.begin(); plane != enriched_mesh.planes.end();++plane){						
			auto vertex_descr = boost::add_vertex(graph);
			graph[vertex_descr].primitive = &(*plane);
			primitive_to_vertex[&(*plane)] = vertex_descr;
		}
	}

	/**
	 * Only pairs of vertices closer than threshold are considered.
	 */
	void compute_edges(Mesh & mesh, Enriched_mesh_& enriched_mesh,int offset,double threshold){
		std::cerr << "compute edges";
		UDBGVALUE(threshold);

		// // exact distances
		Pairwise_proxy_distances<Mesh> proxy_distances(mesh,enriched_mesh);
		for(auto p1 = enriched_mesh.begin() ; p1 != enriched_mesh.end(); ++p1){
			for(auto p2 = p1 ; ++p2 != enriched_mesh.end();){
				auto p1_pt = &(*p1);
				auto p2_pt = &(*p2);

				if(proxy_distances.distance(p1_pt,p2_pt) <= threshold){
					boost::add_edge( primitive_to_vertex[p1_pt],primitive_to_vertex[p2_pt], graph );
				}

			}
		}
		//
	}

public:
	void compute_graph(Mesh & mesh, Enriched_mesh_& enriched_mesh,double threshold,int offset=-1){
		clear();
		compute_vertices(enriched_mesh);
		compute_edges(mesh,enriched_mesh,offset,threshold);

		compute_corners(mesh,enriched_mesh);


		UDBGVALUE(threshold);
		UDBGVALUE(num_vertices());
		UDBGVALUE(num_edges());

	}



	void compute_corners(Mesh & mesh, Enriched_mesh_& enriched_mesh){
		corner_cluster.init(mesh);
	}

	void print_edges(){
		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor v1 = source(*ei);
			Graph_vertex_descriptor v2 = target(*ei);
			std::cerr << "Num crease edges : " << num_crease_edges(primitive(v1), primitive(v2))<<std::endl;
		}
	}


	void clear(){
		UDBG("clear graph");

		primitive_to_vertex.clear();
		graph.clear();
		corner_cluster.clear();
	}



	/**
	 * remove the vertex corresponding to the proxy after
	 * having removed all edges that passes through the vertex.
	 *
	 * the caller has to call compute_corners if he wants corners to be
	 * updated.
	 */
	void remove_primitive_vertex(Plane_primitive_* proxy_to_be_removed){
		Graph_vertex_descriptor v = primitive_to_vertex[proxy_to_be_removed];
		typename boost::graph_traits<Graph>::adjacency_iterator n_v_end,n_v;
		// remove edges passing through v
		for (tie(n_v, n_v_end) = adjacent_vertices(v, graph); n_v != n_v_end; ++n_v){
			boost::remove_edge(v,*n_v,graph);
		}
		// remove v by deactivating it
		graph[v].is_active = false;
	}


	/**
	 * collapse the edge [p1,p2] except that *p2 is not removed
	 * but only deactivated after the edge collapse in order
	 * to preserve iterators
	 */
	void collapse_edge(Plane_primitive_* p1, Plane_primitive_* p2){
		//		assert(contains_edge(p1,p2));
		Graph_vertex_descriptor v1 = primitive_to_vertex[p1];
		Graph_vertex_descriptor v2 = primitive_to_vertex[p2];

		typename boost::graph_traits<Graph>::adjacency_iterator n_v2, n_v2_end;
		for (tie(n_v2, n_v2_end) = adjacent_vertices(v2, graph); n_v2 != n_v2_end; ++n_v2){
			if(*n_v2 !=  v1){
				boost::add_edge(v1,*n_v2, graph );
				boost::remove_edge(v2,*n_v2,graph);
			}
		}

		boost::remove_edge(v1,v2,graph);

		graph[v2].is_active = false;
	}

	void collapse_first_edge(){
		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor v1 = source(*ei);
			Graph_vertex_descriptor v2 = target(*ei);
			collapse_edge(graph[v1].primitive,graph[v2].primitive);
			return;
		}

	}

	bool contains_edge(Plane_primitive_* p1,Plane_primitive_* p2){
		return boost::edge(
				primitive_to_vertex[p1],primitive_to_vertex[p2],graph).second;
	}


	/**
	 * beware that it also counts the deactivated vertices.
	 */
	unsigned num_vertices() const{
		return boost::num_vertices(graph);
	}







	/**
	 * number of active vertices, in O(n)
	 */
	unsigned num_active_vertices() const{
		unsigned res = 0;
		Graph_vertex_iterator vi, vi_end;
		for (boost::tie(vi, vi_end) = vertices(); vi != vi_end; ++vi)
			if(is_active(*vi)) ++res;
		assert(res<=num_vertices());
		return res;
	}


	unsigned num_edges() const{
		return boost::num_edges(graph);
	}


	void remove_random_edges(unsigned number){
		while(number--)
			remove_random_edge();
	}

	void add_random_edges(unsigned number){
		std::list<unsigned> random_edges = draw_random_missing_edge_indices(number);

		unsigned current_index_missing_edge = 0;

		typename boost::graph_traits<Graph>::vertex_iterator u,vert_end;

		for (boost::tie(u, vert_end) = vertices(); u != vert_end; ++u){
			if(random_edges.empty()) return;
			for(auto v = u; ++v != vert_end; ){
				if(is_active(*u) && is_active(*v) && is_missing_edge(*u,*v)){
					if(current_index_missing_edge == random_edges.front()){
//						UDBGVALUE(graph[*u].primitive);
//						UDBGVALUE(graph[*v].primitive);
//						UDBGVALUE(*(graph[*v].primitive));
						boost::add_edge(*u,*v, graph );
						random_edges.pop_front();
					}
					++current_index_missing_edge;
				}
				if(random_edges.empty()) return;
			}
		}
	}

private:
	bool is_missing_edge(
			Graph_vertex_descriptor v1,
			Graph_vertex_descriptor v2) const{
		return !boost::edge(v1,v2,graph).second;
	}


	void remove_random_edge(){
		if(num_edges()==0) return;

		srand(time(NULL));
		unsigned random_index = ((double) rand() / RAND_MAX)*(num_edges()-1);
		unsigned current_index = 0;

		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor u = source(*ei);
			Graph_vertex_descriptor v = target(*ei);
			if(is_active(u) && is_active(v)){
				if(current_index++ == random_index){
					boost::remove_edge(u,v,graph);
					return;
				}
			}
		}
	}

	// draw 'draw_number' indices of disting missing edge indices.
	// the indices are sorted.
	std::list<unsigned> draw_random_missing_edge_indices(unsigned draw_number) const{
		srand(time(NULL));
		std::set<unsigned> seen;
		draw_number = (std::min)(draw_number,num_missing_edges());
		while(draw_number--){
			unsigned random_index;
			do{
				random_index = ((double) rand() / RAND_MAX)*(num_missing_edges()-1);
			} while(seen.find(random_index)!=seen.end());
			seen.insert(random_index);
		}
		return std::list<unsigned>(seen.begin(),seen.end());
	}


public:

	unsigned num_missing_edges() const{
		unsigned num_vert = num_active_vertices();
		return (num_vert*(num_vert-1)/2)-num_edges();
	}



	/**
	 * return the number of edges such that one vertex belongs to p1 and another to p2
	 * has to be updated by the user.
	 */
	int& num_crease_edges(Plane_primitive_* p1,Plane_primitive_* p2){
		assert(contains_edge(p1,p2));
		return graph[boost::edge(primitive_to_vertex[p1],primitive_to_vertex[p2],graph).first].num_crease_edges_;
	}

	/**
	 * return the number of edges such that one vertex belongs to p1 and another to p2
	 * has to be updated by the user.
	 */
	int num_crease_edges(const Plane_primitive_* p1,const Plane_primitive_* p2) const{
		assert(contains_edge(p1,p2));
		return graph[boost::edge(primitive_to_vertex[p1],primitive_to_vertex[p2],graph).first].num_crease_edges_;
	}


public:

	/**
	 * iteration through the corners
	 */
	Corner_const_iterator begin_corners() const{
		return corner_cluster.begin();
	}
	Corner_const_iterator end_corners() const{
		return corner_cluster.end();
	}

	/**
	 * Returns a reference to the corner that passes through a vertex if any
	 */
	boost::optional<const Corner_info&> corner(Vertex_handle vh){
		//		for(const auto &corner : triangle_to_corner_)
		//			if((corner.second)->current == vh) return *corner.second;
		return corner_cluster.corner(vh);
	}

	/**
	 * check that a vertex is not going further from one of its corner vertices
	 */
	// todo stocker map:Vertex_handle->list<Corner_info*>
	bool check_not_going_further_from_corner(Vertex_handle vh,const Point& placement) const{
		return corner_cluster.check_not_going_further_from_corner(vh,placement);
	}


	void update_corners_after_collapse(Vertex_handle v0,Vertex_handle v1,
			const std::set<Plane_primitive_*>& proxies_v0v1_union,
			const Point& placement)
	{
		corner_cluster.update_after_collapse(v0,v1,proxies_v0v1_union,placement);
	}

private:
	/**
	 * It updates all the corners passing through vh
	 * ie if vh is now closer from a corner than its current corner vertex,
	 * it updates the corresponding corner (its vertex and its distance)
	 */
	void update_corner(Vertex_handle vh,const Point& placement){
		corner_cluster.update_corner(vh,placement);
	}



public:
	Graph_vertex_descriptor source(Graph_edge_descriptor ei) const{
		return  boost::source(ei, graph);
	}

	Graph_vertex_descriptor target(Graph_edge_descriptor ei) const{
		return  boost::target(ei, graph);
	}


	std::pair<Graph_vertex_iterator,Graph_vertex_iterator> vertices() const{
		return boost::vertices(graph);
	}


	/**
	 * vertices may be deactivated when collapsing (merging) edges
	 */
	bool is_active(Graph_vertex_descriptor vi) const{
		return graph[vi].is_active;
	}

	Plane_primitive_* primitive(Graph_vertex_descriptor vi) const{
		return graph[vi].primitive;
	}


	std::pair<Graph_edge_iterator,Graph_edge_iterator> edges() const{
		return boost::edges(graph);
	}




	void save(const std::string& name_file){
		std::ofstream myfile;
		myfile.open (name_file.c_str());
		myfile<<"OFF"<<std::endl;
		myfile<<2*num_vertices()<<" "<<2*num_edges()<<" 0"<<std::endl;
		typename boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
		Graph_edge_iterator ei, ei_end;

		for (boost::tie(vi, vi_end) = vertices(); vi != vi_end; ++vi){
			Plane_primitive_* pt(primitive(*vi));
			myfile << pt->centroid()<<std::endl;
			double x = pt->centroid().x();
			double y = pt->centroid().y();
			double z = pt->centroid().z();
			x+= (rand()%1-.5)*0.1;
			y+= (rand()%1-.5)*0.1;
			z+= (rand()%1-.5)*0.1;
			myfile << x <<" "<< y <<" "<< z<<std::endl;
		}

		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor v1 = source(*ei);
			Graph_vertex_descriptor v2 = target(*ei);

			myfile<< "3 "<< 2*(int)v1 <<" " << 2*(int)v2<<" "<<2*(int)v1+1 <<std::endl;
			myfile<< "3 "<< 2*(int)v2 <<" " << 2*(int)v1<<" "<<2*(int)v1+1 <<std::endl;
		}
		myfile.close();
	}


	template<typename Visitor>
	void look_through_edges(Visitor* visitor){
		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor u = source(*ei);
			Graph_vertex_descriptor v = target(*ei);
			visitor->edge(primitive(u),primitive(v));
		}
	}

	template<typename Visitor>
	void look_through_triangles(Visitor* visitor){
		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(); ei != ei_end; ++ei){
			Graph_vertex_descriptor u = source(*ei);
			Graph_vertex_descriptor w = target(*ei);


			if(primitive(u)<primitive(w)){
				typename boost::graph_traits<Graph>::adjacency_iterator v, u_end,v_end,nv;

				for (tie(v, u_end) = adjacent_vertices(u, graph); v != u_end; ++v){

					if(is_active(u) && is_active(v) && primitive(u)<primitive(*v)){
						for (tie(nv, v_end) = adjacent_vertices(*v, graph); nv != v_end; ++nv){
							if(is_active(w)&& *nv == w &&  ( primitive(*v)<primitive(w))){
								visitor->triangle(primitive(u),primitive(*v),primitive(w));
								break;
							}
						}
					}
				}
			}
		}
	}


	/**
	 * Visitor has to implement a method :
	 *	template <typename Clique,typename Graph>
	 *	void clique(const Clique& c, const Graph& g){
	 * 		corners_clusters_.register_clique<Clique>(c);
	 *	}
	 */
	template<typename Visitor>
	void look_through_maximal_cliques(Visitor& vis){
		boost::bron_kerbosch_all_cliques(graph, vis);
	}



	friend std::ostream& operator<<(std::ostream& stream, const Primitive_graph& p_graph){
		typename boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
		stream<<"\n"<<p_graph.num_vertices()<<" vertices:\n";
		for (boost::tie(vi, vi_end) = vertices(p_graph.graph); vi != vi_end; ++vi){
			stream<<"vertex:"<<p_graph.primitive(*vi);
			if(!p_graph.is_active(*vi)) stream <<" (inactive)"<<std::endl;
			stream<<std::endl;
		}

		stream<<"\n"<<p_graph.num_edges()<<" edges:\n";
		Graph_edge_iterator ei, ei_end;
		for (boost::tie(ei, ei_end) = edges(p_graph.graph); ei != ei_end; ++ei){
			Graph_vertex_descriptor v1 = p_graph.graph.source(*ei);
			Graph_vertex_descriptor v2 = p_graph.graph.target(*ei);
			if(p_graph.is_active(v1) && p_graph.is_active(v2))
				stream<<
				p_graph.primitive(v1)<<" "<<
				p_graph.primitive(v2)<<std::endl;
		}
		return stream;
	}




};





#endif /* PRIMITIVE_GRAPH_H_ */
