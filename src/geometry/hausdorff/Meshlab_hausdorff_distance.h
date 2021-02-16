/*
 * Meshlab_hausdorff_distance.h
 *
 *  Created on: Apr 22, 2014
 *      Author: dsalinas
 */

#ifndef MESHLAB_HAUSDORFF_DISTANCE_H_
#define MESHLAB_HAUSDORFF_DISTANCE_H_

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "urban_utils.h"

/**
 * compute hausdorff distance by using a meshlab script
 */
template<typename Complex>
class Meshlab_hausdorff_distance{
private:
	int initial_num_vertices_;

	/**
	 * simplified to original distances
	 */
	double max_s_to_o_;
	double mean_s_to_o_;
	double rms_s_to_o_;

	/**
	 * original to simplified distances
	 */
	double max_o_to_s_;
	double mean_o_to_s_;
	double rms_o_to_s_;

	bool compute_s_to_o_;
	bool compute_o_to_s_;

public:

	Meshlab_hausdorff_distance(const Complex &initial_mesh):
		max_s_to_o_(-1),mean_s_to_o_(-1),rms_s_to_o_(-1),
		max_o_to_s_(-1),mean_o_to_s_(-1),rms_o_to_s_(-1){
		// save mesh in .off
		// todo -> save only points to be faster
		bool saved = save_off_file("original.off", initial_mesh);
		if (!saved) std::cerr << "Can't write in file original.off \n";
		initial_num_vertices_ = initial_mesh.num_vertices();

		compute_s_to_o_ = true;
		compute_o_to_s_ = true;
	}


	void compute_distance_s_to_o(const Complex &simplified_mesh){
		bool saved = save_off_file("simplified.off", simplified_mesh);
		if (!saved) std::cerr << "Can't write in file simplified.off"  << std::endl;

		saved = save_mesh_lab_script(1);
		if (!saved) std::cerr << "Can't write mesh lab script"  << std::endl;

//		std::string cmd("meshlabserver -i original.off simplified.off -s hausdorff.mlx >& res_dist.tmp");
		std::string cmd("meshlabserver -i simplified.off original.off -s hausdorff.mlx >& res_dist.tmp");
		auto r = system (cmd.c_str());
		if (r); // remove warning
		read_distances_from_file(max_s_to_o_,mean_s_to_o_,rms_s_to_o_);
	}

	void compute_distance(const Complex &simplified_mesh){
		bool saved = save_off_file("simplified.off", simplified_mesh);
		if (!saved) std::cerr << "Can't write in file simplified.off"  << std::endl;

		saved = save_mesh_lab_script(1);
		if (!saved) std::cerr << "Can't write mesh lab script"  << std::endl;

		if(compute_s_to_o_){
			std::string cmd("meshlabserver -i simplified.off original.off -s hausdorff.mlx >& res_dist.tmp");
			UDBGVALUE(cmd);
			auto r = system (cmd.c_str());
			if (r); // remove warning
			read_distances_from_file(max_s_to_o_,mean_s_to_o_,rms_s_to_o_);
		}

		if(compute_o_to_s_){
			std::string cmd("meshlabserver -i original.off simplified.off -s hausdorff.mlx >& res_dist.tmp");
			UDBGVALUE(cmd);
			auto r = system (cmd.c_str());
			if (r); // remove warning
			read_distances_from_file(max_o_to_s_,mean_o_to_s_,rms_o_to_s_);
		}
	}

private:
	void read_distances_from_file(double &max, double & mean,double& rms){
		std::ifstream infile("res_dist.tmp");
		std::string line;
		while (std::getline(infile, line))
		{
			if(line.length()>3){
				std::size_t found_min = line.find("min");
				std::size_t found_max = line.find("max");
				std::size_t found_mean = line.find("mean");
				std::size_t found_RMS = line.find("RMS");

				if((found_min != std::string::npos) && (found_max != std::string::npos) &&
						(found_mean!= std::string::npos) && (found_RMS != std::string::npos) ){
					read_distances_from_line(line,max,mean,rms);
					return;
				}
			}
		}

		std::cerr << "1 / cant read mesh result \n";
		exit(1);
		assert(false);
	}


public:
	/*
	 * compute_distance(mesh) must be called before calling these members
	 * simplified to original distances
	 *
	 */
	double max_s_to_o() const{
		assert(compute_s_to_o_);
		return max_s_to_o_;
	}
	double mean_s_to_o() const{
		assert(compute_s_to_o_);
		return mean_s_to_o_;
	}
	double rms_s_to_o() const{
		assert(compute_s_to_o_);
		return rms_s_to_o_;
	}

