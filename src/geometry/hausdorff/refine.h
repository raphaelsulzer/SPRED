// -*- compile-command: "cd ../build && make -j 8 && cd -"; -*-

#ifndef __REFINE_H__
#define __REFINE_H__

#include <queue>

#include <omp.h>

#define SQRT3_8 0.612372436
#define SQRT2_6 0.577350269

template <typename Kernel>
class CPointH
{
private:

	typedef typename Kernel::Point_3 Point;
	typedef typename Kernel::FT FT;
	Point m_point;
	FT m_hausdorff;

public:

	CPointH ()
{
		m_point = Point (0., 0., 0.);
		m_hausdorff = 0.;
}
	CPointH (const Point& p, FT h = 0.)
	{
		m_point = p;
		m_hausdorff = h;
	}
	CPointH (const CPointH& p)
	{
		m_point = p ();
		m_hausdorff = p.hausdorff ();
	}

	Point operator() () const { return m_point; }
	Point& operator() () { return m_point; }
	FT hausdorff () const { return m_hausdorff; }

};


template <typename Kernel>
class CRefTriangle
{

private:

	typedef typename Kernel::Point_3 Point;
	typedef typename Kernel::FT FT;
	typedef CPointH<Kernel> PointH;
	PointH m_point[3];
	int m_edge;
	FT m_upper_bound;
	FT m_lower_bound;
	Point m_bisector;

public:

	CRefTriangle (const PointH& a, const PointH& b, const PointH& c) // Triangle
{
		m_point[0] = a;
		m_point[1] = b;
		m_point[2] = c;

		FT longest_edge = 0;
		m_lower_bound = 0.;
		FT hausdorff_min = std::numeric_limits<FT>::max ();

		for (int i = 0; i < 3; ++ i)
		{
			if (m_point[i].hausdorff () > m_lower_bound)
				m_lower_bound = m_point[i].hausdorff ();
			if (m_point[i].hausdorff () < hausdorff_min)
				hausdorff_min = m_point[i].hausdorff ();

			PointH& p0 = m_point[i];
			PointH& p1 = m_point[(i+1)%3];
			FT length = CGAL::squared_distance (p0 (), p1 ());
			if (length > longest_edge)
			{
				m_edge = i;
				longest_edge = length;
			}
		}
		longest_edge = std::sqrt (longest_edge);

		m_bisector = Point ((m_point[m_edge]().x () + m_point[(m_edge+1)%3]().x ()) / 2.,
				(m_point[m_edge]().y () + m_point[(m_edge+1)%3]().y ()) / 2.,
				(m_point[m_edge]().z () + m_point[(m_edge+1)%3]().z ()) / 2.);

		m_upper_bound = std::min (m_lower_bound + longest_edge * SQRT2_6,
				hausdorff_min + longest_edge);

}

	CRefTriangle (const CRefTriangle& t)
	{
		m_point[0] = t.points ()[0];
		m_point[1] = t.points ()[1];
		m_point[2] = t.points ()[2];
		m_edge = t.edge ();
		m_lower_bound = t.lower_bound ();
		m_upper_bound = t.upper_bound ();
		m_bisector = t.bisector ();
	}

	FT lower_bound () const
	{
		return m_lower_bound;
	}

	FT upper_bound () const
	{
		return m_upper_bound;
	}

	const Point& bisector () const
	{
		return m_bisector;
	}

	friend bool operator< (const CRefTriangle& a, const CRefTriangle& b)
	{
		return a.upper_bound () < b.upper_bound ();
	}

	const PointH* points () const { return m_point; }
	const int edge () const { return m_edge; }
};



template <typename Kernel>
class CRefiner_2
{
private:

	typedef typename Kernel::Point_3 Point;
	typedef typename Kernel::Triangle_3 Triangle;
	typedef typename Kernel::FT FT;
	typedef CPointH<Kernel> PointH;
	typedef CRefTriangle<Kernel> RefTriangle;
	typedef CNeighbor_search<Kernel> Knn; // a adapter

	std::priority_queue<RefTriangle> m_queue;
	FT m_lower_bound;
	FT m_upper_bound;

public:

	CRefiner_2 ()
{
		m_lower_bound = 0.;
		m_upper_bound = std::numeric_limits<FT>::max ();
}
	~CRefiner_2 () { }

