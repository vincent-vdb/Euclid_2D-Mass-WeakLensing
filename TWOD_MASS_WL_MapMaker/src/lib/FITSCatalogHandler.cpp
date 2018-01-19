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
 * @file src/lib/FITSCatalogHandler.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h"
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include <CCfits/CCfits>

const double pi = 3.14159265358979323846;

using namespace TWOD_MASS_WL_MassMapping;

namespace TWOD_MASS_WL_MapMaker {

FITSCatalogHandler::FITSCatalogHandler(std::string filename): CatalogHandler(filename)
{
}

ShearMap* FITSCatalogHandler::getShearMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                          const unsigned int nbBinsX, const unsigned int nbBinsY,
                                          bool squareMap)
{
  // Retrieve the map from the private method
  std::pair<long, double*> myPair = getMap(shearMap, bounds, nbBinsX, nbBinsY, squareMap);

  return getShearMapFromArray(myPair, nbBinsX, nbBinsY, bounds);
}

ConvergenceMap* FITSCatalogHandler::getConvergenceMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                      const unsigned int nbBinsX, const unsigned int nbBinsY,
                                                      bool squareMap)
{
  // Retrieve the map from the private method
  std::pair<long, double*> myPair = getMap(convMap, bounds, nbBinsX, nbBinsY, squareMap);

  return getConvergenceMapFromArray(myPair, nbBinsX, nbBinsY, bounds);
}

bool FITSCatalogHandler::saveAsFitsCatalog(std::vector<std::vector<double> > inputData)
{

  // Check there are well 5 valarrays
  if (inputData.size() != 5)
  {
    return false;
  }

  int nRows = (inputData[0]).size();

  try
  {
    // Create an output FITS file
    CCfits::FITS *outFits = new CCfits::FITS(m_catalogFilename, CCfits::RWmode::Write, true);

    std::string hduName("outTable");

    std::vector< CCfits::String > colNames = std::vector< CCfits::String >();
    std::vector< CCfits::String > colForms = std::vector< CCfits::String >();
    std::vector< CCfits::String > colUnits = std::vector< CCfits::String >();

    colNames.push_back("RightAsc");
    colNames.push_back("Declination");
    colNames.push_back("redshift");
    colNames.push_back("SNR");
    colNames.push_back("scale");

    colForms.push_back("E");
    colForms.push_back("E");
    colForms.push_back("E");
    colForms.push_back("E");
    colForms.push_back("E");

    colUnits.push_back("degree");
    colUnits.push_back("degree");
    colUnits.push_back("redshift");
    colUnits.push_back("snr");
    colUnits.push_back("u.a.");

    CCfits::Table *table = outFits->addTable(hduName, nRows, colNames, colForms, colUnits, CCfits::BinaryTbl, 1);
    ///////////////////////////////////////////////////////////////////////

    // Create references to the valarrays of the catalog values
    std::vector<double> &raArray = inputData[0];
    std::vector<double> &decArray = inputData[1];
    std::vector<double> &zArray = inputData[2];
    std::vector<double> &SNRArray = inputData[3];
    std::vector<double> &scaleArray = inputData[4];

    table->column(1).write(raArray, 0l);
    table->column(2).write(decArray, 0l);
    table->column(3).write(zArray, 0l);
    table->column(4).write(SNRArray, 0l);
    table->column(5).write(scaleArray, 0l);


    delete outFits;
    outFits = nullptr;

    return true;
  }
  catch (CCfits::FitsException&)
  {
    return false;
  }
}

