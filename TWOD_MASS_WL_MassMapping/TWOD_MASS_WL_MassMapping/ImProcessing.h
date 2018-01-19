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
 * @file TWOD_MASS_WL_MassMapping/ImProcessing.h
 * @date 07/07/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_MASSMAPPING_IMPROCESSING_H
#define _TWOD_MASS_WL_MASSMAPPING_IMPROCESSING_H

#include "TWOD_MASS_WL_MassMapping/Image.h"
#include <vector>

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class ImProcessing
 * @brief class that allows to perform image processing needed for InPainting (e.g. DCT and b spline wavelet)
 *
 */
class ImProcessing {

public:

  /**
   * @brief Destructor
   */
  virtual ~ImProcessing() = default;

  /**
   * @brief Constructor of an ImProcessing object
   * @param[in] sizeXaxis the number of pixels in the X axis
   * @param[in] sizeYaxis the number of pixels in the Y axis
   */
  ImProcessing(unsigned int sizeXaxis, unsigned int sizeYaxis);

  /**
   * @brief performs DCT on an input image
   * @param[in] input the input Image on which to perform the DCT
   * @return the DCT image
   */
  Image performDCT(Image input);

  /**
   * @brief performs IDCT on an input DCT image
   * @param[in] input the input Image on which to perform the IDCT
   * @return the image from the DCT image
   */
  Image performIDCT(Image input);

  /**
   * @brief performs (I)DCT on an input image
   * @param[in] input the input Image on which to perform the DCT
   * @param[in] blockSizeX the number of pixels to use per block on X axis
   * @param[in] blockSizeY the number of pixels to use per block on Y axis
   * @param[in] forward set to true to perform DCT, false to perform IDCT
   * @return the (I)DCT image
   */
  Image performDCT(Image input, unsigned int blockSizeX, unsigned int blockSizeY, bool forward);

  /**
   * @brief performs b spline transformation on an input Image for a given number of scales
   * @param[in] input the input Image on which to perform the transform
   * @param[in] nbScales the number of scales
   * @return a vector containing the Image of the b spline transformations for each scale
   */
  std::vector<Image> transformBspline(Image input, unsigned int nbScales);

  /**
   * @brief applies b spline transformation on a given input image and a given step for holes (algorithm a trous)
   * @param[in] input the input Image on which to perform the transform
   * @param[in] stepTrou the step defining the hole size for the algorithm
   * @return an image after transformation
   */
  Image smoothBspline(Image input, unsigned int stepTrou);

  /**
   * @brief reconstructs an image from the vector of images for each scales (returned by the transformBspline method)
   * @param[in] band the input vector of Images for each scales
   * @return an image after reconstruction
   */
  Image reconsBspline(std::vector<Image> band);

private:

  unsigned int m_sizeXaxis;
  unsigned int m_sizeYaxis;

}; /* End of ImProcessing class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
