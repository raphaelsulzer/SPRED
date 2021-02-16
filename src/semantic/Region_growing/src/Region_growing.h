/*
 * Region_growing.h
 *  Created on: Dec 3, 2014
 * This file is part of the Gudhi Library. The Gudhi library 
 *    (Geometric Understanding in Higher Dimensions) is a generic C++ 
 *    library for computational topology.
 *
 *    Author(s):       David Salinas
 *
 *    Copyright (C) 2014  INRIA Sophia Antipolis-Méditerranée (France)
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */


#ifndef REGION_GROWING_H_
#define REGION_GROWING_H_

#include "Debug_msg.h"
#include <deque>
#include <boost/foreach.hpp>
#include <vector>
#include "CGAL/Plane_3.h"
#include <CGAL/Cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>
#include "Off_reader.h"
#include "Graph.h"


namespace regrow {


typedef CGAL::Cartesian<double>     Kernel;
typedef Kernel::Plane_3 Plane_3;





/**
 * ax+by+cz+d=0
 */
struct Plane{
	double a,b,c,d;
	Plane(double a_,double b_,double c_,double d_):
		a(a_),b(b_),c(c_),d(d_){
	}
};

//template<typename Point>
//class Incremental_pca{
//	const Plane& plane() const{
//		//todo
//	}
//
//	void add_point(const Point& point){
//		//todo
//	}
//};


/**
 * return the quality in 0-1
 */
template<typename PointIterator>
double pca_from_points(PointIterator begin_points,PointIterator end_points,Plane_3& res){
	double quality = CGAL::linear_least_squares_fitting_3 (begin_points,end_points,res,CGAL::Dimension_tag<0>());
	return quality;
}

/**
 * graph must be a boost graph whose vertices are stored in a vector
 * angle tolerance must be in [0,1]
 * error tolerance is the max error allowed to vertex of proxies
 * min_num_points is the minimum number of points for every regions
 * size_offset is the distance of vertices that are added when building neighborhoods (0->the point, 1-> the 1-ring,...)
 */
template<typename BoostGraph>
class Region_growing{
	const double quality_min = 0.3;
public:
	typedef unsigned Region_id;
	typedef unsigned Vertex;



	Region_growing(const BoostGraph& graph,double angle_tolerance,double error_tolerance,int min_num_points,int size_offset):
		graph_(graph),
		vertex_to_estimated_plane_(boost::num_vertices(graph_)),
		vertex_to_regions_(boost::num_vertices(graph_)),
		angle_tolerance_(angle_tolerance),
		error_tolerance_(error_tolerance),
		min_num_points_(min_num_points),
		size_offset_(size_offset){

		compute_initial_planes();

		std::vector<Vertex> seeds(boost::vertices(graph).first,boost::vertices(graph).second);

		// sort seeds by planarity
		std::sort(seeds.begin(),seeds.end(),[&](Vertex v1,Vertex v2){
			return estimated_plane_quality(v1)<estimated_plane_quality(v2);});
		grow_regions(seeds);
	}
private:
	void grow_regions(std::vector<Vertex>& seeds){
		while(!seeds.empty()){
			Vertex v(seeds.back());
			seeds.pop_back();
			if(is_seed_valid(v)){
				regions_.emplace_back(grow(v));
				if(is_region_valid(regions_.back())){
					register_region(regions_.back());
				}
				else regions_.pop_back();
			}
		}
	}
public:




	const Plane_3& plane(Region_id id) const{
		return regions_[id].plane;
	}

	unsigned num_points(Region_id id) const{
		return regions_[id].vertices.size();
	}

	std::vector<Vertex>::const_iterator begin_region_vertices(Region_id id) const{
		return regions_[id].vertices.begin();
	}

	std::vector<Vertex>::const_iterator end_region_vertices(Region_id id) const{
		return regions_[id].vertices.end();
	}

	unsigned num_regions() const{
		return regions_.size();
	}

