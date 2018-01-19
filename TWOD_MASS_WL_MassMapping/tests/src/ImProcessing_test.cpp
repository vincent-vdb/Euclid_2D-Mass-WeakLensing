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
 * @file tests/src/ImProcessing_test.cpp
 * @date 07/07/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/ImProcessing.h"

using namespace TWOD_MASS_WL_MassMapping;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ImProcessing_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( DCT_test ) {

  unsigned int imSize = 32;
  double *values = new double[imSize*imSize];

  // Define arbitraty values for the image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(3+i+2*j);
    }
  }

  // Create the image
  Image myImage(imSize, imSize, values);

  // Create the IP object
  ImProcessing myIP(imSize, imSize);

  // Perform DCT on the image
  Image myDCTImage = myIP.performDCT(myImage);

  // And then perform IDCT on the output DCT image
  Image myImageBack = myIP.performIDCT(myDCTImage);

  // Check the values of the original and back image are close
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), myImageBack.getValue(i, j), 0.1);
    }
  }

  delete [] values;
  values = nullptr;
}


BOOST_AUTO_TEST_CASE( DCTblocks_test ) {

  unsigned int imSize = 32;
  double *values = new double[imSize*imSize];

  // Define arbitraty values for the image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(3+i+2*j);
    }
  }

  // Create the image
  Image myImage(imSize, imSize, values);

  // Create the IP object
  ImProcessing myIP(imSize, imSize);

  // Perform DCT on the image
  unsigned int blockSize(8);
  Image myDCTImage = myIP.performDCT(myImage, blockSize, blockSize, true);

  // And then perform IDCT on the output DCT image
  Image myImageBack = myIP.performDCT(myDCTImage, blockSize, blockSize, false);

  // Check the values of the original and back image are close
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), myImageBack.getValue(i, j), 0.1);
    }
  }

  delete [] values;
  values = nullptr;
}


BOOST_AUTO_TEST_CASE( spline_test ) {

  unsigned int imSize = 32;
  double *values = new double[imSize*imSize];

  // Define arbitraty values for the image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(3+i+2*j);
    }
  }

  // Create the image
  Image myImage(imSize, imSize, values);

  // Create the IP object
  ImProcessing myIP(imSize, imSize);

  // Perform an arbitrary wavelet decomposition
  std::vector<Image> myBand = myIP.transformBspline(myImage, 5);

  // Reconstruct the image
  Image myImageBack = myIP.reconsBspline(myBand);

  // Check the values of the original and back image are close
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), myImageBack.getValue(i, j), 0.1);
    }
  }

  delete [] values;
  values = nullptr;
}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


