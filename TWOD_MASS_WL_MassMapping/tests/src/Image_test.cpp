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
 * @file tests/src/Image_test.cpp
 * @date 07/07/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/Image.h"

using namespace TWOD_MASS_WL_MassMapping;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Image_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( basicConstructor_tests ) {

  // Create an image initialized with zeros
  unsigned int imSize(32);
  Image myImage(imSize, imSize);

  // Check all values are well zeros
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), 0, 1);
    }
  }
}

BOOST_AUTO_TEST_CASE( valuesConstructor_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i-j);
    }
  }

  // Create an image initialized with values
  Image myImage(imSize, imSize, values);

  // Check values are well initialized
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), i-j, 0.01);
    }
  }

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( copyConstructor_operatorEqual_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i-j);
    }
  }

  // Create an image initialized with values
  Image myImage(imSize, imSize, values);

  // Create another image from this one
  Image myImage2(myImage);

  // Check values are well the same
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), myImage2.getValue(i, j), 0.01);
    }
  }

  // Check both images do not share same memory
  myImage.setValue(0, 0, 10);
  BOOST_CHECK_CLOSE(myImage.getValue(0, 0)-10, myImage2.getValue(0, 0), 0.01);

  // Check the operator= now
  myImage2 = myImage;
  // Check values are well the same
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), myImage2.getValue(i, j), 0.01);
    }
  }

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( getDimensions_tests ) {

  // Create an image initialized with zeros
  unsigned int imSize(32);
  Image myImage(imSize, imSize*2);

  // Check the dimensions are correct
  BOOST_CHECK_EQUAL(myImage.getXdim(), imSize);
  BOOST_CHECK_EQUAL(myImage.getYdim(), imSize*2);
}

BOOST_AUTO_TEST_CASE( getandsetvalues_tests ) {

  // Create an image initialized with zeros
  unsigned int imSize(32);
  Image myImage(imSize, imSize);

  // Set the values
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      myImage.setValue(i, j, 2*i+j);
    }
  }

  // Check the values are well set
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myImage.getValue(i, j), 2*i+j, 0.01);
    }
  }

  // Check out of bounds values return bounds values
  // Check the values are well set
  for (unsigned int i=0; i<imSize; i++)
  {
    BOOST_CHECK_CLOSE(myImage.getValue(i, imSize), 2*i+imSize-1, 0.01);
  }
  for (unsigned int j=0; j<imSize; j++)
  {
    BOOST_CHECK_CLOSE(myImage.getValue(imSize, j), 2*(imSize-1)+j, 0.01);
  }
}

BOOST_AUTO_TEST_CASE( getMax_tests ) {

  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i+j);
    }
  }

  // Create two images
  Image myImage(imSize, imSize, values);

  // Get the max
  double max = myImage.getMax();

  // Check the max is the right value
  BOOST_CHECK_CLOSE(max, 2*(imSize-1), 1);

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( getStdev_tests ) {

  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i+j);
    }
  }

  // Create two images
  Image myImage(imSize, imSize, values);

  // Get the standard deviation
  double stdev = myImage.getStandardDeviation();

  // Check the max is the right value
  BOOST_CHECK_CLOSE(stdev, 13.06394529, 1);

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( applyThreshold_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i+j);
    }
  }

  // Create an image
  Image myImage(imSize, imSize, values);

  // Apply a threshold
  myImage.applyThreshold(10.);

  // Check the values below the threshold are well set to zero
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      if (i+j<10.)
      {
        BOOST_CHECK_CLOSE(myImage.getValue(i, j), 0, 1);
      }
      else
      {
        BOOST_CHECK_CLOSE(myImage.getValue(i, j), i+j, 1);
      }
    }
  }

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( localMax_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i+j);
    }
  }

  // Create an image
  Image myImage(imSize, imSize, values);

  // Check there is no local max in this context
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK(myImage.isLocalMax(i, j)==false);
    }
  }

  // Now add arbitrary local maximums
  myImage.setValue(18, 12, 70);
  myImage.setValue(3, 17, 85);

  // Check those pixels are now local maximum
  BOOST_CHECK(myImage.isLocalMax(18, 12)==true);
  BOOST_CHECK(myImage.isLocalMax(3, 17)==true);

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( add_substract_divide_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i-j);
    }
  }

  // Create two images
  Image myImage(imSize, imSize, values);
  Image myImage2(imSize, imSize, values);

  // Get the added image
  Image myAddedImage = myImage.add(myImage2);
  // Get the substracted image
  Image mySubImage = myImage.substract(myImage2);
  // Get the multiplied image
  double factor = 2.58;
  Image myMulImage = myImage.multiply(factor);

  // Check the values
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myAddedImage.getValue(i, j), 2.*double(i-j), 0.01);
      BOOST_CHECK_CLOSE(mySubImage.getValue(i, j), 0., 0.01);
      BOOST_CHECK_CLOSE(myMulImage.getValue(i, j), factor*double(i-j), 0.01);
    }
  }

  delete [] values;
  values = nullptr;
}

BOOST_AUTO_TEST_CASE( bad_add_substract_tests ) {
  // Create two images
  unsigned int imSize(32);
  Image myImage(imSize, imSize);
  Image myImage2(imSize*2, imSize);

  // Get the added image
  Image myAddedImage = myImage.add(myImage2);
  // Get the substracted image
  Image mySubImage = myImage.substract(myImage2);

  // Check those images are empty
  BOOST_CHECK(myAddedImage.getXdim()==0);
  BOOST_CHECK(myAddedImage.getYdim()==0);
  BOOST_CHECK(mySubImage.getXdim()==0);
  BOOST_CHECK(mySubImage.getYdim()==0);
}

BOOST_AUTO_TEST_CASE( getArray_tests ) {
  unsigned int imSize(32);
  double *values = new double[imSize*imSize];

  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      values[j*imSize + i] = double(i-j);
    }
  }

  // Create two images
  Image myImage(imSize, imSize, values);

  // Check the getArray method provided the good values
  double *myArray = myImage.getArray();
  // Define arbitraty values for an image
  for (unsigned int i=0; i<imSize; i++)
  {
    for (unsigned int j=0; j<imSize; j++)
    {
      BOOST_CHECK_CLOSE(myArray[j*imSize + i], values[j*imSize + i], 0.01);
    }
  }

  delete [] values;
  values = nullptr;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


