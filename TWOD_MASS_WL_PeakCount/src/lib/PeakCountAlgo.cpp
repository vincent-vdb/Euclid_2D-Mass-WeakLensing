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
 * @file src/lib/PeakCountAlgo.cpp
 * @date 05/22/17
 * @author user
 */

#include <iostream>

#include "TWOD_MASS_WL_PeakCount/PeakCountAlgo.h"
#include "TWOD_MASS_WL_MassMapping/Image.h"

namespace TWOD_MASS_WL_PeakCount {

PeakCountAlgo::PeakCountAlgo(TWOD_MASS_WL_MassMapping::ConvergenceMap &convMap,
                             TWOD_MASS_WL_MassMapping::GlobalMap &densityMap):
    m_convMap(convMap), m_densityMap(densityMap), m_IP(convMap.getXdim(), convMap.getYdim())
{
  m_sizeXaxis = convMap.getXdim();
  m_sizeYaxis = convMap.getYdim();
  m_sizeZaxis = convMap.getZdim();
}

bool PeakCountAlgo::savePeakCatalog(std::string filename)
{
  // Allocate memory for kappa and DCT kappa
  TWOD_MASS_WL_MassMapping::Image kappaE(m_sizeXaxis, m_sizeYaxis);
  TWOD_MASS_WL_MassMapping::Image kappaB(m_sizeXaxis, m_sizeYaxis);

  std::cout<<"Initializing kappa map"<<std::endl;
  for (unsigned int j=0; j<m_sizeYaxis; j++)
  {
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      kappaE.setValue(i, j, m_convMap.getBinValue(i, j, 0));
      kappaB.setValue(i, j, m_convMap.getBinValue(i, j, 1));
    }
  }

  // Compute the number of scales
  unsigned int nbScales = int(log(m_sizeXaxis)/log(2.))-3.-2.;
  std::cout<<"number of scales: "<<nbScales<<std::endl;

  // Perform the kappa map decomposition
  std::vector<TWOD_MASS_WL_MassMapping::Image> myBand = m_IP.transformBspline(kappaE, nbScales);

  // Get the peaks data
  std::vector<std::vector<double> > inputData = getPeaks(myBand);
  std::cout<<"number of detected peaks: "<<inputData[0].size()<<std::endl;

  // Save those data into the catalog and return false if it does not work
  std::vector<std::string> colNames;
  colNames.push_back("RightAsc");
  colNames.push_back("Declination");
  colNames.push_back("redshift");
  colNames.push_back("SNR");
  colNames.push_back("scale");

  std::vector<std::string> colForms(5, "E");

  std::vector<std::string> colUnits;
  colUnits.push_back("degree");
  colUnits.push_back("degree");
  colUnits.push_back("redshift");
  colUnits.push_back("snr");
  colUnits.push_back("u.a.");

  std::vector<std::vector<std::string> > columns;
  columns.push_back(colNames);
  columns.push_back(colForms);
  columns.push_back(colUnits);

  // Create a FITS catalog
  TWOD_MASS_WL_MapMaker::FITSCatalogHandler myCatalog(filename);
  // And save data to this catalog
  return myCatalog.saveAsFitsCatalog(inputData, columns);
}

TWOD_MASS_WL_MassMapping::Image PeakCountAlgo::getSNRimage(TWOD_MASS_WL_MassMapping::Image inputKappaImage,
                                                           double globalNoise)
{
  // First divide the signal map by the global noise
  TWOD_MASS_WL_MassMapping::Image mySNRimage = inputKappaImage.multiply(1./globalNoise);

  // Then apply local correction according to the galaxy density
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      double density = m_densityMap.getBinValue(i, j, 0);
      if (density>0.)
      {
        mySNRimage.setValue(i, j, mySNRimage.getValue(i, j)/sqrt(density));
      }
      else // no galaxy then zero so that this pixel will never be a peak
      {
        mySNRimage.setValue(i, j, 0.);
      }
    }
  }

  return mySNRimage;
}

std::vector<std::vector<double> > PeakCountAlgo::getPeaks(std::vector<TWOD_MASS_WL_MassMapping::Image> &myBand)
{
  // Create a dummy catalog for coordinate conversion
  TWOD_MASS_WL_MapMaker::FITSCatalogHandler myCatalog("dummy");

  // Get the map characteristics
  double raMin = m_convMap.getBoundaries().getRaMin();
  double raMax = m_convMap.getBoundaries().getRaMax();
  double decMin = m_convMap.getBoundaries().getDecMin();
  double decMax = m_convMap.getBoundaries().getDecMax();
  double ra0 = 0.5*(raMin + raMax);
  double dec0 = 0.5*(decMin + decMax);
  double raRange = raMax - raMin;
  double decRange = decMax - decMin;

  // Create vectors for peaks data
  std::vector<double> peakX;
  std::vector<double> peakY;
  std::vector<double> peakSNR;
  std::vector<double> redshift;
  std::vector<double> scale;

  // Get the noise on the last scale wavelet decomposition
  double stdevNoise = (myBand[myBand.size()-1]).getStandardDeviation();

  // Loop on every scale
  for (unsigned int band=0; band<myBand.size()-1; band++)
  {
    // Divide the image by the noise, to get an SNR image
    TWOD_MASS_WL_MassMapping::Image mySNRimage = getSNRimage(myBand[band], stdevNoise);

    // Loop over all pixels
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        // Check if any pixel is a local maximum
        if (mySNRimage.isLocalMax(i, j))
        {

          // Perform transform from pixel location to ra and dec
          double tmpx = (i+0.5)*raRange/m_convMap.getXdim()*3.14/180-0.5*raRange*3.14/180;
          double tmpy = (j+0.5)*decRange/m_convMap.getYdim()*3.14/180-0.5*decRange*3.14/180;
          std::pair<double, double> radec = myCatalog.getInverseGnomonicProjection(tmpx, tmpy, ra0, dec0);

          // Save the data into the vectors
          peakX.push_back(radec.first);
          peakY.push_back(radec.second);
          peakSNR.push_back(mySNRimage.getValue(i, j));
          scale.push_back(band);
          redshift.push_back(0);
        }
      }
    }
  }

  // Create a vector containing all data
  std::vector<std::vector<double> > inputData;

  // And fill this vector before returning it
  inputData.push_back(peakX);
  inputData.push_back(peakY);
  inputData.push_back(redshift);
  inputData.push_back(peakSNR);
  inputData.push_back(scale);

  return inputData;
}

} // TWOD_MASS_WL_PeakCount namespace
