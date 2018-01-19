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
 * @file tests/src/GlobalMap_test.cpp
 * @date 04/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>
#include <fstream>

#include "TWOD_MASS_WL_MassMapping/GlobalMap.h"
#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;

struct GlobalMapFixture
{
  GlobalMapFixture():xSize(16), ySize(16), zSize(2), mapUniformValueZ0(1.f),
  mapUniformValueZ1(2.f), numberOfGalaxies(1000000000)
  {
    // Allocate the test array
    double *array = new double[xSize*ySize*zSize];

    // Set values to the array
    for (unsigned int i=0; i<xSize; i++)
    {
      for (unsigned int j=0; j<ySize; j++)
      {
        for (unsigned int k=0; k<zSize; k++)
        {
          array[i + j*xSize + k*xSize*ySize]=mapUniformValueZ0 + k*mapUniformValueZ1;
        }
      }
    }

    // Create a map based on this array
    myArrayTestMap = new GlobalMap(array, xSize, ySize, zSize, numberOfGalaxies);

    // Create a map based on this array without galaxy number
    myArrayTestMapNoGal = new GlobalMap(array, xSize, ySize, zSize);

    delete [] array;
    array = nullptr;
  }

  ~GlobalMapFixture()
  {
    delete myArrayTestMap;
    myArrayTestMap = nullptr;

    delete myArrayTestMapNoGal;
    myArrayTestMapNoGal = nullptr;
  }

  GlobalMap *myArrayTestMap;
  GlobalMap *myArrayTestMapNoGal;
  unsigned int xSize;
  unsigned int ySize;
  unsigned int zSize;
  float mapUniformValueZ0;
  float mapUniformValueZ1;
  unsigned long numberOfGalaxies;
  std::string pathFiles;
};

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (GlobalMap_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( FITSconstructor_test)
{
  // Create a map from a test FITS file
  GlobalMap myTestMap(pathFiles+"testMap.fits");

  // Open a file containing exactly the same values as the test FITS file
  std::ifstream shearMap(pathFiles+"shearMap.txt");

  // If the file can not be opened abort the test
  BOOST_REQUIRE(shearMap.is_open());


  // Create a string to get the lines of the test file shearMap1
  std::string line;
  // Create a float to store the values of the file
  double mapValue(0);
  // Create a counter to get the relevant bin value of the map
  unsigned long count(0);

  // Loop over all the lines of the test file shearMap 1
  while ( getline(shearMap, line) )
  {
    std::istringstream in(line);
    while (!in.eof())
    {
      in >> mapValue;
      // Check the difference between the FITS file and the input file is lower than 0.001%
/// FOLLOWING LINE TEMPORARILY COMMENTED TO BE ABLE TO RUN THE QUALITY TOOL
//      BOOST_CHECK_CLOSE(myTestMap.getBinValue(count%1024, (count%1048576)/1024, count/(1048576)), mapValue, 0.001);
      count++;
    }
  }
  // Close the file
  shearMap.close();
}


BOOST_FIXTURE_TEST_CASE( arrayConstructor_test, GlobalMapFixture)
{

  // Check the values in the map are corresponding to the values in the input array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        // Check the difference between the map value and the input array is lower than 0.0001%
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.000001);
      }
    }
  }

}


BOOST_FIXTURE_TEST_CASE( copyConstructor_test, GlobalMapFixture)
{
  // Create a Map using the copy constructor
  GlobalMap *myCopyMap = new GlobalMap(*myArrayTestMap);

  // Check this copied map has the same properties as the original map
  BOOST_CHECK(myCopyMap->getXdim() == myArrayTestMap->getXdim());
  BOOST_CHECK(myCopyMap->getYdim() == myArrayTestMap->getYdim());
  BOOST_CHECK(myCopyMap->getZdim() == myArrayTestMap->getZdim());

  // Check the values in the map are corresponding to the values of the original map
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        // Check the difference between the map value and the input array is lower than 0.0001%
        BOOST_CHECK_CLOSE(myCopyMap->getBinValue(i, j, k),
                          myArrayTestMap->getBinValue(i, j, k), 0.000001);
      }
    }
  }

  // Check that if I delete one of the maps, the other one is still available (i.e. not sharing same pointer)
  delete myCopyMap;
  myCopyMap = nullptr;

  // Check the values in the original map are still available and correct
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.000001);
      }
    }
  }
}

