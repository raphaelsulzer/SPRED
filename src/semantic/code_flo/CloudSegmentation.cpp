/*
 * Cloud_segmentation.cpp
 *
 *  Created on: 21 janv. 2014
 *      Author: dsalinas
 */

#include "semantic/code_flo/Save_segmentation.h"

#include "CloudSegmentation.h"
#include <CGAL/bounding_box.h>
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/HalfedgeDS_vector.h>


#ifdef _WIN32
#pragma warning( disable : 4507 4503 4244 4103 4267 4396 4018 4258 4305 4101)
#endif

using namespace std;

const double PI = 3.141592;

double distance_point_d(Gudhi_kernel::Point p1,Gudhi_kernel::Point p2){return sqrt(pow(p1.x()-p2.x(),2)+pow(p1.y()-p2.y(),2)+pow(p1.z()-p2.z(),2));}


void print_list_points(std::vector<std::set<Polyhedron_vertex_handle> > &list_points2){
	int i = 0;
	for (auto x = list_points2.begin(); x != list_points2.end(); ++x , ++i ) {
		std::cerr <<"Points of primitive "<<i<<":";
		for (auto p = x->begin() ; p != x->end(); ++ p)
			//cerr << (*p)->index() << ",";
			cerr <<" ("<<x->size()<<" in total)"<<endl;
	}

}


Cloud_segmentation::Cloud_segmentation(
		Polyhedron_3* mesh_
):
			mesh(mesh_)
{
	init();
}

int
Cloud_segmentation::num_proxies() const{
	return extracted_planes.size();
}

Cloud_segmentation::Vector
Cloud_segmentation::regularization_normales(Vector normale ,double cos_vertical){

	double vz=cos_vertical;
	double A=1-cos_vertical*cos_vertical;

	if(normale.x()==0){
		Vector res(normale.x(),normale.y(),normale.z());
		FT norm=(1/sqrt(res.squared_length()));
		res=norm*res;
		return res;
	}
	double B= 1+(normale.y()*normale.y())/(normale.x()*normale.x());
	double vx=sqrt(A/B);
	if(normale.x()<0)  vx=-vx;
	double vy=vx*(normale.y()/normale.x());

	Vector res(vx,vy,vz);
	FT norm=(1/sqrt(res.squared_length()));
	res=norm*res;

	return res;
}

Cloud_segmentation::Vector
Cloud_segmentation::regularization_normales_from_prior(Vector normal_parent, Vector normal,double cos_vertical){

	double vz=cos_vertical;
	double vx,vy;

	if(normal_parent.x()!=0){
		double a = (normal_parent.y()*normal_parent.y())/(normal_parent.x()*normal_parent.x()) + 1;
		double b = 2*normal_parent.y()*normal_parent.z()*vz/normal_parent.x();
		double c= vz*vz-1;

		if(4*a*c > b*b) return regularization_normales(normal,cos_vertical);
		else {
			double delta = sqrt(b*b-4*a*c);
			double vy1= (-b-delta)/(2*a);
			double vy2= (-b+delta)/(2*a);

			if( abs(normal.y()-vy1) < abs(normal.y()-vy2) ) vy= vy1;
			else vy= vy2;

			vx= (-normal_parent.y()*vy-normal_parent.z()*vz)/normal_parent.x();
		}
	}
	else if(normal_parent.y()!=0){
		vy=-normal_parent.z()*vz/normal_parent.y();
		vx=sqrt(1-vz*vz-vy*vy); if ( normal.x() <0 ) vx=-vx;
	}

	else{
		return regularization_normales(normal,cos_vertical);
	}

	Vector res(vx,vy,vz);
	FT norm=max(1e-5,1./sqrt(res.squared_length()));
	res=norm*res;
	return res;
}



// find pairs of epsilon-parallel primitives and store them in table_parallel
void
Cloud_segmentation::compute_parralel_primitives(
		const double &epsilon ,
		std::vector < std::vector < bool > >& table_parallel
){
	for(unsigned int i=0;i<extracted_planes.size(); i++){
		std::vector < bool > table_parallel_tmp;
		for( unsigned int j=0;j<extracted_planes.size(); j++){

			Vector v1=extracted_planes[i].orthogonal_vector();
			Vector v2=extracted_planes[j].orthogonal_vector();

			if( abs(v1*v2)>1.-epsilon && i!=j) table_parallel_tmp.push_back(true);
			else table_parallel_tmp.push_back(false);
		}
		table_parallel.push_back(table_parallel_tmp);
	}
}

// find pairs of epsilon-parallel primitives and store them in table_parallel
void
Cloud_segmentation::cluster_parralel_primitives(
		const std::vector < std::vector < bool > >& table_parallel,
		const double& epsilon,
		std::vector < std::vector < int > > &list_parallel_planes,
		std::vector < Vector > &list_cluster_normales,
		std::vector < double >& list_cluster_cosangle_vertical,
		std::vector < double >& list_cluster_area
){
	// std::vector < bool > is_available(extracted_planes.size(),true); instead
	std::vector < bool > is_available(extracted_planes.size(),true);

	for(unsigned int i=0;i<extracted_planes.size();i++){

		if(is_available[i]){

			is_available[i]=false;

			//initialization containers
			std::vector < int > index_container_parallel; index_container_parallel.push_back(i);
			std::vector < int > index_container_former_ring_parallel; index_container_former_ring_parallel.push_back(i);
			std::list < int > index_container_current_ring_parallel;

			//propagation over the pairs of epsilon-parallel primitives
			bool propagation=true;
			Vector cluster_normal=extracted_planes[i].orthogonal_vector();
			double cumulated_area=list_areas[i];

			do{
				propagation=false;

				for(unsigned int k=0;k<index_container_former_ring_parallel.size();k++){

					int plane_index=index_container_former_ring_parallel[k];

					for(unsigned int it=0;it<table_parallel[plane_index].size();it++){

						Vector normal_it=extracted_planes[it].orthogonal_vector();

						if( table_parallel[plane_index][it] && is_available[it] && abs(normal_it*cluster_normal)>1.-epsilon ){

							propagation=true;
							index_container_current_ring_parallel.push_back(it);
							is_available[it]=false;

							if(cluster_normal*normal_it <0) normal_it=-normal_it;
							cluster_normal=(FT)cumulated_area*cluster_normal+(FT)list_areas[it]*normal_it;
							FT norm=1./sqrt(cluster_normal.squared_length());
							cluster_normal=norm*cluster_normal;
							cumulated_area+=list_areas[it];
						}
					}
				}

				//update containers
				index_container_former_ring_parallel.clear();
				for(std::list < int >::iterator it = index_container_current_ring_parallel.begin(); it != index_container_current_ring_parallel.end(); ++it){
					index_container_former_ring_parallel.push_back(*it);
					index_container_parallel.push_back(*it);
				}
				index_container_current_ring_parallel.clear();

			}while(propagation);

			list_parallel_planes.push_back(index_container_parallel);
			list_cluster_normales.push_back(cluster_normal);
			list_cluster_area.push_back(cumulated_area);
			Vector v_vertical(0.,0.,1.);
			list_cluster_cosangle_vertical.push_back(abs(v_vertical*cluster_normal));
		}
	}
}

