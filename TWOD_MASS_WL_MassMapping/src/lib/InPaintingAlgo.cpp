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
 * @file src/lib/InPaintingAlgo.cpp
 * @date 07/07/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/InPaintingAlgo.h"
#include "fftw3.h"
#include "math.h"
#include <iostream>

namespace TWOD_MASS_WL_MassMapping {

InPaintingAlgo::~InPaintingAlgo()
{
  delete m_maskValues;
  m_maskValues = nullptr;
}

InPaintingAlgo::InPaintingAlgo(ShearMap &shearMap, ConvergenceMap &convMap, unsigned int nbScales,
                               float minThreshold, float maxThreshold):
    m_shearMap(shearMap), m_convMap(convMap), m_nbScales(nbScales), m_minThreshold(minThreshold),
    m_maxThreshold(maxThreshold), m_IP(shearMap.getXdim(), shearMap.getYdim())
{
  typedef boost::multi_array<double, 3>::index index;

  // Retrieve the size of the maps
  m_sizeXaxis = m_shearMap.getXdim();
  m_sizeYaxis = m_shearMap.getYdim();
  m_sizeZaxis = m_shearMap.getZdim();

  std::cout<<"axis dim: "<<m_sizeXaxis<<" "<<m_sizeYaxis<<" "<<m_sizeZaxis<<std::endl;

  // Declare the mask
  m_maskValues = new boost::multi_array<int, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  unsigned int count(0);

  // Initialize the mask
  for (index j = 0; j != m_sizeYaxis; ++j)
  {
    for (index i = 0; i != m_sizeXaxis; ++i)
    {

      if (fabs(m_shearMap.getBinValue(i, j, 0))<0.0000000001 && fabs(m_shearMap.getBinValue(i, j, 1))<0.0000000001)
      {
        (*m_maskValues)[i][j][0] = 0;
        (*m_maskValues)[i][j][1] = 0;
        count++;
      }
      else
      {
        (*m_maskValues)[i][j][0] = 1;
        (*m_maskValues)[i][j][1] = 1;
      }
    }
  }
  std::cout<<"number of zeros: "<<count<<std::endl;
  std::cout<<"over the number of pixels: "<<m_sizeXaxis*m_sizeYaxis*m_sizeZaxis<<std::endl;
}

InPaintingAlgo::InPaintingAlgo(const InPaintingAlgo& copy):
    m_shearMap(copy.m_shearMap), m_convMap(copy.m_convMap), m_sizeXaxis(copy.m_sizeXaxis),
    m_sizeYaxis(copy.m_sizeYaxis), m_sizeZaxis(copy.m_sizeZaxis), m_nbScales(copy.m_nbScales),
    m_minThreshold(copy.m_minThreshold), m_maxThreshold(copy.m_maxThreshold), m_IP(copy.m_IP)
{
  typedef boost::multi_array<double, 3>::index index;

  // Declare the mask
  m_maskValues = new boost::multi_array<int, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Initialize the mask
  for (index j = 0; j != m_sizeYaxis; ++j)
  {
    for (index i = 0; i != m_sizeXaxis; ++i)
    {
      (*m_maskValues)[i][j][0] = (*(copy.m_maskValues))[i][j][0];
      (*m_maskValues)[i][j][1] = (*(copy.m_maskValues))[i][j][1];
    }
  }
}

ConvergenceMap* InPaintingAlgo::performInPaintingAlgo(unsigned int nbIter, bool sigmaBounds, bool bModeZeros)
{
  // Create a copy of the conv map
  ConvergenceMap *kappaMapIter = new ConvergenceMap(m_convMap);

  double maxThreshold(m_maxThreshold);
  double minThreshold(m_minThreshold);

  for (unsigned int iter = 0; iter<nbIter; iter++)
  {
    std::cout<<"iteration "<<iter<<" beginning"<<std::endl;

    // Allocate memory for kappa and DCT kappa
    Image kappaE(m_sizeXaxis, m_sizeYaxis);
    Image kappaB(m_sizeXaxis, m_sizeYaxis);

    std::cout<<"Initializing kappa map"<<std::endl;
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      for (unsigned int i=0; i<m_sizeXaxis; i++)
      {
        kappaE.setValue(i, j, kappaMapIter->getBinValue(i, j, 0));
        kappaB.setValue(i, j, kappaMapIter->getBinValue(i, j, 1));
      }
    }

    // Perform the DCT
    Image DCTkappaE = m_IP.performDCT(kappaE);
    Image DCTkappaB = m_IP.performDCT(kappaB);

    // Update the threshold value with the max value at first iteration
    if (iter==0)
    {
      // If no threshold given, take the max
      if (maxThreshold<=0.)
      {
        maxThreshold = DCTkappaE.getMax();
      }
    }

    double lambda = minThreshold + (maxThreshold-minThreshold)*(erfc(2.8*iter/nbIter));
    if (iter==nbIter-1)
    {
      lambda = minThreshold;
    }
    std::cout<<"threshold: "<<lambda<<std::endl;

    // Keep in memory the 0 values of kappa
    double DCTkappaE0 = DCTkappaE.getValue(0, 0);
    double DCTkappaB0 = DCTkappaB.getValue(0, 0);

    // Cut all values below the threshold value
    DCTkappaE.applyThreshold(lambda);
    DCTkappaB.applyThreshold(lambda);

    // Restore 0 values
    DCTkappaE.setValue(0, 0, DCTkappaE0);
    DCTkappaB.setValue(0, 0, DCTkappaB0);

    // Perform the IDCT
    kappaE = m_IP.performIDCT(DCTkappaE);
    kappaB = m_IP.performIDCT(DCTkappaB);

    // Apply sigma boundaries
    if (sigmaBounds)
    {
      //    applyBoundaries(kappaE);
      kappaE = applyBoundariesOnWavelets(kappaE);
    }

    // Perform the inversion and apply the mask to get the final convergence map
    delete kappaMapIter;
    kappaMapIter = new ConvergenceMap(performInversionMask(kappaE, kappaB, bModeZeros));

    std::cout<<"end of iteration "<<iter<<std::endl;
  }

