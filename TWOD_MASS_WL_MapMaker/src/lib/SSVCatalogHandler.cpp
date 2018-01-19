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
 * @file src/lib/SSVCatalogHandler.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MapMaker/SSVCatalogHandler.h"
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include <CCfits/CCfits>

#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace TWOD_MASS_WL_MassMapping;

namespace TWOD_MASS_WL_MapMaker {


SSVCatalogHandler::SSVCatalogHandler(std::string filename):CatalogHandler(filename)
{
}

ShearMap* SSVCatalogHandler::getShearMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                         const unsigned int nbBinsX, const unsigned int nbBinsY,
                                         bool squareMap)
{
  // Retrieve the map from the private method
  std::pair<long, double*> myPair = getMap(shearMap, bounds, nbBinsX, nbBinsY, squareMap);

  return getShearMapFromArray(myPair, nbBinsX, nbBinsY, bounds);
}

ConvergenceMap* SSVCatalogHandler::getConvergenceMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                     const unsigned int nbBinsX, const unsigned int nbBinsY,
                                                     bool squareMap)
{
  // Retrieve the map from the private method
  std::pair<long, double*> myPair = getMap(convMap, bounds, nbBinsX, nbBinsY, squareMap);

  return getConvergenceMapFromArray(myPair, nbBinsX, nbBinsY, bounds);
}