//discovery orthogonal relationship between clusters
void
Cloud_segmentation::compute_orthogonal_relationship(
		const std::vector < std::vector < int > > &list_parallel_planes,
		const std::vector < Vector > &list_cluster_normales,
		const double & epsilon,
		std::vector < std::vector < bool > >& group_planes_orthogonal
){
	for(unsigned int i=0;i<list_parallel_planes.size(); i++){
		std::vector < bool > gp_tmp(list_parallel_planes.size(),false);
		group_planes_orthogonal.push_back(gp_tmp);
	}

	for(unsigned int i=0; i<group_planes_orthogonal.size();i++){
		for(unsigned int j=0; j<group_planes_orthogonal.size();j++){

			if( i!=j && abs(list_cluster_normales[i]*list_cluster_normales[j])<epsilon){
				group_planes_orthogonal[i][j]=true;
				group_planes_orthogonal[j][i]=true;
			}
		}
	}
}


void
Cloud_segmentation::cluster_cosangle(
		const std::vector < double > &list_cluster_area,
		const std::vector < std::vector < bool > >& group_planes_orthogonal,
		const double & epsilon,
		std::vector < double > &list_cluster_cosangle_vertical
){
	std::vector < double > cosangle_centroids;
	std::vector < int > list_cluster_index;
	for(unsigned int i=0;i<list_cluster_cosangle_vertical.size(); i++)
		list_cluster_index.push_back(-1);
	int mean_index=0;
	for(unsigned int i=0;i<list_cluster_cosangle_vertical.size(); i++){
		if(list_cluster_index[i]<0){
			list_cluster_index[i]=mean_index;
			double mean=list_cluster_area[i]*list_cluster_cosangle_vertical[i];
			double mean_area=list_cluster_area[i];
			for(unsigned int j=i+1; j<list_cluster_cosangle_vertical.size(); j++){
				if( list_cluster_index[j]<0 && abs(list_cluster_cosangle_vertical[j]-mean/mean_area)<epsilon ){
					list_cluster_index[j]=mean_index;
					mean_area+=list_cluster_area[j];
					mean+=list_cluster_area[j]*list_cluster_cosangle_vertical[j];
				}
			}
			mean_index++;
			mean/=mean_area;
			cosangle_centroids.push_back(mean);
		}
	}
	for(unsigned int i=0;i<cosangle_centroids.size(); i++) {
		if(cosangle_centroids[i]<epsilon) cosangle_centroids[i]=0;
		else if(cosangle_centroids[i]>1.-epsilon) cosangle_centroids[i]=1;
	}
	for(unsigned int i=0; i<group_planes_orthogonal.size();i++) list_cluster_cosangle_vertical[i]=cosangle_centroids[list_cluster_index[i]];

	/*
	//display console
	cout<<endl<<endl<<"clusters of parallel primitives:";
	for(unsigned int i=0; i<list_parallel_planes.size();i++){
		cout<<endl<<i<<" -> ";
		for(unsigned int j=0; j<list_parallel_planes[i].size();j++) cout<<list_parallel_planes[i][j]<<"  ";
	}

	cout<<endl<<endl<<"pairs of orthogonal clusters:";
	for(unsigned int i=0; i<group_planes_orthogonal.size();i++){
		cout<<endl<<i<<" -> ";
		for(unsigned int j=0;j<group_planes_orthogonal.size();j++){
			if(group_planes_orthogonal[i][j]) cout<<j<<"  ";
		}
		cout<<"     -> "<<list_cluster_cosangle_vertical[i]<<"  -> "<<cosangle_centroids[list_cluster_index[i]];
	}
	 */
}


void
Cloud_segmentation::find_mutually_orthogonal_clusters(
		const std::vector < std::vector < int > > & list_parallel_planes,
		const std::vector < std::vector < bool > >& group_planes_orthogonal,
		const std::vector < double > &list_cluster_area,
		std::vector < std::vector < int > >& subgraph_clusters,
		std::vector < int >& subgraph_clusters_max_area_index
){
	std::vector < bool > is_free(list_parallel_planes.size(),true);

	for(unsigned int i=0; i<list_parallel_planes.size();i++){
		if(is_free[i]){

			is_free[i]=false;
			double max_area=list_cluster_area[i];
			int index_max_area=i;

			//initialization containers
			std::vector < int > index_container; index_container.push_back(i);
			std::vector < int > index_container_former_ring; index_container_former_ring.push_back(i);
			std::list < int > index_container_current_ring;

			//propagation
			bool propagation=true;
			do{
				propagation=false;

				//neighbors
				for(unsigned int k=0;k<index_container_former_ring.size();k++){

					int cluster_index=index_container_former_ring[k];

					for(unsigned int j=0;j<group_planes_orthogonal[cluster_index].size();j++){
						if(group_planes_orthogonal[cluster_index][j] && is_free[j]){
							propagation=true;
							index_container_current_ring.push_back(j);
							is_free[j]=false;

							if(max_area<list_cluster_area[j]){
								max_area=list_cluster_area[j];
								index_max_area=j;
							}
						}
					}
				}

				//update containers
				index_container_former_ring.clear();
				for(std::list < int >::iterator it = index_container_current_ring.begin(); it != index_container_current_ring.end(); ++it){
					index_container_former_ring.push_back(*it);
					index_container.push_back(*it);
				}
				index_container_current_ring.clear();

			}while(propagation);
			subgraph_clusters.push_back(index_container);
			subgraph_clusters_max_area_index.push_back(index_max_area);
		}
	}
	is_free.clear();
}


