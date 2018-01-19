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
 * @file src/lib/BasicSplitter.cpp
 * @date 07/27/17
 * @author user
 */

#include "TWOD_MASS_WL_CatalogSplitter/BasicSplitter.h"

#include <CCfits/CCfits>

namespace TWOD_MASS_WL_CatalogSplitter {

BasicSplitter::BasicSplitter(std::string inputCatalogFilename, std::string outputCatalogRootName,
                             unsigned int nbCatalogsRa, unsigned int nbCatalogsDec, unsigned int nbCatalogsZ,
                             TWOD_MASS_WL_MassMapping::Boundaries &boundaries):
m_inputCatalogFilename(inputCatalogFilename), m_outputCatalogRootName(outputCatalogRootName),
m_nbCatalogsRa(nbCatalogsRa), m_nbCatalogsDec(nbCatalogsDec), m_nbCatalogsZ(nbCatalogsZ), m_boundaries(boundaries)
{
}

bool BasicSplitter::splitCatalog()
{
  // In case no valid split as input parameters return false
  if (m_nbCatalogsRa==0 || m_nbCatalogsDec==0 || m_nbCatalogsZ==0)
  {
    return false;
  }

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


    std::vector< CCfits::String > colNames = std::vector< CCfits::String >();
    std::vector< CCfits::String > colForms = std::vector< CCfits::String >();
    std::vector< CCfits::String > colUnits = std::vector< CCfits::String >();

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
      else if (it->first.find("z")!=std::string::npos || it->first.find("redshift")!=std::string::npos)
      {
        keyZ = it->first;
      }

      colNames.push_back(it->second->name());
      colForms.push_back(it->second->format());
      colUnits.push_back(it->second->unit());
    }
    // If there is no ra, dec or redshift return false
    if (keyRa.empty() || keyDec.empty() || keyZ.empty())
    {
      return false;
    }

    ////////////////////////////////////// Create all needed subcatalogs
    // Create output FITS files with different names for each subcatalogs
    const unsigned int catalogSize = m_nbCatalogsRa*m_nbCatalogsDec*m_nbCatalogsZ;
    std::vector<CCfits::FITS*> outputCatalogs;//[catalogSize];
    // Create the associated tables
    std::vector<CCfits::Table*> tableSubCatalogs;//[catalogSize];
    // Create a counter for each subcatalog
    std::vector<unsigned int> counterTable(catalogSize, 0);//[catalogSize];

    for (unsigned int iZ=0; iZ<m_nbCatalogsZ; iZ++)
    {
      for (unsigned int iDec=0; iDec<m_nbCatalogsDec; iDec++)
      {
        for (unsigned int iRa=0; iRa<m_nbCatalogsRa; iRa++)
        {
//          unsigned int globalIndex = iRa + iDec*m_nbCatalogsRa + iZ*m_nbCatalogsRa*m_nbCatalogsDec;

          std::string subCatalogName = m_outputCatalogRootName+"ra"+std::to_string(iRa)
                                                              +"dec"+std::to_string(iDec)
                                                              +"z"+std::to_string(iZ)
                                                              +".fits";
          outputCatalogs.push_back(new CCfits::FITS(subCatalogName, CCfits::RWmode::Write, true));

          tableSubCatalogs.push_back(
              outputCatalogs.back()->addTable("hduName", 1, colNames, colForms, colUnits, CCfits::BinaryTbl, 1));
        }
      }
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
        // Get the index for each value of ra, dec and z
        unsigned int raIndex = floor((mapColumnsValues[keyRa][i]-m_boundaries.getRaMin())
                               /((m_boundaries.getRaMax()-m_boundaries.getRaMin())/m_nbCatalogsRa));
        unsigned int decIndex = floor((mapColumnsValues[keyDec][i]-m_boundaries.getDecMin())
                                /((m_boundaries.getDecMax()-m_boundaries.getDecMin())/m_nbCatalogsDec));
        unsigned int zIndex = floor((mapColumnsValues[keyZ][i]- m_boundaries.getZMin())
                              /((m_boundaries.getZMax()-m_boundaries.getZMin())/m_nbCatalogsZ));

        // compute the global index
        unsigned int globalIndex = raIndex + decIndex*m_nbCatalogsRa + zIndex*m_nbCatalogsRa*m_nbCatalogsDec;

        if (globalIndex>=m_nbCatalogsRa*m_nbCatalogsDec*m_nbCatalogsZ)
        {
          globalIndex=m_nbCatalogsRa*m_nbCatalogsDec*m_nbCatalogsZ-1;
        }

        // Increment the counter of that table
        counterTable[globalIndex]++;

        // Fill each column
        int colNumb = 1;
        for (it = myColMap.begin(); it!= myColMap.end(); ++it)
        {
          std::vector<double> tmp;
          tmp.push_back(mapColumnsValues[it->first][i]);

          tableSubCatalogs[globalIndex]->column(colNumb).write(tmp, counterTable[globalIndex]);

          colNumb++;
        }
      }
      readRows+=rowSize;
    }

    // Delete the pointers
    for (unsigned int iGlobal=0; iGlobal<m_nbCatalogsRa*m_nbCatalogsDec*m_nbCatalogsZ; iGlobal++)
    {
      delete outputCatalogs[iGlobal];
      outputCatalogs[iGlobal] = nullptr;
    }

    return true;
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
    return false;
  }
}


