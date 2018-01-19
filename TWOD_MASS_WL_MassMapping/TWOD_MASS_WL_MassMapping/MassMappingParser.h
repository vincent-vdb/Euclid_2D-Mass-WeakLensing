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
 * @file TWOD_MASS_WL_MassMapping/MassMappingParser.h
 * @date 04/28/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MASSMAPPING_MASSMAPPINGPARSER_H
#define TWOD_MASS_WL_MASSMAPPING_MASSMAPPINGPARSER_H

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include <string>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_MassMapping {

class ShearMap;
class ConvergenceMap;

/**
 * @class MassMappingParser
 * @brief
 *
 */
class MassMappingParser : public Elements::Program {

public:

  /**
   * @brief Destructor
   */
  virtual ~MassMappingParser();

  /**
   * @brief Constructor
   */
  MassMappingParser();


  /**
   * @brief Method that defines all the available options
   */
  po::options_description defineSpecificProgramOptions() override;

   /**
     * @brief the main method to be called to use the full program
     * @return Elements::ExitCode::OK
     *
     * This method performs all possible 2D mass mapping depending on the input parameters
     *
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
     * @brief a method to perform 2D mass mapping on an input map
     * @return true if the mass mapping went well, false otherwise
     *
     * This method processes if needed the input map, performs K&S on it, and
     * then processes the output map.
     * To be called only after parseInputParameters(args) and extractMap() returned true
     *
     */
   bool perform2DMassMapping(std::map<std::string, po::variable_value>& args);

   /**
     * @brief a method to perform processing of a ConvergenceMap
     *
     * This method processes the ConvergenceMap if any, by applying as needed
     * gaussian filtering, removing offset, saving the map and displaying mean values
     *
     */
   void processConvergenceMap(std::map<std::string, po::variable_value>& args);

   /**
     * @brief a method to perform processing of a ShearMap
     *
     * This method processes the ShearMap if any, by applying as needed
     * gaussian filtering, removing offset, saving the map and displaying mean values
     *
     */
   void processShearMap(std::map<std::string, po::variable_value>& args);

   /**
     * @brief a method to perform inpainting
     *
     * This method computes the inpainting
     *
     */
   bool performInPainting(std::map<std::string, po::variable_value>& args);


private:

   ShearMap *m_ShearMap;
   ConvergenceMap *m_ConvergenceMap;

   std::string m_inputFITSshearMap;
   std::string m_inputFITSconvergenceMap;
   std::string m_outputFITSshearMap;
   std::string m_outputFITSconvergenceMap;

   std::string m_workDir;

   bool m_getMeanConv;
   bool m_getMeanShear;

   bool m_removeOffsetConv;
   bool m_removeOffsetShear;

   float m_sigmaXconv;
   float m_sigmaYconv;
   float m_sigmaXshear;
   float m_sigmaYshear;

   bool m_bModes;
   bool m_addBorders;
   bool m_sigmaBounded;
   unsigned int m_nbScales;

   float m_minThreshold;
   float m_maxThreshold;

   unsigned int m_numberIter;

   clock_t tStart = clock();

}; /* End of MassMappingParser class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
