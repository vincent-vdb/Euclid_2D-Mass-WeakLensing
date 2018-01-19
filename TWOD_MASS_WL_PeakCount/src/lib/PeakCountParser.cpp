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
 * @file src/lib/PeakCountParser.cpp
 * @date 05/22/17
 * @author user
 */

#include "TWOD_MASS_WL_PeakCount/PeakCountParser.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "TWOD_MASS_WL_PeakCount/PeakCountAlgo.h"

namespace po = boost::program_options;

namespace TWOD_MASS_WL_PeakCount {

PeakCountParser::PeakCountParser(): m_inputFITSconvergenceMap(""), m_inputFITSdensMap(""),
    m_outputPeakCatalogFITS(""), m_workDir("")
{
}

po::options_description PeakCountParser::defineSpecificProgramOptions()
{
  po::options_description options {};
  //
  // !!! Implement the program options here !!!
  //
  options.add_options()

      ("workdir", po::value<std::string>(), "only for pipeline")
      ("logdir", po::value<std::string>(), "only for pipeline")
      ("inputConvMapFITS", po::value<std::string>(), "input FITS convergence map")
      ("inputDensityMapFITS", po::value<std::string>(), "input FITS density map")
      ("outputPeakCatalogFITS", po::value<std::string>(), "output file in which to save the convergence map");

  return options;
}

Elements::ExitCode PeakCountParser::mainMethod(std::map<std::string, po::variable_value>& args)
{
  // Perform the parsing of input parameters
  if (parseInputParameters(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  // perform the peak counting
  if (createPeakCatalog() == false)
  {
    return Elements::ExitCode::OK;
  }

  return Elements::ExitCode::OK;
}


bool PeakCountParser::parseInputParameters(std::map<std::string, po::variable_value>& args)
{
  // Reinitialize the counter to zero in case of multiple call of this method
  unsigned int countInputs = 0;
  std::map<std::string, po::variable_value>::iterator it;
  for (it = args.begin(); it!= args.end(); ++it)
  {
    if (it->first=="workdir")
    {
      m_workDir = args["workdir"].as<std::string>() + std::string("/");
    }
    else if (it->first=="inputConvMapFITS")
    {
      m_inputFITSconvergenceMap = args["inputConvMapFITS"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of m_inputFITSconvergenceMap: "<<m_inputFITSconvergenceMap<<std::endl;
    }
    else if (it->first=="inputDensityMapFITS")
    {
      m_inputFITSdensMap = args["inputDensityMapFITS"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of m_inputFITSdensMap: "<<m_inputFITSdensMap<<std::endl;
    }
    else if (it->first=="outputPeakCatalogFITS")
    {
      m_outputPeakCatalogFITS = args["outputPeakCatalogFITS"].as<std::string>();
      //        std::cout<<"value of m_outputPeakCatalogFITS: "<<m_outputPeakCatalogFITS<<std::endl;
      countInputs++;
    }
  }

  // If no input or multiple inputs are given: output error message
  if (countInputs!=3)
  {
    std::cout<<countInputs<<" input parameters were given while the expected number is 3"<<std::endl;
    return false;
  }

  return true;
}

bool PeakCountParser::createPeakCatalog()
{
  TWOD_MASS_WL_MassMapping::ConvergenceMap *myConvergenceMap = nullptr;

  // Create the convergence map
  if (m_inputFITSconvergenceMap.empty()==false)
  {
    myConvergenceMap = new TWOD_MASS_WL_MassMapping::ConvergenceMap(m_workDir+m_inputFITSconvergenceMap);
  }
  else
  {
    return false;
  }

  // If the map could not be opened return false
  if (myConvergenceMap==nullptr)
  {
    return false;
  }

  TWOD_MASS_WL_MassMapping::GlobalMap* myDensityMap = nullptr;

  // Get the related filename of the density map
  if (m_inputFITSdensMap.empty()==false)
  {
    myDensityMap = new TWOD_MASS_WL_MassMapping::GlobalMap(m_inputFITSdensMap);
  }

  if (myDensityMap==nullptr)
  {
    delete myConvergenceMap;
    myConvergenceMap = nullptr;
    return false;
  }

  // Then create a PeakCountAlgo object
  PeakCountAlgo myPeakCounter(*myConvergenceMap, *myDensityMap);

  bool catalogSaved = false;
  // Finally create the peak catalog
  if (m_outputPeakCatalogFITS.empty()==false)
  {
    catalogSaved = myPeakCounter.savePeakCatalog(m_workDir+m_outputPeakCatalogFITS);
  }

  delete myConvergenceMap;
  myConvergenceMap = nullptr;
  delete myDensityMap;
  myDensityMap = nullptr;

  return catalogSaved;
}

} // TWOD_MASS_WL_PeakCount namespace
