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
 * @file src/lib/MassMappingParser.cpp
 * @date 04/28/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/MassMappingParser.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "TWOD_MASS_WL_MassMapping/InPaintingAlgo.h"

#include <boost/program_options.hpp>

#include <time.h>

namespace po = boost::program_options;


namespace TWOD_MASS_WL_MassMapping {

MassMappingParser::~MassMappingParser()
{
  // Delete the pointers if not done yet
  if (m_ConvergenceMap!=nullptr)
  {
    delete m_ConvergenceMap;
    m_ConvergenceMap = nullptr;
  }
  if (m_ShearMap!=nullptr)
  {
    delete m_ShearMap;
    m_ShearMap = nullptr;
  }
}

MassMappingParser::MassMappingParser(): m_ShearMap(nullptr), m_ConvergenceMap(nullptr), m_inputFITSshearMap(""),
    m_inputFITSconvergenceMap(""), m_outputFITSshearMap(""), m_outputFITSconvergenceMap(""), m_workDir(""),
    m_getMeanConv(false), m_getMeanShear(false), m_removeOffsetConv(false), m_removeOffsetShear(false),
    m_sigmaXconv(0.), m_sigmaYconv(0.), m_sigmaXshear(0.), m_sigmaYshear(0.), m_bModes(false), m_addBorders(false),
    m_sigmaBounded(false), m_nbScales(0), m_minThreshold(0.), m_maxThreshold(-10.), m_numberIter(0)
{
}

po::options_description MassMappingParser::defineSpecificProgramOptions()
{
  po::options_description options {};
  //
  // !!! Implement the program options here !!!
  //
  options.add_options()

      ("m_workDir", po::value<std::string>(), "only for pipeline")
      ("logdir", po::value<std::string>(), "only for pipeline")

      ("inputShearMapFITS", po::value<std::string>(), "input FITS shear map")
      ("inputConvMapFITS", po::value<std::string>(), "input FITS convergence map")

      ("outputConvMapFITS", po::value<std::string>(), "output file in which to save the convergence map")
      ("outputShearMapFITS", po::value<std::string>(), "output file in which to save the shear map")

      ("sigmaConvMap", po::value<std::vector<float> >()->multitoken(),
       "sigma to apply gaussian filter to convergence map (default none)")
      ("sigmaShearMap", po::value<std::vector<float> >()->multitoken(),
       "sigma to apply gaussian filter to shear map (default none)")
/*
      ("getMeanConvMap", po::value<int>()->default_value(0),
       "set to 1 to get the mean values of the shear map (default 0)")
      ("getMeanShearMap", po::value<int>()->default_value(0),
       "set to 1 to get the mean values of the shear map (default 0)")

      ("removeOffsetConvMap", po::value<int>()->default_value(0),
       "set to 1 to remove the offset value of the convergence map (default 0)")
      ("removeOffsetShearMap", po::value<int>()->default_value(0),
       "set to 1 to remove the offset value of the shear map (default 0)")
*/
      ("numberIteration", po::value<int>()->default_value(0),
       "number of iterations used in the inpainting algo")
      ("sigmaBounded", po::value<int>()->default_value(0),
       "set to 1 to force variance in and out of mask (default 0)")
      ("numberScales", po::value<int>()->default_value(0),
       "number of scales used in the inpainting forced variance (default is automatically computed)")
      ("thresholdInpainting", po::value<std::vector<float> >()->multitoken(),
       "threshold min and max to apply for the inpainting (default 0. and max value)")


      ("addBorders", po::value<int>()->default_value(0),
       "set to 1 to add borders to the map during the computation (default 0)")
      ("bModeZeros", po::value<int>()->default_value(0),
       "set to 1 to force B-mode to zeros during inpainting iterations (default 0)");

  return options;
}

Elements::ExitCode MassMappingParser::mainMethod(std::map<std::string, po::variable_value>& args)
{
  // Perform the parsing of input parameters
  if (parseInputParameters(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  // perform 2D mass mapping if needed
  if (perform2DMassMapping(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  // In case inpainting is asked perform it
  if (m_numberIter!=0)
  {
    // perform InPainting
    if (performInPainting(args) == false)
    {
      return Elements::ExitCode::OK;
    }
  }
  else //otherwise delete map pointers
  {
    // Delete the pointers and leave
    if (m_ConvergenceMap!=nullptr)
    {
      delete m_ConvergenceMap;
      m_ConvergenceMap = nullptr;
    }
    if (m_ShearMap!=nullptr)
    {
      delete m_ShearMap;
      m_ShearMap = nullptr;
    }
  }
  return Elements::ExitCode::OK;
}


bool MassMappingParser::parseInputParameters(std::map<std::string, po::variable_value>& args)
{
  // Reinitialize the counter to zero in case of multiple call of this method
  unsigned int countInputs = 0;
  std::map<std::string, po::variable_value>::iterator it;
  for (it = args.begin(); it!= args.end(); ++it)
  {
    if (it->first=="m_workDir")
    {
      m_workDir = args["m_workDir"].as<std::string>() + std::string("/");
    }
    else if (it->first=="inputShearMapFITS")
    {
      m_inputFITSshearMap = args["inputShearMapFITS"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of inputShearMapFITS: "<<m_inputFITSshearMap<<std::endl;
    }
    else if (it->first=="outputConvMapFITS")
    {
      m_outputFITSconvergenceMap = args["outputConvMapFITS"].as<std::string>();
      //        std::cout<<"value of outputConvergenceMapFITS: "<<m_outputFITSconvergenceMap<<std::endl;
    }
    else if (it->first=="inputConvMapFITS")
    {
      m_inputFITSconvergenceMap = args["inputConvMapFITS"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of m_inputFITSconvergenceMap: "<<m_inputFITSconvergenceMap<<std::endl;
    }
    else if (it->first=="outputShearMapFITS")
    {
      m_outputFITSshearMap = args["outputShearMapFITS"].as<std::string>();
      //        std::cout<<"value of m_outputFITSshearMap: "<<m_outputFITSshearMap<<std::endl;
    }
    else if (it->first=="sigmaConvMap")
    {
      std::vector<float> sigmaXY = args["sigmaConvMap"].as<std::vector<float> >();
      m_sigmaXconv = sigmaXY[0];
      if (sigmaXY.size()>1)
      {
        m_sigmaYconv = sigmaXY[1];
      }
      else
      {
        m_sigmaYconv = m_sigmaXconv;
      }
      //      std::cout<<"value of conv sigma x and y: "<<sigmaXconv<<" "<<sigmaYconv<<std::endl;
    }
    else if (it->first=="sigmaShearMap")
    {
      std::vector<float> sigmaXY = args["sigmaShearMap"].as<std::vector<float> >();
      m_sigmaXshear = sigmaXY[0];
      if (sigmaXY.size()>1)
      {
        m_sigmaYshear = sigmaXY[1];
      }
      else
      {
        m_sigmaYshear = m_sigmaXshear;
      }
      //      std::cout<<"value of shear sigma x and y: "<<sigmaXshear<<" "<<sigmaYshear<<std::endl;
    }
    else if (it->first=="getMeanConvMap")
    {
      if (args["getMeanConvMap"].as<int>()==1)
      {
        m_getMeanConv = true;
      }
    }
    else if (it->first=="getMeanShearMap")
    {
      if (args["getMeanShearMap"].as<int>()==1)
      {
        m_getMeanShear = true;
      }
    }
    else if (it->first=="removeOffsetConvMap")
    {
      if (args["removeOffsetConvMap"].as<int>()==1)
      {
        m_removeOffsetConv = true;
      }
    }
    else if (it->first=="removeOffsetShearMap")
    {
      if (args["removeOffsetShearMap"].as<int>()==1)
      {
        m_removeOffsetShear = true;
      }
    }
    else if (it->first=="numberIteration")
    {
      m_numberIter = args["numberIteration"].as<int>();
    }
    else if (it->first=="sigmaBounded")
    {
      if (args["sigmaBounded"].as<int>()==1)
      {
        m_sigmaBounded = true;
      }
    }
    else if (it->first=="numberScales")
    {
        m_nbScales = args["numberScales"].as<int>();
    }
    else if (it->first=="thresholdInpainting")
    {
      std::vector<float> thresholdMinMax = args["thresholdInpainting"].as<std::vector<float> >();
      m_minThreshold = thresholdMinMax[0];
      if (thresholdMinMax.size()>1)
      {
        m_maxThreshold = thresholdMinMax[1];
      }
      else
      {
        m_maxThreshold = m_minThreshold;
      }
    }
    else if (it->first=="addBorders")
    {
      if (args["addBorders"].as<int>()==1)
      {
        m_addBorders = true;
      }
    }
    else if (it->first=="bModeZeros")
    {
      if (args["bModeZeros"].as<int>()==1)
      {
        m_bModes = true;
      }
    }
  }

  // If no input or multiple inputs are given: output error message
  if (countInputs!=1)
  {
    std::cout<<countInputs<<" input parameters were given while the expected number is 1"<<std::endl;
    return false;
  }

  return true;
}

bool MassMappingParser::perform2DMassMapping(std::map<std::string, po::variable_value>& args)
{

  /// Case of convergence map creation from input shear map
  if (m_inputFITSshearMap.empty() == false)
  {
    // Create a ShearMap
    m_ShearMap = new ShearMap(m_workDir+m_inputFITSshearMap);
    if (m_ShearMap==nullptr)
    {
      return false;
    }

    // Perform processing of the shear map
    processShearMap(args);

    // In case borders have to be added
    if (m_addBorders)
    {
      m_ShearMap->addBorders();
    }

    // Perform K&S inversion
    tStart = clock();
    m_ConvergenceMap = new ConvergenceMap(m_ShearMap->getConvergenceMap());
    std::cout<<"time to get the conv map out of the shear map: ";
    std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

    // Perform processing of the convergence map
    processConvergenceMap(args);

  }
  /// Case of shear map creation from input convergence map
  else if (m_inputFITSconvergenceMap.empty() == false)
  {
    // Create a ConvergenceMap
    m_ConvergenceMap = new ConvergenceMap(m_workDir+m_inputFITSconvergenceMap);
    if (m_ConvergenceMap==nullptr)
    {
      return false;
    }

    // Perform processing of the convergence map
    processConvergenceMap(args);

    // In case borders have to be added
    if (m_addBorders)
    {
      m_ConvergenceMap->addBorders();
    }

    // Perform K&S inversion
    tStart = clock();
    m_ShearMap = new ShearMap(m_ConvergenceMap->getShearMap());
    std::cout<<"time to get the shear map out of the conv map: ";
    std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

    // Perform processing of the shear map
    processShearMap(args);
  }
  // else case when both maps are nullptr
  else
  {
    std::cout<<"could not read a map from input map or catalog"<<std::endl;
    return false;
  }

/*
  // Delete the pointers and leave
  if (m_ConvergenceMap!=nullptr)
  {
    delete m_ConvergenceMap;
    m_ConvergenceMap = nullptr;
  }
  if (m_ShearMap!=nullptr)
  {
    delete m_ShearMap;
    m_ShearMap = nullptr;
  }
*/
  return true;
}

void MassMappingParser::processConvergenceMap(std::map<std::string, po::variable_value>& args)
{
  // if needed apply a gaussian filter to the convergence map
   if (fabs(m_sigmaXconv)>0.001)
   {
     tStart = clock();
     m_ConvergenceMap->applyGaussianFilter(m_sigmaXconv, m_sigmaYconv);
     std::cout<<"time to apply the gaussian filter to the map: ";
     std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;
   }

   // If the offset has to be removed from the map
   if (m_removeOffsetConv)
   {
     m_ConvergenceMap->removeOffset(m_ConvergenceMap->getMeanValues());
   }

   // If an output file for the convergence map is provided then save it
   if (m_outputFITSconvergenceMap.empty()==false)
   {
     if (m_ConvergenceMap->saveToFITSfile(m_workDir + m_outputFITSconvergenceMap, true, args)==false)
     {
       std::cout<<"could not save the convergence map to a FITS file"<<std::endl;
     }
   }

   // If the mean of the map if asked then display it
   if (m_getMeanConv)
   {
     std::vector<double> tmpMean = m_ConvergenceMap->getMeanValues();
     std::cout<<"mean value of E-mode: "<<tmpMean[0]<<std::endl;
     std::cout<<"mean value of B-mode: "<<tmpMean[1]<<std::endl;
   }
}

void MassMappingParser::processShearMap(std::map<std::string, po::variable_value>& args)
{
  // if needed apply a gaussian filter to the shear map
  if (fabs(m_sigmaXshear)>0.001)
  {
    tStart = clock();
    m_ShearMap->applyGaussianFilter(m_sigmaXshear, m_sigmaYshear);
    std::cout<<"time to apply the gaussian filter to the map: ";
    std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;
  }

  // If the offset has to be removed from the map
  if (m_removeOffsetShear)
  {
    m_ShearMap->removeOffset(m_ShearMap->getMeanValues());
  }

  // If an output file is provided for the shear map then save it
  if (m_outputFITSshearMap.empty()==false)
  {
    if (m_ShearMap->saveToFITSfile(m_workDir + m_outputFITSshearMap, true, args)==false)
    {
      std::cout<<"could not save the shear map to a FITS file"<<std::endl;
    }
  }

  // If the mean of the map if asked then display it
  if (m_getMeanShear)
  {
    std::vector<double> tmpMean = m_ShearMap->getMeanValues();
    std::cout<<"mean value of G1: "<<tmpMean[0]<<std::endl;
    std::cout<<"mean value of G2: "<<tmpMean[1]<<std::endl;
  }
}

bool MassMappingParser::performInPainting(std::map<std::string, po::variable_value>& args)
{
  // First check maps are well provided and shear map is input
  if (m_ShearMap==nullptr || m_ConvergenceMap==nullptr || m_inputFITSshearMap.empty() == true)
  {
    return false;
  }

  InPaintingAlgo myIPalgo(*m_ShearMap, *m_ConvergenceMap, m_nbScales, m_minThreshold, m_maxThreshold);
  ConvergenceMap *IPconvMap = myIPalgo.performInPaintingAlgo(m_numberIter, m_sigmaBounded, m_bModes);

  if (IPconvMap==nullptr)
  {
    delete m_ShearMap;
    delete m_ConvergenceMap;
    m_ShearMap = nullptr;
    m_ConvergenceMap = nullptr;
    return false;
  }

  // In case borders were added, remove them
  if (m_addBorders)
  {
    IPconvMap->removeBorders();
  }

  IPconvMap->saveToFITSfile(m_workDir + m_outputFITSconvergenceMap, true, args);

  delete m_ShearMap;
  delete m_ConvergenceMap;
  delete IPconvMap;
  m_ShearMap = nullptr;
  m_ConvergenceMap = nullptr;
  IPconvMap = nullptr;

  return true;
}

} // TWOD_MASS_WL_MassMapping namespace
