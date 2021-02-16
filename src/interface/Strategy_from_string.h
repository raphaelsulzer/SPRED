/*
 * Strategy_from_string.h
 *
 *  Created on: Feb 24, 2014 
 *      Author: David Salinas
 *  Copyright 2013 INRIA. All rights reserved
 */

#ifndef STRATEGY_FROM_STRING_H_
#define STRATEGY_FROM_STRING_H_

#include <memory>
#include <map>
#include <string>

#include "urban_utils.h"


#include "gudhi/gudhi_contraction.h"

//#include "geometry/contraction/policies/Cost_policy.h"
//#include "geometry/contraction/policies/Placement_policy.h"
//#include "geometry/contraction/policies/Contraction_visitor.h"
//#include "geometry/contraction/policies/Dummy_valid_contraction.h"



#include "interface/Mesh_context.h"
#include "interface/Contraction_parameter.h"
#include "contraction/policies/Simplification_policies.h"
#include "contraction/policies/Length_midpoint_policies.h"
#include "contraction/policies/GH_policies.h"
#include "contraction/policies/GH_memoryless_policies.h"
#include "contraction/policies/Cached_urban.h"
#include "contraction/policies/Urban_policies.h"
#include "contraction/policies/Tmp_policies.h"

#include "contraction/policies/Current_policies.h"


#include "contraction/CGAL_middle_placement.h"
//#include "contraction/Quadric_placement.h"
#include "contraction/Urban_valid_contraction.h"



#include "contraction/CGAL_edge_length_cost.h"
#include "semantic/Primitive_graph.h"




template<typename TAbstractStrategy> class AbstractFactory{
public:
	virtual ~AbstractFactory(){}
	virtual TAbstractStrategy* operator()()=0;
};
template<typename TAbstractStrategy,typename TStrategy> class CreatorWithoutContext : public AbstractFactory<TAbstractStrategy>{
public:
	virtual ~CreatorWithoutContext(){}
	TAbstractStrategy* operator()(){
		return new TStrategy;
	}
};
template<typename TAbstractStrategy,typename TStrategy,typename TContext> class CreatorWithContext : public AbstractFactory<TAbstractStrategy>{
private:
	TContext* context;
public:
	CreatorWithContext(TContext* context){
		this->context = context;
	}
	TAbstractStrategy* operator()(){
		return new TStrategy(context);
	}
};



template< typename EdgeProfile> class Strategy_from_string{
	std::string name_lt = "Lindstrom Turk";
	std::string name_gh = "Garland Heckbert";
	std::string name_urban = "Structure preserving (memoryless)";
	std::string name_urban_cached = "Structure preserving";
	std::string name_midpoint = "Min length and midpoint";

	typedef typename EdgeProfile::Complex Complex;

	typedef Simplification_policies<EdgeProfile> Simplification_policies_;
	typedef Length_midpoint_policies<EdgeProfile> Length_midpoint_policies_;
	typedef GH_policies<EdgeProfile> GH_policies_;
	typedef GH_memoryless_policies<EdgeProfile> GH_memoryless_policies_;
	typedef Cached_urban_policies<EdgeProfile> Cached_urban_policies_;
	typedef Urban_policies<EdgeProfile> Urban_policies_;
	typedef Tmp_policies<EdgeProfile> Tmp_policies_;
	typedef Current_policies<EdgeProfile> Current_policies_;




	typedef Gudhi::contraction::Cost_policy<EdgeProfile> Cost_policy;
	typedef Gudhi::contraction::Placement_policy<EdgeProfile> Placement_policy;
	typedef Gudhi::contraction::Valid_contraction_policy<EdgeProfile> Valid_contraction_policy;
	typedef Gudhi::contraction::Contraction_visitor<EdgeProfile> Contraction_visitor;
	typedef Gudhi::contraction::Edge_profile_factory<EdgeProfile> Edge_profile_factory;

	typedef Gudhi::contraction::Link_condition_valid_contraction<EdgeProfile> Link_condition_valid;
	typedef Gudhi::contraction::Dummy_valid_contraction<EdgeProfile> Dummy_valid_contraction;
	typedef quadric::Urban_valid_contraction<EdgeProfile> Urban_valid_contraction;


	std::map<string,Simplification_policies_* > string_to_policy;


	Mesh_context& mesh_context_;
public:
	Strategy_from_string( Mesh_context& mesh_context): mesh_context_(mesh_context){
		init_map();
	}

	void init_map(){
		UDBG("--------init_map");

		UDBGVALUE(mesh_context_.params);
		string_to_policy[name_midpoint] = new Length_midpoint_policies_(mesh_context_);
		string_to_policy[name_gh] = new GH_policies_(mesh_context_);
		string_to_policy[name_urban] = new Urban_policies_(mesh_context_);
		string_to_policy[name_urban_cached] = new Cached_urban_policies_(mesh_context_);
	}

	void delete_map(){
		//		while (!string_to_policy.empty())
		//			delete(string_to_policy.begin()->second);
		string_to_policy.clear();
	}

	/**
	 * @brief to be called when the complex has changed. It clears all policies and
	 * recompute  them.
	 */
	void update(){
		//		delete_map();
		init_map();
	}



	~Strategy_from_string(){
		delete_map();
	}


	Cost_policy* get_cost_policy(const string& policy_textual){
		return string_to_policy[policy_textual]->cost();
	}

	Placement_policy* get_placement_policy(const string &policy_textual){
		return string_to_policy[policy_textual]->placement();
	}

	Valid_contraction_policy* get_valid_policy(const string& policy_textual){
		return string_to_policy[policy_textual]->is_valid();
	}

	Contraction_visitor* get_contraction_visitor(const string& policy_textual){
		return string_to_policy[policy_textual]->visitor();
	}

	Edge_profile_factory* get_profile_factory(const string& policy_textual){
		return string_to_policy[policy_textual]->edge_profile_factory();
	}


};



#endif /* STRATEGY_FROM_STRING_H_ */