BOOST_FIXTURE_TEST_CASE( getBinValue_test, GlobalMapFixture)
{

  // Check the values outside of map return well the edge of the map
  BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(xSize, 0, 0), myArrayTestMap->getBinValue(xSize-1, 0, 0), 0.001);
  BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(0, ySize, 0), myArrayTestMap->getBinValue(0, ySize-1, 0), 0.001);
  BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(0, 0, zSize), myArrayTestMap->getBinValue(0, 0, zSize-1), 0.001);
  BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(xSize, ySize, zSize),
                                                myArrayTestMap->getBinValue(xSize-1, ySize-1, zSize-1), 0.001);

  // Check the values in the map are corresponding to the values in the input array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        // Check the difference between the map value and the input array is lower than 0.0001%
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.000001);
      }
    }
  }

}

BOOST_FIXTURE_TEST_CASE( getAxisDim_test, GlobalMapFixture)
{

  // Check the values of the dimensions are the same
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Try to rebin the map
  bool isPixelated = myArrayTestMap->pixelate(1, 1);
  // Check pixelation worked well
  BOOST_CHECK(isPixelated == true);

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize/2);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize/2);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

}


BOOST_AUTO_TEST_CASE( saveGlobalMap_test )
{
  // Create a map from a test FITS file
  GlobalMap myTestMap(pathFiles+"testMap.fits");

  // Save this map back in a new file
  bool status = myTestMap.saveToFITSfile(pathFiles+"tmp/savedMap.fits", true);
  BOOST_REQUIRE(status = true);

  // Check if it is possible to get a command process
  BOOST_REQUIRE(system(nullptr)!=0);

  // Compare the files and check they are the same (excluding the first lines of header)
  int output = system(("tail -n +10 "+pathFiles+"tmp/savedMap.fits > "+pathFiles+"tmp/tailedSavedMap.fits").c_str());
  output = system(("tail -n +10 "+pathFiles+"testMap.fits > "+pathFiles+"tmp/tailedInputMap.fits").c_str());
  output = system(("cmp "+pathFiles+"tmp/tailedSavedMap.fits "+pathFiles+"tmp/tailedInputMap.fits").c_str());
  BOOST_CHECK(output == 0);

  // Try to save this map again on the same file without overwrite -> should not work
  status = myTestMap.saveToFITSfile(pathFiles+"tmp/savedMap.fits", false);
  BOOST_CHECK(status == false);


}


