/*
 * urban_collapse.cpp
 * run the structure aware mesh decimation on a command line
 *  Created on: Jun 30, 2014
 *      Author: dsalinas
 */
#ifndef URBAN_COLLAPSE_CPP_
#define URBAN_COLLAPSE_CPP_

#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

//#include "utils/Utils.h"
//#include "utils/Clock.h"
#include "gudhi/gudhi_common.h"
#include "gudhi/gudhi_contraction.h"


#include "geometry/hausdorff/Meshlab_hausdorff_distance.h"
#include "geometry/hausdorff/Distance_point_to_triangle.h"

#include "typedefs.h"
#include "interface/Mesh_context.h"
#include "semantic/Enriched_mesh.h"
#include "semantic/Primitive_graph.h"
#include "contraction/policies/Urban_policies.h"

#include "complex/complex_tools.h"

using namespace std;


const int num_args = 5;


/**
 * arguments of the program read from the argument of the main
 */
struct progr_args{
	string name_file_input;
	string name_file_output;
	string name_file_proxy;
	int n_min;

	Contraction_parameter contr_params;

	//	double abstraction_weight;
	//	double boundary_weight;

	//	bool link_condition;
	bool proxy_use;



	progr_args(int argc, char* argv[]){
		assert(argc==num_args+1);
		int current_argument = 1; // to reorder easily
		name_file_input = string(argv[current_argument++]);
		name_file_output = string(argv[current_argument++]);
		name_file_proxy = string(argv[current_argument++]);
		n_min= atoi( argv[current_argument++]);

		string name_file_contr_params = string(argv[current_argument++]);
		std::ifstream file(name_file_contr_params,std::ios::in);
		if (!file.is_open())
			std::cerr << "Can't open file "<<name_file_contr_params.c_str()<<std::endl;
		else
			file >> contr_params;

		proxy_use = name_file_proxy.size()>1;
	}

	std::string to_string() const{
		std::ostringstream stream;
		stream << "---- urban_collapse ----  \n ";
		stream 	<< "  name_file_in: "<< name_file_input<<std::endl;
		stream 	<< "  name_file_out: "<< name_file_output<<std::endl;
		stream 	<< "  name_file_proxy: "<< name_file_proxy<<std::endl;
		stream 	<< "  n_min: "<< n_min<<std::endl;


		stream << "  Contr params"<<contr_params<<std::endl;

		//		stream 	<< "  abstraction_weight: "<< abstraction_weight<<std::endl;
		//		stream 	<< "  boundary_weight: "<< boundary_weight<<std::endl;
		//		stream 	<< "link condition:"<< link_condition <<std::endl;
		stream 	<< "  proxy_use:"<< proxy_use<<std::endl;
		stream<< std::endl;
		return stream.str();
	}
};




struct Program{
public:
	typedef typename Complex::Point Point;

	typedef typename Complex::GT GT;

	progr_args args;

	Program(int argc,char* argv[]):
		args(argc,argv),
		mesh_context()
	{
		std::cerr << "---- urban_collapse ----  \n ";
		std::cerr <<"---- urban_collapse ----  Program args : \n"<<args.to_string()<<std::endl;
		if(!init()) std::cerr <<"---- urban_collapse ----  Initialization problem \n";
	}


private:
	Mesh_context mesh_context;

	/**
	 * init the mesh
	 */
	bool init(){
		bool loaded = read_off_two_dimensional_file(args.name_file_input, mesh_context.mesh);
		if (!loaded) {
			std::cerr << "---- urban_collapse ----  Cant load file "<<args.name_file_input<<endl;
			return false;
		}

		std::cerr << "---- urban_collapse ----  Loaded mesh has " << mesh_context.mesh.num_vertices() << " vertices\n";

		if (mesh_context.mesh.num_vertices()<= args.n_min) return true; // stop condition already reached


		for(auto vi : mesh_context.mesh.vertex_range())
			mesh_context.initial_points.push_back(mesh_context.mesh.point(vi));

		mesh_context.params = args.contr_params;

		if(args.proxy_use){
			UDBG("---- urban_collapse ----  Load planes");

			MClock clock;
			//			UDBG("TODO XXX Not laoding planes");
			// xxx not loading planes
			std::ifstream file_planes(args.name_file_proxy);
			if(file_planes.is_open())
				file_planes>>mesh_context.segmentation;

//			mesh_context.segmentation.compute_border_proxy(mesh_context.params.distance_max_to_primitive_border);

			mesh_context.update_graphs(mesh_context.params.distance_max_graph_edges,false);

			std::cerr << "xxx careful reg proxy line added\n";
//			mesh_context.merge_proxies(Merge_proxies_params(20,4,40,8)); //xxx to remove
//			mesh_context.merge_proxies(Merge_proxies_params(10,1,40,8)); //xxx to remove

			mesh_context.update_graphs(mesh_context.params.distance_max_graph_edges,false);


			std::cerr <<"---- urban_collapse ----  Loaded "<<mesh_context.segmentation.size()<<" planes\n";
			clock.end();
		}

		return true;
	}


public:

	/**
	 * run the program
	 */
	bool run(){
		MClock clock;

		std::cerr <<"---- urban_collapse ----  Graph has  "<<mesh_context.primitive_graph.num_vertices()<<" vertices";
		std::cerr <<" ---- urban_collapse ----  and "<<mesh_context.primitive_graph.num_edges()<<" edges\n";


		mesh_context.set_initial_points();

		typedef contraction::Skeleton_blocker_contractor<Complex,Urban_edge_profile<Complex> > Contractor;
		typedef Urban_policies<Urban_edge_profile<Complex> > Urban_policies_;

		auto num_collapses = mesh_context.mesh.num_vertices()-args.n_min;

		Urban_policies_ policies(mesh_context);
		Contractor contractor(
				mesh_context.mesh,
				policies.cost(),
				policies.placement(),
				policies.is_valid(),
				policies.visitor(),
				policies.edge_profile_factory()
		);
		contractor.contract_edges(mesh_context.mesh.num_vertices()-args.n_min);

		std::cerr << "---- urban_collapse -------------------------------------------> Done \n";


		clock.end();
		std::cerr << "---- urban_collapse ----  Edge collapse took "<< clock << " s ("<< num_collapses/clock.num_seconds()<<" contractions/s)\n";

		remove_pending_vertices(mesh_context.mesh);


		compute_distance_with_original_mesh();

		// save the simplified meshes
		bool saved;
		saved = save_off_file(args.name_file_output, mesh_context.mesh);


//		if(args.proxy_use) save_proxies();
		//		if(!mesh_context.segmentation.is_empty()){
		//			string colored_file_name(args.name_file_output);
		//			colored_file_name.insert(colored_file_name.size()-4,string("_colored")); // _colored before the .off
		//			save_segmented_off_file(colored_file_name,mesh_context);
		//		}

		if (!saved)
			cerr << "---- urban_collapse ----  Problem while writing in file " << args.name_file_output << endl;

		return true;
	}

	/**
	 * compute mean symetric distance and
	 * save it to file ${output}_mean_err.dat
	 */
	void compute_distance_with_original_mesh(){
		Meshlab_hausdorff_distance<Complex> meshlab_dist(mesh_context.mesh);
		Distance_point_to_triangle<Complex> point_to_triangles_dist(mesh_context.initial_points);

		meshlab_dist.compute_distance_s_to_o(mesh_context.mesh);
		point_to_triangles_dist.compute_distance(mesh_context.mesh);

		double mean_dist = std::max(
				meshlab_dist.mean_s_to_o(),point_to_triangles_dist.mean_o_to_s()
		);

		std::string name_outpur_err_file = remove_extension(args.name_file_output);

		name_outpur_err_file = name_outpur_err_file + "_mean_err.dat";

		UDBGMSG("\n\n\n\n save error in file:",name_outpur_err_file);

		std::ofstream stream;
		stream.open (name_outpur_err_file);
		if(stream)
			stream << "mean_error: " << mean_dist << std::endl;
		else{
			UDBGMSG("Cant load file ",name_outpur_err_file);
		}
		stream.close();
	}

	void save_proxies(){
		std::string name_seg_file = remove_extension(args.name_file_output);
		// add the .seg instead
		name_seg_file = name_seg_file + ".seg";
		UDBGMSG("Save proxies to file:",name_seg_file);
		save_seg_file(name_seg_file, mesh_context.segmentation);
	}

	/**
	 * Return the name file without the extension
	 * ex if the user provides "output.off", it returns "output"
	 *
	 * assume all extension have length 3
	 */
	std::string remove_extension(const std::string& name_with_extension) const{
		std::size_t dot_position = name_with_extension.find(".");
		if(dot_position==std::string::npos)
			return std::string();
		else{
			std::string res = name_with_extension;
			res.resize(name_with_extension.size()-(name_with_extension.size()-dot_position));
			return res;
		}
	}
};


int main(int argc, char* argv[]){
	if(argc!=num_args+1){
		std::cerr << "Wrong number of arguments : "<<argc<<std::endl;
		std::cerr << "Usage : "<< argv[0]<<" and ...\n";
		std::cerr<< "  - file_in.off : file to load\n";
		std::cerr<< "  - file_out.off : file to save the simplified result\n";
		std::cerr <<"  - file_proxy.seg : file that contains proxies -> set to 0 to avoid proxy during simplification\n";
		std::cerr<< "  - n_min : number of vertices of simplified model\n";
		std::cerr<< "  - contr.param : name of a file that contains simplification parameters\n";
		//contraction param
		//		std::cerr <<"  - link condition (0 or 1)\n";


		return EXIT_FAILURE;
	}

	Program progr(argc,argv);

	progr.run();

	return EXIT_SUCCESS;
}



#endif /* URBAN_COLLAPSE_CPP_ */