	void save_seg(const std::string & name_file) const{
		std::ofstream file(name_file);
		if(file.is_open()){
			for(auto region_id = 0u ; region_id < num_regions(); ++region_id ){

				file<<"#plane: ";
				auto pl(plane(region_id));
				file<<pl.a()<<" "<<pl.b()<<" "<<pl.c()<<" "<<pl.d()<<" ";

				file<<"#area:";
				file<<num_points(region_id);

				file<<"#vertices: ";
				for(auto x = begin_region_vertices(region_id); x != end_region_vertices(region_id); ++x){
					file<<*x<<" ";
				}
				file<<std::endl;
			}
		}
		else std::cerr << "could not open "<<name_file<<std::endl;
	}

private:
	struct Region{
		std::vector<Vertex> vertices;
		Plane_3 plane;
	};
	const BoostGraph& graph_;
	std::vector<Region> regions_;

	// plane and quality
	std::vector<std::pair<Plane_3,double>> vertex_to_estimated_plane_;

	// regions attached to a vertex
	std::vector<std::vector<Region_id>> vertex_to_regions_;

	double angle_tolerance_;
	double error_tolerance_;
	unsigned min_num_points_;
	unsigned size_offset_;

	void compute_initial_planes(){
		auto vert_beg_end = boost::vertices(graph_);
		for(auto v_it = vert_beg_end.first; v_it!= vert_beg_end.second; ++v_it){
			vertex_to_estimated_plane_[*v_it] = compute_local_plane(*v_it);
		}
	}

	const Plane_3& estimated_plane(Vertex v) const{
		return vertex_to_estimated_plane_[v].first;
	}

	double estimated_plane_quality(Vertex v) const{
		return vertex_to_estimated_plane_[v].second;
	}

	bool is_seed_valid(Vertex v){
		//enough inliers \approx enough eigen value
		if(estimated_plane_quality(v)<=0.8)
			return false;

		//only one grow through every seed
		if(!vertex_to_regions_[v].empty())
			return false;
		return true;
	}


	// directs vertices toward a region
	void register_region(Region& region){
		Region_id id = regions_.size()-1;
		for(auto v : region.vertices)
			vertex_to_regions_[v].push_back(id);


		//refit planes
		std::vector<Point> points;
		for(auto v : region.vertices) points.push_back(graph_[v].point);
		pca_from_points(points.begin(),points.end(),region.plane);

	}

	std::vector<Region_id>::const_iterator begin_regions(Vertex v) const{
		return vertex_to_regions_[v].begin();
	}
	std::vector<Region_id>::const_iterator end_regions(Vertex v) const{
		return vertex_to_regions_[v].end();
	}

	// explore vertices from v in the graph with a BFS
	// valid vertices are added to the region
	Region grow(Vertex v){
		Region res;
		res.plane = estimated_plane(v);
		std::deque<Vertex> to_see;
		to_see.push_back(v);

		std::set<Vertex> seen;
		seen.insert(v);

		while(!to_see.empty()){
			Vertex current = to_see.front();
			to_see.pop_front();
			res.vertices.push_back(current);


			BOOST_FOREACH(Vertex nv, boost::adjacent_vertices(current, graph_)){
				if(seen.find(nv)==seen.end() && is_vertex_valid_for_region(res,nv)){
					to_see.push_back(nv);
					seen.insert(nv);
				}
			}
		}
		return res;
	}

	// check not going further current plane
	bool is_vertex_valid_for_region(const Region& region,Vertex v){

		if(!vertex_to_regions_[v].empty()){
			//			//refuse if coplanar ie if scalar product close to 1
			//			//			const Plane_3& first_v_region(plane(vertex_to_regions_[v][0]));
			//			//			if(std::abs(1-scalar_angle(first_v_region,region.plane))>.8)
			//			//				return false;
			//
			//			for(Region_id neighbor_region : vertex_to_regions_[v]){
			//				const Plane_3& first_v_region(plane(neighbor_region));
			//				if(std::abs(1-scalar_angle(first_v_region,region.plane))>.7)
			return false;
			//			}
		}


		bool correct_angle =
				std::abs(1-scalar_angle(estimated_plane(v),region.plane))< angle_tolerance_;
		if(!correct_angle) return false;

		if(CGAL::squared_distance(region.plane,graph_[v].point) > error_tolerance_*error_tolerance_)
			return false;

		//if some region is already registered, check that it is not too close
		//todo in a first time we refuse vertices with two classes

		return true;
	}

