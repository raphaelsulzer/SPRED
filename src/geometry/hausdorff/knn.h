// -*- compile-command: "cd ../build && make -j 8 && cd -"; -*-

/*
  USAGE

  typedef CNeighbor_search<Kernel> Neighbor_search;
  Neighbor_search search;

  std::list<Point> points;
  search.init(points.begin(),points.end());

  // query
  Point p = nearest_point(query);
*/

#ifndef _NEIGHBOR_SEARCH_
#define _NEIGHBOR_SEARCH_

#define PI 3.141592653589793238462643
#define SQRT_3 1.73205081

#include <omp.h>

//using namespace CGAL;

#include <CGAL/basic.h>
//#include <CGAL/eigen.h>

#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Orthogonal_incremental_neighbor_search.h>

#include <CGAL/Search_traits_3.h>
#include <vector>
#include <list>
#include <cmath>

template <class Kernel>
class CNeighbor_search
{
public:
  typedef typename Kernel::FT FT;
  typedef typename Kernel::Point_3 Point_3;
  typedef typename Kernel::Plane_3 Plane;
  typedef typename Kernel::Vector_3 Vector;
  typedef typename Kernel::Line_3 Line;
  typedef typename CGAL::Search_traits_3<Kernel> TreeTraits;
  typedef typename CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
  //  typedef typename CGAL::Orthogonal_incremental_neighbor_search<TreeTraits> Neighbor_search;
  typedef typename Neighbor_search::Tree Tree;
  typedef typename Neighbor_search::iterator Iterator;

private:
  Tree *m_tree;

public:
  CNeighbor_search() { m_tree = NULL; }

  ~CNeighbor_search()
  {
    if (m_tree != NULL)
      delete m_tree;
  }

  template <class InputIterator>
  void init(InputIterator begin,
	    InputIterator end)
  {
    if (m_tree != NULL)
      delete m_tree;
    m_tree = new Tree (begin, end);
  }

  FT sq_distance_nearest_point(const Point_3& query) const
  {
    FT out;
    Neighbor_search search(*m_tree,query,1, 0.5);
    out = search.begin ()->second;
    return out;
  }

  FT av_sqrt_sum_sq_distance_to_knn(const Point_3& query,
				    const unsigned int k) const
  {
    unsigned int nb = 0;
    Neighbor_search search(*m_tree,query,k); 
    Iterator it = search.begin();
    FT sum_sq_distances = 0.0;
    for(unsigned int i=0;i<k;i++,it++)
      {
	if(it == search.end())
	  break;
	Point_3 p = it->first;
	sum_sq_distances += CGAL::squared_distance(query,p);
	nb++;
      }
    return std::sqrt(sum_sq_distances / nb);
  }

  FT knn_function_skip_first (const Point_3& query,
			      const unsigned int k) const
  {
    unsigned int nb = 0;
    Neighbor_search search(*m_tree,query,k+1); 
    Iterator it = search.begin();
    ++ it;
    FT sum_sq_distances = 0.0;
    for(unsigned int i=0;i<k;i++,it++)
      {
	if(it == search.end())
	  break;
	Point_3 p = it->first;
	sum_sq_distances += CGAL::squared_distance(query,p);
	nb++;
      }
    return std::sqrt(sum_sq_distances / nb);
  }

  Point_3 second_nearest_point(const Point_3& query,
			       FT& distance) const
  {
    Neighbor_search search (*m_tree, query, 2);
    Iterator it = search.begin ();
    ++ it;
    distance = std::sqrt (it->second);
    return it->first;
  }

  Point_3 nearest_point(const Point_3& query) const
  {
    Neighbor_search search(*m_tree,query,1, 0.5);
    return search.begin()->first;
  }

};

#endif // _NEIGHBOR_SEARCH_
