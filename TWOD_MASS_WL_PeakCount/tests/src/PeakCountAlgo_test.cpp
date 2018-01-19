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
 * @file tests/src/PeakCountAlgo_test.cpp
 * @date 05/22/17
 * @author user
 */

#include <boost/test/unit_test.hpp>
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include "TWOD_MASS_WL_PeakCount/PeakCountAlgo.h"
#include "TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_PeakCount;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (getSNRImage_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( getSNR_test ) {

  // Create a PeakCountAlgo object
  TWOD_MASS_WL_MassMapping::ConvergenceMap myConvMap(pathFiles+"convMap.fits");
  TWOD_MASS_WL_MassMapping::GlobalMap myGlobalMap(pathFiles+"densMap.fits");

  PeakCountAlgo myAlgo(myConvMap, myGlobalMap);

  unsigned int imSize(1024);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = 2.; //double(i-j);
    }
  }

  // Create an image initialized with values
  TWOD_MASS_WL_MassMapping::Image myImage(imSize, imSize, values);

  // Get the SNR image with arbitrary global noise
  double globalNoise = 1.;
  TWOD_MASS_WL_MassMapping::Image mySNRImage = myAlgo.getSNRimage(myImage, globalNoise);


  // Check the values of the SNR image are ok
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      if (myGlobalMap.getBinValue(i, j, 0)>0.)
      {
        double expected = myImage.getValue(i ,j)/(globalNoise*sqrt(myGlobalMap.getBinValue(i, j, 0)));
        BOOST_CHECK_CLOSE(mySNRImage.getValue(i ,j), expected, 1);
      }
      else
      {
        double expected = 0.;
        BOOST_CHECK_CLOSE(mySNRImage.getValue(i ,j), expected, 1);
      }
    }
  }

  delete [] values;
  values = nullptr;
}


BOOST_AUTO_TEST_CASE( getCatalog_test ) {

  // Create a PeakCountAlgo object
  TWOD_MASS_WL_MassMapping::ConvergenceMap myConvMap(pathFiles+"convMap.fits");
  TWOD_MASS_WL_MassMapping::GlobalMap myGlobalMap(pathFiles+"densMap.fits");

  PeakCountAlgo myAlgo(myConvMap, myGlobalMap);

  // Define a temporary catalog filename
  std::string catalogName(pathFiles+"tmp/trash.fits");

  bool catalogCreated = myAlgo.savePeakCatalog(catalogName);

  BOOST_CHECK(catalogCreated == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