	// return | n1*n2/(||n1|| ||n2||) |
	double scalar_angle(const Plane_3& p1,const Plane_3& p2){
		auto p1_vector(p1.orthogonal_vector());
		p1_vector = p1_vector / std::sqrt(p1_vector.squared_length());
		auto p2_vector(p2.orthogonal_vector());
		p2_vector = p2_vector / std::sqrt(p2_vector.squared_length());
		return p1_vector*p2_vector;
	}


	bool is_region_valid(const Region& region) const{
		//		return true;
		return region.vertices.size()>min_num_points_;
	}

	std::vector<Vertex> compute_neighbor_vertices(Vertex v) const{
		std::vector<Vertex> res;
		std::deque<Vertex> to_be_seen;
		std::map<Vertex,unsigned> dist;
		dist[v]=0;
		to_be_seen.push_back(v);

		while(!to_be_seen.empty()){
			auto current = to_be_seen.front();
			unsigned current_dist = dist[current];
			to_be_seen.pop_front();

			if(current_dist <= size_offset_){
				res.push_back(current);
				BOOST_FOREACH(Vertex nv, boost::adjacent_vertices(current, graph_)){
					if( dist.find(nv) == dist.end() ){ // ie if not already seen
						dist[nv] = current_dist+1;
						to_be_seen.push_back(nv);
					}
				}
			}
		}
		//		std::cerr << "v has a neighborhood of "<<res.size()<<std::endl;
		return res;
	}

	std::pair<Plane_3,double> compute_local_plane(Vertex v) const{
		std::pair<Plane_3,double> res;
		std::vector<Vertex> v_neighbors(compute_neighbor_vertices(v));

		std::vector<Point> points;
		for(auto v : v_neighbors) points.push_back(graph_[v].point);

		res.second = pca_from_points(points.begin(),points.end(),res.first);
		return res;
	}
};

/**
 * While going through off file, it saves the primal graph
 * and the dual graph whose vertices are triangles.
 */
template<typename Graph>
struct Off_graph_visitor{
	Off_graph_visitor(Graph& graph):
		points_(graph){
		points_.clear();
	}

	void init(int dim,int num_vertices,int num_faces,int num_edges){
		//todo do an assert to check that this number are correctly read
	}


	void point(const std::vector<double>& point){
		auto v_descr = boost::add_vertex(points_);
		//		std::cerr <<"vertex:"<<v_descr<<" /point: ";DBGCONT(point);
		points_[v_descr].point = Point(point[0],point[1],point[2]);
	}

	void maximal_face(const std::vector<int>& face){
		for(size_t i = 0 ; i < face.size();++i)
			for(size_t j = i+1 ; j < face.size();++j){
				boost::add_edge(face[i],face[j],points_);
			}
	}

	void done(){
	}

	Graph& points_;
};


template<typename Graph>
struct Off_graph_reader{
	/**
	 * remark: Off_graph_reader closes the stream via Off_reader.
	 */
	Off_graph_reader(std::ifstream& stream,Graph& graph){
		read_stream(stream,graph);
	}

	Off_graph_reader(const std::string& name_file,Graph& graph){
		std::ifstream stream(name_file);
		if(stream.is_open())
			read_stream(stream,graph);
		else {
			std::cerr <<"cant find file "<<name_file<<"\n";
		}
	}

private:
	void read_stream(std::ifstream& stream,Graph& graph){
		Off_reader off_reader(stream);
		Off_graph_visitor<Graph> off_vis(graph);
		off_reader.read<Off_graph_visitor<Graph>>(off_vis);

	}
};

inline void compute_proxies(const std::string& name_file_graph,const std::string& output_seg,
		double angle_tolerance_degree,double error_tolerance,int min_num_points,int size_offset){
	//1 load graph
	//2 compute seg
	//3 save seg
	Graph graph;
	Off_graph_reader<Graph> off_reader(name_file_graph,graph);
	std::cerr << "Graph loaded with "<<boost::num_vertices(graph)<<" vertices and "<<boost::num_edges(graph)<<" edges"<<std::endl;
	double sc_bound(1-std::abs(std::cos(angle_tolerance_degree* 3.14 / 180.)));
	Region_growing<Graph> region_growing(graph,sc_bound,error_tolerance,min_num_points,size_offset);
	std::cerr << "Computed "<<region_growing.num_regions()<<" regions\n";
	region_growing.save_seg(output_seg);
}

} //namespace regrow

#endif /* REGION_GROWING_H_ */
