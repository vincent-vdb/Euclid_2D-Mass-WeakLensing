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
 * @file src/lib/PFAlgo.cpp
 * @date 06/27/17
 * @author user
 */

#include "TWOD_MASS_WL_Launcher/PFAlgo.h"
#include "TWOD_MASS_WL_MapMaker/MapMakerParser.h"
#include "TWOD_MASS_WL_MassMapping/MassMappingParser.h"
#include "TWOD_MASS_WL_PeakCount/PeakCountParser.h"
#include "TWOD_MASS_WL_CatalogSplitter/MaskSplitter.h"

#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_Launcher {


PFAlgo::PFAlgo(bool bModes, unsigned int nbIterInpainting, unsigned int nbIterReducedShear,
               unsigned int nbScaleInpainting, bool variancePerScale, float gaussianSmoothing, float denoisingVal,
               std::string inputFITSCatalog, std::string inputSSVCatalog, std::string outputPeakCatalog,
               std::string outputConvergenceMap, float raStep, float decStep,
               float zStep, bool squareMap, TWOD_MASS_WL_MassMapping::Boundaries boundaries):
                   m_bModes(bModes), m_nbIterInpainting(nbIterInpainting),
                   m_nbIterReducedShear(nbIterReducedShear), m_nbScaleInpainting(nbScaleInpainting),
                   m_variancePerScale(variancePerScale), m_gaussianSmoothing(gaussianSmoothing),
                   m_denoisingVal(denoisingVal), m_inputFITSCatalog(inputFITSCatalog),
                   m_inputSSVCatalog(inputSSVCatalog), m_outputPeakCatalog(outputPeakCatalog),
                   m_outputConvergenceMap(outputConvergenceMap), m_raStep(raStep),
                   m_decStep(decStep), m_zStep(zStep),
                   m_squareMap(squareMap), m_boundaries(boundaries)
{
}

bool PFAlgo::checkParameters()
{
  // If there is no catalog inputs return false
  if (m_inputSSVCatalog.empty()==true && m_inputFITSCatalog.empty()==true)
  {
    return false;
  }

  // If there are two catalogs input return false
  if (m_inputSSVCatalog.empty()==false && m_inputFITSCatalog.empty()==false)
  {
    return false;
  }

  // If there is no output provided then return false
  if (m_outputPeakCatalog.empty()==true && m_outputConvergenceMap.empty()==true)
  {
    return false;
  }

  // If the boundaries are needed... (no step provided)
  if (fabs(m_raStep)<0.000001 || fabs(m_decStep)<0.000001 || fabs(m_zStep)<0.000001)
  {
    // And if the boundaries are not properly given then return false
    if (m_boundaries.getRaMin()>=m_boundaries.getRaMax() ||
        m_boundaries.getDecMin()>=m_boundaries.getDecMax() ||
        m_boundaries.getZMin()>=m_boundaries.getZMax())
    {
      return false;
    }
  }

  return true;
}

