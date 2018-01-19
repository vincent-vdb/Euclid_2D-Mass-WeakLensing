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
 * @file TWOD_MASS_WL_PeakCount/PeakCountParser.h
 * @date 05/22/17
 * @author user
 */

#ifndef TWOD_MASS_WL_PEAKCOUNT_PEAKCOUNTPARSER_H
#define TWOD_MASS_WL_PEAKCOUNT_PEAKCOUNTPARSER_H

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include <string>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_MassMapping {
class ConvergenceMap;
}

namespace TWOD_MASS_WL_PeakCount {

/**
 * @class PeakCountParser
 * @brief
 *
 */
class PeakCountParser : public Elements::Program {

public:

  /**
   * @brief Destructor
   */
  virtual ~PeakCountParser() = default;

  /**
   * @brief Constructor
   */
  PeakCountParser();

  /**
   * @brief Method that defines all the available options
   */
  po::options_description defineSpecificProgramOptions() override;

   /**
     * @brief the main method to be called to use the full program
     * @return Elements::ExitCode::OK
     */
   Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override;

   /**
     * @brief a method to parse input parameters args
     * @return true if parsing of parameters is ok, false if parameters are missing or wrong
     */
   bool parseInputParameters(std::map<std::string, po::variable_value>& args);

   /**
     * @brief a method that creates the peak catalog
     * @return true if a peak catalog is well computed and created, false otherwise
     */
   bool createPeakCatalog();


private:

//   TWOD_MASS_WL_MassMapping::ConvergenceMap *myConvergenceMap = nullptr;

   std::string m_inputFITSconvergenceMap;
   std::string m_inputFITSdensMap;
   std::string m_outputPeakCatalogFITS;
   std::string m_workDir;

}; /* End of PeakCountParser class */

} /* namespace TWOD_MASS_WL_PeakCount */


#endif
