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
 * @file tests/src/MaskSplitter_test.cpp
 * @date 10/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_CatalogSplitter/MaskSplitter.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_CatalogSplitter;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MaskSplitter_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( basicMaskCatalogNoZ_test ) {

  // Define the input catalog
  std::string inputCatalog = pathFiles+"FITScatalog_ok_noz.fits";

  // Define the steps for ra, dec and z
  float raStep = 10.;
  float decStep = 5.;
  float zStep = 1.;

  // Create the MaskSplitter object
  MaskSplitter mySplitter(inputCatalog, raStep, decStep, zStep);

  // Get the mask from the catalog
  std::vector<std::pair<float, float> > myMask = mySplitter.fillBasicMask();

  unsigned int expectedMaskSize = 180./decStep;

  // Check the mask size is the expected one
  BOOST_CHECK(myMask.size()==expectedMaskSize);

  std::vector<TWOD_MASS_WL_MassMapping::Boundaries> myBoundaries = mySplitter.getBoundariesFromBasicMask(myMask);

  // Check the method to get boundaries return a non empty vector
  BOOST_CHECK(myBoundaries.size()!=0);
}

BOOST_AUTO_TEST_CASE( basicMaskCatalogwithZ_test ) {

  // Define the input catalog
  std::string inputCatalog = pathFiles+"FITScatalog_noK_withZ.fits";

  // Define the steps for ra, dec and z
  float raStep = 10.;
  float decStep = 5.;
  float zStep = 1.;

  // Create the MaskSplitter object
  MaskSplitter mySplitter(inputCatalog, raStep, decStep, zStep);

  // Get the mask from the catalog
  std::vector<std::pair<float, float> > myMask = mySplitter.fillBasicMask();

  unsigned int expectedMaskSize = 180./decStep;

  // Check the mask size is the expected one
  BOOST_CHECK(myMask.size()==expectedMaskSize);

  std::vector<TWOD_MASS_WL_MassMapping::Boundaries> myBoundaries = mySplitter.getBoundariesFromBasicMask(myMask);

  // Check the method to get boundaries return a non empty vector
  BOOST_CHECK(myBoundaries.size()!=0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
