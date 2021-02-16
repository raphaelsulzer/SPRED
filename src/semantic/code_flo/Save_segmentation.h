//#ifndef SAVE_SEGMENTATION_H
//#define SAVE_SEGMENTATION_H


//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <fstream>
//#include <string>
//#include <CGAL/Polyhedron_3.h>
//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/IO/Polyhedron_iostream.h>
//#include <CGAL/IO/print_wavefront.h>
//#include <boost/filesystem.hpp>
//#include <CGAL/Polyhedron_incremental_builder_3.h>

//#include <cmath>
//#include <math.h>
//#include <map>


//#include "typedefs.h"


//template<typename VertexHandle>
//Planar_proxy*
//get_majority_proxy_from_triangle(VertexHandle v1,VertexHandle v2,VertexHandle v3){
//	std::map<Planar_proxy*,int> num_proxy;

//	for(auto proxy : v1->primitives()){
//		++num_proxy[proxy];
//	}
//	for(auto proxy : v2->primitives()){
//		++num_proxy[proxy];
//	}
//	for(auto proxy : v3->primitives()){
//		++num_proxy[proxy];
//	}

//	if(num_proxy.empty()) return 0;
//	else{
//		auto pr = std::max_element(num_proxy.begin(), num_proxy.end(),
//				[](const std::pair<Planar_proxy*,int>& p1, const std::pair<Planar_proxy*,int>& p2) {
//			return p1.second < p2.second; });
//		return pr->first;
//	}
//}



//template<typename CloudSegmentation>
//void save_segmentation(CloudSegmentation &segmentation,const std::string& name_file){

//	std::ofstream myfile;
//	myfile.open (name_file.c_str());


//	myfile<<"ply "<<std::endl;
//	myfile<<"format ascii 1.0"<<std::endl;
//	myfile<<"comment author: D. Salinas"<<std::endl;
//	myfile<<"element vertex "<< segmentation.mesh->size_of_vertices() <<std::endl;
//	myfile<<"property float x"<<std::endl;
//	myfile<<"property float y"<<std::endl;
//	myfile<<"property float z"<<std::endl;
//	myfile<<"element face "<< segmentation.mesh->size_of_facets()<<std::endl;
//	myfile<<"property list uchar int vertex_index"<<std::endl;
//	myfile<<"property uchar red"<<std::endl;
//	myfile<<"property uchar green"<<std::endl;
//	myfile<<"property uchar blue"<<std::endl;
//	myfile<<"end_header"<<std::endl;

//	for( auto vi = segmentation.mesh->vertices_begin(); vi != segmentation.mesh->vertices_end(); ++vi){
//		myfile << vi->point()<<std::endl;
//	}


//	for( auto fi = segmentation.mesh->facets_begin(); fi != segmentation.mesh->facets_end(); ++fi){
//		auto edge_it = fi->facet_begin();
//		auto v1 = edge_it->vertex(),
//				v2 = (++edge_it)->vertex(),
//				v3 = (++edge_it)->vertex();

//		//		Planar_proxy* proxy = get_majority_proxy_from_triangle(v1->primitives(),v2->primitives(),v3->primitives());

//		Planar_proxy* proxy = get_majority_proxy_from_triangle(v1,v2,v3);

//		myfile << "3 "<< v2->index() <<" "<<v1->index() <<" "<<v3->index() <<" ";
//		if(proxy)
//			myfile <<(int)proxy->color().red() << " "
//			<< (int)proxy->color().green() << " "
//			<< (int)proxy->color().blue();
//		else
//			myfile << "200 10 10";
//		myfile <<std::endl;
//	}

//	myfile.close();
//}


//#endif