bool BasicSplitter::splitCatalogWithMask()
{
  std::vector<std::pair<float, float> > allPatches = getCenterOfPatches();

  std::vector<std::pair<float, float> > patchCenters = getMaskOfCatalog(allPatches);

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


    std::vector< CCfits::String > colNames = std::vector< CCfits::String >();
    std::vector< CCfits::String > colForms = std::vector< CCfits::String >();
    std::vector< CCfits::String > colUnits = std::vector< CCfits::String >();

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
      else if (it->first.find("z")!=std::string::npos || it->first.find("redshift")!=std::string::npos)
      {
        keyZ = it->first;
      }

      colNames.push_back(it->second->name());
      colForms.push_back(it->second->format());
      colUnits.push_back(it->second->unit());
    }
    // If there is no ra, dec or redshift return false
    if (keyRa.empty() || keyDec.empty() || keyZ.empty())
    {
      return false;
    }

    ////////////////////////////////////// Create all needed subcatalogs
    // Create output FITS files with different names for each subcatalogs
    std::vector<CCfits::FITS*> outputCatalogs;//[catalogSize];
    // Create the associated tables
    std::vector<CCfits::Table*> tableSubCatalogs;// = new CCfits::Table[catalogSize];
    // Create a counter for each subcatalog
    std::vector<unsigned int> counterTable(patchCenters.size(), 0);// = new unsigned int[catalogSize];

    for (unsigned int i=0; i<patchCenters.size(); i++)
    {
          std::string subCatalogName = m_outputCatalogRootName+std::to_string(i)+".fits";
          outputCatalogs.push_back(new CCfits::FITS(subCatalogName, CCfits::RWmode::Write, true));

          tableSubCatalogs.push_back(
              outputCatalogs[i]->addTable("hduName", 1, colNames, colForms, colUnits, CCfits::BinaryTbl, 1));
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
        // Get the index of the closest patch
        unsigned int closestIndex =
            getClosestPatch(patchCenters, mapColumnsValues[keyRa][i], mapColumnsValues[keyDec][i]);

        // Increment the counter of that table
        counterTable[closestIndex]++;

        // Fill each column
        int colNumb = 1;
        for (it = myColMap.begin(); it!= myColMap.end(); ++it)
        {
          std::vector<double> tmp;
          tmp.push_back(mapColumnsValues[it->first][i]);

          tableSubCatalogs[closestIndex]->column(colNumb).write(tmp, counterTable[closestIndex]);

          colNumb++;
        }
      }
      readRows+=rowSize;
    }

    // Delete the pointers
    for (unsigned int i=0; i<patchCenters.size(); i++)
    {
      delete outputCatalogs[i];
      outputCatalogs[i] = nullptr;
    }

    return true;
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
    return false;
  }
}

std::vector<std::pair<float, float> > BasicSplitter::getCenterOfPatches()
{
  // Create the output vector
  std::vector<std::pair<float, float> > output;

  // For all latitudes from -80 to 80 with a step of 10
  for (unsigned int i=0; i<17; i++)
  {
    // Compute the latitude
    float latitude = i*10.-80;
    // Compute the associated list of longitudes
    unsigned int numberOfLongitudes = int(fabs(floor(36*cos(latitude*3.14159265/180.0))));

    for (unsigned int j=0; j<numberOfLongitudes; j++)
    {
      // Compute the longitude
      float longitude = (j+0.5)*360/numberOfLongitudes;
      std::pair<float, float> tmpCoord(longitude, latitude);
      output.push_back(tmpCoord);
    }
  }

  // Finally add the two pole points
  output.push_back(std::pair<float, float>(0, 90));
  output.push_back(std::pair<float, float>(0, -90));

  // Return the output vector
  return output;
}

std::vector<std::pair<float, float> >
  BasicSplitter::getMaskOfCatalog(std::vector<std::pair<float, float> > &patchCenters)
{
  // Create a vector containing true if a patch has information in the catalog
  std::vector<bool> patchInformation(patchCenters.size(), false);

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
        // Get the index of the closest patch
        unsigned int closestIndex =
            getClosestPatch(patchCenters, mapColumnsValues[keyRa][i], mapColumnsValues[keyDec][i]);

        // Set the boolean associated to this patch to true
        patchInformation[closestIndex] = true;
      }
      readRows+=rowSize;
    }
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
    return std::vector<std::pair<float, float> > (1, (std::pair<float, float>(0, 0)));
  }

  // Create the output vector
  std::vector<std::pair<float, float> > output;

  // Put to the output vector only the patches where there is informatio
  for (unsigned int i=0; i<patchCenters.size(); i++)
  {
    if (patchInformation[i]==true)
    {
      output.push_back(patchCenters[i]);
    }
  }

  return output;
}

unsigned int BasicSplitter::getClosestPatch(std::vector<std::pair<float, float> > const &patchCenters,
                                            float ra, float dec)
{
  // Set the index to the first patch
  unsigned int closestIndex = 0;

  // Get the distance to the first patch
  float minDist = sqrt((ra-patchCenters[0].first)*(ra-patchCenters[0].first)
                + (dec-patchCenters[0].second)*(dec-patchCenters[0].second));

  // Loop over all the patches to get the closest patch
  for (unsigned int i=1; i<patchCenters.size(); i++)
  {
    // Compute the distance to the patch i
    float dist = sqrt((ra-patchCenters[i].first)*(ra-patchCenters[i].first)
               + (dec-patchCenters[i].second)*(dec-patchCenters[i].second));

    // If that distance is shorter than the current shortest, then let it be the shortest
    if (dist < minDist)
    {
      minDist = dist;
      closestIndex = i;
    }
  }

  return closestIndex;
}

} // TWOD_MASS_WL_CatalogSplitter namespace
