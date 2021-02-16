/*
 * Color.h
 *
 *  Created on: Sep 15, 2014
 *      Author: dsalinas
 */

#ifndef COLOR_H_
#define COLOR_H_


class Color{
	double r_;
	double g_;
	double b_;
public:
	Color():r_(1.),g_(0.),b_(0.){
	}

	Color(double r,double g,double b):r_(r),g_(g),b_(b){
	}

	Color(const CGAL::Color& color):r_(color.r()/255.),g_(color.g()/255.),b_(color.b()/255.){
	}


	double r() const{
		return r_;
	}
	double g() const{
		return g_;
	}
	double b() const{
		return b_;
	}
};


#endif /* COLOR_H_ */