  return kappaMapIter;
}

ConvergenceMap* InPaintingAlgo::performInPaintingAlgo(unsigned int nbIter, bool sigmaBounds, bool bModeZeros,
                                                      unsigned int blockSizeX, unsigned int blockSizeY)
{
  // Create a copy of the conv map
  ConvergenceMap *kappaMapIter = new ConvergenceMap(m_convMap);

  float threshold1(0);
  float threshold2(0);

  for (unsigned int iter = 0; iter<nbIter; iter++)
  {
    std::cout<<"iteration "<<iter<<" beginning"<<std::endl;

    // Allocate memory for kappa and DCT kappa
    Image kappaE(m_sizeXaxis, m_sizeYaxis);
    Image kappaB(m_sizeXaxis, m_sizeYaxis);

    std::cout<<"Initializing kappa map"<<std::endl;
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      for (unsigned int i=0; i<m_sizeXaxis; i++)
      {
        kappaE.setValue(i, j, kappaMapIter->getBinValue(i, j, 0));
        kappaB.setValue(i, j, kappaMapIter->getBinValue(i, j, 1));
      }
    }

    // Perform the DCT
    bool forward = true;
    Image DCTkappaE = m_IP.performDCT(kappaE, blockSizeX, blockSizeY, forward);
    Image DCTkappaB = m_IP.performDCT(kappaB, blockSizeX, blockSizeY, forward);

    // Update the threshold value with the max value at first iteration
    if (iter==0)
    {
      threshold1 = DCTkappaE.getMax();
    }

    float lambda = threshold2 + (threshold1-threshold2)*(erfc(2.8*iter/nbIter));
    std::cout<<"threshold: "<<lambda<<std::endl;


    // Cut all values below the threshold value excluding 0 values of each block
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        bool isNotFirstBlockValue = i%blockSizeX!=0 || j%blockSizeY!=0;
        if (isNotFirstBlockValue && fabs(DCTkappaE.getValue(i, j)) < lambda)
        {
          DCTkappaE.setValue(i, j, 0);
        }
        if (isNotFirstBlockValue && fabs(DCTkappaB.getValue(i, j)) < lambda)
        {
          DCTkappaB.setValue(i, j, 0);
        }
      }
    }

    // Perform the IDCT
    forward = false;
    kappaE = m_IP.performDCT(DCTkappaE, blockSizeX, blockSizeY, forward);
    kappaB = m_IP.performDCT(DCTkappaB, blockSizeX, blockSizeY, forward);

    // Apply sigma boundaries
    if (sigmaBounds)
    {
      //    applyBoundaries(kappaE);
      kappaE = applyBoundariesOnWavelets(kappaE);
    }

    // Perform the inversion and apply the mask to get the final convergence map
    delete kappaMapIter;
    kappaMapIter = new ConvergenceMap(performInversionMask(kappaE, kappaB, bModeZeros));
