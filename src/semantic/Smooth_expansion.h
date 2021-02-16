/*
 * Smooth_expansion.h
 *
 *  Created on: May 1, 2014
 *      Author: dsalinas
 */

#ifndef SMOOTH_EXPANSION_H_
#define SMOOTH_EXPANSION_H_

#include "urban_utils.h"
#include "Smooth_proxy.h"
#include <queue>

template<class T> class Smooth_proxy;


template<typename Mesh> class Smooth_expander{
private:
	struct Proj_operation{
		typedef typename Mesh::Vertex_handle Vertex_handle;
		typedef typename Mesh::Graph_vertex::Planar_proxy_handle Planar_proxy_handle;

		Vertex_handle v_;
		Planar_proxy_handle proxy_;
		double cost_;

		Proj_operation(const Mesh& complex,Vertex_handle v):v_(v),proxy_(0){
			cost_ = 1e10;
			Planar_proxy_handle min_cost_proxy;

			for(auto n_v : complex.vertex_range(v)){
				if(!complex[n_v].empty_plane_primitives()){
					for(auto proxy : complex[n_v].plane_primitives()){
						double current_cost = compute_cost(complex,proxy);
						if(current_cost<cost_){
							cost_ = current_cost;
							proxy_ = proxy;
						}
					}
				}
			}
		}

		bool operator<(const Proj_operation& other) const{
			return (cost_ > other.cost_);
		}

		friend std::ostream& operator<< (std::ostream& stream, const Proj_operation& proj_op) {
			stream << "[ "<<proj_op.v_<<", proxy = "<<proj_op.proxy_<<", cost = "<<proj_op.cost_<<" ]";
			return stream;
		}

	private:
		/**
		 * return the distance of v_ to the proxy
		 */
		double compute_cost(const Mesh& complex,Planar_proxy_handle proxy){
			return squared_distance(proxy->plane(), complex.point(v_));
		}
	};




	typedef typename Mesh::Vertex_handle Vertex_handle;

	Mesh& mesh_;
	std::list<Proj_operation> affectations_;
	double cost_;
	double inf_cost_;



public:
	Smooth_expander(Mesh& mesh):mesh_(mesh),cost_(0){
	}


	void compute_propagation(Smooth_proxy<Mesh>& smooth_proxy_){
		affectations_.clear();
		cost_ = 0;
		inf_cost_ = 0;

		// 1 - compute vertices that are in smooth regions but have an edge with a non smooth one
		// 2 - compute the heap of projection
		// 3 - run greedy expansion

		std::priority_queue<Proj_operation > proj_heap;
		std::set<Vertex_handle> vertices_seen;


		for(auto vi = smooth_proxy_.begin_vertices(); vi != smooth_proxy_.end_vertices(); ++vi){
			for(auto n_vi : mesh_.vertex_range(*vi)){
				if(!mesh_[n_vi].empty_plane_primitives() && vertices_seen.find(n_vi)==vertices_seen.end()){
					proj_heap.push(Proj_operation(mesh_,*vi));
					vertices_seen.insert(n_vi);
					break;
				}
			}
		}

		while (!proj_heap.empty())
		{
			auto proj_op = proj_heap.top();
			affectations_.push_back(proj_op);
			proj_heap.pop();

			// we assign --temporary-- a proxy to proj_op.v_
			mesh_[proj_op.v_].add_plane_primitive(proj_op.proxy_);

			cost_+= proj_op.cost_;
			inf_cost_= (std::max)(inf_cost_,proj_op.cost_);

			for(auto nv : mesh_.vertex_range(proj_op.v_)){
				if(mesh_[nv].empty_plane_primitives() && vertices_seen.find(nv)==vertices_seen.end()){
					proj_heap.push(Proj_operation(mesh_,nv));
					vertices_seen.insert(nv);
				}
			}
		}

		for(auto vi = smooth_proxy_.begin_vertices(); vi != smooth_proxy_.end_vertices(); ++vi){
			mesh_[*vi].clear_primitives();
		}
	}


	/**
	 * return the sum of distances of the vertices of the regions to their projection on their plane
	 */
	double cost() const{
		return cost_;
	}

	/**
	 * return the max distance of the regions to its projected plane
	 */
	double inf_cost() const{
		return inf_cost_;
	}

	void assign(Smooth_proxy<Mesh>& smooth_proxy_){
		for(auto proj_op : affectations_){
			mesh_[proj_op.v_].add_plane_primitive(proj_op.proxy_);
			proj_op.proxy_->add_vertex(proj_op.v_);
		}

	}

	void project(Smooth_proxy<Mesh>& smooth_proxy_){
		for(auto proj_op : affectations_){
			mesh_[proj_op.v_].add_plane_primitive(proj_op.proxy_);
			mesh_.point(proj_op.v_) = proj_op.proxy_->plane().projection(mesh_.point(proj_op.v_));
		}
	}

};




#endif /* SMOOTH_EXPANSION_H_ */
