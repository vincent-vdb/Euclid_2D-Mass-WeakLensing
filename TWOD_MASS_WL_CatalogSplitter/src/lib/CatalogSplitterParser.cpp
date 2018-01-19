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
 * @file src/lib/CatalogSplitterParser.cpp
 * @date 07/27/17
 * @author user
 */

#include "TWOD_MASS_WL_CatalogSplitter/CatalogSplitterParser.h"
#include "TWOD_MASS_WL_CatalogSplitter/BasicSplitter.h"
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

namespace po = boost::program_options;

namespace TWOD_MASS_WL_CatalogSplitter {

CatalogSplitterParser::CatalogSplitterParser(): m_inputFITSCatalog(""), m_outputFITSCatalogRoot(""), m_workDir(""),
    m_nbCatalogsRa(0), m_nbCatalogsDec(0), m_nbCatalogsZ(0), m_raMin(0), m_raMax(360), m_decMin(-90), m_decMax(90),
    m_zMin(0), m_zMax(15)
{
}

po::options_description CatalogSplitterParser::defineSpecificProgramOptions()
{
  po::options_description options {};
  //
  // !!! Implement the program options here !!!
  //
  options.add_options()

      ("workdir", po::value<std::string>(), "only for pipeline")
      ("logdir", po::value<std::string>(), "only for pipeline")

      ("inputFITSCatalog", po::value<std::string>(), "input FITS catalog to split")
      ("outputFITSCatalogRoot", po::value<std::string>(), "root name of the output subcatalogs")

      ("raMinMax", po::value<std::vector<float> >()->multitoken(),
       "right ascension min and max values for which to extract the map")
      ("decMinMax", po::value<std::vector<float> >()->multitoken(),
       "declination min and max values for which to extract the map")
      ("zMinMax", po::value<std::vector<float> >()->multitoken(),
       "redshift min and max values for which to extract the map")

      ("nbCatalogsRa", po::value<int>(),
       "number of subcatalogs to be created for right ascension values")
      ("nbCatalogsDec", po::value<int>(),
       "number of subcatalogs to be created for declination values")
      ("nbCatalogsZ", po::value<int>(),
       "number of subcatalogs to be created for redshift values");

  return options;
}

Elements::ExitCode CatalogSplitterParser::mainMethod(std::map<std::string, po::variable_value>& args)
{
  // Perform the parsing of input parameters
  if (parseInputParameters(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  TWOD_MASS_WL_MassMapping::Boundaries boundaries(m_raMin, m_raMax, m_decMin, m_decMax, m_zMin, m_zMax);

  BasicSplitter mySplitter(m_workDir+m_inputFITSCatalog, m_workDir+m_outputFITSCatalogRoot,
                           m_nbCatalogsRa, m_nbCatalogsDec, m_nbCatalogsZ, boundaries);

  if (mySplitter.splitCatalog()==false)
  {
    return Elements::ExitCode::OK;
  }

  return Elements::ExitCode::OK;
}

bool CatalogSplitterParser::parseInputParameters(std::map<std::string, po::variable_value>& args)
{
  // Reinitialize the counter to zero in case of multiple call of this method
  std::map<std::string, po::variable_value>::iterator it;
  for (it = args.begin(); it!= args.end(); ++it)
  {
    if (it->first=="m_workDir")
    {
      m_workDir = args["m_workDir"].as<std::string>() + std::string("/");
    }
    else if (it->first=="inputFITSCatalog")
    {
      m_inputFITSCatalog = args["inputFITSCatalog"].as<std::string>();
      //        std::cout<<"value of inputFITSCatalog: "<<m_inputFITSCatalog<<std::endl;
    }
    else if (it->first=="outputFITSCatalogRoot")
    {
      m_outputFITSCatalogRoot = args["outputFITSCatalogRoot"].as<std::string>();
      //        std::cout<<"value of outputFITSCatalogRoot: "<<m_outputFITSCatalogRoot<<std::endl;
    }
    else if (it->first=="nbCatalogsRa")
    {
      m_nbCatalogsRa = args["nbCatalogsRa"].as<int>();
      //        std::cout<<"value of nbCatalogsRa: "<<m_nbCatalogsRa<<std::endl;
    }
    else if (it->first=="nbCatalogsDec")
    {
      m_nbCatalogsDec = args["nbCatalogsDec"].as<int>();
      //        std::cout<<"value of nbCatalogsDec: "<<m_nbCatalogsDec<<std::endl;
    }
    else if (it->first=="nbCatalogsZ")
    {
      m_nbCatalogsZ = args["nbCatalogsZ"].as<int>();
      //        std::cout<<"value of nbCatalogsZ: "<<m_nbCatalogsZ<<std::endl;
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
    }

  }

  // Check all values are well filled
  if (m_inputFITSCatalog.empty()==true || m_outputFITSCatalogRoot.empty()==true)
  {
    std::cout<<"Input and/or output catalogs filenames not provided"<<std::endl;
    return false;
  }
  if (m_nbCatalogsRa==0 || m_nbCatalogsDec==0 || m_nbCatalogsZ==0)
  {
    std::cout<<"Number of splits to be performed not provided"<<std::endl;
    return false;
  }

  return true;
}

} // TWOD_MASS_WL_CatalogSplitter namespace
