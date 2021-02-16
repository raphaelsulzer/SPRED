/*
 * Region_growing_proxy_detection.h
 *
 *  Created on: Jun 27, 2014
 *      Author: dsalinas
 */

#ifndef REGION_GROWING_PROXY_DETECTION_H_
#define REGION_GROWING_PROXY_DETECTION_H_

#include <string>
#include <fstream>
#include <cstdio>

//#include <CGAL/IO/scan_OFF.h>
#include "Proxy_detection.h"
#include "code_flo/CloudSegmentation.h"
#include "urban_utils.h"
#include "code_flo/Polyhedron_segmentation.h"
#include "complex/reorient.h"


/**
 * Proxy detection with region growing
 */
template<typename SkBlComplex> class Region_growing_proxy_detection : public Proxy_detection<SkBlComplex>{

private:
	std::string name_file_off;

public:
	Region_growing_proxy_detection(const SkBlComplex& complex,const std::string& name_seg_file,const Detection_params& proxy_params):
		name_file_off("tmp.off")
{
		UDBG("Region growing proxy detection");

		if( remove( (name_seg_file+".seg").c_str() ) != 0 ){
			UDBGMSG("Error deleting file ",name_seg_file+".seg");
		}

		save_mesh(complex);
		clean_mesh();
		detect_planes(complex,name_seg_file,proxy_params);
}



private:

	bool save_mesh(const SkBlComplex& complex) const{
		if (!save_off_file(name_file_off.c_str(),complex)){
			std::cerr << "Can't write in file "<<name_file_off.c_str()<<std::endl;
			return false;
		}
		else return true;
	}

	void clean_mesh() const{
		// reorient triangles with meshlab
		reorient<Complex>(name_file_off,name_file_off);
//		std::string cmd(
//				"meshlabserver -i "
//				+name_file_off
//				+" -o "
//				+ name_file_off
//				+ " -s ~/Documents/CodeSVN/urban/code/data/meshlab_reorient.mlx");
//		UDBGVALUE(cmd);
//		auto r = system (cmd.c_str());
//		if (r); // remove warning
	}

	void detect_planes(const SkBlComplex& complex,const std::string& name_seg_file,const Detection_params& proxy_params) const{
		Polyhedron_3 polyhedron;
		std::ifstream file(name_file_off.c_str(),std::ios::in);
		if (!file.is_open()){
			std::cerr << "Can't open file "<<name_file_off.c_str()<<std::endl;
			return;
		}

		scan_OFF(file, polyhedron,true);

		UDBGVALUE(polyhedron.size_of_vertices());

		if (!polyhedron.is_valid() ||  polyhedron.size_of_vertices()==0) {
			std::cerr << "Only valid combinatorial meshes are currently supported" << std::endl;
			return ;
		}
		polyhedron.normalize_border();
		std::cout << polyhedron.size_of_vertices() << " vertices, " << polyhedron.size_of_halfedges()/2 << " edges, "<<polyhedron.size_of_facets() << " facets" << std::endl;

		// call Cloud_segmentation for computing the segmentation
		Cloud_segmentation cloud_segmentation(&polyhedron);


		// polyhedron just stores one cc
		// matching_polyhedron map vertices handle from the complex to the polyhedron
		std::vector<int> matching_polyhedron;
		compute_matching(complex,polyhedron,matching_polyhedron);


		assert(proxy_params.max_angle>=0. && proxy_params.max_angle <= 90.);
		double sc_bound = std::cos(proxy_params.max_angle * 3.14 / 180.);

		cloud_segmentation.plane_extraction_for_mesh(proxy_params.max_dist,proxy_params.min_area,1-sc_bound);

		cloud_segmentation.save_clusters_to_file(name_seg_file,matching_polyhedron);
	}

	/**
	 * Compute the matching between vertices of the polyhedron and vertices of the complex
	 * (which may be placed differently than for polyhedron since the Off loader of CGAL remove
	 * all isolated vertices)
	 */
	void compute_matching(const SkBlComplex& mesh,Polyhedron_3& polyhedron,std::vector<int> &matching_polyhedron) const{
		assert((int)polyhedron.size_of_vertices()<=mesh.num_vertices());
		matching_polyhedron = std::vector<int>(polyhedron.size_of_vertices());

		auto v_complex = mesh.vertex_range().begin();
		for (auto v_polyhedron = polyhedron.vertices_begin(); v_polyhedron!= polyhedron.vertices_end() ;++v_polyhedron){
			while(mesh.degree(*v_complex)==0) ++v_complex;
			matching_polyhedron[v_polyhedron->index()]= (*v_complex).vertex;
			++v_complex;
		}
	}

};




#endif /* REGION_GROWING_PROXY_DETECTION_H_ */
