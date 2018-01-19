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
 * @file TWOD_MASS_WL_CatalogSplitter/MaskSplitter.h
 * @date 10/27/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_CATALOGSPLITTER_MASKSPLITTER_H
#define _TWOD_MASS_WL_CATALOGSPLITTER_MASKSPLITTER_H

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include <vector>
#include <string>

namespace TWOD_MASS_WL_CatalogSplitter {

/**
 * @class MaskSplitter
 * @brief performs a split of the sky based on a mask computed on the input data
 *
 */
class MaskSplitter {

public:

  /**
   * @brief Destructor
   */
  virtual ~MaskSplitter() = default;

  MaskSplitter(std::string inputCatalog, float raStep, float decStep, float zStep);

  /**
   * @brief Method that gets the min and max right ascension of the data for each dec bin of 10 degrees
   * @return min and max values of the right ascension for each 10 degrees dec bin
   */
  std::vector<std::pair<float, float> > fillBasicMask();

  /**
   * @brief Method that parses the all catalog and retrieves patch where information exists
   * @param[in] a vector containing the basic mask
   * @return a vector of Boundaries of patches where information exists
   */
  std::vector<TWOD_MASS_WL_MassMapping::Boundaries>
    getBoundariesFromBasicMask(std::vector<std::pair<float, float> > &basicMask);

private:

  std::string m_inputCatalogFilename;
  float m_raStep;
  float m_decStep;
  float m_zStep;

  float m_zMin;
  float m_zMax;

}; /* End of MaskSplitter class */

} /* namespace TWOD_MASS_WL_CatalogSplitter */


#endif
