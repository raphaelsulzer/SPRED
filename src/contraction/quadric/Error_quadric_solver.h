/*
 * Error_quadric_solver.h
 *
 *  Created on: Apr 29, 2014
 *      Author: dsalinas
 */

#ifndef ERROR_QUADRIC_SOLVER_H_
#define ERROR_QUADRIC_SOLVER_H_

#include "Error_quadric.h"

#include "complex/complex_tools.h"


template <typename EdgeProfile> class Error_quadric_direct_solver{
public :
	typedef typename EdgeProfile::Complex Complex;
	typedef typename EdgeProfile::GT GT;
	typedef typename GT::Point Point;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	boost::optional<Point> operator()(
			const Error_quadric_& quadric,
			const EdgeProfile& profile
	) const{
		const double min_determinant = 1e-10;

		boost::optional<Point> pt_res;

		double determinant_ = quadric.grad_determinant() ;

		if (determinant_<0) determinant_*= -1 ;
		// xxx abs fait un arondi??
		// determinant = std::abs(determinant);
		if (determinant_>min_determinant) {
			pt_res = quadric.solve_linear_gradient(determinant_);
		}
		else{
			//		quadric.set_is_singular(false);
			pt_res.reset();
		}
		return pt_res;

	}
};



template <typename EdgeProfile> class Error_quadric_svd_solver{
public :
	typedef typename EdgeProfile::Complex Complex;
	typedef typename EdgeProfile::GT GT;
	typedef typename GT::Point Point;
	typedef quadric::Error_quadric<GT> Error_quadric_;

	/**
	 * svd_threshold is the ratio to threshold to 0
	 * small eigen value
	 *
	 * direct_computation indicates if the solver
	 * computes directly the solution (faster but less
	 * accurate)
	 */
	boost::optional<Point> operator()(
			const Error_quadric_& quadric,
			const EdgeProfile& profile,
			double svd_threshold,
			bool direct_computation
	) const{
		//xxx todo avoid this other link computation
		auto star_center = star_centroid(profile.complex(),profile.edge_handle());
		return this->operator ()(quadric,profile,star_center,svd_threshold,direct_computation);
	}


	/**
	 * svd_threshold is the ratio to threshold to 0
	 * small eigen value
	 *
	 * direct_computation indicates if the solver
	 * computes directly the solution (faster but less
	 * accurate)
	 */
	boost::optional<Point> operator()(
			const Error_quadric_& quadric,
			const EdgeProfile& profile,
			const Point& star_centroid,
			double svd_threshold,
			bool direct_computation
	) const{
		boost::optional<Point> returned_point(quadric.min_cost_svd(star_centroid,svd_threshold,direct_computation));
		assert(returned_point);
		return returned_point;
	}
};


template <typename EdgeProfile> class Error_quadric_hybrid_solver{
public :
	boost::optional<typename EdgeProfile::GT::Point> operator()(
			const quadric::Error_quadric<typename EdgeProfile::GT>& quadric,
			const EdgeProfile& profile
	) const{
		typedef typename quadric::Error_quadric<typename EdgeProfile::GT> Error_quadric_;
		typedef typename Error_quadric_::Intersection_type Intersection_type;

		const double spring_value = 1;

		auto star_center = star_centroid(profile.complex(),profile.edge_handle());

		Intersection_type intersection_type;

		auto optimal_point(quadric.min_cost_svd(star_center,intersection_type));

		double cost_p0,cost_p1,cost_mid;
		switch (intersection_type) {
		case Error_quadric_::PLANE:
			//			UDBG("plane");
			quadric += compute_spring_quadric(profile,spring_value);
			return((quadric.min_cost_svd(star_center)));
			break;
		case Error_quadric_::LINE:
			//			UDBG("line");
			cost_p0 = quadric.cost(profile.p0());
			cost_p1 = quadric.cost(profile.p1());
			cost_mid = quadric.cost(CGAL::midpoint(profile.p0(),profile.p1()));

			if(cost_p0 < std::min(cost_p1,cost_mid))
				return(profile.p0());
			else
				if (cost_p1 < std::min(cost_p0,cost_mid))
					return profile.p1();
				else
					return CGAL::midpoint(profile.p0(),profile.p1());
			break;
		default:
			return optimal_point;
			break;
		}

	}
};



#endif /* ERROR_QUADRIC_SOLVER_H_ */
