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
 * @file tests/src/CatalogHandler_test.cpp
 * @date 04/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MapMaker/CatalogHandler.h"

using namespace TWOD_MASS_WL_MapMaker;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogHandler_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( gnomonicProjection_test ) {

  CatalogHandler myCatalog(std::string("dummyCatalog.txt"));

  // Loop over right ascensions
  for (int i=0; i<36; i++)
  {
    // Loop over declinations
    for (int j=-9; j<9; j++)
    {
      double ra0 = 10.*i;
      double dec0 = 10.*j;

      double ra = ra0;
      double dec = dec0;

      std::pair<double, double> myXY = myCatalog.getGnomonicProjection(ra, dec, ra0, dec0);
      std::pair<double, double> myBackRaDec = myCatalog.getInverseGnomonicProjection(myXY.first, myXY.second, ra0, dec0);

      BOOST_CHECK_CLOSE(ra, myBackRaDec.first, 0.01);
      BOOST_CHECK_CLOSE(dec, myBackRaDec.second, 0.01);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