bool PFAlgo::performProcessingFunction(std::string outputPeakMap, std::string outputConvMap,
    TWOD_MASS_WL_MassMapping::Boundaries boundaries)
{
  // Check parameters are valid
  if (outputPeakMap.empty()==true && outputConvMap.empty()==true)
  {
    return false;
  }
  if (boundaries.getRaMin()>=boundaries.getRaMax() ||
      boundaries.getDecMin()>=boundaries.getDecMax() ||
      boundaries.getZMin()>=boundaries.getZMax())
  {
    return false;
  }

  // Create a map of parameters
  std::map<std::string, po::variable_value> params;

  // Define the place where to put the temporary shear map
  std::string shearMapFITSfile = "/tmp/tmpShearMapLauncher"
#ifdef _OPENMP
                                 +std::to_string(omp_get_thread_num())
#endif
                                 +".fits";

  /////////////////////////////////////
  /// Provide all needed parameters for the MapMaker module
  /////////////////////////////////////

  // Temp: add bins, ra and dec min max
  params["outputMapBinXY"] = po::variable_value(boost::any(std::vector<int>(2, 1024)), false);
  std::vector<float> raMinMax;
  raMinMax.push_back(boundaries.getRaMin());
  raMinMax.push_back(boundaries.getRaMax());
  params["raMinMax"] = po::variable_value(boost::any(raMinMax), false);
  std::vector<float> decMinMax;
  decMinMax.push_back(boundaries.getDecMin());
  decMinMax.push_back(boundaries.getDecMax());
  params["decMinMax"] = po::variable_value(boost::any(decMinMax), false);
  std::vector<float> zMinMax;
  zMinMax.push_back(boundaries.getZMin());
  zMinMax.push_back(boundaries.getZMax());
  params["zMinMax"] = po::variable_value(boost::any(zMinMax), false);
  params["squareMap"] = po::variable_value(boost::any(m_squareMap?int(1):int(0)), false);


  if (m_inputFITSCatalog.empty()==false)
  {
    params["inputFITSCatalog"] = po::variable_value(boost::any(m_inputFITSCatalog), false);
  }
  else if (m_inputSSVCatalog.empty()==false)
  {
    params["inputSSVCatalog"] = po::variable_value(boost::any(m_inputSSVCatalog), false);
  }
  params["outputShearMapFITS"] = po::variable_value(boost::any(std::string(shearMapFITSfile)), false);

  // Perform the map making from the catalog
  TWOD_MASS_WL_MapMaker::MapMakerParser myMapMakerParser;
  if (myMapMakerParser.mainMethod(params)!=Elements::ExitCode::OK)
  {
    return false;
  }

  /////////////////////////////////////
  /// Perform the reduced shear iterations
  /////////////////////////////////////

  if (m_nbIterReducedShear<1)
  {
    m_nbIterReducedShear = 1;
  }
  for (unsigned int reducedIter = 0; reducedIter<m_nbIterReducedShear; reducedIter++)
  {
    /////////////////////////////////////
    /// Provide all needed parameters for the MassMapping module
    /////////////////////////////////////

    // First change the output shear map to an input shear map
    params["inputShearMapFITS"] = po::variable_value(boost::any(std::string(shearMapFITSfile)), false);
    params.erase("outputShearMapFITS");

    // Then provide all other input parameters
    //  m_denoisingVal
    params["numberIteration"] = po::variable_value(boost::any(int(m_nbIterInpainting)), false);
    params["bModeZeros"] = po::variable_value(boost::any(m_bModes?int(1):int(0)), false);
    params["sigmaBounded"] = po::variable_value(boost::any(m_variancePerScale?1:0), false);
    params["outputConvMapFITS"] = po::variable_value(boost::any(outputConvMap), false);
    params["numberScales"] = po::variable_value(boost::any(int(m_nbScaleInpainting)), false);
    params["sigmaConvMap"] = po::variable_value(boost::any(std::vector<float>(2, m_gaussianSmoothing)), false);
    params["ReducedShearIteration"] = po::variable_value(boost::any(int(m_nbIterReducedShear)), false);

    // Perform the mass mapping
    TWOD_MASS_WL_MassMapping::MassMappingParser myMassMappingParser;
    if (myMassMappingParser.mainMethod(params)!=Elements::ExitCode::OK)
    {
      return false;
    }

    /////////////////////////////////////
    // Then perform reduced shear computation
    /////////////////////////////////////
    TWOD_MASS_WL_MassMapping::ShearMap reducedShear(shearMapFITSfile);
    TWOD_MASS_WL_MassMapping::ConvergenceMap convMap(outputConvMap);

    reducedShear.computeReducedShear(convMap);

    reducedShear.saveToFITSfile(shearMapFITSfile, true);
  }

  /////////////////////////////////////
  /// Provide all needed parameters for the peak count module
  /////////////////////////////////////

  // First change the output conv map to an input conv map
  params["inputConvMapFITS"] = po::variable_value(boost::any(outputConvMap), false);
  params.erase("outputConvMapFITS");

  // And provide the input density map
  std::size_t found =  shearMapFITSfile.rfind(".");
  shearMapFITSfile.insert(found, "_density");
  params["inputDensityMapFITS"] = po::variable_value(boost::any(shearMapFITSfile), false);

  // Then provide the ouput catalog
  params["outputPeakCatalogFITS"] = po::variable_value(boost::any(outputPeakMap), false);

  // Perform the peak counting
  TWOD_MASS_WL_PeakCount::PeakCountParser myPeakCountParser;
  if (myPeakCountParser.mainMethod(params)!=Elements::ExitCode::OK)
  {
    return false;
  }

  return true;
}


