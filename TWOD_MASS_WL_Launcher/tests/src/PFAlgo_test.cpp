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
 * @file tests/src/PFAlgo_test.cpp
 * @date 06/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_Launcher/PFAlgo.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_Launcher;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PFAlgo_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( checkParameters_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(40, 50, 0, 10, 0, 10);

  // Case no input catalog
  PFAlgo myAlgo1(true, 2, 1, 5, true, 1.0, 1., "", "", "/tmp", "/tmp", 10, 10, 10, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo1.checkParameters(), false);

  // Case two input catalogs
  PFAlgo myAlgo2(true, 2, 1, 5, true, 1.0, 1., "/tmp", "/tmp", "/tmp", "/tmp", 10, 10, 10, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo2.checkParameters(), false);

  // Case no output
  PFAlgo myAlgo3(true, 2, 1, 5, true, 1.0, 1., "/tmp", "", "", "", 10, 10, 10, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo3.checkParameters(), false);

  TWOD_MASS_WL_MassMapping::Boundaries badBoundaries(20, 0, 0, 10, 0, 10);
  // Case bad boundaries and needed
  PFAlgo myAlgo4(true, 2, 1, 5, true, 1.0, 1., "/tmp", "", "/tmp", "/tmp", 0, 0, 0, true, badBoundaries);
  BOOST_CHECK_EQUAL(myAlgo4.checkParameters(), false);

  // Case bad boundaries but not needed
  PFAlgo myAlgo5(true, 2, 1, 5, true, 1.0, 1., "/tmp", "", "/tmp", "/tmp", 10, 10, 10, true, badBoundaries);
  BOOST_CHECK_EQUAL(myAlgo5.checkParameters(), true);
}


BOOST_AUTO_TEST_CASE( performProcessingFunction_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries boundaries(40, 50, 0, 10, 0, 10);

  PFAlgo myAlgo(true, 2, 1, 5, true, 1.0, 1., "",
                 pathFiles+"SSVcatalog_ok_withz.txt",
                 pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", 10, 10, 10, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo.checkParameters(), true);

  // Check without output it returns false
  BOOST_CHECK_EQUAL(myAlgo.performProcessingFunction("", "", boundaries), false);

  // Check with bad boundaries it returns false
  TWOD_MASS_WL_MassMapping::Boundaries badBoundaries(20, 0, 0, 10, 0, 10);
  BOOST_CHECK_EQUAL(myAlgo.performProcessingFunction(pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", badBoundaries), false);

  // Check it returns true when all is ok
//  BOOST_CHECK_EQUAL(myAlgo.performProcessingFunction(pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", boundaries), true);
}


BOOST_AUTO_TEST_CASE( launchParalPF_test ) {
  TWOD_MASS_WL_MassMapping::Boundaries badBoundaries(20, 0, 0, 10, 0, 10);

  PFAlgo myAlgo1(true, 2, 1, 5, true, 1.0, 1., "",
                 pathFiles+"SSVcatalog_ok_withz.txt",
                 pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", 0, 0, 0, true, badBoundaries);
  BOOST_CHECK_EQUAL(myAlgo1.checkParameters(), false);

  // Check without steps and bad boundaries it returns false
  BOOST_CHECK_EQUAL(myAlgo1.launchParalPF(), false);



  TWOD_MASS_WL_MassMapping::Boundaries boundaries(40, 50, 0, 10, 0, 10);
  PFAlgo myAlgo2(true, 2, 1, 5, true, 1.0, 1., "",
                 pathFiles+"SSVcatalog_ok_withz.txt",
                 pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", 0, 0, 0, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo2.checkParameters(), true);

  // Check without steps and good boundaries it returns true
//  BOOST_CHECK_EQUAL(myAlgo2.launchParalPF(), true);


  PFAlgo myAlgo3(true, 2, 1, 5, true, 1.0, 1., "",
                 pathFiles+"SSVcatalog_ok_withz.txt",
                 pathFiles+"tmp/peakCatalog.fits", pathFiles+"tmp/convMap.fits", 10, 10, 10, true, boundaries);
  BOOST_CHECK_EQUAL(myAlgo3.checkParameters(), true);

  // Check with steps and good boundaries it returns true
//  BOOST_CHECK_EQUAL(myAlgo3.launchParalPF(), true);
}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
