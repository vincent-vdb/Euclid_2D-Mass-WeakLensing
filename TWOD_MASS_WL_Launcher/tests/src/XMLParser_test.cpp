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
 * @file tests/src/XMLParser_test.cpp
 * @date 06/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_Launcher/XMLParser.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_Launcher;

TWOD_MASS_WL_MassMapping::DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (XMLParser_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( badInputXML_test ) {

  // Provide a file that does not exist as input and check it returns false
  XMLParser myXMLParser1("dummyInputXML.xml", "dummyOutputXML.xml");
  BOOST_CHECK(myXMLParser1.parseInputFile()==false);

  // Provide a XML without tags and check it returns false
  XMLParser myXMLParser2(pathFiles+"badXML.xml", "dummyOutputXML.xml");
  BOOST_CHECK(myXMLParser2.parseInputFile()==false);

  // Provide an empty XML and check it returns false
  XMLParser myXMLParser3(pathFiles+"emptyXML.xml", "dummyOutputXML.xml");
  BOOST_CHECK(myXMLParser3.parseInputFile()==false);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
