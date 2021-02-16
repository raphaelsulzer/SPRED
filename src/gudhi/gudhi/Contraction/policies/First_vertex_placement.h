 /*    This file is part of the Gudhi Library. The Gudhi library 
  *    (Geometric Understanding in Higher Dimensions) is a generic C++ 
  *    library for computational topology.
  *
  *    Author(s):       David Salinas
  *
  *    Copyright (C) 2014  INRIA Sophia Antipolis-Mediterranee (France)
  *
  *    This program is free software: you can redistribute it and/or modify
  *    it under the terms of the GNU General Public License as published by
  *    the Free Software Foundation, either version 3 of the License, or
  *    (at your option) any later version.
  *
  *    This program is distributed in the hope that it will be useful,
  *    but WITHOUT ANY WARRANTY; without even the implied warranty of
  *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *    GNU General Public License for more details.
  *
  *    You should have received a copy of the GNU General Public License
  *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef GUDHI_FIRST_VERTEX_PLACEMENT_H_
#define GUDHI_FIRST_VERTEX_PLACEMENT_H_

#include "Placement_policy.h"

namespace Gudhi{

namespace contraction {


/**
 * @brief Places the contracted point onto the first point of the edge
 */
template< typename EdgeProfile> class First_vertex_placement : public Placement_policy<EdgeProfile>{

public:
	typedef typename EdgeProfile::Point Point;
	typedef typename EdgeProfile::Edge_handle Edge_handle;

	typedef typename Placement_policy<EdgeProfile>::Placement_type Placement_type;

	Placement_type operator()(const EdgeProfile& profile) const override{
		return Placement_type(profile.p0());
	}
};
}  // namespace contraction

}  // namespace GUDHI


#endif /* GUDHI_FIRST_VERTEX_PLACEMENT_H_ */
