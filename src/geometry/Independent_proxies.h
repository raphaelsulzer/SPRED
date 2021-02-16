/*
 * Independent_proxies.h
 *
 *  Created on: May 27, 2014
 *      Author: dsalinas
 */

#ifndef INDEPENDENT_PROXIES_H_
#define INDEPENDENT_PROXIES_H_


#include "urban_utils.h"



//todo determinant a la place pour eviter pb liés a l'orientation





const double default_alpha_value = 0.2;

/**
 * Lindstrom Turk : v1 and v2 are alpha-independant if their angle is greater than alpha
 */
template<typename Vector>
bool is_independent(const Vector& v1,const Vector& v2,double alpha){
	//determinant
//	Vector v1_norm = v1;
//	Vector v2_norm = v2;

//	// Lindstrom Turk : v1 and v2 are alpha-independant if their angle is greater than alpha
	double v1_scal_v2 = v1*v2;
	return v1_scal_v2*v1_scal_v2 < v1.squared_length()*v2.squared_length()*std::cos(alpha)*std::cos(alpha);
}




/**
 * Lindstrom Turk : v1,v2 and v3 are alpha-independant if v1 and v2 are alpha-independent
 * and if v3 is not alpha-dependent of a linear combination of v1 and v2
 */
template<typename Vector>
bool is_independent(const Vector& v1,const Vector& v2,const Vector& v3,double alpha){

//// determinant
//	Vector v1_norm = v1;
//	Vector v2_norm = v2;
//	Vector v3_norm = v3;
//	v1_norm /= v1.std::sqrt(squared_length());
//	v2_norm /= v2.std::sqrt(squared_length());
//	v3_norm /= v3.std::sqrt(squared_length());
//	return std::abs(CGAL::determinant(v1_norm,v2_norm,v3_norm) >= alpha);


	// Lindstrom Turk : v1,v2 and v3 are alpha-independant if v1 and v2 are alpha-independent
	// and if v3 is not alpha-dependent of a linear combination of v1 and v2
	if(!is_independent(v1,v2,alpha)) return false;
	Vector v1_cross_v2 = CGAL::cross_product(v1,v2);
	double sc_prod = v1_cross_v2*v3;
	return sc_prod*sc_prod > v1_cross_v2.squared_length() * v3.squared_length() * std::sin(alpha)*std::sin(alpha);
}

template<typename Proxy>
bool is_alpha_independent_proxies(Proxy* p0,Proxy* p1,double alpha = default_alpha_value){
	return is_independent(
			p0->plane().orthogonal_vector(),
			p1->plane().orthogonal_vector(),
			alpha
	);
}

template<typename Proxy>
bool is_alpha_independent_proxies(Proxy* p0,Proxy* p1,Proxy* p2,double alpha = default_alpha_value){
	return is_independent(
			p0->plane().orthogonal_vector(),
			p1->plane().orthogonal_vector(),
			p2->plane().orthogonal_vector(),
			alpha
	);
}


template<typename Proxy,typename ProxyIterator>
std::vector<Proxy*> alpha_independent_proxies(
		ProxyIterator begin_proxies,
		ProxyIterator end_proxies,
		double alpha = default_alpha_value
){
	std::vector<Proxy*> res;
	if (begin_proxies != end_proxies){
		ProxyIterator current_proxy = begin_proxies;
		res.push_back(*current_proxy);
		++current_proxy;
		while(current_proxy != end_proxies && res.size()<3){
			if(res.size()==1){
				if(is_alpha_independent_proxies(res[0],*current_proxy,alpha)){
					res.push_back(*current_proxy);
				}
			}
			else{
				if(is_alpha_independent_proxies(res[0],res[1],*current_proxy,alpha)){
					res.push_back(*current_proxy);
				}

			}
			++current_proxy;
		}

	}
	return res;

}



#endif /* INDEPENDENT_PROXIES_H_ */