void
Cloud_segmentation::cluster_normals_regularization(
		const std::vector < double > & list_cluster_cosangle_vertical,
		const std::vector < std::vector < int > >& subgraph_clusters_prop,
		const std::vector < int > & subgraph_clusters_max_area_index,
		const std::vector < std::vector < bool > >& group_planes_orthogonal,
		std::vector < Vector >  &list_cluster_normales
){
	std::vector< bool > cluster_is_available(list_cluster_cosangle_vertical.size(),true);
	//for(unsigned int i=0;i<list_cluster_cosangle_vertical.size();i++) cluster_is_available.push_back(true);

	for(unsigned int i=0; i<subgraph_clusters_prop.size();i++){

		int index_current=subgraph_clusters_max_area_index[i];
		Vector vec_current=regularization_normales(list_cluster_normales[index_current],list_cluster_cosangle_vertical[index_current]);
		list_cluster_normales[index_current]=vec_current;
		cluster_is_available[index_current]=false;

		//initialization containers
		std::vector < int > index_container; index_container.push_back(index_current);
		std::vector < int > index_container_former_ring; index_container_former_ring.push_back(index_current);
		std::list < int > index_container_current_ring;

		//propagation
		bool propagation=true;
		do{
			propagation=false;

			//neighbors
			for(unsigned int k=0;k<index_container_former_ring.size();k++){

				int cluster_index=index_container_former_ring[k];

				for(unsigned int j=0;j<group_planes_orthogonal[cluster_index].size();j++){

					if(group_planes_orthogonal[cluster_index][j] && cluster_is_available[j]){

						propagation=true;
						index_container_current_ring.push_back(j);
						cluster_is_available[j]=false;

						Vector new_vect=regularization_normales_from_prior(list_cluster_normales[cluster_index], list_cluster_normales[j], list_cluster_cosangle_vertical[j]);
						list_cluster_normales[j]=new_vect;
					}
				}
			}

			//update containers
			index_container_former_ring.clear();
			for(std::list < int >::iterator it = index_container_current_ring.begin(); it != index_container_current_ring.end(); ++it){
				index_container_former_ring.push_back(*it);
				index_container.push_back(*it);
			}
			index_container_current_ring.clear();
		}while(propagation);
	}
}


void
Cloud_segmentation::detect_coplanarity(
		const std::vector < Vector > & list_cluster_normales,
		const std::vector < std::vector < int > > &list_parallel_planes,
		const double& tolerance_coplanarity,
		std::vector< std::vector< std::vector < int > > > &list_coplanar_prim
){
	//detecting co-planarity and store in list_coplanar_prim
	for(unsigned int i=0; i<list_parallel_planes.size();i++){

		std::vector< std::vector < int > > list_coplanar_prim_tmp;
		Vector vec_reg=list_cluster_normales[i];
		std::vector < int > list_cop_index(list_parallel_planes[i].size(),-1);
		int cop_index=0;

		for(unsigned int j=0; j<list_parallel_planes[i].size();j++){
			int index_prim=list_parallel_planes[i][j];

			if(list_cop_index[j]<0){

				std::vector < int > list_coplanar_prim_tmp_tmp;
				list_cop_index[j]=cop_index;
				list_coplanar_prim_tmp_tmp.push_back(index_prim);

				Point pt_reg=extracted_planes[index_prim].projection(list_centroid[index_prim]);
				Plane_3 plan_reg(pt_reg,vec_reg);

				for(unsigned int k=j+1; k<list_parallel_planes[i].size(); k++){
					if( list_cop_index[k]<0){

						int index_prim_next=list_parallel_planes[i][k];
						Point pt_reg_next=extracted_planes[index_prim_next].projection(list_centroid[index_prim_next]);
						Point pt_proj=plan_reg.projection(pt_reg_next);
						double distance=distance_point_d(pt_reg_next,pt_proj);

						if(distance<tolerance_coplanarity ){
							list_cop_index[k]=cop_index;
							list_coplanar_prim_tmp_tmp.push_back(index_prim_next);
						}
					}
				}
				list_coplanar_prim_tmp.push_back(list_coplanar_prim_tmp_tmp);
				cop_index++;
			}
		}
		list_coplanar_prim.push_back(list_coplanar_prim_tmp);
	}
}

//regularize primitive position by computing barycenter of coplanar planes
void
Cloud_segmentation::regularize_primitive_position(
		const std::vector< std::vector< std::vector < int > > > &list_coplanar_prim,
		const double& epsilon//,
		//std::vector < std::vector < int > >& list_primitive_reg_index_extracted_planes,
		//std::vector < Plane_3 >& list_primitive_reg
)
{
	for(unsigned int i=0;i<list_coplanar_prim.size();i++){
		for(unsigned int j=0;j<list_coplanar_prim[i].size();j++){


			// new test
			Point pt_bary(0.,0.,0.);
			double area=0;

			for(unsigned int k=0; k<list_coplanar_prim[i][j].size();k++){
				int index_prim=list_coplanar_prim[i][j][k];
				Point pt_reg=extracted_planes[index_prim].projection(list_centroid[index_prim]);

				pt_bary=barycenter(pt_bary, area,pt_reg,list_areas[index_prim]);
				area+=list_areas[index_prim];
			}
			Vector vec_reg=extracted_planes[list_coplanar_prim[i][j][0]].orthogonal_vector();

			Plane_3 plane_reg(pt_bary,vec_reg);

			bool is_reg_used=false;
			std::vector< int > list_primitive_reg_index_extracted_planes_tmp1;
			for(unsigned int k=0; k<list_coplanar_prim[i][j].size();k++){
				int index_prim=list_coplanar_prim[i][j][k];

				Plane_3 plane_reg(extracted_planes[index_prim].projection(list_centroid[index_prim]),vec_reg);
//				Plane_3 plane_reg(list_centroid[index_prim],vec_reg);


				if( abs(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()) > 1. - epsilon){
					if(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()<0)
						extracted_planes[index_prim]=plane_reg.opposite();
					else
						extracted_planes[index_prim]=plane_reg;
					is_reg_used=true;
					list_primitive_reg_index_extracted_planes_tmp1.push_back(index_prim);
					cout<<"+ ";
				}
				else{
					list_primitive_reg.push_back(extracted_planes[index_prim]);
					std::vector< int > list_primitive_reg_index_extracted_planes_tmp;
					list_primitive_reg_index_extracted_planes_tmp.push_back(index_prim);
					list_primitive_reg_index_extracted_planes.push_back(list_primitive_reg_index_extracted_planes_tmp);
					cout<<"- ";
				}
			}
			if(is_reg_used) {
				list_primitive_reg.push_back(plane_reg);
				list_primitive_reg_index_extracted_planes.push_back(list_primitive_reg_index_extracted_planes_tmp1);
			}


			//			// old bug ici pour le calcul de pt_reg
			//			Point pt_bary(0.,0.,0.);
			//			double area=0;
			//
			//			for(unsigned int k=0; k<list_coplanar_prim[i][j].size();k++){
			//				int index_prim=list_coplanar_prim[i][j][k];
			//				Point pt_reg=extracted_planes[index_prim].projection(list_centroid[index_prim]);
			//
			//				pt_bary=barycenter(pt_bary, area,pt_reg,list_areas[index_prim]);
			//				area+=list_areas[index_prim];
			//			}
			//			Vector vec_reg=extracted_planes[list_coplanar_prim[i][j][0]].orthogonal_vector();
			//
			//			Plane_3 plane_reg(pt_bary,vec_reg);
			//
			//			bool is_reg_used=false;
			//			std::vector< int > list_primitive_reg_index_extracted_planes_tmp1;
			//			for(unsigned int k=0; k<list_coplanar_prim[i][j].size();k++){
			//				int index_prim=list_coplanar_prim[i][j][k];
			//				if( abs(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()) > 1. - epsilon){
			//					if(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()<0)
			//						extracted_planes[index_prim]=plane_reg.opposite();
			//					else
			//						extracted_planes[index_prim]=plane_reg;
			//					is_reg_used=true;
			//					list_primitive_reg_index_extracted_planes_tmp1.push_back(index_prim);
			//					cout<<"+ ";
			//				}
			//				else{
			//					list_primitive_reg.push_back(extracted_planes[index_prim]);
			//					std::vector< int > list_primitive_reg_index_extracted_planes_tmp;
			//					list_primitive_reg_index_extracted_planes_tmp.push_back(index_prim);
			//					list_primitive_reg_index_extracted_planes.push_back(list_primitive_reg_index_extracted_planes_tmp);
			//					cout<<"- ";
			//				}
			//			}
			//			if(is_reg_used) {
			//				list_primitive_reg.push_back(plane_reg);
			//				list_primitive_reg_index_extracted_planes.push_back(list_primitive_reg_index_extracted_planes_tmp1);
			//			}
		}
	}

}