bool FITSCatalogHandler::saveAsFitsCatalog(std::vector<std::vector<double> > inputData,
                                           std::vector<std::vector<std::string> > columns)
{
  int nRows = (inputData[0]).size();
  int nCols = inputData.size();

  try
  {
    // Create an output FITS file
    CCfits::FITS *outFits = new CCfits::FITS(m_catalogFilename, CCfits::RWmode::Write, true);

    std::string hduName("outTable");

    std::vector< CCfits::String > colNames = std::vector< CCfits::String >();
    std::vector< CCfits::String > colForms = std::vector< CCfits::String >();
    std::vector< CCfits::String > colUnits = std::vector< CCfits::String >();

    for (int i=0; i<nCols; i++)
    {
      colNames.push_back(columns[0][i]);
      colForms.push_back(columns[1][i]);
      colUnits.push_back(columns[2][i]);
    }

    CCfits::Table *table = outFits->addTable(hduName, nRows, colNames, colForms, colUnits, CCfits::BinaryTbl, 1);
    ///////////////////////////////////////////////////////////////////////

    // Create references to the valarrays of the catalog values
    for (int i=0; i<nCols; i++)
    {
      std::vector<double> &array = inputData[i];
      table->column(i+1).write(array, 0l);
    }

    delete outFits;

    return true;
  }
  catch (CCfits::FitsException&)
  {
    return false;
  }
}

