/*
 * NN_region_growing.h
 *
 *  Created on: Sep 18, 2014
 *      Author: dsalinas
 */

#ifndef NN_REGION_GROWING_H_
#define NN_REGION_GROWING_H_


#include <set>
#include <list>
#include "Proxy_detection.h"
#include "urban_utils.h"

#include "boost/iterator/iterator_facade.hpp"

#include <boost/graph/adjacency_list.hpp>

#include "geometry/K_nearest_graph.h"

/**
 * Proxy detection with region growing
 */
template<typename SkBlComplex> class NN_region_growing : public Proxy_detection<SkBlComplex>{

public:

	typedef typename SkBlComplex::Vertex_handle Vertex_handle;
	typedef typename SkBlComplex::Simplex_handle Simplex_handle;
	typedef typename SkBlComplex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef typename GT::Vector_3 Vector_3;
	typedef typename GT::Point_3 Point_3;


	const SkBlComplex& complex_;
	const Detection_params& proxy_params_;

	K_nearest_graph<SkBlComplex> k_nearest_graph;


	typedef typename K_nearest_graph<SkBlComplex>::GAit GAit;


	NN_region_growing(
			const SkBlComplex& complex,
			const std::string& name_seg_file,
			const Detection_params& proxy_params,
			unsigned nb_closest_neighbors = 10)
	:complex_(complex),
	 proxy_params_(proxy_params),
	 k_nearest_graph(complex,10){
		std::ofstream stream;

		if(open_file(stream,name_seg_file)){
			stream<<"!vertices: ";
			std::copy(
					complex.vertex_range().begin(),
					complex.vertex_range().end(),
					std::ostream_iterator<Vertex_handle>(stream," ")
			);
			stream<<std::endl;

			k_nearest_graph.set_all_unexplored();


			std::list<Vertex_handle> to_see(complex.vertex_range().begin(),complex.vertex_range().end());

			to_see.sort([&](Vertex_handle v1,Vertex_handle v2){
				Plane_3 res;
				double quality1 = best_fitting_plane_neighborhood(res,v1);
				double quality2 = best_fitting_plane_neighborhood(res,v2);
				return quality1>quality2;});

			while(!to_see.empty()){
				Vertex_handle v = to_see.front();
				to_see.pop_front();
				Region region(complex,*this,v);
				if(is_region_valid(region)){
					stream<<region.to_string()<<std::endl;
				}
				else{
					region.reset_vertices_tag();
				}
			}
			stream.close();
		}
	}


	/**
	 * return quality of fitting in [0,1]
	 */
	double best_fitting_plane_neighborhood(Plane_3& res,Vertex_handle v){
		std::list<Vertex_handle> v_neighbors = k_nearest_graph.neighbors(v);
		return best_fitting_plane(res,v_neighbors);
	}


	double best_fitting_plane(Plane_3& res,const std::list<Vertex_handle>& v_neighbors){
		std::list<Point_3> points;
		for(auto vh : v_neighbors) points.push_back(complex_.point(vh));
		auto points_begin = points.begin();
		auto points_end = points.end();
		//			Iterator_vertices_to_points points_begin(complex_,v_neighbors.begin());
		//			Iterator_vertices_to_points points_end(complex_,v_neighbors.end());
		return CGAL::linear_least_squares_fitting_3 (points_begin,points_end,res,CGAL::Dimension_tag<0>());
	}



private:

	bool open_file(std::ofstream& stream,const std::string& name_seg_file){
		if(name_seg_file.find(".seg") != std::string::npos)
			stream.open (name_seg_file);
		else
			stream.open (name_seg_file+".seg");

		UDBGMSG("NN_region_growing save in file:",name_seg_file);

		if(!stream.is_open())
			UDBGMSG("NN_region_growing : could not open ",name_seg_file);
		return stream.is_open();
	}



	class Region{
	private:
		const SkBlComplex& complex_;
		std::set<Vertex_handle> vertices_;
		std::list<Vertex_handle> to_be_seen_;


		Plane_3 plane_;
		NN_region_growing& region_growing_;
	public:

		Region(
				const SkBlComplex& complex,
				NN_region_growing& region_growing,
				Vertex_handle v):
					complex_(complex),
					region_growing_(region_growing){
			region_growing_.best_fitting_plane_neighborhood(plane_,v);
			//			init_plane(region_growing_.k_nearest_graph.neighbors(v));
			grow(v);
		}

		void reset_vertices_tag(){
			for(auto vh : vertices_)
				region_growing_.k_nearest_graph.set_unexplored(vh);
		}



		std::string to_string() const{
			std::ostringstream stream;
			stream<<"#plane: "<< plane().a()<<" "<< plane().b()<<" "<< plane().c()<<" "<< plane().d();
			stream<<" #area:" << num_vertices() ;
			stream<<" #vertices: ";
			for(auto x: vertices_)	stream<<x<<" ";
			return stream.str();
		}

		size_t size() const{
			return vertices_.size();
		}

	private:
		//BFS
		void grow(Vertex_handle v){
			if(already_seen(v))
				return ;

			to_be_seen_.push_front(v);
			region_growing_.k_nearest_graph.set_explored(v);

			if( is_valid(v) )
				vertices_.insert(v);

			while(!to_be_seen_.empty()){
				Vertex_handle current_v = to_be_seen_.front();
				to_be_seen_.pop_front();

				if(is_valid(current_v)){
					std::list<Vertex_handle> v_neighbors = region_growing_.k_nearest_graph.neighbors(current_v);

					for(auto vh : v_neighbors){
						if(vh == current_v) continue;
						if(is_valid(vh)){
							if(!already_seen(vh)){
								region_growing_.k_nearest_graph.set_explored(vh);
								to_be_seen_.push_back(vh);

								if( is_valid(vh) ){
									vertices_.insert(vh);
								}
							}
						}
					}

					//					recompute_plane(); // this would be slow (need to see again all vertices)
					// instead recompute locally or update
				}
			}
		}


		bool is_valid(Vertex_handle v){
			if(distance_to_plane(v)>max_dist()) return false;
			return check_normals(v);
		}

		bool check_normals(Vertex_handle v){
//			for(auto t : complex_.triangle_range()){}
			std::list<Vertex_handle> neigh(region_growing_.k_nearest_graph.neighbors(v));
//			std::list<Vertex_handle> neigh(complex_.vertex_range(v).begin(),complex_.vertex_range(v).end());
			neigh.push_back(v);

			Plane_3 pca_neigh;
			double qual = region_growing_.best_fitting_plane(pca_neigh,neigh);

			double min_qual = 0.1;
			if(qual<min_qual) return false;


			// check normal pca_neigh and current plane
			double sc_prod = scalar_product_normals(pca_neigh,plane_);
			double sc_bound = std::cos(region_growing_.proxy_params_.max_angle * 3.14 / 180.);
			return (std::abs(sc_prod) > sc_bound);
		}

		bool already_seen(Vertex_handle v){
			return region_growing_.k_nearest_graph.is_explored(v);
		}

		bool contains(Vertex_handle v){
			return vertices_.find(v) != vertices_.end();
		}

		//		void init_plane(const std::list<Vertex_handle>& v_neighbors){
		//			std::list<Point_3> points;
		//
		//			for(auto vh : v_neighbors) points.push_back(complex_.point(vh));
		//			auto points_begin = points.begin();
		//			auto points_end = points.end();
		//			//			Iterator_vertices_to_points points_begin(complex_,v_neighbors.begin());
		//			//			Iterator_vertices_to_points points_end(complex_,v_neighbors.end());
		//			CGAL::linear_least_squares_fitting_3 (points_begin,points_end,plane_,CGAL::Dimension_tag<0>());
		//		}


		void push_neighbors_on_queue(Vertex_handle v){
			std::list<Vertex_handle> v_neighbors = region_growing_.k_nearest_graph.neighbors(v);
			for(auto vh : v_neighbors){
				if(region_growing_.k_nearest_graph.is_explored(vh)){
					region_growing_.k_nearest_graph.set_explored(vh);
					to_be_seen_.push_back(vh);
				}
			}
		}

		double distance_to_plane(Vertex_handle v) const{
			return CGAL::squared_distance(plane_,complex_.point(v));
		}


		double distance_to_plane(std::list<Vertex_handle> vertices) const{
			//1 compute plane
			//2 return max dist
			return 0;
		}

		double max_dist() const{
			return region_growing_.proxy_params_.max_dist;
		}

		template<typename It>
		class Iterator_vertices_to_points : public boost::iterator_facade <
		Iterator_vertices_to_points<It>
		, Point_3
		, boost::forward_traversal_tag
		, Point_3 	>
		{
			const SkBlComplex& complex_;
			It vertices_it_;

		public:
			Iterator_vertices_to_points(
					const SkBlComplex& complex,
					It it
			):complex_(complex),vertices_it_(it){
			}

			bool equal(const Iterator_vertices_to_points& other) const{
				return vertices_it_ == other.vertices_it_;
			}
			void increment () {
				++vertices_it_;
			}
			const Point_3&  dereference() const	{
				return complex_.point(*vertices_it_);
			}
		};

		void recompute_plane(){
			std::list<Point_3> points;
			for(auto vh : vertices_) points.push_back(complex_.point(vh));
			auto points_begin = points.begin();
			auto points_end = points.end();

			//			Iterator_vertices_to_points points_begin(complex_,vertices_.begin());
			//			Iterator_vertices_to_points points_end(complex_,vertices_.end());
			CGAL::linear_least_squares_fitting_3 (points_begin,points_end,plane_,CGAL::Dimension_tag<0>());
		}

		void compute_plane(const std::list<Vertex_handle>& vertices){
			std::list<Point_3> points;
			for(auto vh : vertices) points.push_back(complex_.point(vh));
			auto points_begin = points.begin();
			auto points_end = points.end();
			//			Iterator_vertices_to_points points_begin(complex_,vertices.begin());
			//			Iterator_vertices_to_points points_end(complex_,vertices.end());
			CGAL::linear_least_squares_fitting_3 (points_begin,points_end,plane_,CGAL::Dimension_tag<0>());
		}


		public:

		Plane_3& plane(){
			return plane_;
		}

		const Plane_3& plane() const{
			return plane_;
		}

		unsigned num_vertices() const{
			return vertices_.size();
		}
		const std::set<Vertex_handle>& vertices() const{
			return vertices_;
		}
	};


	bool is_region_valid(const Region& region) const{
		return region.num_vertices() >= (unsigned)proxy_params_.min_area;
	}



};




#endif /* NN_REGION_GROWING_H_ */
