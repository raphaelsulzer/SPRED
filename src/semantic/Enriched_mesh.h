/*
 * Enriched_mesh.h
 *
 *  Created on: 20 janv. 2014
 *      Author: dsalinas
 */

#ifndef ENRICHED_MESH_H_
#define ENRICHED_MESH_H_


#include <ctime>
#include <CGAL/Polygon_2.h>
#include <unordered_map>

#include "typedefs.h"
#include "Plane_primitive_border.h"
#include "interface/Contraction_parameter.h"
#include "semantic/io_proxies.h"

#include <CGAL/Cartesian.h>
//#include <CGAL/IO/scan_OFF.h>

#include <CGAL/Polygon_2_algorithms.h>


#include "NN_region_growing.h"
#include "SkBl_region_growing.h"

#include "Region_growing_proxy_detection.h"
#include "Mesh_region_growing.h"

#include "complex/complex_tools.h"
#include "iocomplex.h"
#include "urban_utils.h"
#include "utils/MClock.h"

#include "semantic/io_proxies.h"

#include "semantic/Region_growing/src/Region_growing.h"

//class Cloud_segmentation;


/**
 * @description : Stores primitives of a mesh
 * 	expects that :
 * 		-every vertex handle in the mesh has a method 'primitives()'
 * 		that returns its primitives
 *
 * @todo todo templater par Complex
 */
template<typename Complex>
class Enriched_mesh {
public:
	typedef CGAL::Cartesian<double> KK;
	typedef CGAL::Polygon_2<KK > Polygon_2;


	typedef typename Complex::GT Gudhi_kernel;

	typedef typename Complex::Graph_vertex::Planar_proxy Planar_proxy;


	typedef typename Gudhi_kernel::Point_2 Point_2;
	typedef typename Gudhi_kernel::Point Point;



	Complex& mesh;
private:
	const Contraction_parameter& params;
	//	struct compare_proxy_ptr {
	//			bool operator()(const Planar_proxy* x,const Planar_proxy* y) const { return *x<*y; }
	//		};


	std::unordered_map<Planar_proxy*,Plane_primitive_border<Gudhi_kernel>*> proxy_to_border;
public:




	std::list<Planar_proxy> planes;

	Plane_primitive_border<Gudhi_kernel>*	get_border_from_proxy(Planar_proxy * proxy){
		return proxy_to_border[proxy];
	}


	Enriched_mesh(Complex& mesh_,const Contraction_parameter& params_):
		mesh(mesh_),params(params_),proxy_to_border(),planes()
	{
	}


private:
	void delete_proxies(){
		while(!proxy_to_border.empty()){
			delete(proxy_to_border.begin()->second);
			proxy_to_border.erase(proxy_to_border.begin());
		}
	}

public:

	~Enriched_mesh(){
		clear();
	}




	void clear(){
		while(!planes.empty())
			remove_plane_proxy(&planes.front());
	}


public:
	typename std::list<Planar_proxy>::iterator begin(){
		return planes.begin();
	}

	typename std::list<Planar_proxy>::iterator end(){
		return planes.end();
	}

	typename std::list<Planar_proxy>::const_iterator begin() const{
		return planes.cbegin();
	}

	typename std::list<Planar_proxy>::const_iterator end() const{
		return planes.cend();
	}




	/**
	 * @brief Add the plane 'plane_to_insert' in 'planes'
	 * @return A pointer to the plane proxy stored in 'planes'
	 */
	Planar_proxy* add_plane_proxy(const Planar_proxy& plane_to_insert){
		planes.push_front(plane_to_insert);
		return &(*planes.begin());
	}


	/**
	 * @brief Add the plane proxy
	 * @return A pointer to the plane proxy stored in 'planes'
	 */
	Planar_proxy* add_plane_proxy(const Plane_3& plane,
			const std::list<Vertex_handle>& vertices_handle,
			const std::vector<Point> &vertices_coord,
			double area){
		Planar_proxy proxy(plane,vertices_handle,vertices_coord);
		planes.push_front(proxy);
		Planar_proxy* proxy_pt =  &(*planes.begin());
		proxy_pt->area() = area;
		register_primitive(vertices_handle,proxy_pt);
		return proxy_pt;
	}


