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
 * @file TWOD_MASS_WL_MassMapping/InPaintingAlgo.h
 * @date 07/07/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_MASSMAPPING_INPAINTINGALGO_H
#define _TWOD_MASS_WL_MASSMAPPING_INPAINTINGALGO_H

#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "TWOD_MASS_WL_MassMapping/ImProcessing.h"

#include "boost/multi_array.hpp"

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class InPaintingAlgo
 * @brief class that allows to perform inpainting for given shear and convergence maps
 *
 */
class InPaintingAlgo {

public:

  /**
   * @brief Destructor
   */
  virtual ~InPaintingAlgo();

  /**
   * @brief Constructor of an InPainting object
   * @param[in] shearMap the input shear map
   * @param[in] convMap the input convergence map
   */
  InPaintingAlgo(ShearMap &shearMap, ConvergenceMap &convMap, unsigned int nbScales = 0,
                 float minThreshold = 0., float maxThreshold = -10.);

  /**
   * @brief Copy constructor of an InPainting object
   * @param[in] copy the InPainting object to copy
   */
  InPaintingAlgo(const InPaintingAlgo& copy);

  /**
   * @brief Method to perform inpainting
   * @param[in] nbIter the number of iterations
   * @param[in] sigmaBounds set to true to force same variance in and out of the mask
   * @param[in] bModeZeros set to true to force the B modes at zero in the iterations
   */
  ConvergenceMap* performInPaintingAlgo(unsigned int nbIter, bool sigmaBounds, bool bModeZeros);

  /**
   * @brief Method to perform inpainting for given block size of the image
   * @param[in] nbIter the number of iterations
   * @param[in] sigmaBounds set to true to force same variance in and out of the mask
   * @param[in] bModeZeros set to true to force the B modes at zero in the iterations
   * @param[in] blockSizeX size of the block on X dimensions
   * @param[in] blockSizeY size of the block on Y dimensions
   */
  ConvergenceMap* performInPaintingAlgo(unsigned int nbIter, bool sigmaBounds, bool bModeZeros,
                                        unsigned int blockSizeX, unsigned int blockSizeY);

private:

  ShearMap m_shearMap;
  ConvergenceMap m_convMap;

  boost::multi_array<int, 3> *m_maskValues;

  unsigned int m_sizeXaxis;
  unsigned int m_sizeYaxis;
  unsigned int m_sizeZaxis;
  unsigned int m_nbScales;

  float m_minThreshold;
  float m_maxThreshold;

  ImProcessing m_IP;

  Image applyBoundariesOnWavelets(Image input);

  ConvergenceMap performInversionMask(Image kappaE, Image kappaB, bool bModeZeros);

}; /* End of InPaintingAlgo class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
