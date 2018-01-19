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
 * @file tests/src/DataFilesLoader_test.cpp
 * @date 01/17/18
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DataFilesLoader_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( example_test ) {

  DataFilesLoader myLoader;
  std::string pathFiles = myLoader.downloadTestFiles();

  BOOST_CHECK_EQUAL(pathFiles, "ccirdeuclid.in2p3.fr/euclid-fr/home/pfle3write/wl/2dmass-wl/tests/data/");

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