	/**
	 * original to simplified distances
	 */
	double max_o_to_s() const{
		assert(compute_o_to_s_);
		return max_o_to_s_;
	}
	double mean_o_to_s() const{
		assert(compute_o_to_s_);
		return mean_o_to_s_;
	}
	double rms_o_to_s() const{
		assert(compute_o_to_s_);
		return rms_o_to_s_;
	}


	double hausdorff_distance_max() const{
		assert(compute_o_to_s_ && compute_s_to_o_);
		return std::max(max_s_to_o_,max_o_to_s_);
	}

	double hausdorff_distance_mean() const{
		assert(compute_o_to_s_ && compute_s_to_o_);
		return std::max(mean_s_to_o_,mean_o_to_s_);
	}

	double hausdorff_distance_rms() const{
		assert(compute_o_to_s_ && compute_s_to_o_);
		return std::max(rms_s_to_o_,rms_o_to_s_);
	}

private:

	/**
	 * example, for the following line, read 2e-6
	 * line:      min : 0.000000   max 0.000002   mean : 0.000000   RMS : 0.000000
	 * LOG: 2      min : 0.000000   max 0.139524   mean : 0.000494   RMS : 0.002417
	 *
	 *
	 */
	void read_distances_from_line(const std::string& line,double &max, double & mean,double& rms){
		std::string token;
		std::istringstream iss(line);
		if(!(iss>> token >> token>> token >> token >> token >> token >> max >> token >> token >> mean >> token >> token >> rms))
		{
			std::cerr << "2/ cant read mesh result \n";
			exit(1);
			assert(false);
		}
	}




private:
	bool save_mesh_lab_script(double scale){
		// save hausdorff.mlx
		std::ofstream script_file;
		script_file.open ("hausdorff.mlx");
		if (!script_file.is_open()){
			std::cerr << "Unable to open file hausdorff.mlx\n";
			return false;
		}
		script_file << "<!DOCTYPE FilterScript>" <<std::endl;
		script_file << "<FilterScript> "<<std::endl;
		script_file << " <filter name=\"Hausdorff Distance\"> "<<std::endl;
		script_file << "<Param type=\"RichMesh\" value=\"1\" name=\"SampledMesh\"/> "<<std::endl;
		script_file << "<Param type=\"RichMesh\" value=\"0\" name=\"TargetMesh\"/> "<<std::endl;
		script_file << "<Param type=\"RichBool\" value=\"false\" name=\"SaveSample\"/> "<<std::endl;
		script_file << "<Param type=\"RichBool\" value=\"true\" name=\"SampleVert\"/> "<<std::endl;
		script_file << "<Param type=\"RichBool\" value=\"false\" name=\"SampleEdge\"/> "<<std::endl;
		script_file << "<Param type=\"RichBool\" value=\"false\" name=\"SampleFauxEdge\"/> "<<std::endl;
		script_file << "<Param type=\"RichBool\" value=\"false\" name=\"SampleFace\"/> "<<std::endl;
		script_file << "<Param type=\"RichInt\" value=\" "<<initial_num_vertices_<<"\" name=\"SampleNum\"/> "<<std::endl;
		script_file << "<Param type=\"RichAbsPerc\" value=\"2.34635\" min=\"0\" name=\"MaxDist\" max=\"46.927\"/> "<<std::endl;
		script_file << "</filter> "<<std::endl;
		script_file << "</FilterScript> "<<std::endl;
		script_file.close();
		return true;
	}
};



#endif /* MESHLAB_HAUSDORFF_DISTANCE_H_ */
