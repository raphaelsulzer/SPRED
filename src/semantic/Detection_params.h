/*
 * Detection_params.h
 *
 *  Created on: Aug 7, 2014
 *      Author: dsalinas
 */

#ifndef DETECTION_PARAMS_H_
#define DETECTION_PARAMS_H_

#include <iostream>
#include <string>
#include <sstream>


#include "urban_utils.h"



//todo remove reg_eps et reg_coplan
/**
 * Parameters for proxy detection
 */
struct Detection_params{
	double max_dist;

	/**
	 * angle in degree in (0,90)
	 */
	double max_angle;
	double min_area;

	/**
	 * regularization parameter
	 */
	double reg_epsilon;
	double reg_coplanarity;

//	/**
//	 * merge parameters.
//	 * Every pair of proxies with angle less than
//	 * max_angle_merge and st the new plane
//	 * is at distance lower than max_dist_merge than their points
//	 * will be merged.
//	 * Afterwards, only proxies with area at least min_area_merge
//	 * are kept.
//	 */
//	double max_angle_merge;
//	double max_dist_merge;
//	double min_area_merge;




private:
	std::string detection_method_;

public:

	Detection_params():max_dist(-1),max_angle(20),min_area(10.),reg_epsilon(0.02),reg_coplanarity(0.1),
	detection_method_("Florent")
	{
	}

	Detection_params(
			double max_dist_,
			double max_angle_,
			double min_area_
	):max_dist(max_dist_),max_angle(max_angle_),min_area(min_area_),reg_epsilon(0.02),reg_coplanarity(0.1),
			detection_method_("Florent")
	{
	}


	Detection_params(
			double max_dist_,
			double max_angle_,
			double min_area_,
			const std::string& method_
	):max_dist(max_dist_),max_angle(max_angle_),min_area(min_area_),reg_epsilon(0.02),reg_coplanarity(0.1),
			detection_method_(method_)
	{
	}


	Detection_params(
				double max_dist_,
				double max_angle_,
				double min_area_,
				double reg_epsilon_,
				double reg_coplanarity_
		):max_dist(max_dist_),max_angle(max_angle_),min_area(min_area_),reg_epsilon(reg_epsilon_),reg_coplanarity(reg_coplanarity_),
				detection_method_("Florent")
	{
	}


	const std::string& detection_method() const{
		return detection_method_;
	}


	void set_detection_method(const std::string& new_method){
		detection_method_ = new_method;
	}


	friend std::ostream & operator<<(std::ostream& oss,const Detection_params& params){
		oss <<	"%Detection parameters:\n";
		oss << "max_angle: "<< params.max_angle<<"\n"
			<< "max_dist: "<<params.max_dist<<"\n"
			<<  "min_area: "<<params.min_area<<"\n"
			<< "reg_epsilon: "<<params.reg_epsilon<<"\n"
			<< "reg_coplanarity: "<<params.reg_coplanarity<<"\n"
			<< "detection_method: "<<params.detection_method_;
		return oss;
	}


	friend std::istream& operator>>(std::istream& stream, Detection_params& params)
	{
		read_param(params.max_angle,stream);
		read_param(params.max_dist,stream);
		read_param(params.min_area,stream);
		read_param(params.reg_epsilon,stream);
		read_param(params.reg_coplanarity,stream);
		read_param(params.detection_method_,stream);
		return stream;
	}

};





#endif /* DETECTION_PARAMS_H_ */