//    if (iter == nbIter-1)
//    {
//      kappaMapIter->saveToFITSfile("/home/user/convMapInPainted.fits", true);
//    }
    std::cout<<"end of iteration "<<iter<<std::endl;
  }

  return kappaMapIter;
}

Image InPaintingAlgo::applyBoundariesOnWavelets(Image input)
{
  if (m_nbScales==0)
  {
    m_nbScales = int(log(m_sizeXaxis)/log(2.))-3.-2.;
  }

  std::vector<Image> myBand = m_IP.transformBspline(input, m_nbScales);

  for (unsigned int kScale=0; kScale<m_nbScales-1; kScale++)
  {
    double maskMean = 0.;
    double maskSquareMean = 0.;
    double maskCount = 0.;
    double imMean = 0.;
    double imSquareMean = 0.;
    double imCount = 0.;

    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        if ((*m_maskValues)[i][j][0]==0)
        {
          double tmp = myBand[kScale].getValue(i, j);
          maskMean += tmp;
          maskSquareMean += tmp*tmp;
          maskCount++;
        }
        else
        {
          double tmp = myBand[kScale].getValue(i, j);
          imMean += tmp;
          imSquareMean += tmp*tmp;
          imCount++;
        }
      }
    }


    double maskSigma = sqrt((maskSquareMean/maskCount) - (maskMean*maskMean/maskCount/maskCount));
    double imSigma = sqrt((imSquareMean/imCount) - (imMean*imMean/imCount/imCount));

    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        if ((*m_maskValues)[i][j][0]==0)
        {
          if ((maskCount > 2) && (imCount > 2) && (maskSigma > 0))
          {
            myBand[kScale].setValue(i, j, myBand[kScale].getValue(i, j)*imSigma/maskSigma);
          }
        }
      }
    }
  }

  return m_IP.reconsBspline(myBand);
}

ConvergenceMap InPaintingAlgo::performInversionMask(Image kappaE, Image kappaB, bool bModeZeros)
{
  // Allocate memory for the kappa array
  double *kappaArray = new double[m_sizeXaxis*m_sizeYaxis*m_sizeZaxis];

  // Fill the convergence map array
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      kappaArray[j*m_sizeXaxis +i] = kappaE.getValue(i, j);
      if ((*m_maskValues)[i][j][0]==0 && bModeZeros==true)
      {
        kappaArray[m_sizeXaxis*m_sizeYaxis + j*m_sizeXaxis +i] = 0;
      }
      else
      {
        kappaArray[m_sizeXaxis*m_sizeYaxis + j*m_sizeXaxis +i] = kappaB.getValue(i, j);
      }
    }
  }

  ConvergenceMap kappaMap(kappaArray, m_sizeXaxis, m_sizeYaxis, m_sizeZaxis);

  // Free the memory
  delete [] kappaArray;
  kappaArray = nullptr;


  // Get the shear map from this convergence map
  ShearMap gammaMap = kappaMap.getShearMap();

  // Apply the mask on the shear map and perform inpainting on it
  double *gammaCorrArray = new double[m_sizeXaxis*m_sizeYaxis*m_sizeZaxis];
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      unsigned int globalIndex = j*m_sizeXaxis +i;
      gammaCorrArray[globalIndex] = gammaMap.getBinValue(i, j, 0)*(1-(*m_maskValues)[i][j][0])
                                   +m_shearMap.getBinValue(i, j, 0)*(*m_maskValues)[i][j][0];
      globalIndex += m_sizeXaxis*m_sizeYaxis;
      gammaCorrArray[globalIndex] = gammaMap.getBinValue(i, j, 1)*(1-(*m_maskValues)[i][j][1])
                                   +m_shearMap.getBinValue(i, j, 1)*(*m_maskValues)[i][j][1];
    }
  }

  Boundaries bounds = m_shearMap.getBoundaries();
  ShearMap gammaMapCorr(gammaCorrArray, m_sizeXaxis, m_sizeYaxis, m_sizeZaxis,
                        bounds, m_shearMap.getNumberOfGalaxies());

  delete [] gammaCorrArray;
  gammaCorrArray = nullptr;


  ConvergenceMap kappaMapBack = gammaMapCorr.getConvergenceMap();

  return kappaMapBack;
}

} // TWOD_MASS_WL_MassMapping namespace
