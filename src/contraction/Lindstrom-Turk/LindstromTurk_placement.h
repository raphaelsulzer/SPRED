// Copyright (c) 2006  GeometryFactory (France). All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
// Author(s)     : Fernando Cacciola <fernando.cacciola@geometryfactory.com>
//
#ifndef LINDSTROMTURK_PLACEMENT_H
#define LINDSTROMTURK_PLACEMENT_H 1

//#include "Collapse/Surface_mesh_simplification/Detail/Common.h"

#include <boost/optional.hpp>

#include "geometry/contraction/Edge_profile.h"
#include "Lindstrom_Turk_core.h"

namespace LT {

template<class ECM_>
class LindstromTurk_placement
{
public:

	typedef ECM_ ECM ;

	typedef contraction::Edge_profile<ECM> Profile ;

	typedef ECM::Point Point;

	typedef boost::optional<Point> result_type ;

public:

	LindstromTurk_placement( LindstromTurk_params const& aParams = LindstromTurk_params() ) : mParams(aParams) {}

	result_type operator()( Profile const& aProfile) const
	{
		return LindstromTurkCore<ECM>(mParams,aProfile).compute_placement() ;
	}

private:

	LindstromTurk_params mParams ;

};

}  // namespace LT

#endif //LINDSTROMTURK_PLACEMENT_HH //
// EOF //

