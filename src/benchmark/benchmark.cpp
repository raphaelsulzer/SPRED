/*
 * benchmark.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: dsalinas
 */

#ifndef BENCHMARK_CPP_
#define BENCHMARK_CPP_

#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>


#include "interface/Mesh_context.h"
#include "geometry/hausdorff/Meshlab_hausdorff_distance.h"
#include "geometry/hausdorff/Distance_point_to_triangle.h"
#include "typedefs.h"

#include "gudhi/gudhi_common.h"


using namespace std;

#include "interface/Contractor_interface.h"
#include "interface/Contraction_parameter.h"

#include "semantic/Enriched_mesh.h"
#include "semantic/Primitive_graph.h"

#include "geometry/geometry_tools.h"
#include "geometry/hausdorff/Hausdorff_distance.h"

#include "interface/Contraction_parameter.h"
#include "semantic/Detection_params.h"



const int num_args = 8;

/**
 * arguments of the program read from the argument of the main
 */
struct bench_args{
	string name_file_input;
	string name_file_output;
	string name_file_proxy;

	int n_start;
	int n_step;
	int n_min;

	Contraction_parameter contr_params;
	string strategy;

	bool proxy_use;



	bench_args(int argc, char* argv[]){
		assert(argc==num_args+1);
		int current_argument = 1; // to reorder easily
		name_file_input = string(argv[current_argument++]);
		name_file_output = string(argv[current_argument++]);
		name_file_proxy = string(argv[current_argument++]);

		n_start= atoi( argv[current_argument++]);
		n_step= atoi( argv[current_argument++]);
		n_min= atoi( argv[current_argument++]);
	
		read_contr_params(string(argv[current_argument++]));

		strategy = string(argv[current_argument++]);

		proxy_use = name_file_proxy.size()>1;
	}

	void read_contr_params(const string& name_file_contr_params){
		std::ifstream file(name_file_contr_params,std::ios::in);
		if (!file.is_open())
			std::cerr << "Can't open file "<<name_file_contr_params.c_str()<<std::endl;
		else
			file >> contr_params;
	}

	std::string to_string() const{
		std::ostringstream stream;
		stream << "---- benchmark params ----  \n ";
		stream 	<< "  name_file_in: "<< name_file_input<<std::endl;
		stream 	<< "  name_file_out: "<< name_file_output<<std::endl;
		stream 	<< "  name_file_proxy: "<< name_file_proxy<<std::endl;
		stream 	<< "  n_min: "<< n_min<<std::endl;
		stream << "  Contr params"<<contr_params<<std::endl;
		stream 	<< "  proxy_use:"<< proxy_use<<std::endl;
		stream<< std::endl;
		return stream.str();
	}
};

struct Bench{
public:
	typedef Complex::Point Point;

	typedef Complex::GT GT;

	bench_args args;

	Bench(int argc,char* argv[]):
		args(argc,argv),
		mesh_context(),
		collapsor(mesh_context)
	{
		std::cerr <<"Bench args : "<<args.to_string()<<std::endl;

		if(!init()) std::cerr <<"Initialization problem \n";
	}


private:
	Mesh_context mesh_context;
	Contractor_interface collapsor;


	/**
	 * init the mesh, the collapsor, ...
	 */
	bool init(){

		if(!load_mesh()) return false;

		std::cerr << "Loaded mesh has " << mesh_context.mesh.num_vertices() << " vertices\n";

		if (stop_condition_reached()) return true;

		set_initial_points_for_error();

		mesh_context.params = args.contr_params;

		load_proxies_with_args_proxy_file();

		if(args.proxy_use)
			mesh_context.update_graphs(mesh_context.params.distance_max_graph_edges,false);
		else
			mesh_context.params.abstraction_weight = 0;

		// set the cost and placement
		if(!set_strategy()){
			std::cerr << "Problem setting the strategy\n";
			return false;
		}

		std::cerr << "loaded parameters : "<<mesh_context.params<<std::endl;

		return true;
	}

	bool load_mesh(){
		bool loaded = read_off_two_dimensional_file(args.name_file_input, mesh_context.mesh);
		if (!loaded) {
			std::cerr << "Cant load file "<<args.name_file_input<<endl;
		}
		std::cerr << "Loaded mesh has " << mesh_context.mesh.num_vertices() << " vertices\n";
		return loaded;
	}

	void set_initial_points_for_error(){
		for(auto vi : mesh_context.mesh.vertex_range())
			mesh_context.initial_points.push_back(mesh_context.mesh.point(vi));

	}

	bool stop_condition_reached() const{
		return mesh_context.mesh.num_vertices()<=args.n_min;
	}

	bool load_proxies_with_args_proxy_file(){
		std::ifstream file_planes(args.name_file_proxy);
		if(file_planes.is_open()){
			file_planes>>mesh_context.segmentation;
		}
		return file_planes.is_open();
	}


	bool set_strategy(){
		// set the cost and placement
		if(args.strategy.compare(string("LT"))==0){
			collapsor.set_policy(string("LT"));
		}
		else{
			if(args.strategy.compare(string("GH"))==0){
				collapsor.set_policy(string("GH"));
			}
			else{
				if(args.strategy.compare(string("Middle"))==0){
					collapsor.set_policy(string("Lenght_midpoint"));

				}
				else{
					if(args.strategy.compare(string("UB"))==0) {
						collapsor.set_policy(string("Urban"));
					}
					else{
						if(args.strategy.compare(string("UBcached"))==0) {
							collapsor.set_policy(string("Cached_urban"));
						}
						else{
							if(args.strategy.compare(string("ML"))==0) {
								collapsor.set_policy(string("GH_memoryless"));
							}
							else{
								exit(1);
								return false;
							}
						}
					}
				}
			}
		}
		return true;
	}



public:

	/**
	 * run the benchmark
	 */
	bool run(){
		clock_t start_benchmark;

		ofstream stats;

		stats.open(args.name_file_output);
		if (!stats.is_open()){
			std::cerr << "Unable to open file "<<args.name_file_output<<"\n";
			return false;
		}

		start_benchmark = clock();

		Meshlab_hausdorff_distance<Complex> meshlab_dist(mesh_context.mesh);
		Distance_point_to_triangle<Complex> point_to_triangles_dist(mesh_context.initial_points);

		if(stop_condition_reached()) return false;

		if(args.n_start>0 && (mesh_context.mesh.num_vertices()-args.n_start >0))
			collapsor.contract_edges(mesh_context.mesh.num_vertices()-args.n_start);

		while(!stop_condition_reached()){
			UDBG("\n\n**********");
			UDBGVALUE(mesh_context.mesh.num_vertices());

			int old_num_vertices = mesh_context.mesh.num_vertices();
			int nb_collapses = get_num_collapses_with_linear_scale(old_num_vertices);


			collapsor.contract_edges(nb_collapses);
			if (mesh_context.mesh.num_vertices() == old_num_vertices){
				std::cerr << "\n\n\nNo edge collapsible anymore, leaving\n\n\n";
				break; //no edge collapsible anymore
			}

			compute_distances_and_ouput_to_arg(
					stats,
					mesh_context.mesh,
					meshlab_dist,
					point_to_triangles_dist
					);
		}

		stats.close();


		std::cerr << "\n\n\n---------------------------------------\nDone with: \n";
		std::cerr << args.to_string()<<endl;


		std::cerr << "Benchmark took "<< ( (float)(clock()-start_benchmark))/CLOCKS_PER_SEC << " seconds\n";
		return true;
	}

private:
	int get_num_collapses_with_log_scale(int num_vertices){
		int num_numbers = std::floor(std::log10(num_vertices-1));

		int num_collapses = std::ceil(std::pow(10,num_numbers) / (double)args.n_step *10) ;

		if( num_vertices - num_collapses < args.n_min){
			num_collapses = num_vertices-args.n_min;
		}
		else{
			if(  num_vertices - num_collapses < std::pow(10,num_numbers))
				num_collapses = num_vertices - std::pow(10,num_numbers);
		}
		return num_collapses;
	}

	int get_num_collapses_with_linear_scale(int num_vertices){
		return args.n_step;
	}


	void compute_distances_and_ouput_to_arg(
			ofstream& stats,
			Complex& mesh,
			Meshlab_hausdorff_distance<Complex>& meshlab_dist,
			Distance_point_to_triangle<Complex>& point_to_triangles_dist){
		clock_t start_distance = clock();
		point_to_triangles_dist.compute_distance(mesh_context.mesh);
		double t_my = (float)(clock()-start_distance)/CLOCKS_PER_SEC;
		std::cerr << "My distance took "<< t_my << " seconds\n";


		start_distance = clock();

		start_distance = clock();
		meshlab_dist.compute_distance(mesh_context.mesh);

		auto proxy_avg_dist = sum_avg_distance_to_proxies(mesh) / mesh.num_vertices();



		stats << mesh_context.mesh.num_vertices()
				<<" "<<meshlab_dist.mean_s_to_o()
				<<" "<<meshlab_dist.mean_o_to_s()
				<<" "<<(std::max)(meshlab_dist.mean_s_to_o(),meshlab_dist.mean_o_to_s())
				<<endl;

		UDBG("");
		UDBGVALUE(meshlab_dist.mean_s_to_o());
		UDBGVALUE(meshlab_dist.mean_o_to_s());
		UDBGVALUE((std::max)(meshlab_dist.mean_o_to_s(),meshlab_dist.mean_s_to_o()));


		std::cerr << "Proxy_avg_dist : "<<proxy_avg_dist<<std::endl;
		std::cerr << "Mean: "<<(std::max)(meshlab_dist.mean_s_to_o(),meshlab_dist.mean_o_to_s())<<std::endl;


	}
};


int main(int argc, char* argv[]){


	if(argc != num_args+1){
		std::cerr << "Usage : "<< argv[0]<<" and ...\n";
		std::cerr<< "  - file.off : file to load\n";
		std::cerr<< "  - stats.txt : name of the file where the stats must be saved\n";
		std::cerr<< "  - name_file_proxy.seg : name of the file where the proxies must be loaded (put any file if not proxy)\n";
		std::cerr<< "  - n_start : starting number of vertices (the nb of vertices of the complex is initially reduced to this number if -1 no collapse is done before counting stats)\n";
		std::cerr<< "  - n_step : number of times stats are registered for each interval [10^i, 10^i+1]\n";
		std::cerr<< "  - n_min : minimal number of vertices (no collapse when this number of vertices is reached)\n";
		std::cerr <<"  - contr_params.dat : contraction parameters\n";
		std::cerr <<"  - strategy : simplifictation strat (LT,GH,UB)\n";

		std::cerr << "\n\n ex : ./bin/Release/benchmark lans_church.off stats_file.txt lans_church.seg -1 20 30 contr_params.dat UB \n";

		//		std::cerr<< "example : ./"<< argv[0]<<" lans_church.off Quadric Quadrics 100 100 stats.txt\n";
		return EXIT_FAILURE;
	}

	Bench dans_ma_bench_bench_bench(argc,argv);

	dans_ma_bench_bench_bench.run();

	return EXIT_SUCCESS;
}



#endif /* BENCHMARK_CPP_ */