std::pair<long, double*> FITSCatalogHandler::getMap(mapEnum mapType, TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                    const unsigned int nbBinsX, const unsigned int nbBinsY,
                                                    bool squareMap)
{

  // Check variables inputs ra, dec and z are fine
  if (checkRaDecZ(bounds)==false)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  // Check the enum type is well provided
  if (mapType!=convMap && mapType!=shearMap)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  try
  {
    ////////////////////////////////////// Open the FITS file if it exists
    // Create a FITS pointer
    std::shared_ptr<CCfits::FITS> pInputFile(new CCfits::FITS(m_catalogFilename, CCfits::Read));

    // Reading the HDU
    CCfits::ExtHDU& table = pInputFile->extension(1);

    // Get the map from the FITS file
    std::map<std::string, CCfits::Column*> myColMap = table.column();

    // Create map of columns and vector of keys
//      std::map<std::string, std::valarray<double> > mapColumnsValues;
    std::vector<std::string> mapKeys;

    ////////////////////////////////////// Read headers and perform some checks on them
    // Create empty keys for each info
    std::string keyRa("");
    std::string keyDec("");
    std::string keyKappa("");
    std::string keyGamma1("");
    std::string keyGamma2("");
    std::string keyZ("");
    std::string keyWeight("");

    // Create an iterator of the map and loop over it to retrieve the columns keys if they exist
    std::map<std::string, CCfits::Column*>::iterator it;
    for (it = myColMap.begin(); it!= myColMap.end(); ++it)
    {
      if (it->first.find("ra")!=std::string::npos || it->first.find("RightAsc")!=std::string::npos)
      {
        keyRa = it->first;
      }
      else if (it->first.find("dec")!=std::string::npos  || it->first.find("Dec")!=std::string::npos)
      {
        keyDec = it->first;
      }
      else if (it->first.find("kappa")!=std::string::npos)
      {
        keyKappa = it->first;
      }
      else if (it->first.find("gamma1")!=std::string::npos)
      {
        keyGamma1 = it->first;
      }
      else if (it->first.find("gamma2")!=std::string::npos)
      {
        keyGamma2 = it->first;
      }
      else if (it->first.find("z")!=std::string::npos  || it->first.find("edshift")!=std::string::npos)
      {
        keyZ = it->first;
      }
      else if (it->first.find("weight")!=std::string::npos)
      {
        keyWeight = it->first;
      }
    }

    // If the convMap is needed but no kappa info return false
    if (mapType==convMap && keyKappa.empty())
    {
      return std::pair<long, double*> (0, nullptr);
    }

    // If the shearMap is needed but no gamma info return false
    if (mapType==shearMap && (keyGamma1.empty() || keyGamma2.empty()))
    {
      return std::pair<long, double*> (0, nullptr);
    }

    // If there is no ra or dec info then return false
    if (keyRa.empty() || keyDec.empty())
    {
      return std::pair<long, double*> (0, nullptr);
    }

    // Create references to the valarrays of the catalog values
//      std::valarray<double> &decArray = mapColumnsValues[mapKeys[0]];
//      std::valarray<double> &raArray = mapColumnsValues[mapKeys[3]];
//      std::valarray<double> &gamma1Array = mapColumnsValues[mapKeys[1]];
//      std::valarray<double> &gamma2Array = mapColumnsValues[mapKeys[2]];
//      std::valarray<double> &zArray = mapColumnsValues[m_mapKeys[4]];

    // Define variables of ra and dec min and max
    double raMin = bounds.getRaMin();
    double raMax = bounds.getRaMax();
    double decMin = bounds.getDecMin();
    double decMax = bounds.getDecMax();

    // Define the total number of rows and rows read
    long totalNumberOfRows(table.rows());
    long readRows(0);
    // Get the optimal number of rows to read by CCfits
    long rowSize = table.getRowsize()*1000;

    // Define ra0 and dec0 at the center of min and max values
    double ra0 = 0.5*(raMax + raMin);
    double dec0 = 0.5*(decMax + decMin);
    double raRange = (raMax - raMin);
    double decRange = (decMax - decMin);

    // Declare the bins size on X and Y
    double binXSize, binYSize;

    std::pair<double, double> xyMin = getGnomonicProjection(raMin, decMin, ra0, dec0);

    // In case a square map is not expected, perform some basic radec projection ranges selection
    if (squareMap == false)
    {
      std::pair<double, double> xyMax = getGnomonicProjection(raMax, decMax, ra0, dec0);

//      std::cout<<"ra min and max: "<<raMin<<" "<<raMax<<std::endl;
//      std::cout<<"dec min and max: "<<decMin<<" "<<decMax<<std::endl;
//      std::cout<<"X min and max: "<<xyMin.first<<" "<<xyMax.first<<std::endl;
//      std::cout<<"Y min and max: "<<xyMin.second<<" "<<xyMax.second<<std::endl;

      // Define the bins sizes
      binXSize = (xyMax.first - xyMin.first)/nbBinsX;
      binYSize = (xyMax.second - xyMin.second)/nbBinsY;
    }
    // In case a square map is expected, perform some fancier computation to find out radec projection ranges
    else
    {
      std::pair<double, double> raDec1 = getInverseGnomonicProjection(-0.5*raRange*pi/180.,
                                                                      -0.5*decRange*pi/180.,
                                                                      ra0, dec0);
      std::pair<double, double> raDec2 = getInverseGnomonicProjection(0,
                                                                      -0.5*decRange*pi/180.,
                                                                      ra0, dec0);
      std::pair<double, double> raDec3 = getInverseGnomonicProjection(0.5*raRange*pi/180.,
                                                                      -0.5*decRange*pi/180.,
                                                                      ra0, dec0);
      std::pair<double, double> raDec4 = getInverseGnomonicProjection(0.5*raRange*pi/180.,
                                                                      0.5*decRange*pi/180.,
                                                                      ra0, dec0);
      std::pair<double, double> raDec5 = getInverseGnomonicProjection(0,
                                                                      0.5*decRange*pi/180.,
                                                                      ra0, dec0);
      std::pair<double, double> raDec6 = getInverseGnomonicProjection(-0.5*raRange*pi/180.,
                                                                      0.5*decRange*pi/180.,
                                                                      ra0, dec0);

      // Get the min and max values of ra and dec according to the geometrical effects of projection
      raMin = raDec1.first < raDec6.first ? raDec1.first : raDec6.first;
      decMin = raDec1.second < raDec2.second ? raDec1.second : raDec2.second;
      raMax = raDec3.first > raDec4.first ? raDec3.first : raDec4.first;
      decMax = raDec4.second > raDec5.second ? raDec4.second : raDec5.second;

      std::cout<<"ra dec 1: "<<raDec1.first<<" "<<raDec1.second<<std::endl;
      std::cout<<"ra dec 2: "<<raDec2.first<<" "<<raDec2.second<<std::endl;
      std::cout<<"ra dec 3: "<<raDec3.first<<" "<<raDec3.second<<std::endl;
      std::cout<<"ra dec 4: "<<raDec4.first<<" "<<raDec4.second<<std::endl;
      std::cout<<"ra dec 5: "<<raDec5.first<<" "<<raDec5.second<<std::endl;
      std::cout<<"ra dec 6: "<<raDec6.first<<" "<<raDec6.second<<std::endl;
      std::cout<<"ra min and max: "<<raMin<<" "<<raMax<<std::endl;
      std::cout<<"dec min and max: "<<decMin<<" "<<decMax<<std::endl;

      // Define the bins sizes
      binXSize = raRange*pi/180./nbBinsX;
      binYSize = decRange*pi/180./nbBinsY;
    }
//      std::cout<<"size of bins on X and Y: "<<binXSize<<" "<<binYSize<<std::endl;

    // Create and initialize to zeros maps
    double *mapArray = initializeArray<double>(nbBinsX, nbBinsY, 2);
    double *countArray = initializeArray<double>(nbBinsX, nbBinsY, 1);

    // Create a counter of the galaxies watched
    unsigned int galCount(0);
    // Create a counter of the galaxies selected
    unsigned int selGalCount(0);

    // Loop as long as all the rows are not read
    while (readRows<totalNumberOfRows)
    {
      // Create map of columns and vector of keys
      std::map<std::string, std::valarray<double> > mapColumnsValues;
      // Loop over it to retrieve the columns info
//        std::map<std::string, CCfits::Column*>::iterator it;
      for (it = myColMap.begin(); it!= myColMap.end(); ++it)
      {
        std::valarray<double> tmpArray;
//        it->second->read(tmpArray, readRows+1, readRows + rowSize);

        // Doing here some crazy bullshit to avoid memory increasing
        CCfits::ColumnData<float> *myCol = dynamic_cast< CCfits::ColumnData<float>* >(it->second);
        CCfits::ColumnData<float> *tmpCol = new CCfits::ColumnData<float>(*myCol);
        tmpCol->read(tmpArray, readRows+1, readRows + rowSize);

        // Fill the member mapColumnsValues with the values
        mapColumnsValues[it->first]=tmpArray;

        //          std::cout<<it->second->format()<<std::endl;
        //          std::cout<<it->second->name()<<std::endl;
        //          std::cout<<it->second->unit()<<std::endl;

        // Fill the member mapColumnsValues with the values
        mapColumnsValues[it->first]=tmpArray;
        mapKeys.push_back(it->first);

        delete tmpCol;
      }

      // Loop over the values of the valarrays and keep the ones fulfilling the criteria
      for (unsigned int i = 0; i<mapColumnsValues[keyDec].size(); i++)
      {
        // Define the weight of each galaxy if any
        double weight = 1.;
        if (keyWeight.empty()==false)
        {
          weight = mapColumnsValues[keyWeight][i];
        }

        // Check the values fulfil the right ascension and declination constraints
        if ( mapColumnsValues[keyDec][i] >= decMin && mapColumnsValues[keyDec][i] <= decMax )
        {
          if ( mapColumnsValues[keyRa][i] >= raMin && mapColumnsValues[keyRa][i] <= raMax )
          {
            // If the redshift is provided check the values are ok too
            bool redshiftOk = !keyZ.empty() &&
                              mapColumnsValues[keyZ][i] >= bounds.getZMin() &&
                              mapColumnsValues[keyZ][i] <= bounds.getZMax();
            // If no redshift provided in the file then just go ahead
            if (redshiftOk || keyZ.empty())
            {
              int tmpx, tmpy;
              // In case square map needed
              if (squareMap == true)
              {
                // project the selected radec on gnomonic plan
                std::pair<double, double> tmpXY = getGnomonicProjection(mapColumnsValues[keyRa][i],
                                                                        mapColumnsValues[keyDec][i],
                                                                        ra0, dec0);

                //  calculate where it is on the binning
                tmpx = int(floor((tmpXY.first+0.5*raRange*pi/180.)/binXSize));
                tmpy = int(floor((tmpXY.second+0.5*decRange*pi/180.)/binYSize));
              }
              // in case square map not needed
              else
              {
                std::pair<double, double> tmpXY = getGnomonicProjection(mapColumnsValues[keyRa][i],
                                                                        mapColumnsValues[keyDec][i],
                                                                        ra0, dec0);

                tmpx = int(floor((tmpXY.first-xyMin.first)/binXSize));
                tmpy = int(floor((tmpXY.second-xyMin.second)/binYSize));
              }
              //          if (tmpx > nbBinsX || tmpx<0) std::cout<<tmpx<<std::endl;
              //          if (tmpy > nbBinsY || tmpy<0) std::cout<<tmpy<<std::endl;
              if (tmpx>=0 && tmpx<int(nbBinsX) && tmpy>=0 && tmpy<int(nbBinsY))
              {
                if (mapType==shearMap)
                {
                  // Apply correction for the projection
                  std::pair<double, double> xy1 = getGnomonicProjection(mapColumnsValues[keyRa][i],
                                                                        mapColumnsValues[keyDec][i],
                                                                        ra0, dec0);
                  std::pair<double, double> xy2 = getGnomonicProjection(mapColumnsValues[keyRa][i],
                                                                        (mapColumnsValues[keyDec][i]+0.01),
                                                                        ra0, dec0);

                  double rotationAngle = -atan((xy2.first-xy1.first)/(xy2.second-xy1.second));

                  double gamma1cor = mapColumnsValues[keyGamma1][i]*cos(2*rotationAngle)
                                    -mapColumnsValues[keyGamma2][i]*sin(2*rotationAngle);
                  double gamma2cor = mapColumnsValues[keyGamma1][i]*sin(2*rotationAngle)
                                    +mapColumnsValues[keyGamma2][i]*cos(2*rotationAngle);

                  mapArray[tmpy*nbBinsX + tmpx] += gamma1cor*weight;
                  mapArray[nbBinsX*nbBinsY + tmpy*nbBinsX + tmpx] += gamma2cor*weight;
                }
                else if (mapType==convMap)
                {
                  mapArray[tmpy*nbBinsX + tmpx] += mapColumnsValues[keyKappa][i]*weight;
                }
                countArray[tmpy*nbBinsX + tmpx] += weight;
                selGalCount+=weight;
              }
            }
          }
        }
        galCount+=weight;
      }

      readRows+=rowSize;
//        std::cout<<"read "<<readRows<<" rows over a total of "<<totalNumberOfRows<<std::endl;
    }

    std::cout<<"number of galaxies selected: "<<selGalCount<<std::endl;
    std::cout<<"over the total number of galaxies: "<<galCount<<std::endl;


    // Normalize the values to have the mean shear in each bin
    for (unsigned int i = 0; i<nbBinsX*nbBinsY; i++)
    {
      if (countArray[i]>1)
      {
        mapArray[i] /= countArray[i];
        mapArray[i + nbBinsX*nbBinsY] /= countArray[i];
      }
    }

    if (mapType==shearMap)
    {
      // Save the density map
      m_galDensity = new GlobalMap(countArray, nbBinsX, nbBinsY, 1, bounds, selGalCount);
    }

    // Delete dynamic arrays
    delete [] countArray;

    return std::pair<long, double*> (selGalCount, mapArray);
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
    return std::pair<long, double*> (0, nullptr);
  }

  return std::pair<long, double*> (0, nullptr);
}

} // TWOD_MASS_WL_MapMaker namespace
