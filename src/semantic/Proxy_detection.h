/*
 * Proxy_detection.h
 *
 *  Created on: Jun 27, 2014
 *      Author: dsalinas
 */

#ifndef PROXY_DETECTION_H_
#define PROXY_DETECTION_H_

#include <string>
#include <iostream>
#include <sstream>
#include "Detection_params.h"


/**
 * Extract proxies and write it in a file "*.seg".
 * With the following format :
 *   first line : % 10 proxies -> (number of proxies line starting with % are not read)
 *   and lines
 * "#plane:  0.5 0.2 0.1 2  #area: 12.0 #vertices: 1 0 10 22 " -> for vertices of the proxy 1,0,10,22 with plane 0.5x+0.2y+0.1z+2 = 0 and area 12.0
 */
template<typename SkBlComplex> class Proxy_detection{

public:
	virtual ~Proxy_detection(){};

	//only base class should be called
protected:
	Proxy_detection(){};

	Proxy_detection(const SkBlComplex& complex,const std::string& name_seg_file,const Detection_params& proxy_params)
	{}
};





#endif /* PROXY_DETECTION_H_ */