/**
 * Compute the matching between vertices of the polyhedron and vertices of the complex
 * (which may be placed differently than for polyhedron since the Off loader of CGAL remove
 * all isolated vertices)
 */
template<typename SkBlComplex>
void compute_matching(SkBlComplex& mesh,Polyhedron_3& polyhedron,std::vector<int> &matching_polyhedron){
	assert((int)polyhedron.size_of_vertices()<=mesh.num_vertices());
	matching_polyhedron = std::vector<int>(polyhedron.size_of_vertices());

	auto v_complex = mesh.vertex_range().begin();
	for (auto v_polyhedron = polyhedron.vertices_begin(); v_polyhedron!= polyhedron.vertices_end() ;++v_polyhedron){
		while(mesh.degree(*v_complex)==0) ++v_complex;
		matching_polyhedron[v_polyhedron->index()]= (*v_complex).vertex;
		++v_complex;
	}
}




std::vector<Cloud_segmentation::Point>
Cloud_segmentation::intersect_Plane_BBox(const bounding_3 &bb, const Plane_3 &p)
{
	Plane_3 vp[6];
	vp[0] = Plane_3(bb[0],bb[1],bb[6] );vp[1] = Plane_3(bb[1],bb[2],bb[7] );  vp[2] = Plane_3(bb[2],bb[3],bb[4] );
	vp[3] = Plane_3(bb[3],bb[0],bb[5] );vp[4] = Plane_3(bb[5],bb[6],bb[7] );  vp[5] = Plane_3(bb[1],bb[0],bb[3] );

	set<Point_2d> list_point_tmp;
	Vector_2 tmp(0,0);
	for (unsigned int i=0;i<6;i++)
	{
		CGAL::Object result = CGAL::intersection(vp[i],p);
		Line_3 iline;
		if(assign(iline,result))
			//if (*iline = CGAL::object_cast<Line_3  >(&result))
		{
			for (unsigned int j=0;j<6;j++)
			{
				if (i==j) continue;
				CGAL::Object result2 = CGAL::intersection(vp[j],iline);
				Point ipoint;
				if(assign(ipoint,result2))
					//if ( *ipoint = CGAL::object_cast<Point  >(&result2))
				{
					if (ipoint.x() >= bb.xmin() &&  ipoint.x() <= bb.xmax()  &&
							ipoint.y() >= bb.ymin() &&  ipoint.y() <= bb.ymax()  &&
							ipoint.z() >= bb.zmin() &&  ipoint.z() <= bb.zmax())
					{
						Point_2d p2 = p.to_2d(ipoint);
						list_point_tmp.insert(p2);
					}
				}
			}
		}
	}
	vector<Point_2d> list_point;
	std::copy(list_point_tmp.begin(),list_point_tmp.end(), std::back_inserter(list_point));

	if (list_point.size() == 0) {printf("no intersection !\n");	 return  vector<Point>(0);}

	for (unsigned int i=0;i<	list_point.size();i++)
		tmp = tmp+(list_point[i]-CGAL::ORIGIN);
	tmp=tmp/list_point.size();

	Point_2d center = CGAL::ORIGIN+tmp;

	vector<float> angle_to_x;
	vector<int> indices_points;
	for (unsigned int i=0;i<	list_point.size();i++)
	{
		indices_points.push_back(i);

		Vector_2 v = list_point[i]-center;
		v=v/sqrt(v.squared_length());
		float theta =acos(v*Vector_2(1,0));
		if (v.y() < 0) theta = 2*M_PI - theta;
		angle_to_x.push_back(theta);
	}

	functor_comp_x cmp(angle_to_x);
	std::sort(indices_points.begin(), indices_points.end(), cmp);

	vector<Point> res(list_point.size());

	for (unsigned int i=0;i<	 res.size();i++)
	{
		Point p3D = p.to_3d(list_point[indices_points[i]]);
		res[i]	= p3D;
		// cout<<p3D<<endl;
	}

	//now the points are in order, create the triangles
	/*	vector<vector<Point> > list_triangles;
		for (unsigned int i=0;i<	 res.size()-2;i++)
		{
		vector<Point> tri(3);
		tri[0] = res[0];
		tri[1] = res[1+i];
		tri[2] = res[2+i];
		list_triangles.push_back(tri);

		}*/

	/*	Polyhedron poly = createClusterPoly(list_triangles);
		ofstream save("plane_cut.off");
		save << poly;
		save.close();*/

	return  res;
}