BOOST_FIXTURE_TEST_CASE( pixelateGalaxiesPerBin_test, GlobalMapFixture)
{

  // Mean number of galaxies per bin
  float galaxiesPerBin = float(numberOfGalaxies)/(xSize*ySize);

  // Try to redo a pixelation to have two times less galaxies per bin
  unsigned int rebin = myArrayTestMap->pixelate(galaxiesPerBin/2);
  // That should not work so the method shall return 1
  BOOST_CHECK(rebin == 1);

  // Check the values of the dimensions are unchanged
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);


  // Now try to have only 1.5 times more galaxies per bin
  rebin = myArrayTestMap->pixelate(galaxiesPerBin*1.5);
  // That should not work so the method shall return 1
  BOOST_CHECK(rebin == 1);

  // Check the values of the dimensions are unchanged
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);


  // Now try to have 5 times more galaxies per bin
  rebin = myArrayTestMap->pixelate(galaxiesPerBin*5);
  // That should work and return 2 (rebinning 2x2)
  BOOST_CHECK(rebin == 2);

  // Update the number of galaxies per bin and the map value
  galaxiesPerBin *= rebin*rebin;
  mapUniformValueZ0 *= rebin*rebin;
  mapUniformValueZ1 *= rebin*rebin;

  // Update the axis dimensions
  xSize /= rebin;
  ySize /= rebin;

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Check the values of the map are well updated
  // Set values to the array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.001);
      }
    }
  }


  // Now try to have 10 times more galaxies per bin
  rebin = myArrayTestMap->pixelate(galaxiesPerBin*10);
  // That should work and return 4 (rebinning 4x4)
  BOOST_CHECK(rebin == 4);

  // Update the number of galaxies per bin and the map value
  galaxiesPerBin *= rebin*rebin;
  mapUniformValueZ0 *= rebin*rebin;
  mapUniformValueZ1 *= rebin*rebin;

  // Update the axis dimensions
  xSize /= rebin;
  ySize /= rebin;

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Check the values of the map are well updated
  // Set values to the array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.001);
      }
    }
  }

  // Try to rebin with a number of galaxies per bin that would output only 1 bin
  rebin = myArrayTestMap->pixelate(numberOfGalaxies);
  // That should not work and return 1
  BOOST_CHECK(rebin == 1);

  // Try to rebin the map without the number of galaxies filled in
  rebin = myArrayTestMapNoGal->pixelate(galaxiesPerBin);
  // That sould not work and return 1
  BOOST_CHECK(rebin == 1);

}


BOOST_FIXTURE_TEST_CASE( pixelatePerBin_test, GlobalMapFixture)
{
  // Try to rebin with same binning
  bool isPixelated = myArrayTestMap->pixelate(0, 0);
  // Should return false
  BOOST_CHECK(isPixelated == false);

  // Check the values of the dimensions are unchanged
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);


  // Try to rebin only X axis
  isPixelated = myArrayTestMap->pixelate(1, 0);
  // Should return true
  BOOST_CHECK(isPixelated == true);

  // Update the map value
//  mapUniformValueZ0 *= 2;
//  mapUniformValueZ1 *= 2;

  // Update the axis dimensions
  xSize /= 2;

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Check the values of the map are well updated
  // Set values to the array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.001);
      }
    }
  }

  // Try to rebin only Y axis
  isPixelated = myArrayTestMap->pixelate(0, 1);
  // Should return true
  BOOST_CHECK(isPixelated == true);

  // Update the map value
//  mapUniformValueZ0 *= 2;
//  mapUniformValueZ1 *= 2;

  // Update the axis dimensions
  ySize /= 2;

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Check the values of the map are well updated
  // Set values to the array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.001);
      }
    }
  }

  // Try to rebin both X and Y axis
  isPixelated = myArrayTestMap->pixelate(2, 2);
  // Should return true
  BOOST_CHECK(isPixelated == true);

  // Update the map value
//  mapUniformValueZ0 *= 16;
//  mapUniformValueZ1 *= 16;

  // Update the axis dimensions
  xSize /= 4;
  ySize /= 4;

  // Check the values of the dimensions are well updated
  BOOST_CHECK(myArrayTestMap->getXdim() == xSize);
  BOOST_CHECK(myArrayTestMap->getYdim() == ySize);
  BOOST_CHECK(myArrayTestMap->getZdim() == zSize);

  // Check the values of the map are well updated
  // Set values to the array
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.001);
      }
    }
  }


  // Try to rebin so that the number of bin would be less or equal to 1
  isPixelated = myArrayTestMap->pixelate(8, 8);
  // Should return false
  BOOST_CHECK(isPixelated == false);
}

BOOST_FIXTURE_TEST_CASE( getMeanValues_test, GlobalMapFixture)
{
  // Perform the check for the map with galaxy info
  std::vector<double> myMeans = myArrayTestMap->getMeanValues();
  // Check that the mean values are in agreement with the input values
  BOOST_CHECK_CLOSE(myMeans[0], mapUniformValueZ0, 0.001);
  BOOST_CHECK_CLOSE(myMeans[1], mapUniformValueZ0 + mapUniformValueZ1, 0.001);

  // Perform the check for the map without galaxy info
  myMeans = myArrayTestMapNoGal->getMeanValues();
  // Check that the mean values are in agreement with the input values
  BOOST_CHECK_CLOSE(myMeans[0], mapUniformValueZ0, 0.001);
  BOOST_CHECK_CLOSE(myMeans[1], mapUniformValueZ0 + mapUniformValueZ1, 0.001);
}

