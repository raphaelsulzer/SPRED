/*
 * Contractor_interface.h
 *
 *  Created on: Feb 17, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef CONTRACTOR_INTERFACE_H_
#define CONTRACTOR_INTERFACE_H_



#include <memory>

#include "gudhi/gudhi_common.h"
#include "Mesh_context.h"

#include "iocomplex.h"
#include "contraction/CGAL_contractor.h"


#include "gudhi/gudhi_contraction.h"
//#include "geometry/contraction/Edge_profile.h"
//#include "geometry/contraction/Skeleton_blocker_contractor.h"
//#include "geometry/contraction/policies/Cost_policy.h"
//#include "geometry/contraction/policies/Placement_policy.h"
//#include "geometry/contraction/policies/Valid_contraction_policy.h"


#include "contraction/policies/Urban_edge_profile.h"

#include "typedefs.h"

#include "interface/Strategy_from_string.h"
#include "interface/Contraction_parameter.h"
#include "complex/complex_tools.h"
#include "complex/reorient.h"


template<typename C> class Primitive_graph;


class Contractor_interface{
	std::string name_lt = "Lindstrom Turk";

private:
	typedef Skeleton_blocker_contractor<Complex,Urban_edge_profile<Complex> > Contractor;
//	typedef GUDHI::contraction::Edge_handle Edge_handle;
	typedef Contractor::Profile Profile;

	typedef Gudhi::contraction::Cost_policy<Profile> Cost_policy;
	typedef Gudhi::contraction::Placement_policy<Profile> Placement_policy;
	typedef Gudhi::contraction::Valid_contraction_policy<Profile> Valid_contraction_policy;

	typedef Complex::Point Point;

	Mesh_context& mesh_context_;

	string policy_text;

	Strategy_from_string<Profile> strategy_from_string;

	//todo smart pointer
	Contractor * contractor;

	bool need_update_;



public:

	Contractor_interface(Mesh_context& mesh_context):
		mesh_context_(mesh_context),
		policy_text("Length_midpoint"),
		strategy_from_string(mesh_context),
		contractor(0),
		need_update_(true){	}

	void update(){
		//delete(contractor);
		if(!is_lindstrom()){
			UDBGVALUE(mesh_context_.params.abstraction_weight);
			strategy_from_string.update();
			contractor = new Contractor(
					mesh_context_.mesh,
					strategy_from_string.get_cost_policy(policy_text),
					strategy_from_string.get_placement_policy(policy_text),
					strategy_from_string.get_valid_policy(policy_text),
					strategy_from_string.get_contraction_visitor(policy_text),
					strategy_from_string.get_profile_factory(policy_text)
			);
		}
		need_update_ = false;
	}


	void contract_edges(int num_contractions){
		if(is_lindstrom()){
			simplification_cgal_collapse(num_contractions);
		}
		else{
			if(need_update_) update();
			MClock clock;
			contractor->contract_edges(num_contractions);
			clock.end();
			if(clock.num_seconds()!=0)
				std::cerr << "Edge contractions took "<< clock << " seconds (eg "<<num_contractions/clock.num_seconds()<<" contractions/s)\n";
		}
	}


	void set_policy(const std::string &policy_description_){
		policy_text = policy_description_;
		need_update_ = true;
	}

	boost::optional<std::pair<Edge_handle,boost::optional<Point> > > get_best_edge(){
		boost::optional<std::pair<Edge_handle,boost::optional<Point> > >  res;
		if(!is_lindstrom())
			res = contractor->top_edge();
		return res;
	}


	bool is_heap_empty() const{
		return contractor->is_heap_empty();
	}

	/**
	 * returns the placement and the cost of the edge it is in the heap.
	 * Otherwise, its return value is uninitialized.
	 */
	boost::optional<std::pair<boost::optional<Point>,boost::optional<double> > >  get_placement_and_cost(Edge_handle edge){
		boost::optional<std::pair<boost::optional<Point>,boost::optional<double> > >  res;

		if(!is_lindstrom()){
			auto placement = strategy_from_string.get_placement_policy(policy_text);
			auto cost = strategy_from_string.get_cost_policy(policy_text);

			auto profile_factory(strategy_from_string.get_profile_factory(policy_text));

			Profile profile(profile_factory->make_profile(mesh_context_.mesh,edge));

			std::pair<boost::optional<Point>,boost::optional<double> > placement_cost;
			placement_cost.first = (*placement)(profile);
			placement_cost.second= (*cost)(profile,placement_cost.first);
			res = placement_cost;

			auto is_valid = strategy_from_string.get_valid_policy(policy_text);

			UDBGMSG("Is valid:",(*is_valid)(profile,placement_cost.first));

			UDBGMSG("Is in heap:",contractor->is_in_heap(edge));

		}
		return res;
	}


	/**
	 * to be called when something has changed (a priority, the model, ...)
	 */
	void need_update(){
		need_update_ = true;
	}

private:

	/**
	 * can simplify whether with sk/bl ds or with cgal ds when the policy is lindstrom
	 */
	bool is_lindstrom() const{
		return (policy_text== name_lt);
	}

	void simplification_cgal_collapse(int num_collapse){
		UDBG("cgal");
		clock_t start = clock();

		string name_file("tmp.off");
		bool saved = save_off_file(name_file, mesh_context_.mesh);
		if (!saved){
			cerr << "Can't write in file " << name_file << endl;
		}

		reorient<Complex>(name_file, name_file);

//		reorient_faces(name_file);

		cgal_contraction::cgal_collapse(name_file,num_collapse);

		bool loaded = read_off_two_dimensional_file(name_file, mesh_context_.mesh);
		if (!loaded) {
			std::cerr << "Can't load complex in file:"<<name_file<<" \n";
			return;
		}

		std::cerr << "Edge contractions took "<< ( (float)(clock()-start))/CLOCKS_PER_SEC << " seconds\n";
	}

};


#endif /* CONTRACTOR_INTERFACE_H_ */