bool
Cloud_segmentation::update_bbox_2d(){

	list_bbox_2d.clear();

	for(unsigned int i=0;i<list_clusters.size();i++){

		std::list<Point_2d> list_2d;
		Plane_3 optimal_plane=extracted_planes[i];

		Vector vortho=optimal_plane.orthogonal_vector();
		Vector b1=optimal_plane.base1();
		Vector b2=optimal_plane.base2();
		FT norme1=sqrt(pow(b1.x(),2)+pow(b1.y(),2)+pow(b1.z(),2)); if(norme1<1e-7){norme1=1e-7;}
		FT norme2=sqrt(pow(b2.x(),2)+pow(b2.y(),2)+pow(b2.z(),2)); if(norme2<1e-7){norme2=1e-7;}
		Vector vb1=(1/norme1)*b1;
		Vector vb2=(1/norme2)*b2;

		for(unsigned int kk=0;kk<list_clusters[i].size();kk++){
			Facet_iterator f = index_to_facet[list_clusters[i][kk]];
			Point center_facet=f->center();

			Point ptp=optimal_plane.projection(center_facet);
			FT X1=vb1.x()*ptp.x()+vb1.y()*ptp.y()+vb1.z()*ptp.z();
			FT X2=vb2.x()*ptp.x()+vb2.y()*ptp.y()+vb2.z()*ptp.z();
			Point_2d ptp2(X1,X2);
			list_2d.push_back(ptp2);
		}
		list_bbox_2d.push_back(CGAL::bounding_box(list_2d.begin(), list_2d.end()));

	}

	return true;
}

void
Cloud_segmentation::clear(){
	extracted_planes.clear();
	list_bbox_2d.clear();
	list_centroid.clear();
	list_areas.clear();
	list_color.clear();
	list_clusters.clear();
	list_neighborg_facet.clear();
	//for random access of facets (this is quite dangerous if mesh changes, to use with cautions)
	index_to_facet.clear();
	list_vertices.clear();
}

void
Cloud_segmentation::compute_bounding_box_and_indices(){
	int count = 0;
	std::list < Point > points_temp2;
	list_vertices = vector<Vertex_iterator>(mesh->size_of_vertices());
	for( Polyhedron_3::Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi, ++count)
	{
		vi->index()=count;
		vi->primitives().clear();
		list_vertices[count] = vi;
		Point ptrt(vi->point().x(),vi->point().y(),vi->point().z());
		points_temp2.push_back(ptrt);
	}

	BBox_scan = CGAL::bounding_box(points_temp2.begin(), points_temp2.end());
	cout << "BBox_scan: "<<BBox_scan<<endl;
}

bool
Cloud_segmentation::init()
{

	clear();


	srand (1);

	compute_bounding_box_and_indices();

	index_to_facet = vector<Facet_iterator>(mesh->size_of_facets());

	//computation FACET normals
	int count = 0;
	std::cout << "computing facet normal...";
	Point p;

	for (Facet_iterator f = mesh->facets_begin() ; f != mesh->facets_end() ; ++f, ++count)
	{
		index_to_facet[count] = (f);
		Halfedge_handle
		h1 = f->halfedge(),
		h2 = h1->next(),
		h3 = h2->next();

		//calcul des normals
		Polyhedron_3::Vertex_handle
		v1 = h1->vertex(),
		v2 = h2->vertex(),
		v3 = h3->vertex();

		f->normal() = CGAL::orthogonal_vector(v1->point(), v2->point(), v3->point()) / float(2);
		f->area() = std::max<float>( CGAL::sqrt(f->normal() * f->normal()), 1e-7 );
		f->normal() = f->normal() / f->area();
		f->index()=count;
        f->color()=CGAL::white();


		p = Point ((v1->point().x() + v2->point().x() + v3->point().x() ) /3.f ,
				(v1->point().y() + v2->point().y() + v3->point().y() ) /3.f ,
				(v1->point().z() + v2->point().z() + v3->point().z() ) /3.f );
		f->center() = p;
	}
	std::cout << "done\n";

	count  = 0;
	//set index halfedge
	for (Halfedge_iterator h = mesh->halfedges_begin() ; h != mesh->halfedges_end() ; ++h, count++)
		h->index() = count;

	//get 1 ring facet
	list_neighborg_facet = vector<vector<Facet_handle> >(mesh->size_of_facets());
	{

		std::cout << "computing 1ring facet...";

		for (unsigned int i = 0;i<  mesh->size_of_facets();i++)
			//for (Facet_iterator f = mesh->facets_begin() ; f != mesh->facets_end() ; ++f)
		{
			Facet_iterator f = index_to_facet[i];
			//get neigborg
			Halfedge_handle
			h1 = f->halfedge(),
			h2 = h1->next(),
			h3 = h2->next();

			//calcul des normals
			vector<Polyhedron_3::Vertex_handle> vert(3);
			vert[0] = h1->vertex(); vert[1] = h2->vertex(); vert[2] = h3->vertex();

			vector<int> index_facet_neighborg;


			set<int> index_neighborg_facet;

			Halfedge_around_vertex_circulator h0 , h;
			for (unsigned int i=0;i<vert.size();i++)
			{

				h0 = vert[i]->vertex_begin(); h = h0;
				do {
					Facet_handle f1 = h->facet();
					Facet_handle f2 = h->opposite()->facet();
					if(!h->is_border_edge() && f != f1 && f != f2)
					{
						index_neighborg_facet.insert(f1->index());
						index_neighborg_facet.insert(f2->index());
					}
				} while (++h != h0);
			}

			if 	(!h1->is_border_edge())
				list_neighborg_facet[i].push_back(h1->opposite()->facet());

			if 	(!h2->is_border_edge())
				list_neighborg_facet[i].push_back(h2->opposite()->facet());

			if 	(!h3->is_border_edge())
				list_neighborg_facet[i].push_back(h3->opposite()->facet());


			//then the remaining
			for (set<int>::iterator it = index_neighborg_facet.begin();	it != index_neighborg_facet.end();it++)
			{
				list_neighborg_facet[i].push_back(index_to_facet[(*it)]);
			}
		}

		std::cout << "done\n";
	}

	std::cout<<"init done\n";
	return true;
}

