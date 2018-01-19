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
 * @file TWOD_MASS_WL_MapMaker/MapMakerParser.h
 * @date 04/28/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MAPMAKER_MAPMAKERPARSER_H
#define TWOD_MASS_WL_MAPMAKER_MAPMAKERPARSER_H

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include <string>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_MassMapping {
class ShearMap;
class ConvergenceMap;
class GlobalMap;
}

namespace TWOD_MASS_WL_MapMaker {

/**
 * @class MapMakerParser
 * @brief
 *
 */
class MapMakerParser : public Elements::Program {

public:

  /**
   * @brief Destructor
   */
  virtual ~MapMakerParser();

  /**
   * @brief Constructor
   */
  MapMakerParser();

  po::options_description defineSpecificProgramOptions() override;

  /**
    * @brief the main method to be called to use the full program
    * @return Elements::ExitCode::OK
    */
  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override;

  /**
    * @brief a method to parse input parameters args
    * @return true if parsing of parameters is ok, false if parameters are missing or wrong
    *
    * This method parses all input parameters and checks that nothing is missing or wrong
    *
    */
  bool parseInputParameters(std::map<std::string, po::variable_value>& args);

  /**
    * @brief a method to extract the map from the input parameters
    * @return true if a map could be extracted, false otherwise
    *
    * This method extracts a map (either from an input catalog, or directly from an input map).
    * To be called only after parseInputParameters(args) returned true
    *
    */
  bool extractMap();

private:

  TWOD_MASS_WL_MassMapping::ShearMap *m_ShearMap;
  TWOD_MASS_WL_MassMapping::ConvergenceMap *m_ConvergenceMap;

  std::string m_inputSSVcatalog;
  std::string m_inputFITScatalog;
  std::string m_outputFITSshearMap;
  std::string m_outputFITSconvergenceMap;
  std::string m_outputFITSdensityMap;
  float m_raMin;
  float m_raMax;
  float m_decMin;
  float m_decMax;
  float m_zMin;
  float m_zMax;
  unsigned int m_nbBinsX;
  unsigned int m_nbBinsY;

  std::string m_workDir;

  bool squareMap;

  clock_t tStart = clock();

}; /* End of MapMakerParser class */

} /* namespace TWOD_MASS_WL_MapMaker */

#endif
