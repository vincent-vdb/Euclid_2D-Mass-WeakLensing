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
 * @file TWOD_MASS_WL_CatalogSplitter/CatalogSplitterParser.h
 * @date 07/27/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_CATALOGSPLITTER_CATALOGSPLITTERPARSER_H
#define _TWOD_MASS_WL_CATALOGSPLITTER_CATALOGSPLITTERPARSER_H

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include <string>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_CatalogSplitter {

/**
 * @class CatalogSplitterParser
 * @brief
 *
 */
class CatalogSplitterParser : public Elements::Program {

public:

  /**
   * @brief Destructor
   */
  virtual ~CatalogSplitterParser() = default;


  /**
   * @brief Constructor
   */
  CatalogSplitterParser();

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
     *
     * This method parses all input parameters and checks that nothing is missing or wrong
     *
     */
   bool parseInputParameters(std::map<std::string, po::variable_value>& args);

private:

   std::string m_inputFITSCatalog;
   std::string m_outputFITSCatalogRoot;
   std::string m_workDir;
   unsigned int m_nbCatalogsRa;
   unsigned int m_nbCatalogsDec;
   unsigned int m_nbCatalogsZ;
   float m_raMin;
   float m_raMax;
   float m_decMin;
   float m_decMax;
   float m_zMin;
   float m_zMax;

}; /* End of CatalogSplitterParser class */

} /* namespace TWOD_MASS_WL_CatalogSplitter */


#endif