	/**
	 * return true if the proxy is there and remove it.
	 * It also remove the reference of the vertices to this proxy.
	 *
	 * caller must update the graph
	 *
	 */
	bool remove_plane_proxy(Planar_proxy* proxy_to_remove){
		// find the proxy
		auto proxy_location =find_if(planes.begin(),planes.end(),
				[&](const Planar_proxy& current_proxy){
			return (&current_proxy == proxy_to_remove);
		});

		if(proxy_location==planes.end()){
			return false;
		}

		// remove its vertices
		for(auto vh = proxy_location->begin_initial_vertices();
				vh != proxy_location->end_initial_vertices();
				++vh){
			mesh[*vh].remove_plane_primitive(proxy_to_remove);
		}

		// remove the proxy from the plane list
		planes.erase(proxy_location);
		return true;
	}

	/**
	 * @brief merge two proxies into the first one, the second one is removed.
	 * The newplane is best plane passing through points of p0 and p1.
	 */
	void merge_two_proxies(Planar_proxy* p0,Planar_proxy* p1){
		merge_plane_primitive(mesh,p0,p1);
		//		p0->merge(*p1,mesh);

		unsigned size_list = planes.size();

		// remove p1 from planes
		planes.remove_if(
				[&](const Planar_proxy& current_proxy){
			return (&current_proxy == p1);
		});

		if(size_list<1) exit(1);
		assert(size_list == planes.size()+1);
	}


	/**
	 * compute a cgal polygon of points that are the projection of vertices of 'border_vertices'
	 */
	std::list<Point_2>* compute_polygon_border(Planar_proxy* primitive_pt,std::list<Vertex_handle> border_vertices){
		return 0;
	}

	/**
	 * return true iff the projection of x onto the plane of proxy
	 * is in the neighborhood of the proxy.
	 */
	bool is_in_offset_polygon(Planar_proxy* proxy,const Point& x) {
		return proxy_to_border[proxy]->is_in_border_polygon(x);

	}


public:
	/**
	 * @brief Register the plane 'primitive_pt' for 'points' and 'facets' (i.e. adds a pointer from these simplices to 'primitive_pt')
	 */
	void register_primitive(
			const std::list<Vertex_handle>& points_ind,
			Planar_proxy* primitive_pt
	){
		typedef typename Complex::Simplex_handle Simplex_handle;

		//todo faire sans le simplex_handle
		// to ensure unicity??
		Simplex_handle vertices_simplex;
		for(auto vi : points_ind) vertices_simplex.add_vertex(vi);

		primitive_pt->initial_vertices_.insert(vertices_simplex.begin(),vertices_simplex.end());
		primitive_pt->current_vertices_.insert(vertices_simplex.begin(),vertices_simplex.end());

		for (auto vi : vertices_simplex){
			(mesh)[vi].add_plane_primitive(primitive_pt);
		}
	}


	/**
	 * angle_max : tolerance angle in (0,90)
	 */
	void compute_planes( std::string  name_file,const Detection_params& detect_params)
	{
		clear();

		if(name_file.find(".seg") == std::string::npos)
			name_file.append(".seg");


		MClock time;
		if(detect_params.detection_method()[0] == 'D'){
			SkBl_region_growing<Complex> proxy_test(
					mesh,name_file,
					detect_params
			);
		}
		if(detect_params.detection_method()[0] == 'N'){
			// write segmentation in file "segmented_mesh.seg"
			unsigned num_neighbors = 10;
			NN_region_growing<Complex> proxy_detection(
					mesh,name_file,detect_params,
					num_neighbors
			);
		}

		if(detect_params.detection_method()[0] == 'F'){
			UDBGVALUE("Flo region growing detection");
			// write segmentation in file "segmented_mesh.seg"
			Region_growing_proxy_detection<Complex> proxy_detection(
					mesh,name_file,
					detect_params
			);
		}

		if(detect_params.detection_method()[0] == 'M'){
			UDBGVALUE("Mesh region growing detection");

			if (!save_off_file("tmp.off",mesh)){
				std::cerr << "Can't write in file tmp.off"<<std::endl;
			}

			std::cerr << "write in "<<name_file<<std::endl;

			// write segmentation in file "segmented_mesh.seg"
			std::cout << "max cos:"<<1-std::cos(detect_params.max_angle* 3.14 / 180.)<<std::endl;
			regrow::compute_proxies("tmp.off",name_file,
					detect_params.max_angle,detect_params.max_dist,detect_params.min_area,2);
		}

		time.end();
		std::cerr << "Proxy computation took :"<<time<<std::endl;



		// read the resulting segmentation in file "segmented_mesh.seg"
		std::ifstream file_planes;
		if(name_file.find(".seg") != std::string::npos)
			file_planes.open (name_file);
		else
			file_planes.open (name_file+".seg");

		if(file_planes.is_open()){
			file_planes>>*this;
		}
		else{
			std::cerr << "Enriched mesh: Error while opening file "<<name_file<<"\n";
			return;
		}
		file_planes.close();
	}