	bool empty ()
	{
		return m_queue.empty ();
	}
	void reset (FT lower_bound = 0.)
	{
		m_queue = std::priority_queue<RefTriangle> ();
		m_lower_bound = lower_bound;
		m_upper_bound = std::numeric_limits<FT>::max ();
	}

	inline FT uncertainty () const
	{
		return m_upper_bound - m_lower_bound;
	}

	inline FT mid () const
	{
		return ((m_upper_bound + m_lower_bound) / 2.);
	}

	inline FT lower_bound () const
	{
		return m_lower_bound;
	}

	inline FT upper_bound () const
	{
		return m_upper_bound;
	}

	inline FT relative_error () const
	{
		return 0.5 * uncertainty () / mid ();
	}

	void add (Triangle& t)
	{
		PointH p0 (t.vertex (0));
		PointH p1 (t.vertex (1));
		PointH p2 (t.vertex (2));
		m_queue.push (RefTriangle (p0, p1, p2));
	}

	bool add (Triangle& t, FT lower_bound, FT upper_bound = 1e30)
	{
		PointH p0 (t.vertex (0));
		PointH p1 (t.vertex (1));
		PointH p2 (t.vertex (2));
		RefTriangle r (p0, p1, p2);
		if (r.lower_bound () > upper_bound)
		{
			reset ();
			m_lower_bound = r.lower_bound ();
			m_upper_bound = r.upper_bound ();
			return false;
		}
		if (r.upper_bound () > lower_bound)
			m_queue.push (r);
		return true;
	}


	bool add (Triangle& t, const Knn& knn, FT lower_bound = 0., FT upper_bound = 1e30)
	{
		PointH p0 (t.vertex (0), std::sqrt (knn.sq_distance_nearest_point (t.vertex (0))));
		PointH p1 (t.vertex (1), std::sqrt (knn.sq_distance_nearest_point (t.vertex (1))));
		PointH p2 (t.vertex (2), std::sqrt (knn.sq_distance_nearest_point (t.vertex (2))));
		RefTriangle r (p0, p1, p2);

		if (r.lower_bound () > upper_bound)
		{
			m_lower_bound = r.lower_bound ();
			m_upper_bound = r.upper_bound ();
			return false;
		}
		if (r.upper_bound () > lower_bound)
			m_queue.push (r);
		return true;
	}


	// Constructeur conseille
	bool add (const Point& a, const Point& b, const Point& c, // Triangle
			const Knn& knn, // Classe avec fonction sd_distance_nearest_point (Point)
			FT lower_bound = 0., FT upper_bound = 1e30) // Bornes
	{
		RefTriangle r (PointH (a, std::sqrt (knn.sq_distance_nearest_point (a))),
				PointH (b, std::sqrt (knn.sq_distance_nearest_point (b))),
				PointH (c, std::sqrt (knn.sq_distance_nearest_point (c))));

		if (r.lower_bound () > upper_bound)
		{
			m_queue.push (r);
			return false;
		}
		if (r.upper_bound () > lower_bound)
			m_queue.push (r);
		return true;
	}


	// Calcul hausdorff
	FT refine (FT limit, // Erreur limite
			const Knn& knn, // Toujours la classe
			FT upper_bound = 1e30) // borne
	{
		if (m_queue.empty ())
			return 0.;

		while (uncertainty () > limit && !(m_queue.empty ()))
		{
			const RefTriangle& current = m_queue.top ();

			m_upper_bound = current.upper_bound ();

			if (m_lower_bound > upper_bound)
				return mid ();

			Point bisector = current.bisector ();

			FT hausdorff = std::sqrt (knn.sq_distance_nearest_point (bisector));
			if (hausdorff > m_lower_bound)
				m_lower_bound = hausdorff;


			PointH new_point (bisector, hausdorff);
			int i = current.edge ();

			PointH p0 (current.points()[i]);
			PointH p1 (current.points()[(i+1)%3]);
			PointH p2 (current.points()[(i+2)%3]);

			m_queue.pop ();
			m_queue.push (RefTriangle (new_point, p0, p2));
			m_queue.push (RefTriangle (new_point, p1, p2));
		}

		return mid ();
	}


};



#endif
