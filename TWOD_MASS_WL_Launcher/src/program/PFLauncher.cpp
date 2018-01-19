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
 * @file src/program/PFLauncher.cpp
 * @date 07/11/17
 * @author user
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

#include "TWOD_MASS_WL_Launcher/PFLauncherParser.h"
//
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_Launcher/PFAlgo.h"


namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PFLauncher");

class PFLauncher : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options {};
    //
    // !!! Implement the program options here !!!
    //
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("PFLauncher");

    logger.info("#");
    logger.info("# Entering mainMethod()");
    logger.info("#");

    //
    // !!! Implement you program here !!!
    //

    TWOD_MASS_WL_MassMapping::Boundaries myBounds(10, 20, 55, 65, 0, 5);


    TWOD_MASS_WL_Launcher::PFAlgo myPFAlgo(true, 10, 1, 0, false, 0., 0.,
        "/media/sf_vvandenb/1591.fits",
        "",
        "/home/user/peakCatalLauncher.fits",
        "/home/user/convergenceMapLauncher.fits",
        10, 10., 10, true, myBounds);
    myPFAlgo.launchParalPF();

//    TWOD_MASS_WL_MassMapping::Boundaries myBounds(10, 20, 55, 65, 0, 5);
/*
    TWOD_MASS_WL_Launcher::PFAlgo myPFAlgo(true, 100, 3, 0, true,
                    0., 0., "/media/sf_vvandenb/1736.fits", "", "/home/user/PeakCat_100it_Variance_Borders_3red.fits",
                    "/home/user/ConvMapKS_100it_Variance_Borders_3red.fits", myBounds, true);

    myPFAlgo.performProcessingFunction();
*/
    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};
//
MAIN_FOR(PFLauncher)//TWOD_MASS_WL_Launcher::PFLauncherParser)//PFLauncher)