void
Cloud_segmentation::add_primitive(
		const std::list<Point_2d>& list_2d,
		const std::vector < int >& ind_vertex_classe_true,
		Plane_3& optimal_plane,
		const std::vector < Point >& point_for_least_squares,
		const std::vector < float> & list_area,
		double area
){

	list_bbox_2d.push_back(CGAL::bounding_box(list_2d.begin(), list_2d.end()));

	//list area
	list_areas.push_back(area);

	//index facet
	list_clusters.push_back(ind_vertex_classe_true);

	//plane reorientation
	Facet_iterator f = index_to_facet[ind_vertex_classe_true[0]];
	Vector normal_vec(f->normal().x(),f->normal().y(),f->normal().z());
	if(normal_vec*optimal_plane.orthogonal_vector()<0) optimal_plane=optimal_plane.opposite();
	extracted_planes.push_back(optimal_plane);

	//centroid cluster
	Point centroid(0.,0.,0.);
	FT area_cumul=0;
	for (unsigned int kk=0;kk< point_for_least_squares.size();kk++){
		area_cumul+=list_area[kk];
		centroid=Point(centroid.x()+point_for_least_squares[kk].x(),
				centroid.y()+point_for_least_squares[kk].y(),
				centroid.z()+point_for_least_squares[kk].z());
	}
	list_centroid.push_back(Point(centroid.x()/point_for_least_squares.size(),centroid.y()/point_for_least_squares.size(),centroid.z()/point_for_least_squares.size()));
	//list_clusters.push_back(Cluster(ind_vertex_classe, normal, sum_std/area, sum_meanCurv/area, area, optimal_plane, quality));

	//color list of clusters
	int red=(int)floor((double)126*rand()/RAND_MAX)+130;
	int green=(int)floor((double)126*rand()/ RAND_MAX)+130;
	int blue=(int)floor((double)126*rand()/ RAND_MAX)+130;
	CGAL::Color col(std::max(0,std::min(255,red)),std::max(0,std::min(255,green)),std::max(0,std::min(255,blue)),120);
	list_color.push_back(col);

	// color mesh AND add all pointers to this primitive
	std::set<Polyhedron_vertex_handle> list_points_to_add;
	std::set<Facet_handle> list_facets_to_add;

	for(unsigned int kk=0;kk<ind_vertex_classe_true.size();kk++){
		Facet_iterator f = index_to_facet[ind_vertex_classe_true[kk]];

		list_facets_to_add.insert(f);
		auto edge_it = f->facet_begin();
		Polyhedron_vertex_handle v1 = edge_it->vertex(),
				v2 = (++edge_it)->vertex(),
				v3 = (++edge_it)->vertex();

		v1->color() = col;
		v2->color() = col;
		v3->color() = col;

		list_points_to_add.insert(v1);
		list_points_to_add.insert(v2);
		list_points_to_add.insert(v3);
	}


	//	for( auto fi = mesh->facets_begin(); fi != mesh->facets_end(); ++fi){
	//		auto edge_it = fi->facet_begin();
	//		auto v1 = edge_it->vertex(),
	//				v2 = (++edge_it)->vertex(),
	//				v3 = (++edge_it)->vertex();
	//		UDBGVALUE(fi->color());
	//	}


	list_points.push_back(list_points_to_add);
	list_facets.push_back(list_facets_to_add);
}

