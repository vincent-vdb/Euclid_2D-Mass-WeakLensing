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
 * @file tests/src/CatalogSplitterParser_test.cpp
 * @date 07/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_CatalogSplitter/CatalogSplitterParser.h"

using namespace TWOD_MASS_WL_CatalogSplitter;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogSplitterParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( missingParameters_test ) {

  CatalogSplitterParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // check the parameters parsing returns false if no input given
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Let's provide inputs one after the other and keep trying
  args["inputFITSCatalog"] = po::variable_value(boost::any(std::string("dummyCatalog.fits")), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  args["outputFITSCatalogRoot"] = po::variable_value(boost::any(std::string("dummyOutput.fits")), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  args["nbCatalogsRa"] = po::variable_value(boost::any(5), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  args["nbCatalogsDec"] = po::variable_value(boost::any(3), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  args["nbCatalogsZ"] = po::variable_value(boost::any(2), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == true);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
