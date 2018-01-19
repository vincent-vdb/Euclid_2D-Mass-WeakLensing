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
 * @file tests/src/PeakCountParser_test.cpp
 * @date 05/22/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_PeakCount/PeakCountParser.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_PeakCount;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PeakCountParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( ParserInput_test ) {

  PeakCountParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // check the parameters parsing returns false if no input given
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Provide only convergence map input
  args["inputConvMapFITS"] = po::variable_value(boost::any(
      std::string("../TWOD_MASS_WL_MassMapping/tests/src/convMap.fits")), false);

  // Check the parameters parsing returns false because only conv map provided
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Now provide shear map input
  args["inputDensityMapFITS"] = po::variable_value(boost::any(
      std::string("../TWOD_MASS_WL_MassMapping/tests/src/convMap.fits")), false);

  // Check the parameters parsing returns false because no output provided
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Now provide peak catalog output
  args["outputPeakCatalogFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"tmp/totrash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

}


BOOST_AUTO_TEST_CASE( createPeakCatalog_test ) {

  PeakCountParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // Provide needed inputs
  args["inputConvMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"convMap.fits")), false);
  args["inputDensityMapFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"shearMap.fits")), false);
  args["outputPeakCatalogFITS"] = po::variable_value(boost::any(
      std::string(pathFiles+"tmp/totrash.fits")), false);

  // check the parameters parsing returns true
  BOOST_CHECK(myParser.parseInputParameters(args) == true);

  // Check catalog creation works well
  BOOST_CHECK(myParser.createPeakCatalog() == true);
}



//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
