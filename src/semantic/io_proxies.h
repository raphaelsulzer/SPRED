/*
 * io_proxies.h
 *
 *  Created on: Apr 29, 2014
 *      Author: dsalinas
 */

#ifndef IO_PROXIES_H_
#define IO_PROXIES_H_

#include <iostream>
#include <fstream>
#include "urban_utils.h"
#include <string>
/**
 * each line in a ".seg" file contains info to parse a proxy_token.
 */
template<typename GT>
struct proxy_token{
	std::list<int>  vertices;
	double area;
	typename GT::Plane_3 plane;


	proxy_token(const std::string& line):area(-1){
		// find "#vertices:", "#plane:" and "#area:" position
		std::string plane_token("plane:");
		std::string area_token("area:");
		std::string vertices_token("vertices:");
		std::size_t pos_vertices = line.find(vertices_token);
		std::size_t pos_plane = line.find(plane_token);
		std::size_t pos_area = line.find(area_token);

		// line format condition
		if (
				pos_plane==std::string::npos ||
				pos_vertices==std::string::npos ||
				pos_area == std::string::npos)
		{
			std::cerr<< "Error while reading file\n";
			std::cerr<< "Current line:"<<line<<"\n";
			exit(-1);
		}
		else{
			if(!(pos_plane < pos_area && pos_area<pos_vertices  )){
				std::cerr<< "Wrong order for token 'plane' 'area' and 'vertices'\n";
				std::cerr<< "Current line:"<<line<<"\n";
				exit(-1);
			}

			else{
				//read plane
				// plane_string = "0.5 0.2 0.1 2"
				std::string plane_string = line.substr (pos_plane+plane_token.size(), pos_area-pos_plane - plane_token.size());
				std::vector< double > plane_coordinates;
				std::istringstream iss_plane( plane_string );
				double coord;
				while(iss_plane >> coord) { plane_coordinates.push_back(coord); }
				if(plane_coordinates.size()!=4){
					std::cerr<< "Error while reading file, plane with more than 4 coordinates.\n";
					exit(-1);
				}

				plane = typename GT::Plane_3(plane_coordinates[0],plane_coordinates[1],plane_coordinates[2],plane_coordinates[3]);

				//read area
				// area_string = "20.0"
				std::string area_string = line.substr (pos_area+area_token.size(), pos_vertices - pos_area - area_token.size());
				std::istringstream iss_area( area_string );
				iss_area>>area;

				// read vertices
				std::string vertices_string = line.substr (pos_vertices+vertices_token.size());
				// vertices_string = "1 0 10 22"
				std::istringstream iss_vertices( vertices_string );
				int vertex;
				while(iss_vertices >> vertex) { vertices.push_back(vertex); }
			}
		}
	}
};


inline bool is_comment_line(const std::string& line){
	assert(!line.empty());
	return line[0]=='%';
}


/**
 * Read proxies from file with the following format:
 *
 * first line % 10 proxies (line starting with % are not read)
 * and lines
 * "#plane:  0.5 0.2 0.1 2  #area: 12.0 #vertices: 1 0 10 22 " -> for vertices of the proxy 1,0,10,22 with plane 0.5x+0.2y+0.1z+2 = 0 and area 12.0
 */
template<typename GT>
bool read_proxies(std::istream &stream,
		std::list<proxy_token<GT> >& primitives)
{
	UDBG("-- Read proxies from file");

	primitives.clear();

	std::string line;
	while ( getline (stream,line) )
	{
		// if not comment line
		if(!line.empty() && !is_comment_line(line))
			primitives.push_back(proxy_token<GT>(line));
	}

	UDBG("-- Sort proxies per area");
	primitives.sort([&](const proxy_token<GT>& lhs,const proxy_token<GT>& rhs){
		return lhs.area>rhs.area;
	});
	return true;
}

template<typename EnrichedMesh>
bool save_seg_file(const std::string& name_file_output, const EnrichedMesh& segmentation){
	if(!segmentation.is_empty()){
		std::ofstream stream;
		stream.open (name_file_output);
		if(stream)
			stream << segmentation;
		else{
			std::cerr << "Cant load file "<<name_file_output<<std::endl;
			return false;
		}
		stream.close();
	}
	return true;
}


inline std::map<int,int> read_initial_vertices(std::istream& stream){
	std::map<int,int> int_to_vh;
	std::string first_line;
	getline(stream,first_line);
	std::istringstream iss(first_line);

	std::string vertices_token;
	iss>>vertices_token;

	int vertex;
	int current = 0;
	while(iss>>vertex){
		assert(vertex>=0);
		int_to_vh[vertex] = current++;
	}
	return int_to_vh;
}




#endif /* IO_PROXIES_H_ */
