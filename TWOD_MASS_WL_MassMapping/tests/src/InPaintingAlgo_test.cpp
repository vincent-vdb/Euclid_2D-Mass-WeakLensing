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
 * @file tests/src/InPaintingAlgo_test.cpp
 * @date 07/07/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/InPaintingAlgo.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (InPaintingAlgo_test)

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_CASE( copyConstructor_test ) {

  // Import shear and convergence maps
  ShearMap myShearMap(pathFiles+"shear_holes_test.fits");
  ConvergenceMap myConvMap(pathFiles+"conv_holes_test.fits");

  // Create the InPaintingAlgo object with the maps
  InPaintingAlgo *myInPainting = new InPaintingAlgo(myShearMap, myConvMap);

  // Create an InPaintingAlgo using copy constructor
  InPaintingAlgo *myCopy = new InPaintingAlgo(*myInPainting);

  // Delete the original object
  delete myInPainting;
  myInPainting = nullptr;

  // Then try to perform inpainting (should not work if copy constructor did not work)
  ConvergenceMap* myInPaintedMap = myCopy->performInPaintingAlgo(2, false, false);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;

  delete myCopy;
  myCopy = nullptr;
}

BOOST_AUTO_TEST_CASE( basicInPaintingAlgo_test ) {

  // Import shear and convergence maps
  ShearMap myShearMap(pathFiles+"shear_holes_test.fits");
  ConvergenceMap myConvMap(pathFiles+"conv_holes_test.fits");

  // Create the InPaintingAlgo object with the maps, without other options
  InPaintingAlgo myInPainting(myShearMap, myConvMap);

  // Get the inpainted convergence map without options
  ConvergenceMap* myInPaintedMap = myInPainting.performInPaintingAlgo(2, false, false);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;

  // Create the InPaintingAlgo object with the maps, with scales to zero
  InPaintingAlgo myInPainting2(myShearMap, myConvMap, 0);

  // Get the inpainted convergence map with sigma bounds option
  myInPaintedMap = myInPainting2.performInPaintingAlgo(2, true, false);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;


  // Create the InPaintingAlgo object with the maps, without scales and min threshold to 0.05
  InPaintingAlgo myInPainting3(myShearMap, myConvMap, 0., 0.05);

  // Get the inpainted convergence map with B-modes to zero option
  myInPaintedMap = myInPainting3.performInPaintingAlgo(2, false, true);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;


  // Create the InPaintingAlgo object with the maps, without scales to 5, min and max threshold to 0 and 0.5
  InPaintingAlgo myInPainting4(myShearMap, myConvMap, 5, 0., 0.5);

  // Get the inpainted convergence map with both options
  myInPaintedMap = myInPainting4.performInPaintingAlgo(2, true, true);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;
}


BOOST_AUTO_TEST_CASE( blockSizeInPaintingAlgo_test ) {

  // Import shear and convergence maps
  ShearMap myShearMap(pathFiles+"shear_holes_test.fits");
  ConvergenceMap myConvMap(pathFiles+"conv_holes_test.fits");

  // Create the InPaintingAlgo object with the maps
  InPaintingAlgo myInPainting(myShearMap, myConvMap);

  // Get the inpainted convergence map without options
  ConvergenceMap* myInPaintedMap = myInPainting.performInPaintingAlgo(2, false, false, 512, 512);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;

  // Get the inpainted convergence map with sigma bounds option
  myInPaintedMap = myInPainting.performInPaintingAlgo(2, true, false, 512, 512);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;

  // Get the inpainted convergence map with B-modes to zero option
  myInPaintedMap = myInPainting.performInPaintingAlgo(2, false, true, 512, 512);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;

  // Get the inpainted convergence map with both options
  myInPaintedMap = myInPainting.performInPaintingAlgo(2, true, true, 512, 512);
  // Check a convergence map is well returned
  BOOST_CHECK(myInPaintedMap!=nullptr);

  delete myInPaintedMap;
  myInPaintedMap = nullptr;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


