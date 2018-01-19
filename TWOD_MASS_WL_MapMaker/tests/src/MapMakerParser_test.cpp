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
 * @file tests/src/MapMakerParser_test.cpp
 * @date 04/28/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MapMaker/MapMakerParser.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;
using namespace TWOD_MASS_WL_MapMaker;

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MapMakerParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( badRaInputs_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // check the parameters parsing returns false
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the declination values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  decMinMax.push_back(10);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(2, 256)), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns false (only ra min)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  raMinMax.push_back(-20);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns false (bad ra max)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  raMinMax.pop_back();
  raMinMax.push_back(20);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);


  raMinMax.clear();
  raMinMax.push_back(-10);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);
  // check the parameters parsing returns false (bad ra in)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

}


BOOST_AUTO_TEST_CASE( badDecInputs_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(1, 256)), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Add the right ascension values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns false (only dec min)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  decMinMax.push_back(95);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns false (bad dec max)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  decMinMax.pop_back();
  decMinMax.push_back(20);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);


  decMinMax.clear();
  decMinMax.push_back(-100);
  decMinMax.push_back(10);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);
  // check the parameters parsing returns false (bad dec in)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

}

BOOST_AUTO_TEST_CASE( badZInputs_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(1, 256)), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Add the right ascension values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  decMinMax.push_back(20);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);


  // Add the redshift  values
  std::vector<float> zMinMax;
  zMinMax.push_back(0);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);


  // check the parameters parsing returns false (missing z max)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Add the redshift  values
  zMinMax.push_back(10);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);


  // Add a second output file
  // Provide an output file for the shear map
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns false (two outputs)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);
}

BOOST_AUTO_TEST_CASE( badBinsInputs_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Add the right ascension values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  decMinMax.push_back(20);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(1, 0)), false);

  // check the parameters parsing returns false (0 bins asked)
  BOOST_CHECK(myParser.parseInputParameters(args) == false);
}

BOOST_AUTO_TEST_CASE( SSVCatalogInput_extractShear_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the declination values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  decMinMax.push_back(10);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(2, 256)), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check no map object is created because no info in that catalog
  BOOST_CHECK(myParser.extractMap() == false);


  // Increase the ra dec ranges to be able to build a map
  decMinMax.pop_back();
  decMinMax.push_back(45);
  raMinMax.pop_back();
  raMinMax.push_back(45);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);

}

BOOST_AUTO_TEST_CASE( SSVCatalogInput_extractConv_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a SSV catalog as input parameter
  args["inputSSVCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"SSVcatalog_ok_withz.txt")), false);

  // Add the number of bins needed
  args["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(2, 256)), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(10);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the declination values
  std::vector<float> decMinMax;
  decMinMax.push_back(0);
  decMinMax.push_back(10);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // Provide an output file for the shear map
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check no map object is created because no info in that catalog
  BOOST_CHECK(myParser.extractMap() == false);

  // Increase the ra dec ranges to be able to build a map
  decMinMax.pop_back();
  decMinMax.push_back(45);
  raMinMax.pop_back();
  raMinMax.push_back(45);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);

  // Add the redshift values
  std::vector<float> zMinMax;
  zMinMax.push_back(0);
  zMinMax.push_back(10);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);
}

BOOST_AUTO_TEST_CASE( FITSCatalogInput_extractShear_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a F catalog as input parameter
  args["inputFITSCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"FITScatalog_ok_noz.fits")), false);

  // Add the declination values
  std::vector<float> decMinMax;
  decMinMax.push_back(-10);
  decMinMax.push_back(-5);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(0);
  raMinMax.push_back(40);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the number of bins needed
  std::vector<int> bins;
  bins.push_back(256);
  bins.push_back(128);
  args["outputMapBinXY"] = po::variable_value(boost::any(bins), false);

  // Add the redshift values
  std::vector<float> zMinMax;
  zMinMax.push_back(0);
  zMinMax.push_back(10);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check no map object is create (no galaxy in the radec range)
  BOOST_CHECK(myParser.extractMap() == false);

  // Provide good values of dec
  decMinMax.clear();
  decMinMax.push_back(0);
  decMinMax.push_back(40);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);

}

BOOST_AUTO_TEST_CASE( FITSCatalogInput_extractConv_test ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a FITS catalog as input parameter
  args["inputFITSCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"FITScatalog_ok_noz.fits")), false);

  // Add the number of bins needed
  std::vector<int> bins;
  bins.push_back(256);
  // Provide only one value to test this case
  args["outputMapBinXY"] = po::variable_value(boost::any(bins), false);

  // Add the right ascension values
  std::vector<float> raMinMax;
  raMinMax.push_back(180);
  raMinMax.push_back(190);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // Add the declination values
  std::vector<float> decMinMax;
  decMinMax.push_back(-10);
  decMinMax.push_back(0);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);

  // check the parameters parsing returns false because nothing to be extracted
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Provide an output file for the conv map
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check no map object is created because no info in that catalog
  BOOST_CHECK(myParser.extractMap() == false);

  // Increase the ra dec ranges to be able to build a map
  decMinMax.clear();
  decMinMax.push_back(0);
  decMinMax.push_back(45);
  raMinMax.clear();
  raMinMax.push_back(0);
  raMinMax.push_back(45);
  args["decMinMax"] = po::variable_value(boost::any(decMinMax), false);
  args["raMinMax"] = po::variable_value(boost::any(raMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);

  // Add the redshift values
  std::vector<float> zMinMax;
  zMinMax.push_back(0);
  zMinMax.push_back(10);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);
}

BOOST_AUTO_TEST_CASE( FITSCatalogInput_extractConv_CenterMapInputstest ) {

  MapMakerParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a FITS catalog as input parameter
  args["inputFITSCatalog"] = po::variable_value(boost::any(
      std::string(pathFiles+"FITScatalog_ok_noz.fits")), false);

  // Add the pixel size
  std::vector<float> pixelSize;
  pixelSize.push_back(0.04);
  // Provide only one value to test this case
  args["pixelSize"] = po::variable_value(boost::any(pixelSize), false);

  // Add the map size
  std::vector<float> mapSize;
  mapSize.push_back(40.);
  args["mapSize"] = po::variable_value(boost::any(mapSize), false);

  // Add map center
  std::vector<float> mapCenter;
  mapCenter.push_back(20.);
  mapCenter.push_back(20.);
  args["mapCenter"] = po::variable_value(boost::any(mapCenter), false);

  // check the parameters parsing returns false because nothing to be extracted
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Provide an output file for the conv map
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);

  // Add the redshift values
  std::vector<float> zMinMax;
  zMinMax.push_back(0);
  zMinMax.push_back(10);
  args["zMinMax"] = po::variable_value(boost::any(zMinMax), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);
  // Check a map object is created
  BOOST_CHECK(myParser.extractMap() == true);
}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
