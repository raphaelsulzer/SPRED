/*
 * Mesh_region_growing.h
 *
 *  Created on: Sep 18, 2014
 *      Author: dsalinas
 */

#ifndef MESH_REGION_GROWING_H_
#define MESH_REGION_GROWING_H_



#include <set>
#include <list>
#include "Proxy_detection.h"
#include "urban_utils.h"

#include "complex/complex_tools.h"

#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <boost/graph/adjacency_list.hpp>

#include "geometry/K_nearest_graph.h"

//todo cache normals to avoid several computation
//todo choose seed first that are planars

/**
 * Proxy detection with region growing
 */
template<typename SkBlComplex> class Mesh_region_growing : public Proxy_detection<SkBlComplex>{

public:
	typedef typename SkBlComplex::Vertex_handle Vertex_handle;
	typedef typename SkBlComplex::Simplex_handle Simplex_handle;
	typedef typename SkBlComplex::GT GT;
	typedef typename GT::Plane_3 Plane_3;
	typedef typename GT::Triangle_3 Triangle_3;
	typedef typename GT::Vector_3 Vector_3;
	typedef typename GT::Point_3 Point_3;

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
		Iterator_vertices_to_points(const SkBlComplex& complex,It it):
			complex_(complex),vertices_it_(it){}

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


	const SkBlComplex& complex_;
	const Detection_params& proxy_params_;
	int size_offset;


	Mesh_region_growing(
			const SkBlComplex& complex,
			const std::string& name_seg_file,
			const Detection_params& proxy_params)
	:complex_(complex),
	 proxy_params_(proxy_params)
	{
		std::ofstream stream;
		size_offset = 1;

		if(open_file(stream,name_seg_file)){
			stream<<"!vertices: ";
			std::copy(
					complex.vertex_range().begin(),
					complex.vertex_range().end(),
					std::ostream_iterator<Vertex_handle>(stream," ")
			);
			stream<<std::endl;

			set_all_unexplored();

			std::list<Vertex_handle> to_see(complex.vertex_range().begin(),complex.vertex_range().end());

			sort_vertex_by_planarity(to_see);

			while(!to_see.empty()){
				Vertex_handle v = to_see.front();
				to_see.pop_front();
				Region region(complex,*this,v);
				if(is_region_valid(region)){
//					region.recompute_plane();
					stream<<region.to_string()<<std::endl;
				}
				else{
					region.reset_vertices_tag();
				}
			}
			stream.close();
		}
	}

	private:

	std::vector<bool> explored_;
	void set_all_unexplored(){
		int v_max = 0;
		for(auto v : complex_.vertex_range())
			v_max = std::max(v_max,v.vertex);
		UDBGVALUE(v_max);
		explored_.reserve(v_max+1);
		for(int i = 0; i < v_max+1; ++i)
			explored_.push_back(false);
	}

	void sort_vertex_by_planarity(std::list<Vertex_handle>& vertices){
		vertices.sort([&](Vertex_handle v1,Vertex_handle v2){
			Plane_3 res;
			double quality1 = best_fitting_plane_neighborhood(res,v1);
			double quality2 = best_fitting_plane_neighborhood(res,v2);
			return quality1>quality2;});
	}


	void set_unexplored(Vertex_handle vh){
		explored_[vh.vertex] = false;
	}

	void set_explored(Vertex_handle vh){
		explored_[vh.vertex] = true;
	}

	bool is_explored(Vertex_handle vh){
		return explored_[vh.vertex];
	}


	public:

	std::set<Vertex_handle> list_neighbors(Vertex_handle v,int size_ring_offset){
		return neighbors(complex_,v,size_ring_offset);
	}


	/**
	 * return quality of fitting in [0,1]
	 */
	double best_fitting_plane_neighborhood(Plane_3& res,Vertex_handle v){
		auto neighbors = list_neighbors(v,size_offset);
		return best_fitting_plane(res,neighbors.begin(),neighbors.end());
	}

	template<typename VertexHandleIterator>
	double best_fitting_plane(Plane_3& res,VertexHandleIterator begin,VertexHandleIterator end){
		Iterator_vertices_to_points<VertexHandleIterator> points_begin(complex_,begin);
		Iterator_vertices_to_points<VertexHandleIterator> points_end(complex_,end);
		return CGAL::linear_least_squares_fitting_3 (points_begin,points_end,res,CGAL::Dimension_tag<0>());


		//		std::list<Point_3> points;
		//
		//		auto lambda = [&](Vertex_handle v) -> Point_3 {return complex_.point(v);};
		//		auto v_t_pt= boost::bind1st(complex_);
		//		//		auto test = std::bind(lambda);
		//		auto beg_pt = boost::make_transform_iterator(begin,v_t_pt);
		//
		//		std::for_each(begin,end,[&](Vertex_handle v){points.push_back(complex_.point(v));});
		//		//			Iterator_vertices_to_points points_begin(complex_,v_neighbors.begin());
		//		//			Iterator_vertices_to_points points_end(complex_,v_neighbors.end());
		//		return CGAL::linear_least_squares_fitting_3 (points.begin(),points.end(),res,CGAL::Dimension_tag<0>());
	}

	Plane_3 estimate_tangent_plane(Vertex_handle v,double& quality){
		auto neigh(list_neighbors(v,size_offset));
		neigh.insert(v);
		Plane_3 pca_neigh;
		quality = best_fitting_plane(pca_neigh,neigh.begin(),neigh.end());
		return pca_neigh;
	}


	private:

	bool open_file(std::ofstream& stream,const std::string& name_seg_file){
		if(name_seg_file.find(".seg") != std::string::npos)
			stream.open (name_seg_file);
		else
			stream.open (name_seg_file+".seg");

		UDBGMSG("Mesh_region_growing save in file:",name_seg_file);

		if(!stream.is_open())
			UDBGMSG("Mesh_region_growing : could not open ",name_seg_file);
		return stream.is_open();
	}

	class Region{
	private:
		const SkBlComplex& complex_;
		std::set<Vertex_handle> vertices_;
		std::list<Vertex_handle> to_be_seen_;


		Plane_3 plane_;
		Mesh_region_growing& region_growing_;
	public:

		Region(
				const SkBlComplex& complex,
				Mesh_region_growing& region_growing,
				Vertex_handle v):
					complex_(complex),
					region_growing_(region_growing){
			region_growing_.best_fitting_plane_neighborhood(plane_,v);
			grow(v);
		}

		void reset_vertices_tag(){
			for(auto vh : vertices_)
				region_growing_.set_unexplored(vh);
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
			region_growing_.set_explored(v);

			if( is_valid(v) )
				vertices_.insert(v);

			while(!to_be_seen_.empty()){
				Vertex_handle current_v = to_be_seen_.front();
				to_be_seen_.pop_front();

				if(is_valid(current_v)){
					auto v_neighbors = region_growing_.list_neighbors(current_v,region_growing_.size_offset);

					for(auto vh : v_neighbors){
						if(vh == current_v) continue;
						if(is_valid(vh)){
							if(!already_seen(vh)){
								region_growing_.set_explored(vh);
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
			double qual;
			Plane_3 tangent_plane_v( region_growing_.estimate_tangent_plane(v,qual));

			double min_qual = .1;
			if(qual<min_qual) return false;

			double sc_prod = scalar_product_normals(tangent_plane_v,plane_);
			double sc_bound = std::cos(region_growing_.proxy_params_.max_angle * 3.14 / 180.);
			return (std::abs(sc_prod) > sc_bound);
		}

		bool already_seen(Vertex_handle v){
			return region_growing_.is_explored(v);
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
			auto v_neighbors = region_growing_.list_neighbors(v);
			for(auto vh : v_neighbors){
				if(region_growing_.is_explored(vh)){
					region_growing_.set_explored(vh);
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

		void recompute_plane(){
			std::list<Point_3> points;
			for(auto vh : vertices_) points.push_back(complex_.point(vh));
			auto points_begin = points.begin();
			auto points_end = points.end();

			//			Iterator_vertices_to_points points_begin(complex_,vertices_.begin());
			//			Iterator_vertices_to_points points_end(complex_,vertices_.end());
			CGAL::linear_least_squares_fitting_3 (points_begin,points_end,plane_,CGAL::Dimension_tag<0>());
		}

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

	friend class Region;


	bool is_region_valid(const Region& region) const{
		return region.num_vertices() >= (unsigned)proxy_params_.min_area;
	}



};




#endif /* MESH_REGION_GROWING_H_ */