void
Cloud_segmentation::plane_extraction_for_mesh(double max_dist, float area_min,float angle_tolerance
)
{
	vector<int> label_region(mesh->size_of_facets(),-1);
	float area;
	Vector normal;
	int count;

	cerr<<"1. start planar clustering...";
	for (Facet_iterator f = mesh->facets_begin() ; f != mesh->facets_end() ; ++f)
	{
		if (label_region[f->index()]==-1)
		{
			label_region[f->index()]=list_clusters.size();
			//	f->id_cluster() = list_clusters.size();
			std::vector < int > ind_vertex_classe_true; ind_vertex_classe_true.push_back(f->index());
			std::vector < Facet_handle > ind_vertex_classe; ind_vertex_classe.push_back(f);
			std::vector < Point > point_for_least_squares;  point_for_least_squares.push_back(f->center());
			std::vector < float> list_area;	list_area.push_back(f->area());
			std::vector < int > ind_vertex_dernierement_ajoutes;
			ind_vertex_dernierement_ajoutes.push_back(f->index());
			area=f->area();
			normal = f->normal();
			Plane_3  optimal_plane(f->center(),f->normal());
			int nb_update_normal = 0;
			//			float max_distance;
			bool propagation=true;
			do{
				std::vector < int > ind_vertex_a_ajoutes;
				propagation=false;
				for(unsigned int k=0;k<ind_vertex_dernierement_ajoutes.size();k++)
				{
					int indice_pt_cur=ind_vertex_dernierement_ajoutes[k];
					count = 0;
					for(vector<Facet_handle>::iterator it = list_neighborg_facet[indice_pt_cur].begin(); it != list_neighborg_facet[indice_pt_cur].end() && count < 3;it++, count++)
					{
						int indice_pt_voisin=(*it)->index();
						if(label_region[indice_pt_voisin]==-1)
						{


							double dist_2_optimal_plane =  CGAL::squared_distance((*it)->center(),optimal_plane);

							//							volume_difference =  CGAL::squared_distance((*it)->center(),optimal_plane);

							Halfedge_handle h1 = f->halfedge(),h2 = h1->next(), h3 = h2->next();

							double squared_d1 = CGAL::squared_distance(h1->vertex()->point(),optimal_plane);
							double squared_d2 = CGAL::squared_distance(h2->vertex()->point(),optimal_plane);
							double squared_d3 = CGAL::squared_distance(h3->vertex()->point(),optimal_plane);

							dist_2_optimal_plane = max(dist_2_optimal_plane,max(squared_d1,max(squared_d2,squared_d3)));




							// l2 distance with Pierre formula
							//							float volume_difference = ( squared_d1+squared_d2+squared_d3 +sqrt(squared_d1*squared_d2)+sqrt(squared_d2*squared_d3)+sqrt(squared_d3*squared_d1) )/6 * f->area();


							//							double distance_proj = std::sqrt(std::max(std::max(squared_d1,squared_d2),squared_d3));
							//							double max_distance = volume_tolerance;

							//xxx test de propagation
							if(
									(dist_2_optimal_plane < max_dist*max_dist)
									&&	abs((optimal_plane.orthogonal_vector())*(*it)->normal()) > 1-angle_tolerance

									//								&& distance_proj < max_distance
									//&&	max(float((index_to_facet[indice_pt_cur]->normal_Vector())*(*it)->normal_Vector()),0.f) > 1-angle_tolerance
							)
							{
								//PRINT(volume_difference);
								// update the plane
								point_for_least_squares.push_back((*it)->center());
								if (point_for_least_squares.size() > 3 )
									CGAL::linear_least_squares_fitting_3 ( point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());



								normal 	= normal + (*it)->normal();

								label_region[indice_pt_voisin]=list_clusters.size();
								propagation=true;
								ind_vertex_classe.push_back(*it);

								list_area.push_back((*it)->area());

								ind_vertex_a_ajoutes.push_back(indice_pt_voisin);
								//	(*it)->id_cluster() = list_clusters.size();
								area+=(*it)->area();
							}
						}


						//if (area > (nb_update_normal+1)*min_size_cluster_m2 && point_for_least_squares.size() > 3 )
						if (area > (nb_update_normal+1)*area_min && point_for_least_squares.size() > 3 )
						{
							//update normal
							CGAL::linear_least_squares_fitting_3 ( point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());
							nb_update_normal++;
						}

					}
				}
				for(unsigned int fgh=0;fgh<ind_vertex_dernierement_ajoutes.size();fgh++) ind_vertex_classe_true.push_back(ind_vertex_dernierement_ajoutes[fgh]);
				ind_vertex_dernierement_ajoutes =  ind_vertex_a_ajoutes;


			}while(propagation);

			if(area >=area_min && point_for_least_squares.size()>3)
				//if(area >=area_min && sum_std/ind_vertex_classe.size() < min_std_cluster_mess)
			{
				//TO DO: distance a caluler sur les vertex et pas les centroids des facets
				normal =normal/ind_vertex_classe.size();  //1/N sum x
				//make sure normal has positive Z
				if (normal.z() < 0) normal = normal.operator- () ;
				float quality = 0;


				quality = CGAL::linear_least_squares_fitting_3 ( point_for_least_squares.begin(), point_for_least_squares.end(),optimal_plane,CGAL::Dimension_tag<0>());
				quality = 0;
				//#pragma omp parallel for reduction(+:quality)
				for (unsigned int kk=0;kk< point_for_least_squares.size();kk++)
					quality += sqrt(CGAL::squared_distance(point_for_least_squares[kk],optimal_plane));
				//quality/=area;
				quality = quality/point_for_least_squares.size();

				// HERE FLO
				//faire une bbox en 2D
				std::list<Point_2d> list_2d;

				Vector vortho=optimal_plane.orthogonal_vector();
				Vector b1=optimal_plane.base1();
				Vector b2=optimal_plane.base2();
				FT norme1=sqrt(pow(b1.x(),2)+pow(b1.y(),2)+pow(b1.z(),2)); if(norme1<1e-7){norme1=1e-7;}
				FT norme2=sqrt(pow(b2.x(),2)+pow(b2.y(),2)+pow(b2.z(),2)); if(norme2<1e-7){norme2=1e-7;}
				Vector vb1=(1/norme1)*b1;
				Vector vb2=(1/norme2)*b2;

				for (unsigned int kk=0;kk< point_for_least_squares.size();kk++){

					Point ptp=optimal_plane.projection(point_for_least_squares[kk]);
					FT X1=vb1.x()*ptp.x()+vb1.y()*ptp.y()+vb1.z()*ptp.z();
					FT X2=vb2.x()*ptp.x()+vb2.y()*ptp.y()+vb2.z()*ptp.z();
					Point_2d ptp2(X1,X2);
					list_2d.push_back(ptp2);
				}

				add_primitive(list_2d,ind_vertex_classe_true,optimal_plane,point_for_least_squares,list_area,area);
			}
			else //nettoyage
			{
				for(vector<Facet_handle>::iterator it = ind_vertex_classe.begin(); it != ind_vertex_classe.end();it++)
				{
					label_region[(*it)->index()]   = -1;
					//(*it)->id_cluster() = -1;
				}
				ind_vertex_classe_true.clear();
			}
		}
	}
	cout<<list_clusters.size()<<" planar clusters found. done"<<endl;

//	detection_regularities_new(0.01,0.05);
}




bool
Cloud_segmentation::detection_regularities_new(double epsilon,double tolerance_coplanarity){

	std::vector < std::vector < bool > > table_parallel;
	std::vector < std::vector < int > > list_parallel_planes;
	std::vector < Vector > list_cluster_normales;
	std::vector < double > list_cluster_cosangle_vertical;
	std::vector < double > list_cluster_area;
	std::vector < std::vector < bool > > group_planes_orthogonal;


	// find pairs of epsilon-parallel primitives and store them in table_parallel
	compute_parralel_primitives(epsilon,table_parallel);

	// clustering the parallel primitives and store them in list_parallel_planes
	// & compute the normal, size and cos angle to the vertical of each cluster, and store that in list_cluster_normales, list_cluster_angle and list_cluster_area
	cluster_parralel_primitives(
			table_parallel,
			epsilon,
			list_parallel_planes,
			list_cluster_normales,
			list_cluster_cosangle_vertical,
			list_cluster_area
	);

	//discovery orthogonal relationship between clusters
	compute_orthogonal_relationship(
			list_parallel_planes,
			list_cluster_normales,
			epsilon,
			group_planes_orthogonal
	);

	//clustering the vertical cosangle and store their centroids in cosangle_centroids and the centroid index of each cluster in list_cluster_index
	std::vector < double > cosangle_centroids;
	std::vector < int > list_cluster_index;
	cluster_cosangle(
			list_cluster_area,
			group_planes_orthogonal,
			epsilon,
			list_cluster_cosangle_vertical
	);

	std::vector < std::vector < int > > subgraph_clusters;
	std::vector < int > subgraph_clusters_max_area_index;

	//find subgraphs of mutually orthogonal clusters (store index of clusters in subgraph_clusters), and select the cluster of largest area
	find_mutually_orthogonal_clusters(
			list_parallel_planes,
			group_planes_orthogonal,
			list_cluster_area,
			subgraph_clusters,
			subgraph_clusters_max_area_index
	);


	//create subgraphs of mutually orthogonal clusters in which the largest cluster is excluded and store in subgraph_clusters_prop
	std::vector < std::vector < int > > subgraph_clusters_prop;
	for(unsigned int i=0;i<subgraph_clusters.size(); i++){
		int index=subgraph_clusters_max_area_index[i];
		std::vector < int > subgraph_clusters_prop_temp;
		for(unsigned int j=0;j<subgraph_clusters[i].size(); j++){
			if(subgraph_clusters[i][j]!=index) subgraph_clusters_prop_temp.push_back(subgraph_clusters[i][j]);
		}
		subgraph_clusters_prop.push_back(subgraph_clusters_prop_temp);
	}



	//	//display console
	//	for(unsigned int i=0;i<subgraph_clusters_prop.size(); i++){
	////		cout<<endl<<endl<<"subgraph "<<i<<" ("<<subgraph_clusters_max_area_index[i]<<" max area): ";
	//		for(unsigned int j=0;j<subgraph_clusters_prop[i].size(); j++) cout<<subgraph_clusters_prop[i][j]<<"  ";
	//	}



	//regularization of cluster normals : in eachsubgraph, we start from the largest area cluster
	//and we propage over the subgraph by regularizing the normals of the clusters accorting
	// to orthogonality and cosangle to vertical
	cluster_normals_regularization(
			list_cluster_cosangle_vertical,
			subgraph_clusters_prop,
			subgraph_clusters_max_area_index,
			group_planes_orthogonal,
			list_cluster_normales
	);

	//	//recompute optimal plane for each primitive after normal regularization
	//	for(unsigned int i=0; i<list_cluster_normales.size();i++){
	//
	//		Vector vec_reg=list_cluster_normales[i];
	//
	//		for(unsigned int j=0; j<list_parallel_planes[i].size();j++){
	//
	//
	//
	//			int index_prim=list_parallel_planes[i][j];
	//			Point pt_reg=extracted_planes[index_prim].projection(list_centroid[index_prim]);
	//			if( extracted_planes[index_prim].orthogonal_vector() * vec_reg < 0) vec_reg=-vec_reg;
	//			Plane_3 plane_reg(pt_reg,vec_reg);
	//			if( abs(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()) > 1. - epsilon)
	//				extracted_planes[index_prim]=plane_reg;
	//
	//
	////			int index_prim=list_parallel_planes[i][j];
	////			Point pt_reg = list_centroid[index_prim];
	////			Plane_3 plane_reg(pt_reg,vec_reg);
	////
	////			extracted_planes[index_prim]=plane_reg;
	////
	////			if( abs(extracted_planes[index_prim].orthogonal_vector()*plane_reg.orthogonal_vector()) > 1. - epsilon)
	////				extracted_planes[index_prim]=plane_reg;
	//		}
	//	}

	std::vector< std::vector< std::vector < int > > > list_coplanar_prim;
	detect_coplanarity(
			list_cluster_normales,
			list_parallel_planes,
			tolerance_coplanarity,
			list_coplanar_prim
	);

	regularize_primitive_position(
			list_coplanar_prim,
			epsilon//,
			//			list_primitive_reg_index_extracted_planes,
			//		list_primitive_reg
	);
	cout<<endl<<endl<<"NB planes final = "<<list_primitive_reg.size()<<endl<<endl;
	return true;
}





