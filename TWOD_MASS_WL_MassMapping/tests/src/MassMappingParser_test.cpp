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
 * @file tests/src/MassMappingParser_test.cpp
 * @date 04/28/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/MassMappingParser.h"
#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MassMappingParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( badInput_test ) {

  MassMappingParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // check the parameters parsing returns false if no input given
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Let's provide more than one input
  args["inputShearMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"testMap.fits")), false);
  args["inputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"convMap.fits")), false);

  // check the parameters parsing returns false if more than one input given
  BOOST_CHECK(myParser.parseInputParameters(args) == false);
}

BOOST_AUTO_TEST_CASE( shearMapInput_test ) {

  MassMappingParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a shear map as input parameter
  args["inputShearMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"testMap.fits")), false);

  // Add a shear map as output parameter
  args["outputShearMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"tmp/trash.fits")), false);

  // Provide an output file for the convergence map
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Ask to output the means and to remove the offsets of both maps
  args["getMeanShearMap"] = po::variable_value(boost::any(1), false);
  args["getMeanConvMap"] = po::variable_value(boost::any(1), false);
  args["removeOffsetShearMap"] = po::variable_value(boost::any(1), false);
  args["removeOffsetConvMap"] = po::variable_value(boost::any(1), false);

  // Perform values for gaussian filtering of both maps
  args["sigmaShearMap"] = po::variable_value(boost::any(std::vector<float>(2, 0.1)), false);
  args["sigmaConvMap"] = po::variable_value(boost::any(std::vector<float>(2, 0.1)), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

  // Then check that
  BOOST_CHECK(myParser.perform2DMassMapping(args) == true);
}

BOOST_AUTO_TEST_CASE( convMapInput_test ) {

  MassMappingParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Add a conv map as input parameter
  args["inputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"convMap.fits")), false);

  // Add a conv map as output file
  args["outputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"tmp/trash.fits")), false);

  // Provide an output file for the shear map
  args["outputShearMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  // Ask to output the means and to remove the offsets of both maps
  args["getMeanShearMap"] = po::variable_value(boost::any(1), false);
  args["getMeanConvMap"] = po::variable_value(boost::any(1), false);
  args["removeOffsetShearMap"] = po::variable_value(boost::any(1), false);
  args["removeOffsetConvMap"] = po::variable_value(boost::any(1), false);

  // Perform values for gaussian filtering of both maps
  args["sigmaShearMap"] = po::variable_value(boost::any(std::vector<float>(1, 0.1)), false);
  args["sigmaConvMap"] = po::variable_value(boost::any(std::vector<float>(1, 0.1)), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

  // Then check that 2D mass mapping can be performed
  BOOST_CHECK(myParser.perform2DMassMapping(args) == true);

}

BOOST_AUTO_TEST_CASE( InPaintingGoodInputs_test ) {

  MassMappingParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Provide all needed inputs
  args["inputShearMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"shear_holes_test.fits")), false);
  args["outputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"conv_holes_test.fits")), false);
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  args["numberIteration"] = po::variable_value(boost::any(2), false);

  args["sigmaBounded"] = po::variable_value(boost::any(1), false);
  args["bModeZeros"] = po::variable_value(boost::any(1), false);

  // check the parameters parsing returns false if not all needed inputs are provided
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

  // Then check that 2D mass mapping can be performed
  BOOST_CHECK(myParser.perform2DMassMapping(args) == true);
  // Check the inpainting can be performed
  BOOST_CHECK(myParser.performInPainting(args) == true);
}


BOOST_AUTO_TEST_CASE( InPaintingGoodInputsWithThresholds_test ) {

  MassMappingParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Provide all needed inputs
  args["inputShearMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"shear_holes_test.fits")), false);
  args["outputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"conv_holes_test.fits")), false);
  args["outputConvMapFITS"] = po::variable_value(boost::any(std::string(pathFiles+"tmp/trash.fits")), false);

  args["numberIteration"] = po::variable_value(boost::any(2), false);

  args["sigmaBounded"] = po::variable_value(boost::any(1), false);
  args["bModeZeros"] = po::variable_value(boost::any(1), false);
  args["numberScales"] = po::variable_value(boost::any(5), false);
  args["addBorders"] = po::variable_value(boost::any(1), false);

  std::vector<float> threshold;
  threshold.push_back(0.);
  threshold.push_back(0.5);

  args["thresholdInpainting"] = po::variable_value(boost::any(threshold), false);

  // check the parameters parsing returns false if not all needed inputs are provided
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

  // Then check that 2D mass mapping can be performed
  BOOST_CHECK(myParser.perform2DMassMapping(args) == true);
  // Check the inpainting can be performed
  BOOST_CHECK(myParser.performInPainting(args) == true);
}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


