/*
 * Clock.h
 *
 *  Created on: Jun 17, 2014
 *      Author: dsalinas
 */

#ifndef CLOCK_H_
#define CLOCK_H_


#include <boost/date_time/posix_time/posix_time.hpp>

class MClock{

public:
	MClock():end_called(false){
		startTime = boost::posix_time::microsec_clock::local_time( );
	}

	void begin() const{
		end_called = false;
		startTime = boost::posix_time::microsec_clock::local_time( );
	}

	void end() const{
		end_called = true;
		endTime = boost::posix_time::microsec_clock::local_time( );
	}

	friend std::ostream& operator<< (std::ostream& stream,const MClock& clock){
		if(!clock.end_called)
			clock.end();

		if(!clock.end_called) stream << "end not called";
		else{
			stream << clock.num_seconds() <<"s";
		}
		return stream;

	}

	double num_seconds() const{
		if(!end_called) return -1;
		return (endTime-startTime).total_milliseconds()/1000.;
	}

private:
	mutable boost::posix_time::ptime startTime, endTime;
	mutable bool end_called;
};


#endif /* CLOCK_H_ */
