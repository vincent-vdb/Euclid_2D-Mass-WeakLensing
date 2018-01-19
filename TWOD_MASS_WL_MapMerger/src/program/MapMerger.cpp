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
 * @file src/program/MapMerger.cpp
 * @date 08/23/17
 * @author user
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
/*
#include "healpix_base.h"
#include "healpix_map.h"
#include "healpix_data_io.h"
#include "healpix_map_fitsio.h"

#include <alm.h>
#include <alm_fitsio.h>
#include <alm_healpix_tools.h>

// tools for Cls
#include <powspec.h>
#include <powspec_fitsio.h>
#include <alm_powspec_tools.h>
*/
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("MapMerger");

class MapMerger : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options {};
    //
    // !!! Implement the program options here !!!
    //
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("MapMerger");

    logger.info("#");
    logger.info("# Entering mainMethod()");
    logger.info("#");

    //
    // !!! Implement you program here !!!
    //

//    Healpix_Map<double> map;
//    Alm<xcomplex<double> > alm(2, 2);

    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(MapMerger)
