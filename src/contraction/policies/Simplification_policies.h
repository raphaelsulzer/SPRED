/*
 * Simplification_policies.h
 *
 *  Created on: Apr 28, 2014
 *      Author: dsalinas
 */

#ifndef SIMPLIFICATION_POLICIES_H_
#define SIMPLIFICATION_POLICIES_H_


#include "gudhi/gudhi_contraction.h"

//#include "geometry/contraction/policies/Cost_policy.h"
//#include "geometry/contraction/policies/Placement_policy.h"
//#include "geometry/contraction/policies/Contraction_visitor.h"
//#include "geometry/contraction/policies/Valid_contraction_policy.h"

/**
 * Wraps the set of needed features to describe a
 * simplification policy.
 */
template< typename EdgeProfile> class Simplification_policies{
public:
	typedef Gudhi::contraction::Cost_policy<EdgeProfile> Cost_policy;
	typedef Gudhi::contraction::Placement_policy<EdgeProfile> Placement_policy;
	typedef Gudhi::contraction::Valid_contraction_policy<EdgeProfile> Valid_contraction_policy;
	typedef Gudhi::contraction::Contraction_visitor<EdgeProfile> Contraction_visitor;
	typedef Gudhi::contraction::Edge_profile_factory<EdgeProfile> Edge_profile_factory;


	virtual ~Simplification_policies(){}

	/**
	 * such pointers can be NULL in case of an empty policy
	 * (ex no visitor or no validity test)
	 */
	Cost_policy* cost() {
		return cost_;
	}

	Placement_policy* placement() {
		return placement_;
	}

	Valid_contraction_policy* is_valid() {
		return is_valid_;
	}

	Contraction_visitor* visitor() {
		return visitor_;
	}

	Edge_profile_factory* edge_profile_factory(){
		return profile_factory_;
	}

protected :
	Simplification_policies(
			Cost_policy* cost,
			Placement_policy* placement,
			Valid_contraction_policy* is_valid,
			Contraction_visitor* visitor,
			Edge_profile_factory* profile_factory = 0
	):
		cost_(cost),
		placement_(placement),
		is_valid_(is_valid),
		visitor_(visitor),
		profile_factory_(profile_factory)
	{
	}

	Cost_policy* cost_;
	Placement_policy* placement_;
	Valid_contraction_policy* is_valid_;
	Contraction_visitor* visitor_;
	Edge_profile_factory* profile_factory_;


};



#endif /* SIMPLIFICATION_POLICIES_H_ */