	/**
	 * compute the border of the proxies plus their epsilon-offsets.
	 */
	void compute_border_proxy(double epsilon = 0.1){
		delete_proxies();
		for (auto proxy = planes.begin() ; proxy != planes.end() ; ++proxy){
			UDBGMSG("compute border of ",&(*proxy));
			proxy_to_border[&(*proxy)] = new Plane_primitive_border<Gudhi_kernel>(mesh,&(*proxy),epsilon);
			UDBGMSG("compute border of ",&(*proxy));
			UDBGMSG("num border edges",proxy_to_border[&(*proxy)]->num_border_edges());
		}
	}

	unsigned size() const{
		return planes.size();
	}

	bool is_empty() const{
		return size()==0;
	}


	friend std::ostream& operator<< (std::ostream& stream, const Enriched_mesh& segmentation) {
		stream<<"!vertices: ";
		std::copy(
				segmentation.mesh.vertex_range().begin(),
				segmentation.mesh.vertex_range().end(),
				std::ostream_iterator<Vertex_handle>(stream," ")
		);
		stream<<std::endl;

		for( const auto& proxy : segmentation.planes) {
			stream<<"#plane: "<<proxy.plane().a()<<" "<<proxy.plane().b()<<" "<<proxy.plane().c()<<" "<<proxy.plane().d()<<" ";
			stream<<"#area: "<<proxy.area()<<" ";
			stream<<"#vertices: ";
			std::ostream_iterator<Vertex_handle> out_it(stream," ");
			std::copy(proxy.begin_current_vertices(),proxy.end_current_vertices(), out_it);
			stream<<std::endl;
		}
		return stream;
	}




	//todo cleanup
	// file should be like
	// !vertices 12 302 400 401
	// #plane: 0 12 3 4 #area 3 #vertices 12 302 400 401
	friend std::istream& operator>>(std::istream& stream, Enriched_mesh& segmentation)
	{
		srand(0);

		char c = stream.peek();

		bool vertex_map_in_file = (c=='!');

		// to map vertices vertices 12 302 400 401 to 0,1,2,3
		std::map<int,int> int_to_vh;
		if(vertex_map_in_file){
			int_to_vh = read_initial_vertices(stream);
			assert(int_to_vh.size() <= segmentation.mesh.num_vertices());
		}

		segmentation.clear();

		// each line as bellowed is put in a proxy_token
		// #plane: 0 12 3 4 #area 3 #vertices 12 302 400 401
		std::list<proxy_token<KK> > primitives;
		read_proxies<KK>(stream,primitives);

		for(const auto& proxy : primitives){
			segmentation.add_plane_proxy_from_token(proxy,int_to_vh);
		}

		UDBG("compute border proxies");
//		segmentation.compute_border_proxy(segmentation.params.distance_max_to_primitive_border);
		UDBG("border proxies computed");
		return stream;
	}


private:
	void add_plane_proxy_from_token(
			const proxy_token<KK>& proxy_token,
			const std::map<int,int>& int_to_vh){
		bool vertex_map_in_file = !int_to_vh.empty();
		std::list<Vertex_handle> vertices_handle;
		std::vector<Point> vertices_coord;
		for(int int_vi : proxy_token.vertices) {
			Vertex_handle vi;
			if(vertex_map_in_file){
				vi = Vertex_handle((int_to_vh.find(int_vi))->second);
			}
			else
				vi = Vertex_handle(int_vi);
			vertices_coord.push_back(mesh.point(vi));
			vertices_handle.push_back(Vertex_handle(vi));
		}
		add_plane_proxy(proxy_token.plane,vertices_handle,vertices_coord,proxy_token.area);
	}

};







#endif /* ENRICHED_MESH_H_ */