bool PFAlgo::launchParalPF()
{
  // If only one patch has to be processed, do it directly
  if (fabs(m_raStep)<0.000001 || fabs(m_decStep)<0.000001 || fabs(m_zStep)<0.000001)
  {
    if (checkParameters()==true)
    {
      return performProcessingFunction(m_outputPeakCatalog, m_outputConvergenceMap, m_boundaries);
    }
    else
    {
      return false;
    }
  }

   // Create a catalog splitter
   TWOD_MASS_WL_CatalogSplitter::MaskSplitter mySplitter(m_inputFITSCatalog, m_raStep, m_decStep, m_zStep);
   std::vector<std::pair<float, float> > myTmpMask = mySplitter.fillBasicMask();

 //  for (unsigned int i=0; i<myTmpMask.size(); i++)
 //  {
 //    std::cout<<myTmpMask[i].first<<"\t"<<myTmpMask[i].second<<std::endl;
 //  }

   std::vector<TWOD_MASS_WL_MassMapping::Boundaries> goodPatches = mySplitter.getBoundariesFromBasicMask(myTmpMask);
 //  for (unsigned int i=0; i<goodPatches.size(); i++)
 //  {
 //    std::cout<<goodPatches[i].getRaMin()<<"\t"<<goodPatches[i].getRaMax()<<std::endl;
 //  }

 /*
   // Get all patches with data
   std::vector<TWOD_MASS_WL_MassMapping::Boundaries> allPatches = mySplitter.getBoundsOfPatches();
   std::cout<<"getting mask"<<std::endl;
   clock_t tStart = clock();
   std::vector<TWOD_MASS_WL_MassMapping::Boundaries> goodPatches = mySplitter.getMaskOfCatalog(allPatches);
   std::cout<<"time to get the mask: ";
   std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;
   std::cout<<"getting mask done: "<<goodPatches.size()<<" patches"<<std::endl;
 */
   // Define a boolean telling if it all went fine
   bool outputOK = true;

   // Perform the parallelized PF computation
   #pragma omp parallel for num_threads(omp_get_max_threads())
   for (unsigned int i=0; i<goodPatches.size(); i++)
   {
     // Give the output catalog and convergence map a unique filename
     std::string tmpOutputConvMap = m_outputConvergenceMap;
     std::string tmpOutputPeakCatalog = m_outputPeakCatalog;
     if (tmpOutputConvMap.empty()==false)
     {
       std::size_t found = tmpOutputConvMap.rfind(".fits");
       tmpOutputConvMap.insert(found, std::to_string(i));
       std::cout<<"name of the conv map: "<<tmpOutputConvMap<<std::endl;
     }
     if (tmpOutputPeakCatalog.empty()==false)
     {
       std::size_t found = tmpOutputPeakCatalog.rfind(".fits");
       tmpOutputPeakCatalog.insert(found, std::to_string(i));
       std::cout<<"name of the peak catalog: "<<tmpOutputPeakCatalog<<std::endl;
     }


     if (checkParameters()==true)
     {
       performProcessingFunction(tmpOutputPeakCatalog, tmpOutputConvMap, goodPatches[i]);
     }
     else
     {
       outputOK = false;
     }
   }

   return outputOK;
}


} // TWOD_MASS_WL_Launcher namespace
