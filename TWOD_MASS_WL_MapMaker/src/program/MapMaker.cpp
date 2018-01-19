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
 * @file src/program/MapMaker.cpp
 * @date 04/28/17
 * @author user
 */

#include <map>
#include <string>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"

#include "TWOD_MASS_WL_MapMaker/MapMakerParser.h"

//#include <CCfits/CCfits>
//#include <fstream>
//#include <iostream>

namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("MapMaker");

class MapMaker : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options {};
    //
    // !!! Implement the program options here !!!
    //
    return options;
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    Elements::Logging logger = Elements::Logging::getLogger("MapMaker");

    logger.info("#");
    logger.info("# Entering mainMethod()");
    logger.info("#");

    //
    // !!! Implement you program here !!!
    //
/*
    //////////////// for converting SSV to FITS catalog try
    // Loop over all the lines of the test file shearMap 1

    /// part where you read a ssv file
    std::ifstream catalog("/home/user/vvandenb/mice_v2_0_shear_order_by_unique_gal_id.ssv");
    // Create a string to get the lines of the file
    std::string line;

    //Read the first line of headers and skip it
    getline(catalog, line);


    std::vector<double> ra, dec, kappa, gamma1, gamma2, z;

    unsigned long counter(0);

    while ( getline(catalog, line) ) // && galCount<10000000)
    {
      double tmpgalId, tmpra, tmpdec, tmpkappa, tmpgamma1, tmpgamma2;

      std::istringstream in(line);
      in>>tmpgalId>>tmpra>>tmpdec>>tmpkappa>>tmpgamma1>>tmpgamma2;

      ra.push_back(tmpra);
      dec.push_back(tmpdec);
      kappa.push_back(tmpkappa);
      gamma1.push_back(tmpgamma1);
      gamma2.push_back(tmpgamma2);
      z.push_back(1.);

      counter++;

      if (counter%1000000==0) std::cout<<"counter to "<<counter<<std::endl;
      if (counter==50000000) break;
    }

    // Create a vector containing all data
    std::vector<std::vector<double> > inputData;
    inputData.push_back(ra);
    inputData.push_back(dec);
    inputData.push_back(z);
    inputData.push_back(gamma1);
    inputData.push_back(gamma2);


    // Save those data into the catalog and return false if it does not work
    std::vector<std::string> colNames;
    colNames.push_back("RightAsc");
    colNames.push_back("Declination");
    colNames.push_back("redshift");
    colNames.push_back("gamma1");
    colNames.push_back("gamma2");

    std::vector<std::string> colForms(5, "E");

    std::vector<std::string> colUnits;
    colUnits.push_back("degree");
    colUnits.push_back("degree");
    colUnits.push_back("redshift");
    colUnits.push_back("u.a.");
    colUnits.push_back("u.a.");

    std::vector<std::vector<std::string> > columns;
    columns.push_back(colNames);
    columns.push_back(colForms);
    columns.push_back(colUnits);

    /// part where you write into a fits file

    int nRows = (inputData[0]).size();
    unsigned int nCols = inputData.size();

    try
    {
      // Create an output FITS file
      CCfits::FITS *outFits = new CCfits::FITS("/home/user/SSVtoFITSCatalog_50000000gal.fits",
                                               CCfits::RWmode::Write, true);

      std::string hduName("outTable");

      std::vector< CCfits::String > colNames = std::vector< CCfits::String >();
      std::vector< CCfits::String > colForms = std::vector< CCfits::String >();
      std::vector< CCfits::String > colUnits = std::vector< CCfits::String >();

      for (unsigned int i=0; i<nCols; i++)
      {
        colNames.push_back(columns[0][i]);
        colForms.push_back(columns[1][i]);
        colUnits.push_back(columns[2][i]);
      }

      CCfits::Table *table = outFits->addTable(hduName, nRows, colNames, colForms, colUnits, CCfits::BinaryTbl, 1);
      ///////////////////////////////////////////////////////////////////////

      // Create references to the valarrays of the catalog values
      for (unsigned int i=0; i<nCols; i++)
      {
        std::vector<double> &array = inputData[i];
        table->column(i+1).write(array, 0l);
      }

      delete outFits;

    }
    catch (CCfits::FitsException&)
    {
      std::cout<<"exception caught"<<std::endl;
    }
*/
    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};


MAIN_FOR(TWOD_MASS_WL_MapMaker::MapMakerParser)
