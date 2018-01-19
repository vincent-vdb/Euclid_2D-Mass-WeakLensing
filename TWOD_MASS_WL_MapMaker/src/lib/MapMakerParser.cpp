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
 * @file src/lib/MapMakerParser.cpp
 * @date 04/28/17
 * @author user
 */

#include "TWOD_MASS_WL_MapMaker/MapMakerParser.h"
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MapMaker/SSVCatalogHandler.h"
#include "TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include <boost/program_options.hpp>

#include <time.h>

namespace po = boost::program_options;

using namespace TWOD_MASS_WL_MassMapping;

namespace TWOD_MASS_WL_MapMaker {

MapMakerParser::~MapMakerParser()
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

MapMakerParser::MapMakerParser(): m_ShearMap(nullptr), m_ConvergenceMap(nullptr), m_inputSSVcatalog(""),
m_inputFITScatalog(""), m_outputFITSshearMap(""), m_outputFITSconvergenceMap(""), m_outputFITSdensityMap(""),
m_raMin(360.), m_raMax(0.), m_decMin(90.), m_decMax(-90.), m_zMin(0.), m_zMax(100.), m_nbBinsX(0), m_nbBinsY(0),
m_workDir(""), squareMap(true)
{
}


po::options_description MapMakerParser::defineSpecificProgramOptions()
{
  po::options_description options {};
  //
  // !!! Implement the program options here !!!
  //
  options.add_options()

      ("workdir", po::value<std::string>(), "only for pipeline")
      ("logdir", po::value<std::string>(), "only for pipeline")


      ("inputFITSCatalog", po::value<std::string>(), "input FITS catalog to read")
      ("inputSSVCatalog", po::value<std::string>(), "input SSV catalog to read")

      ("outputConvMapFITS", po::value<std::string>(), "output file in which to save the convergence map")
      ("outputShearMapFITS", po::value<std::string>(), "output file in which to save the shear map")
      ("outputShearDensityMapFITS", po::value<std::string>(),
       "output file in which to save the density map (default is [outputShearMapFITS]_density.fits)")

      ("outputMapBinXY", po::value<std::vector<int> >()->multitoken(),
       "number of bins X and Y into the output map")
      ("raMinMax", po::value<std::vector<float> >()->multitoken(),
       "right ascension min and max values for which to extract the map")
      ("decMinMax", po::value<std::vector<float> >()->multitoken(),
       "declination min and max values for which to extract the map")
      ("zMinMax", po::value<std::vector<float> >()->multitoken(),
       "redshift min and max values for which to extract the map")
      ("pixelSize", po::value<std::vector<float> >()->multitoken(),
       "physical pixel size in degrees")
      ("mapSize", po::value<std::vector<float> >()->multitoken(),
       "physical size in degrees of the map")
      ("mapCenter", po::value<std::vector<float> >()->multitoken(),
       "physical center in degrees of the map");
//      ("squareMap", po::value<int>()->default_value(0),
//       "set to 1 to force the map to be square with the given radec on the projection plan (default 0)");


  return options;

}

Elements::ExitCode MapMakerParser::mainMethod(std::map<std::string, po::variable_value>& args)
{
  // Perform the parsing of input parameters
  if (parseInputParameters(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  // extract the map from catalog or from files
  if (extractMap() == false)
  {
    return Elements::ExitCode::OK;
  }

  return Elements::ExitCode::OK;
}


bool MapMakerParser::parseInputParameters(std::map<std::string, po::variable_value>& args)
{
  // Reinitialize the counter to zero in case of multiple call of this method
  unsigned int countInputs(0);

  float mapSizeX(0);
  float mapSizeY(0);
  float pixelSizeX(0);
  float pixelSizeY(0);
  float mapCenterX(0);
  float mapCenterY(0);

  std::map<std::string, po::variable_value>::iterator it;
  for (it = args.begin(); it!= args.end(); ++it)
  {
    if (it->first=="workdir")
    {
      m_workDir = args["workdir"].as<std::string>() + std::string("/");
    }
    else if (it->first=="inputFITSCatalog")
    {
      m_inputFITScatalog = args["inputFITSCatalog"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of inputFitsCatalog: "<<inputFITScatalog<<std::endl;
    }
    else if (it->first=="inputSSVCatalog")
    {
      m_inputSSVcatalog = args["inputSSVCatalog"].as<std::string>();
      countInputs++;
      //        std::cout<<"value of inputSSVCatalog: "<<inputSSVcatalog<<std::endl;
    }
    else if (it->first=="outputConvMapFITS")
    {
      m_outputFITSconvergenceMap = args["outputConvMapFITS"].as<std::string>();
      //        std::cout<<"value of outputConvergenceMapFITS: "<<m_outputFITSconvergenceMap<<std::endl;
    }
    else if (it->first=="outputShearMapFITS")
    {
      m_outputFITSshearMap = args["outputShearMapFITS"].as<std::string>();
      //        std::cout<<"value of outputFITSshearMap: "<<m_outputFITSshearMap<<std::endl;
    }
    else if (it->first=="outputShearDensityMapFITS")
    {
      m_outputFITSdensityMap = args["outputShearDensityMapFITS"].as<std::string>();
    }
    else if (it->first=="outputMapBinXY")
    {
      std::vector<int> binsXY = args["outputMapBinXY"].as<std::vector<int> >();
      m_nbBinsX = binsXY[0];
      if (binsXY.size()>1)
      {
        m_nbBinsY = binsXY[1];
      }
      else
      {
        m_nbBinsY = m_nbBinsX;
      }
      //        std::cout<<"value of bins X and Y: "<<m_nbBinsX<<" "<<m_nbBinsY<<std::endl;
    }
    else if (it->first=="raMinMax")
    {
      std::vector<float> raMinMax = args["raMinMax"].as<std::vector<float> >();
      m_raMin = raMinMax[0];
      if (raMinMax.size()>1)
      {
        m_raMax = raMinMax[1];
      }
      else
      {
        std::cout<<"only value of ra min provided, not ra max"<<std::endl;
        return false;
      }
      //        std::cout<<"value of ra min and max: "<<m_raMin<<" "<<m_raMax<<std::endl;
    }
    else if (it->first=="decMinMax")
    {
      std::vector<float> decMinMax = args["decMinMax"].as<std::vector<float> >();
      m_decMin = decMinMax[0];
      if (decMinMax.size()>1)
      {
        m_decMax = decMinMax[1];
      }
      else
      {
        std::cout<<"only value of dec min provided, not dec max"<<std::endl;
        return false;
      }
      //        std::cout<<"value of dec min and max: "<<m_decMin<<" "<<m_decMax<<std::endl;
    }
    else if (it->first=="zMinMax")
    {
      std::vector<float> zMinMax = args["zMinMax"].as<std::vector<float> >();
      m_zMin = zMinMax[0];
      if (zMinMax.size()>1)
      {
        m_zMax = zMinMax[1];
      }
      else
      {
        std::cout<<"only value of z min provided, not z max"<<std::endl;
        return false;
      }
      //        std::cout<<"value of dec min and max: "<<m_decMin<<" "<<m_decMax<<std::endl;
    }
    else if (it->first=="pixelSize")
    {
      std::vector<float> pixelSize = args["pixelSize"].as<std::vector<float> >();
      pixelSizeX = pixelSize[0];
      if (pixelSize.size()>1)
      {
        pixelSizeY = pixelSize[1];
      }
      else
      {
        pixelSizeY = pixelSizeX;
      }
    }
    else if (it->first=="mapSize")
    {
      std::vector<float> mapSize = args["mapSize"].as<std::vector<float> >();
      mapSizeX = mapSize[0];
      if (mapSize.size()>1)
      {
        mapSizeY = mapSize[1];
      }
      else
      {
        mapSizeY = mapSizeX;
      }
    }
    else if (it->first=="mapCenter")
    {
      std::vector<float> mapCenter = args["mapCenter"].as<std::vector<float> >();
      mapCenterX = mapCenter[0];
      if (mapCenter.size()>1)
      {
        mapCenterY = mapCenter[1];
      }
      else
      {
        mapCenterY = mapCenterX;
      }
    }
    else if (it->first=="squareMap")
    {
      if (args["squareMap"].as<int>()==1)
      {
        squareMap=true;
      }
    }
  }

  // If no input or multiple inputs are given: output error message
  if (countInputs!=1)
  {
    std::cout<<countInputs<<" input catalog files were given while the expected number is 1"<<std::endl;
    return false;
  }

  // in case the provided inputs are with center and pixel size make conversion
  if (fabs(mapSizeX)>0 && fabs(mapSizeY)>0 && fabs(pixelSizeX)>0 && fabs(pixelSizeY)>0 && fabs(mapCenterX)>0)
  {
    m_raMin = mapCenterX - mapSizeX/2.;
    m_raMax = m_raMin + mapSizeX;
    m_decMin = mapCenterY - mapSizeY/2.;
    m_decMax = m_decMin + mapSizeY;
    m_nbBinsX = int(mapSizeX/pixelSizeX);
    m_nbBinsY = int(mapSizeY/pixelSizeY);
  }

  // Check the ra, dec and bins are well provided
  if (m_raMin<0. || m_raMin>360.)
  {
    std::cout<<"value of the min right ascension to take on the catalog is not valid"<<std::endl;
    return false;
  }
  if (m_raMax<0. || m_raMax>360)
  {
    std::cout<<"value of the max right ascension to take on the catalog is not valid"<<std::endl;
    return false;
  }
  if (m_decMin<-90 || m_decMin>90)
  {
    std::cout<<"value of the min declination to take on the catalog is not valid"<<std::endl;
    return false;
  }
  if (m_decMax<-90 || m_decMax>90)
  {
    std::cout<<"value of the max declination to take on the catalog is not valid"<<std::endl;
    return false;
  }
  if (m_nbBinsX==0)
  {
    std::cout<<"value of the X bins to make the map out of the catalog is not valid"<<std::endl;
    return false;
  }
  if (m_nbBinsY==0)
  {
    std::cout<<"value of the Y bins to make the map out of the catalog is not valid"<<std::endl;
    return false;
  }

  // Check an output file for the shear or the convergence map is provided
  if (m_outputFITSshearMap.empty() && m_outputFITSconvergenceMap.empty())
  {
    std::cout<<"no output file is provided to save an extracted map"<<std::endl;
    return false;
  }

  // If both shear and convergence want to be extracted: not handled
  if (m_outputFITSshearMap.empty() == false && m_outputFITSconvergenceMap.empty() == false)
  {
    std::cout<<"both shear and convergence maps can not be extracted at the same time"<<std::endl;
    return false;
  }

  return true;
}

bool MapMakerParser::extractMap()
{

  // Create a Boundaries object to gather ra, dec and z data
  TWOD_MASS_WL_MassMapping::Boundaries bounds(m_raMin, m_raMax, m_decMin, m_decMax, m_zMin, m_zMax);

  // if the density map name is not provided give the default one
  if (m_outputFITSdensityMap.empty() == true)
  {
    if (m_outputFITSshearMap.empty() == false)
    {
      std::size_t found =  m_outputFITSshearMap.rfind(".");
      m_outputFITSdensityMap = m_outputFITSshearMap;
      m_outputFITSdensityMap.insert(found, "_density");
    }
    else if (m_outputFITSconvergenceMap.empty() == false)
    {
      std::size_t found =  m_outputFITSconvergenceMap.rfind(".");
      m_outputFITSdensityMap = m_outputFITSconvergenceMap;
      m_outputFITSdensityMap.insert(found, "_density");
    }
  }

  /// Case of a SSV catalog: retrieve the needed map out of it
  if (m_inputSSVcatalog.empty()==false)
  {
    // Create the catalog handler
    SSVCatalogHandler mySSVhandler(m_workDir + m_inputSSVcatalog);

    // In case the shear map has to be extracted
    if (m_outputFITSshearMap.empty() == false)
    {
      tStart = clock();
      m_ShearMap = mySSVhandler.getShearMap(bounds, m_nbBinsX, m_nbBinsY, squareMap);

      std::cout<<"time to extract the shear map of the SSV catalog: ";
      std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

      // In case no shear map could be extracted from the catalog warn the user
      if (m_ShearMap==nullptr)
      {
        std::cout<<"needed information to build a shear map not provided into the catalog"<<std::endl;
        return false;
      }

      // Save the extracted map
      m_ShearMap->saveToFITSfile(m_workDir + m_outputFITSshearMap, true);

      // Save the density map if any
      if (mySSVhandler.getDensityMap()!=nullptr)
      {
        mySSVhandler.getDensityMap()->saveToFITSfile(m_workDir + m_outputFITSdensityMap, true);
      }
    }
    // In case the convergence map has to be extracted
    else if (m_outputFITSconvergenceMap.empty() == false)
    {
      tStart = clock();
      m_ConvergenceMap = mySSVhandler.getConvergenceMap(bounds, m_nbBinsX, m_nbBinsY, squareMap);

      std::cout<<"time to extract the convergence map of the SSV catalog: ";
      std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

      // In case no shear map could be extracted from the catalog warn the user
      if (m_ConvergenceMap==nullptr)
      {
        std::cout<<"needed information to build a convergence map not provided into the catalog"<<std::endl;
        return false;
      }

      // Save the extracted map
      m_ConvergenceMap->saveToFITSfile(m_workDir + m_outputFITSconvergenceMap, true);
      // Save the density map if any
      if (mySSVhandler.getDensityMap()!=nullptr)
      {
        mySSVhandler.getDensityMap()->saveToFITSfile(m_workDir + m_outputFITSdensityMap, true);
      }
    }
  }
  /// Case of a FITS catalog: retrieve the needed map out of it
  else if (m_inputFITScatalog.empty()==false)
  {
    // Create the catalog handler
    FITSCatalogHandler myFITShandler(m_workDir + m_inputFITScatalog);

    // In case the shear map has to be extracted
    if (m_outputFITSshearMap.empty() == false)
    {
      tStart = clock();

      m_ShearMap = myFITShandler.getShearMap(bounds, m_nbBinsX, m_nbBinsY, squareMap);

      std::cout<<"time to extract the shear map of the FITS catalog: ";
      std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

      // In case no shear map could be extracted from the catalog warn the user
      if (m_ShearMap==nullptr)
      {
        std::cout<<"needed information to build a shear map not provided into the catalog"<<std::endl;
        return false;
      }

      // Save the extracted map
      m_ShearMap->saveToFITSfile(m_workDir + m_outputFITSshearMap, true);

      // Save the density map if any
      if (myFITShandler.getDensityMap()!=nullptr)
      {
        myFITShandler.getDensityMap()->saveToFITSfile(m_workDir + m_outputFITSdensityMap, true);
      }
    }
    // In case the convergence map has to be extracted
    else if (m_outputFITSconvergenceMap.empty() == false)
    {
      tStart = clock();
      m_ConvergenceMap = myFITShandler.getConvergenceMap(bounds, m_nbBinsX, m_nbBinsY, squareMap);

      std::cout<<"time to extract the convergence map of the FITS catalog: ";
      std::cout<<double(clock() - tStart)/CLOCKS_PER_SEC<<std::endl;

      // In case no convergence map could be extracted from the catalog warn the user
      if (m_ConvergenceMap==nullptr)
      {
        std::cout<<"needed information to build a convergence map not provided into the catalog"<<std::endl;
        return false;
      }

      // Save the extracted map
      m_ConvergenceMap->saveToFITSfile(m_workDir + m_outputFITSconvergenceMap, true);

      // Save the density map if any
      if (myFITShandler.getDensityMap()!=nullptr)
      {
        myFITShandler.getDensityMap()->saveToFITSfile(m_workDir + m_outputFITSdensityMap, true);
      }
    }
  }
  /// case no input provided
  else
  {
    return false;
  }

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

  return true;

  return true;
}


} // TWOD_MASS_WL_MapMaker namespace
