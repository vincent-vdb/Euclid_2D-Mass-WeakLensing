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
 * @file src/lib/MaskSplitter.cpp
 * @date 10/27/17
 * @author user
 */

#include "TWOD_MASS_WL_CatalogSplitter/MaskSplitter.h"

#include <CCfits/CCfits>

namespace TWOD_MASS_WL_CatalogSplitter {

MaskSplitter::MaskSplitter(std::string inputCatalog, float raStep, float decStep, float zStep)
: m_inputCatalogFilename(inputCatalog), m_raStep(raStep), m_decStep(decStep), m_zStep(zStep), m_zMin(10.), m_zMax(0.)
{
}

std::vector<std::pair<float, float> > MaskSplitter::fillBasicMask()
{
  unsigned int numberOfDecBins = 180./m_decStep;

  std::vector<std::pair<float, float> > basicMask(numberOfDecBins, std::pair<float, float>(360, 0));

  try
  {
    ////////////////////////////////////// Open the FITS file if it exists
    // Create a FITS pointer
    std::shared_ptr<CCfits::FITS> pInputFile(new CCfits::FITS(m_inputCatalogFilename, CCfits::Read));

    // Reading the HDU
    CCfits::ExtHDU& table = pInputFile->extension(1);

    // Get the map from the FITS file
    std::map<std::string, CCfits::Column*> myColMap = table.column();

    // Create map of columns and vector of keys
    std::vector<std::string> mapKeys;

    ////////////////////////////////////// Read headers and perform some checks on them
    // Create empty keys for each location info
    std::string keyRa("");
    std::string keyDec("");
    std::string keyZ("");

    // Create an iterator of the map and loop over it to retrieve the columns keys if they exist
    std::map<std::string, CCfits::Column*>::iterator it;
    for (it = myColMap.begin(); it!= myColMap.end(); ++it)
    {
      if (it->first.find("ra")!=std::string::npos || it->first.find("RightAsc")!=std::string::npos)
      {
        keyRa = it->first;
      }
      else if (it->first.find("dec")!=std::string::npos || it->first.find("Declination")!=std::string::npos)
      {
        keyDec = it->first;
      }
      else if (it->first.find("z")!=std::string::npos || it->first.find("edshift")!=std::string::npos)
      {
        keyZ = it->first;
      }
    }
    // If there is no ra, dec or redshift return false
    if (keyRa.empty() || keyDec.empty())
    {
      return std::vector<std::pair<float, float> > (1, (std::pair<float, float>(0, 0)));
    }

    // Define the total number of rows and rows read
    long totalNumberOfRows(table.rows());
    long readRows(0);
    // Get the optimal number of rows to read by CCfits
    long rowSize = table.getRowsize();

    // Loop as long as all the rows are not read
    while (readRows<totalNumberOfRows)
    {
      // Create map of columns and vector of keys
      std::map<std::string, std::valarray<double> > mapColumnsValues;

      // Loop over it to retrieve the columns info
      for (it = myColMap.begin(); it!= myColMap.end(); ++it)
      {
        std::valarray<double> tmpArray;
        it->second->read(tmpArray, readRows+1, readRows + rowSize);

        // Fill the member mapColumnsValues with the values
        mapColumnsValues[it->first]=tmpArray;
        mapKeys.push_back(it->first);
      }

      // Loop over the values of the valarrays
      for (unsigned int i = 0; i<mapColumnsValues[keyDec].size(); i++)
      {
        unsigned int maskIndex = (mapColumnsValues[keyDec][i] + 90)/m_decStep;
        // Keep the min ra value in the first value of the pair
        if (mapColumnsValues[keyRa][i] < basicMask[maskIndex].first)
        {
          basicMask[maskIndex].first = mapColumnsValues[keyRa][i];
        }
        // Keep the max ra value in the second value of the pair
        if (mapColumnsValues[keyRa][i] > basicMask[maskIndex].second)
        {
          basicMask[maskIndex].second = mapColumnsValues[keyRa][i];
        }

        // Keep the min and max redshift value if provided
        if (keyZ.empty()==false)
        {
          if (mapColumnsValues[keyZ][i] > m_zMax)
          {
            m_zMax = mapColumnsValues[keyZ][i];
          }
          if (mapColumnsValues[keyZ][i] < m_zMin)
          {
            m_zMin = mapColumnsValues[keyZ][i];
          }
        }
      }
      readRows+=rowSize;
    }
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
    return std::vector<std::pair<float, float> > (1, (std::pair<float, float>(0, 0)));
  }

  return basicMask;
}

std::vector<TWOD_MASS_WL_MassMapping::Boundaries>
  MaskSplitter::getBoundariesFromBasicMask(std::vector<std::pair<float, float> > &basicMask)
{
  // Create the output vector
  std::vector<TWOD_MASS_WL_MassMapping::Boundaries> output;

  // Loop over all the declinations in the basic mask
  for (unsigned int i=0; i<basicMask.size(); i++)
  {
    // If there is some data in that part of the sky
    if (basicMask[i].first<basicMask[i].second)
    {
      // Get the declination of that part of the mask
      float decMask = -90.+(i+0.5)*m_decStep;

      // Initialize raRange to zero: will be updated right after depending on the sign of decMask
      float raRange = 0.;
      if (decMask>0)
      {
        raRange = m_raStep/cos((decMask+0.4*m_decStep)*3.141593/180.);
      }
      else
      {
        raRange = m_raStep/cos((decMask-0.4*m_decStep)*3.141593/180.);
      }

      // Make sure raRange does not exceed 180 degrees
      if (raRange>180.)
      {
        raRange = 180.;
      }

      // Define min and max values of right ascension and declination
      float raMin = basicMask[i].first<0. ? 0. : basicMask[i].first;
      float raMax = raMin + raRange;
      float decMin = decMask - 0.5*m_decStep;
      float decMax = decMask + 0.5*m_decStep;

      // Loop over all values of right ascension in the mask
      while (raMin<basicMask[i].second)
      {
//        std::cout<<raMin<<" "<<raMax<<" "<<decMin<<" "<<decMax<<std::endl;
        // Loop over all values of redshift if known
        if (m_zMin<m_zMax)
        {
          float tmpRedshift = m_zMin;
          while (tmpRedshift<m_zMax)
          {
            output.push_back(TWOD_MASS_WL_MassMapping::Boundaries(raMin, raMax, decMin, decMax,
                                                                  tmpRedshift, tmpRedshift+m_zStep));
            tmpRedshift += m_zStep;
          }
        }
        else
        {
          output.push_back(TWOD_MASS_WL_MassMapping::Boundaries(raMin, raMax, decMin, decMax, 0., 10.));
        }
        raMin += raRange;
        raMax += raRange;
      }
    }
  }

  return output;
}

} // TWOD_MASS_WL_CatalogSplitter namespace
