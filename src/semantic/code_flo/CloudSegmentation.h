#ifndef CLOUD_SEGMENTATION_H
#define CLOUD_SEGMENTATION_H


#ifdef _WIN32
#pragma warning( disable : 4507 4503 4244 4103 4267 4396 4018 4258 4305 4101)
#endif

#include "typedefs.h"
#include "Polyhedron_segmentation.h"

typedef unsigned char byte;


typedef Polyhedron_3::Vertex_handle Polyhedron_vertex_handle;
typedef Polyhedron_3::Vertex_iterator Vertex_iterator;



class Cloud_segmentation
{
	typedef Gudhi_kernel::Point Point;
	typedef Gudhi_kernel::Vector_3 Vector;
	typedef Gudhi_kernel::FT FT;



	struct functor_comp_x{
		std::vector<float> angle_to_x;
		functor_comp_x(std::vector<float> &_v){angle_to_x = _v;}

		bool operator()(const float &a, const float &b){
			return angle_to_x[a] <  angle_to_x[b];}
	};


public:
	typedef Polyhedron_3::Facet_handle Facet_handle;
	typedef Polyhedron_3::Facet_iterator Facet_iterator;
	typedef Polyhedron_3::Vertex_iterator Vertex_iterator;
	typedef Polyhedron_3::Traits::Kernel Kernel;
	typedef Kernel::Iso_rectangle_2 bounding_2;
	typedef Kernel::Iso_cuboid_3 bounding_3;
	typedef Kernel::Point_2 Point_2d;

	std::vector < Plane_3 > extracted_planes;
	std::vector< bounding_2 > list_bbox_2d;
	std::vector<Point> list_centroid;
	std::vector< double > list_areas;
	std::vector<CGAL::Color> list_color;
	std::vector< std::vector <int > > list_clusters;
	std::vector < std::vector < int > > list_primitive_reg_index_extracted_planes;
	std::vector < Plane_3 > list_primitive_reg;

	/**
	 * for every planar proxy 'i', list_points[i] is the list of Polyhedron_vertex_handle of vertices that belong
	 * to this proxy
	 */
	std::vector<std::set<Polyhedron_vertex_handle> > list_points;
	std::vector<std::set<Facet_handle> > list_facets;

	bounding_3 BBox_scan;
	double BBox_diagonal;
	Polyhedron_3* mesh;

	std::vector<std::vector<Facet_handle> > list_neighborg_facet;
	//for random access of facets (this is quite dangerous if mesh changes, to use with cautions)
	std::vector<Facet_iterator> index_to_facet;
	std::vector<Vertex_iterator> list_vertices;

	Cloud_segmentation(Polyhedron_3* pol);

	int num_proxies() const;


private:
	Vector regularization_normales(Vector normale ,double cos_vertical);

	Vector regularization_normales_from_prior(Vector normal_parent, Vector normal,double cos_vertical);

	/**
	 * @brief find pairs of epsilon-parallel primitives and store them in table_parallel
	 *
	 * input epsilon
	 * output table_parallel
	 */
	void compute_parralel_primitives(
			const double &epsilon ,
			std::vector < std::vector < bool > >& table_parallel
	);

	void compute_orthogonal_relationship(
			const std::vector < std::vector < int > > &list_parallel_planes,
			const std::vector < Vector > &list_cluster_normales,
			const double & epsilon,
			std::vector < std::vector < bool > >& group_planes_orthogonal
	);


	void cluster_cosangle(
			const std::vector < double > &list_cluster_area,
			const std::vector < std::vector < bool > >& group_planes_orthogonal,
			const double & epsilon,
			std::vector < double > &list_cluster_cosangle_vertical
	);

	void find_mutually_orthogonal_clusters(
			const std::vector < std::vector < int > > & list_parallel_planes,
			const std::vector < std::vector < bool > >& group_planes_orthogonal,
			const std::vector < double > &list_cluster_area,
			std::vector < std::vector < int > > &subgraph_clusters,
			std::vector < int > &subgraph_clusters_max_area_index
	);

	void cluster_normals_regularization(
			const std::vector < double > & list_cluster_cosangle_vertical,
			const std::vector < std::vector < int > >& subgraph_clusters_prop,
			const std::vector < int > & subgraph_clusters_max_area_index,
			const std::vector < std::vector < bool > >& group_planes_orthogonal,
			std::vector < Vector >  &list_cluster_normales
	);

	void cluster_parralel_primitives(
			const std::vector < std::vector < bool > >& table_parallel,
			const double & 	epsilon,
			std::vector < std::vector < int > > &list_parallel_planes,
			std::vector < Vector > &list_cluster_normales,
			std::vector < double >& list_cluster_cosangle_vertical,
			std::vector < double >& list_cluster_area
	);


	//detecting co-planarity and store in list_coplanar_prim
	void detect_coplanarity(
			const std::vector < Vector > & list_cluster_normales,
			const std::vector < std::vector < int > > &list_parallel_planes,
			const double &tolerance_coplanarity,
			std::vector< std::vector< std::vector < int > > > &list_coplanar_prim
	);


	//regularize primitive position by computing barycenter of coplanar planes
	void regularize_primitive_position(
			const std::vector< std::vector< std::vector < int > > > &list_coplanar_prim,
			const double& epsilon
	);



public:
	/*
	 * matching_polyhedron is the matching between vertices of the polyhedron
	 * and vertices of the complex
	 * (which may be placed differently than for polyhedron since the Off loader
	 * of CGAL remove all isolated vertices)
	 */
	void save_clusters_to_file(
			const std::string & name_file,
			const std::vector<int>& matching_polyhedron
	);


	bool detection_regularities_new(double epsilon,double tolerance_coplanarity);

private:
	std::vector<Point> intersect_Plane_BBox(const bounding_3 &bb, const Plane_3 &p);


public:

	bool vertex_labeling();

	bool update_bbox_2d();

	void clear();

	void compute_bounding_box_and_indices();

	bool init();

	void add_primitive(
			const std::list<Point_2d>& list_2d,
			const std::vector < int >& ind_vertex_classe_true,
			Plane_3& optimal_plane,
			const std::vector < Point >& point_for_least_squares,
			const std::vector < float> & list_area,
			double area
	);

	void plane_extraction_for_mesh(double distance_max_plane, float area_min,float angle_max);

	// faire un container set avec cette relation d'egalité
	static bool is_similar(Plane_3& plane1,Plane_3& plane2,Point & pt){
		double epsilon = .07 ;
		Vector n1 = plane1.orthogonal_vector();
		Vector n2 = plane2.orthogonal_vector();

		if (plane1==plane2) return true;
		if (CGAL::cross_product(n1,n2).squared_length()>epsilon*epsilon) {
			//cerr <<"cross.squared_length():"<<CGAL::cross_product(n1,n2).squared_length()<<endl;
			return false;
		}
		else {
			// the planes are almost parralel

			//xxx bad do anything else
			Point proj=plane2.projection(pt);
			double dist = squared_distance(pt,proj);
			//cerr << "d(proj,pt):"<<dist;
			return dist<epsilon;
		}
	}

	// ugly unefficient stuff -> it should be done in the detection of planes
	std::set<int> purify_similar_planes(std::set<int>& planes,Point& pt);

	std::set<int> get_adjacent_planes();

	std::string adjacent_planes_to_string(Polyhedron_3::Vertex_iterator v);
protected: 


};





#endif 
