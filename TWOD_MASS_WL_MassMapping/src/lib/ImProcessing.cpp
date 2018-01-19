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
 * @file src/lib/ImProcessing.cpp
 * @date 07/07/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/ImProcessing.h"
#include "fftw3.h"
#include "math.h"

namespace TWOD_MASS_WL_MassMapping {

ImProcessing::ImProcessing(unsigned int sizeXaxis, unsigned int sizeYaxis)
: m_sizeXaxis(sizeXaxis), m_sizeYaxis(sizeYaxis)
{
}

Image ImProcessing::performDCT(Image input)
{
  // Create an output image
  Image DCToutput(m_sizeXaxis, m_sizeYaxis);

  // Create the plan on which to perform the transform
  fftw_plan DCTplan;

  #pragma omp critical
  {
    DCTplan = fftw_plan_r2r_2d(m_sizeXaxis, m_sizeYaxis, input.getArray(), DCToutput.getArray(),
                               FFTW_REDFT10, FFTW_REDFT10, FFTW_ESTIMATE);
  }

  // Perform the transformation
  fftw_execute(DCTplan);

  // Free memory
  fftw_destroy_plan(DCTplan);

  // Rescale the output
  double dctFactor = 2*sqrt(m_sizeXaxis*m_sizeYaxis);
  return DCToutput.multiply(1./dctFactor);
}

Image ImProcessing::performIDCT(Image input)
{
  // Create an output image
  Image output(m_sizeXaxis, m_sizeYaxis);

  // Create the plan on which to perform the inverse transform
  fftw_plan IDCTplan;

  #pragma omp critical
  {
    IDCTplan = fftw_plan_r2r_2d(m_sizeXaxis, m_sizeYaxis, input.getArray(), output.getArray(),
                                FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
  }

  // Perform the transformation
  fftw_execute(IDCTplan);

  // Free memory
  fftw_destroy_plan(IDCTplan);

  // Rescale the output
  double dctFactor = 2*sqrt(m_sizeXaxis*m_sizeYaxis);
  return output.multiply(1./dctFactor);
}

Image ImProcessing::performDCT(Image input, unsigned int blockSizeX, unsigned int blockSizeY, bool forward)
{
  // Create an output image
  Image output(m_sizeXaxis, m_sizeYaxis);
  ImProcessing myIP(blockSizeX, blockSizeY);

  for (unsigned iblock=0; iblock<m_sizeXaxis/blockSizeX; iblock++)
  {
    for (unsigned jblock=0; jblock<m_sizeYaxis/blockSizeY; jblock++)
    {
      // Fill the block map
      Image blockInput(blockSizeX, blockSizeY);
      for (unsigned int i=0; i<blockSizeX; i++)
      {
        for (unsigned int j=0; j<blockSizeY; j++)
        {
          blockInput.setValue(i, j, input.getValue(iblock*blockSizeX+i, jblock*blockSizeY+j));
        }
      }

      // Perform DCT on that block map
      Image blockOutput = forward ? myIP.performDCT(blockInput) : myIP.performIDCT(blockInput);

      // Fill the global output with that block map
      for (unsigned int i=0; i<blockSizeX; i++)
      {
        for (unsigned int j=0; j<blockSizeY; j++)
        {
          output.setValue(iblock*blockSizeX+i, jblock*blockSizeY+j, blockOutput.getValue(i, j));
        }
      }
    }
  }

  // return the global output
  return output;
}
/*
Image ImProcessing::performIDCT(Image input, unsigned int blockSizeX, unsigned int blockSizeY)
{
  // Create an output image
  Image output(m_sizeXaxis, m_sizeYaxis);
  ImProcessing myIP(blockSizeX, blockSizeY);

  for (unsigned iblock=0; iblock<m_sizeXaxis/blockSizeX; iblock++)
  {
    for (unsigned jblock=0; jblock<m_sizeYaxis/blockSizeY; jblock++)
    {
      // Fill the block map
      Image blockInput(blockSizeX, blockSizeY);
      for (unsigned int i=0; i<blockSizeX; i++)
      {
        for (unsigned int j=0; j<blockSizeY; j++)
        {
          blockInput.setValue(i, j, input.getValue(iblock*blockSizeX+i, jblock*blockSizeY+j));
        }
      }

      // Perform DCT on that block map
      Image blockOutput = myIP.performIDCT(blockInput);

      // Fill the global output with that block map
      for (unsigned int i=0; i<blockSizeX; i++)
      {
        for (unsigned int j=0; j<blockSizeY; j++)
        {
          output.setValue(iblock*blockSizeX+i, jblock*blockSizeY+j, blockOutput.getValue(i, j));
        }
      }
    }
  }

  // return the global output
  return output;
}
*/
std::vector<Image> ImProcessing::transformBspline(Image input, unsigned int nbScales)
{
  // Create a vector of images
  std::vector<Image> band;

  // Add the first image to this vector
  band.push_back(input);

  // Loop over the number of scales
  for (unsigned int step=0; step<nbScales-1; step++)
  {
    // Apply the b spline transfo with algorithm a trous
    Image imageOut = smoothBspline(band[step], step);

    // Add the output image to the vector
    band.push_back(imageOut);
    band[step] = band[step].substract(band[step+1]);
  }

  return band;
}

Image ImProcessing::smoothBspline(Image input, unsigned int stepTrou)
{
  // Define some values for the transform
  float h0 = 3./8.;
  float h1 = 1./4.;
  float h2 = 1./16.;
  int step = int(pow(2., double(stepTrou))+0.5);

  // Create an intermediate image
  Image tmpImage(m_sizeXaxis, m_sizeYaxis);

  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      tmpImage.setValue(i, j, h0*input.getValue(i, j)
                             +h1*(input.getValue(i, int(j-step))+input.getValue(i, j+step))
                             +h2*(input.getValue(i, int(j-2*step))+input.getValue(i, j+2*step)));
    }
  }

  // Create the output image
  Image imageOut(m_sizeXaxis, m_sizeYaxis);
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      imageOut.setValue(i, j, h0*tmpImage.getValue(i, j)
                             +h1*(tmpImage.getValue(int(i-step), j)+tmpImage.getValue(i+step, j))
                             +h2*(tmpImage.getValue(int(i-2*step), j)+tmpImage.getValue(i+2*step, j)));
    }
  }

  return imageOut;
}

Image ImProcessing::reconsBspline(std::vector<Image> band)
{
  // Create the output image
  Image imageOut(band[0].getXdim(), band[0].getYdim());

  // Just add the images from all the scales
  for (unsigned int i=0; i<band.size(); i++)
  {
    imageOut = imageOut.add(band[i]);
  }

  return imageOut;
}

} // TWOD_MASS_WL_MassMapping namespace
