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
 * @file tests/src/PFLauncherParser_test.cpp
 * @date 08/08/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_Launcher/PFLauncherParser.h"

using namespace TWOD_MASS_WL_Launcher;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PFLauncherParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( parameters_test ) {

  PFLauncherParser myParser;

  // let's create a mock input map for the values of the main method
  std::map<std::string, po::variable_value> args;

  // check the parameters parsing returns false if no input given
  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Provide only the input XML and check it returns false
  args["inputXMLfile"] = po::variable_value(boost::any(std::string("dummyInputFile.xml")), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == false);

  // Now provide also the output XML and check it returns true
  args["outputXMLfile"] = po::variable_value(boost::any(std::string("dummyOutputFile.xml")), false);

  BOOST_CHECK(myParser.parseInputParameters(args) == true);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


