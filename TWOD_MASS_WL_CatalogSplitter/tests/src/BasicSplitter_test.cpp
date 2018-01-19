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
 * @file tests/src/BasicSplitter_test.cpp
 * @date 07/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_CatalogSplitter/BasicSplitter.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_CatalogSplitter;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BasicSplitter_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( badInputCatalog_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(0, 90, 0, 90, 0, 10);

  // Check with a non existing file it will return false
  BasicSplitter mySplitter("fakeCatalog.fits", pathFiles+"tmp/dummyOutput.fits", 2, 2, 2, boundaries);
  BOOST_CHECK(mySplitter.splitCatalog() == false);

  // Check with a catalog with missing redshift it will return false
  std::string inputCatalog = pathFiles+"FITScatalog_ok_noz.fits";
  BasicSplitter mySplitter2(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 2, 2, boundaries);
  BOOST_CHECK(mySplitter2.splitCatalog() == false);

}

BOOST_AUTO_TEST_CASE( goodInputCatalog_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(0, 90, 0, 90, 0, 10);

  std::string inputCatalog = pathFiles+"FITScatalog_noK_withZ.fits";
  BasicSplitter mySplitter(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 2, 2, boundaries);
  BOOST_CHECK(mySplitter.splitCatalog() == true);
}

BOOST_AUTO_TEST_CASE( badInputSplitParams_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(0, 90, 0, 90, 0, 10);

  std::string inputCatalog = pathFiles+"FITScatalog_noK_withZ.fits";
  BasicSplitter mySplitter1(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 0, 2, 2, boundaries);
  BOOST_CHECK(mySplitter1.splitCatalog() == false);

  BasicSplitter mySplitter2(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 0, 2, boundaries);
  BOOST_CHECK(mySplitter2.splitCatalog() == false);

  BasicSplitter mySplitter3(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 2, 0, boundaries);
  BOOST_CHECK(mySplitter3.splitCatalog() == false);
}

BOOST_AUTO_TEST_CASE( SplitWithMask_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(0, 90, 0, 90, 0, 10);

  std::string inputCatalog = pathFiles+"FITScatalog_noK_withZ.fits";
  BasicSplitter mySplitter(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 2, 2, boundaries);
  BOOST_CHECK(mySplitter.splitCatalogWithMask() == true);
}

BOOST_AUTO_TEST_CASE( getCenterOfPatches_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(0, 90, 0, 90, 0, 10);

  std::string inputCatalog = pathFiles+"FITScatalog_noK_withZ.fits";
  BasicSplitter mySplitter(inputCatalog, pathFiles+"tmp/dummyOutput.fits", 2, 2, 2, boundaries);

  std::vector<std::pair<float, float> > centers = mySplitter.getCenterOfPatches();

  BOOST_CHECK_EQUAL(centers.size(), 408);
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
