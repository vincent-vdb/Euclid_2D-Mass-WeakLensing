/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file tests/src/Boundaries_test.cpp
 * @date 05/23/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

using namespace TWOD_MASS_WL_MassMapping;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Boundaries_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( Overall_test ) {

  double raMin(1.3);
  double raMax(10.);
  double decMin(-20.2);
  double decMax(42.9);
  double zMin(0.5);
  double zMax(1.2);
  Boundaries bounds(raMin, raMax, decMin, decMax, zMin, zMax);

  BOOST_CHECK_CLOSE(bounds.getRaMin(), raMin, 0.1);
  BOOST_CHECK_CLOSE(bounds.getRaMax(), raMax, 0.1);
  BOOST_CHECK_CLOSE(bounds.getDecMin(), decMin, 0.1);
  BOOST_CHECK_CLOSE(bounds.getDecMax(), decMax, 0.1);
  BOOST_CHECK_CLOSE(bounds.getZMin(), zMin, 0.1);
  BOOST_CHECK_CLOSE(bounds.getZMax(), zMax, 0.1);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