std::pair<long, double*> SSVCatalogHandler::getMap(mapEnum mapType, TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                   const unsigned int nbBinsX, const unsigned int nbBinsY,
                                                   bool squareMap)
{
  /////////////////////////////////// Perform some prior checks on the inputs
  // Check variables inputs ra, dec and z are fine
  if (checkRaDecZ(bounds)==false)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  // Open the file containing the data
  std::ifstream catalog(m_catalogFilename);

  // Check the file was correctly opened
  if (catalog.is_open()==false)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  // Check the enum type is well provided
  if (mapType!=convMap && mapType!=shearMap)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  /////////////////////////////////// Read headers to perform some checks and gather info

  // Create a string to get the lines of the file
  std::string line;

  //Read the first line of headers and skip it
  getline(catalog, line);
  // And use it to get the variables to record
  std::vector<std::string> headers;
  boost::split(headers, line, boost::is_any_of(" "));

  // Create a set of indices to store the values
  // Initialize them to 0 so that you know if they will be found
  int idxRa(-1);
  int idxDec(-1);
  int idxKappa(-1);
  int idxGamma1(-1);
  int idxGamma2(-1);
  int idxZ(-1);
  int idxWeight(-1);

  // For each of them check if they are the ra, dec, kappa, gamma1, gamma2 or z
  for (unsigned int i=0; i<headers.size(); i++)
  {
//      std::cout<<"header "<<i<<" :"<<headers[i]<<std::endl;

    if (headers[i].find("ra")!=std::string::npos)
    {
      idxRa = i;
//        std::cout<<"index of ra: "<<i<<std::endl;
    }
    else if (headers[i].find("dec")!=std::string::npos)
    {
      idxDec = i;
//        std::cout<<"index of dec: "<<i<<std::endl;
    }
    else if (headers[i].find("kappa")!=std::string::npos)
    {
      idxKappa = i;
//        std::cout<<"index of kappa: "<<i<<std::endl;
    }
    else if (headers[i].find("gamma1")!=std::string::npos)
    {
      idxGamma1 = i;
//        std::cout<<"index of gamma1: "<<i<<std::endl;
    }
    else if (headers[i].find("gamma2")!=std::string::npos)
    {
      idxGamma2 = i;
//        std::cout<<"index of gamma2: "<<i<<std::endl;
    }
    else if (headers[i].find("z")!=std::string::npos)
    {
      idxZ = i;
//        std::cout<<"index of z: "<<i<<std::endl;
    }
    else if (headers[i].find("weight")!=std::string::npos)
    {
      idxWeight = i;
//        std::cout<<"index of z: "<<i<<std::endl;
    }
  }

  /////////////////////////////////// Perform some checks on headers info
  // If the convMap is needed but no kappa info return false
  if (mapType==convMap && idxKappa==-1)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  // If the shearMap is needed but no gamma info return false
  if (mapType==shearMap && (idxGamma1==-1 || idxGamma2==-1))
  {
    return std::pair<long, double*> (0, nullptr);
  }

  // If there is no ra or dec info then return false
  if (idxRa == -1 || idxDec == -1)
  {
    return std::pair<long, double*> (0, nullptr);
  }

  /////////////////////////////////// Define some variables to read the data and create the map
  // Define variables of ra and dec min and max
  double raMin = bounds.getRaMin();
  double raMax = bounds.getRaMax();
  double decMin = bounds.getDecMin();
  double decMax = bounds.getDecMax();

  // Create a buffer for each columns and initialize it to zeros
  double *buffer = new double[headers.size()];

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
    std::pair<double, double> raDec1 = getInverseGnomonicProjection(-0.5*raRange*3.14159/180.,
                                                                    -0.5*decRange*3.14159/180.,
                                                                    ra0, dec0);
    std::pair<double, double> raDec2 = getInverseGnomonicProjection(0,
                                                                    -0.5*decRange*3.14159/180.,
                                                                    ra0, dec0);
    std::pair<double, double> raDec3 = getInverseGnomonicProjection(0.5*raRange*3.14159/180.,
                                                                    -0.5*decRange*3.14159/180.,
                                                                    ra0, dec0);
    std::pair<double, double> raDec4 = getInverseGnomonicProjection(0.5*raRange*3.14159/180.,
                                                                    0.5*decRange*3.14159/180.,
                                                                    ra0, dec0);
    std::pair<double, double> raDec5 = getInverseGnomonicProjection(0,
                                                                    0.5*decRange*3.14159/180.,
                                                                    ra0, dec0);
    std::pair<double, double> raDec6 = getInverseGnomonicProjection(-0.5*raRange*3.14159/180.,
                                                                    0.5*decRange*3.14159/180.,
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
    binXSize = raRange*3.14159/180./nbBinsX;
    binYSize = decRange*3.14159/180./nbBinsY;
  }

//    std::cout<<"size of bins on X and Y: "<<binXSize<<" "<<binYSize<<std::endl;

  // Create and initialize to zeros maps
  double *mapArray = initializeArray<double>(nbBinsX, nbBinsY, 2);
  double *countArray = initializeArray<double>(nbBinsX, nbBinsY, 1);

  // Create a counter of the galaxies watched
  unsigned long galCount(0);
  // Create a counter of the galaxies selected
  unsigned long selGalCount(0);

  // Loop over all the lines of the test file shearMap 1
  while ( getline(catalog, line) ) // && galCount<10000000)
  {
    std::istringstream in(line);
    for (unsigned int i=0; i<headers.size(); i++)
    {
      in>>buffer[i];
      //        std::cout<<buffer[i]<<" ";
    }
    //      std::cout<<std::endl;
    //std::cout<<"gal ID: "<<galID<<" Ra: "<<tmpRa<<" dec: "<<tmpDec<<" kappa: ";
    //std::cout<<tmpKappa<<" G1: "<<tmpGamma1<<" G2: "<<tmpGamma2<<std::endl;

    // If the weight is provided then use it, otherwise set it to 1
    double weight(1.);
    if (idxWeight!=-1)
    {
      weight = buffer[idxWeight];
    }

    // Check the values fulfill the right ascension and declination constraints
    if ( buffer[idxDec] >= decMin && buffer[idxDec] <= decMax )
    {
      if ( buffer[idxRa] >= raMin && buffer[idxRa] <= raMax )
      {
        // If the redshift is provided check the values are ok too
        bool redshiftOk = idxZ!=-1 && buffer[idxZ] >= bounds.getZMin() && buffer[idxZ] <= bounds.getZMax();
        // If no redshift provided in the file then just go ahead
        if (redshiftOk || idxZ==-1)
        {
          int tmpx, tmpy;
          // In case square map needed
          if (squareMap == true)
          {
            // project the selected radec on gnomonic plan
            std::pair<double, double> tmpXY = getGnomonicProjection(buffer[idxRa], buffer[idxDec], ra0, dec0);

            //  calculate where it is on the binning
            tmpx = int(floor((tmpXY.first+0.5*raRange*3.14159/180.)/binXSize));
            tmpy = int(floor((tmpXY.second+0.5*decRange*3.14159/180.)/binYSize));
          }
          // in case square map not needed
          else
          {
            std::pair<double, double> projXY = getGnomonicProjection(buffer[idxRa], buffer[idxDec], ra0, dec0);

            tmpx = int(floor((projXY.first-xyMin.first)/binXSize));
            tmpy = int(floor((projXY.second-xyMin.second)/binYSize));
          }

//            if (tmpx > nbBinsX || tmpx<0) std::cout<<tmpx<<std::endl;
//            if (tmpy > nbBinsY || tmpy<0) std::cout<<tmpy<<std::endl;
          if (tmpx>=0 && tmpx<int(nbBinsX) && tmpy>=0 && tmpy<int(nbBinsY))
          {
            if (mapType==shearMap)
            {
              mapArray[tmpy*nbBinsX + tmpx] += buffer[idxGamma1]*weight;
              mapArray[nbBinsX*nbBinsY + tmpy*nbBinsX + tmpx] += buffer[idxGamma2]*weight;
            }
            else if (mapType==convMap)
            {
              mapArray[tmpy*nbBinsX + tmpx] += buffer[idxKappa]*weight;
  //            mapArray[nbBinsX*nbBinsY + tmpy*nbBinsX + tmpx] += 0;
            }
            countArray[tmpy*nbBinsX + tmpx] += weight;
            selGalCount+=weight;
          }
        }
      }
    }
    galCount+=weight;

    if (galCount%100000000 == 0)
    {
      std::cout<<"galaxy number "<<galCount<<" read"<<std::endl;
    }
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

  // Close the file
  catalog.close();

  // Delete dynamic arrays
  delete [] buffer;
  delete [] countArray;

  // Return the map and the number of galaxies selected
  return std::pair<long, double*> (selGalCount, mapArray);
}

} // TWOD_MASS_WL_MapMaker namespace