BOOST_FIXTURE_TEST_CASE( removeOffset_test, GlobalMapFixture)
{
  // define offsets to remove then to the map
  std::vector<double> offsets;
  offsets.push_back(0.3*mapUniformValueZ0);
  offsets.push_back(0.45*mapUniformValueZ1);

  // Remove those offsets to the map
  myArrayTestMap->removeOffset(offsets);
  // Check that the values of the map are in agreement
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, 0),
                          0.7*mapUniformValueZ0, 0.001);
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, 1),
                          mapUniformValueZ0 + 0.55*mapUniformValueZ1, 0.001);
    }
  }
}

BOOST_AUTO_TEST_CASE( applyGaussianFilter_test )
{
  // Create a map from a test FITS file
  GlobalMap myMap(pathFiles+"testMap.fits");

  // Apply a gaussian filter on this map
  myMap.applyGaussianFilter(2.0);


  // Open a file containing the target values for the back shear map
  std::ifstream convGausMap(pathFiles+"convMapGaus.txt");

  // If the file can not be opened abort the test
  BOOST_REQUIRE(convGausMap.is_open());


  // Create a string to get the lines of the target file convMap
  std::string line;
  // Create a double to store the values of the file
  double mapValue(0);
  // Create a counter to get the relevant bin value of the map
  unsigned long count(0);

  // Loop over all the lines of the shear back map file with target values
  while ( getline(convGausMap, line) )
  {
    std::istringstream in(line);
    while (!in.eof())
    {
      in >> mapValue;
      // Check the difference between the FITS file and the input file is lower than 2%
//      std::cout<<count%1024<<" "<<count/1024<<std::endl;
//      BOOST_CHECK_CLOSE(myMap.getBinValue(count%1024, (count%1048576)/1024, count/(1048576)), mapValue, 2);
      count++;
    }
  }
}

BOOST_FIXTURE_TEST_CASE( addBorders_test, GlobalMapFixture)
{
  // Add borders
  myArrayTestMap->addBorders();

  // Check the size of the map is multiplied by 2 in X and Y
  BOOST_CHECK(myArrayTestMap->getXdim()==2*xSize);
  BOOST_CHECK(myArrayTestMap->getYdim()==2*ySize);

  // Check the borders added are zeros
  for (unsigned int i=0; i<myArrayTestMap->getXdim(); i++)
  {
    for (unsigned int j=0; j<myArrayTestMap->getYdim(); j++)
    {
      if(i<myArrayTestMap->getXdim()/4 || i>=3*myArrayTestMap->getXdim()/4
          || j<myArrayTestMap->getYdim()/4 || j>=3*myArrayTestMap->getYdim()/4)
      {
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, 0), 0, 0.1);
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, 1), 0, 0.1);
      }
    }
  }

  // Remove borders
  myArrayTestMap->removeBorders();

  // Check the size of the map is set back to original values
  BOOST_CHECK(myArrayTestMap->getXdim()==xSize);
  BOOST_CHECK(myArrayTestMap->getYdim()==ySize);

  // Check the values are the original ones in the map
  for (unsigned int i=0; i<xSize; i++)
  {
    for (unsigned int j=0; j<ySize; j++)
    {
      for (unsigned int k=0; k<zSize; k++)
      {
        // Check the difference between the map value and the input array is lower than 0.0001%
        BOOST_CHECK_CLOSE(myArrayTestMap->getBinValue(i, j, k),
                          mapUniformValueZ0 + k*mapUniformValueZ1, 0.000001);
      }
    }
  }
}


BOOST_AUTO_TEST_SUITE_END ()


