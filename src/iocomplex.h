#ifndef IOCOMPLEX_H_
#define IOCOMPLEX_H_


#include "complex/complex_tools.h"
#include "gudhi/gudhi_common.h"
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <cstdio>
#include <cstring>
#include <algorithm>


/*
 * @remark to read a flag complex
 */
template<typename Complex>
class Two_dimensional_complex_wrapper{
	Complex& complex_;
	typedef typename Complex::Vertex_handle Vertex_handle;
	typedef typename Complex::Simplex_handle Simplex_handle;
	typedef typename Complex::Point Point;

public:
	std::multimap<Vertex_handle,Simplex_handle*> vertex_to_triangle;

	Two_dimensional_complex_wrapper(Complex& complex):complex_(complex){}


	void init(int dim,int num_vertices,int num_faces,int num_edges){
	}


	void point(const std::vector<double>& xyz){
		assert(xyz.size()>=3);
		complex_.add_vertex(Point(xyz[0],xyz[1],xyz[2]));
	}


	void maximal_face(const std::vector<int>& vertices){
		if(vertices.size()==2){
			complex_.add_edge(Vertex_handle(vertices[0]),Vertex_handle(vertices[1]));
		}
		else{
			assert(vertices.size()==3);
			Vertex_handle v0(vertices[0]);
			Vertex_handle v1(vertices[1]);
			Vertex_handle v2(vertices[2]);
			complex_.add_edge(v0,v1);
			complex_.add_edge(v1,v2);
			complex_.add_edge(v2,v0);

			// and register the triangle to the vertex minimum
			Vertex_handle v_min = (std::min)( v0, (std::min)(v1,v2));
			vertex_to_triangle.insert(std::pair<Vertex_handle,Simplex_handle*>(v_min,new Simplex_handle(v0,v1,v2)));
		}
	}

	void done(){
	}
};


/*
 * @brief Read a two dimensional mesh in a OFF file
 */
template<typename SkBlComplex>
bool read_off_two_dimensional_file(std::string file_name,SkBlComplex &complex){

	std::cerr << "read_off_two_dimensional_file\n";
	typedef typename SkBlComplex::Vertex_handle Vertex_handle;
	typedef typename SkBlComplex::Simplex_handle Simplex_handle;

	complex.clear();
	Two_dimensional_complex_wrapper<SkBlComplex> wrapper(complex);

	std::ifstream stream(file_name);
	if(!stream.is_open()) return false;

	Gudhi::Off_reader off_reader(stream);
	bool loaded = off_reader.read<Two_dimensional_complex_wrapper<SkBlComplex>>(wrapper);
	if (!loaded) return false;

	std::list<Simplex_handle> blockers_to_add;

	for (auto t : complex.triangle_range()){
		std::vector<Vertex_handle> vertices_t(3);

		auto v = t.begin();
		Vertex_handle v0( *v);
		Vertex_handle v1(*(++v));
		Vertex_handle v2(*(++v));

		Vertex_handle v_min = std::min( v0, std::min(v1,v2));


		// If we dont find the triangle corresponding to 't' in the map which stores
		// the triangles around v_min, we have a blocker.
		bool is_t_blocker = true;
		auto triangles_around_v_min = wrapper.vertex_to_triangle.equal_range(v_min);
		for(auto triangle_around_v_min = triangles_around_v_min.first;
				triangle_around_v_min != triangles_around_v_min.second;
				++triangle_around_v_min){
			if (* triangle_around_v_min->second == t){
				is_t_blocker = false;
				break;
			}
		}
		if (is_t_blocker){
			blockers_to_add.push_back(t);
		}

	}
	for (auto blocker : blockers_to_add){
		complex.add_blocker(Simplex_handle(blocker));
	}

	return loaded;

}


/**
 * @brief Save a pure 2d mesh into a OFF file
 * @remark If n is the number of vertices, note that the
 * vertices dont have to be between [0,n-1] but they will
 * be converted to integer in this range.
 */
template<typename Complex>
bool save_off_file(std::string file_name,const Complex &complex){

	// the map allows us to convert vertices handle
	// to integers in [0,n-1]
	typedef typename Complex::Vertex_handle Vertex_handle;
	std::map<Vertex_handle,unsigned> map;

	std::ofstream file (file_name.c_str());
	if (file.is_open())
	{
		int num_vertices = complex.num_vertices();
		int num_edges = complex.num_edges();

		int num_faces = 0;
		for (auto f : complex.triangle_range())	++num_faces;

		file << "OFF"<<std::endl;
		file << num_vertices<<" "<<num_faces<<" "<<num_edges<<std::endl;
		unsigned current_vertex = 0;
		for (auto v : complex.vertex_range()){
			file << complex.point(v) <<std::endl;
			map[v] = current_vertex++;
		}
		assert(num_vertices == current_vertex);

		for (auto f : complex.triangle_range()){
			file << "3 ";
			for (auto vertex : f)
				file << map[vertex] <<" ";
			file<<std::endl;
		}
		file.close();
		return true;
	}
	else {
		std::cerr << "Unable to open file "<<file_name.c_str()<<std::endl;
		return false;
	}
}



/**
 * @brief Save a mesh into a OFF file. It uses the color of the proxies
 * for the triangles.
 * @remark If n is the number of vertices, note that the
 * vertices dont have to be between [0,n-1] but they will
 * be converted to integer in this range.
 */
template<typename MeshContext>
bool save_segmented_off_file(std::string file_name,const MeshContext &context){

	// the map allows us to convert vertices handle
	// to integers in [0,n-1]
	typedef typename Complex::Vertex_handle Vertex_handle;
	std::map<Vertex_handle,int> map;

	std::ofstream file (file_name.c_str());
	if (file.is_open())
	{
		int num_vertices = context.mesh.num_vertices();
		int num_edges = context.mesh.num_edges();

		int num_faces = 0;
		for (auto f : context.mesh.triangle_range())	++num_faces;

		file << "COFF"<<std::endl;
		file << num_vertices<<" "<<num_faces<<" "<<num_edges<<std::endl;
		int current_vertex = 0;
		for (auto v : context.mesh.vertex_range()){
			file << context.mesh.point(v) <<std::endl;
			map[v] = current_vertex++;
		}
		assert(num_vertices == current_vertex);
		for (auto f : context.mesh.triangle_range()){
			file << "3 ";
			for (auto vertex : f)
				file << map[vertex] <<" ";

			// we not set the color
			auto proxy = get_best_proxy_for_triangle_normal(context.mesh,f);
			if(proxy){
				file << (int)proxy->color().r()<<" "<<(int)proxy->color().g()<<" "<<(int)proxy->color().b();
			}
			else
				file << " 127 127 127";
			//				file << " 0.5 0.5 0.5";
			file<<std::endl;
		}
		file.close();
		return true;
	}
	else {
		std::cerr << "Unable to open file "<<file_name.c_str()<<std::endl;
		return false;
	}
}



#endif