// ugly unefficient stuff -> it should be done in the detection of planes
std::set<int>
Cloud_segmentation::purify_similar_planes(set<int>& planes,Point& pt){
	struct Comparator_planes {
		Cloud_segmentation* segmentation;
		Point& pt;
		Comparator_planes(Cloud_segmentation * segmentation,Point& pt_ ):segmentation(segmentation),pt(pt_){}
		bool operator()(const int&lhs,const int& rhs){
			return lhs<rhs && !Cloud_segmentation::is_similar(segmentation->extracted_planes[lhs],segmentation->extracted_planes[rhs],pt);
		}
	};
	Comparator_planes comparator(this,pt);
	std::set<int,Comparator_planes> planes2(comparator);

	for(auto x =  planes.cbegin();x!=planes.cend();++x)
		planes2.insert(*x);

	set<int> res ;
	for(auto x =  planes2.cbegin();x!=planes2.cend();++x)
		res.insert(*x);
	return res;
}

std::set<int>
Cloud_segmentation::get_adjacent_planes(){
	set<int> aaa;
	return aaa;
	/*
		Polyhedron_3::Vertex_iterator v0,const Polyhedron_3::Point & p0,
		Polyhedron_3::Vertex_iterator v1,const Polyhedron_3::Point & p1
		){
			// compute the union of the two set of planes
			set<int> adjacent_planes_v0v1=v0->adjacent_planes();
			for (auto x = v1->adjacent_planes().begin();x!=v1->adjacent_planes().end();x++)
				adjacent_planes_v0v1.insert(*x);
			auto PT_res= midpoint(p0,p1);
			Point a_pt(PT_res.x(),PT_res.y(),PT_res.z());
			adjacent_planes_v0v1 = purify_similar_planes(adjacent_planes_v0v1,a_pt);
			return adjacent_planes_v0v1;
	 */
}

std::string
Cloud_segmentation::adjacent_planes_to_string(Polyhedron_3::Vertex_iterator v){
	std::stringstream  stream;
	stream <<"adjacent planes of "<<v->index()<<": ";
	for (auto x = v->primitives().begin();x!=v->primitives().end();++x)
		stream <<*x<<",";
	return stream.str();
}




void
Cloud_segmentation::save_clusters_to_file(
		const std::string & name_file,
		const std::vector<int>& matching_polyhedron
){
	// save the proxies
	std::ofstream myfile;

	if(name_file.find(".seg") != std::string::npos)
		myfile.open (name_file);
	else
		myfile.open (name_file+".seg");

	int nb_proxies_detected = num_proxies();
	myfile<<"% "<<nb_proxies_detected<<" proxies"<<std::endl;

	//	std::vector<int> matching_polyhedron;
	//	compute_matching(enriched_mesh.mesh,*mesh,matching_polyhedron);
	//	enriched_mesh.compute_matching(*mesh,matching_polyhedron);

	// for all clusters, we add the corresponding line in the file
	// "vertices: 1 0 10 22 plane:  0.5 0.2 0.1 2"# for vertices of the proxy 1,0,10,22 with plane 0.5x+0.2y+0.1z+2 = 0
	for( int current_proxy  = 0 ; current_proxy < nb_proxies_detected ; ++current_proxy){

		auto plane = extracted_planes[current_proxy];
		std::set<Polyhedron_vertex_handle> points;
		points.insert(list_points[current_proxy].begin(),list_points[current_proxy].end());

		list<Vertex_handle> points_indices;

		myfile<<"#plane: "<< plane.a()<<" "<< plane.b()<<" "<< plane.c()<<" "<< plane.d();

		myfile<<" #area:" << list_areas[current_proxy] ;

		myfile<<" #vertices: ";
		for(auto x:points){
			points_indices.push_back(Vertex_handle(matching_polyhedron[x->index()]));
		}

		for(auto x:points_indices){
			myfile<<x<<" ";
		}

		myfile<<std::endl;

	}
	myfile.close();
}

