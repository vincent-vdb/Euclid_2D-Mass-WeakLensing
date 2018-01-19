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
 * @file TWOD_MASS_WL_MassMapping/ConvergenceMap.h
 * @date 04/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MASSMAPPING_CONVERGENCEMAP_H
#define TWOD_MASS_WL_MASSMAPPING_CONVERGENCEMAP_H

#include "TWOD_MASS_WL_MassMapping/GlobalMap.h"

namespace TWOD_MASS_WL_MassMapping {

class ShearMap;

/**
 * @class ConvergenceMap
 * @brief map that contains convergence data, deriving from GlobalMap class
 *
 */
class ConvergenceMap : public GlobalMap {

public:

  /**
   * @brief Destructor
   */
  virtual ~ConvergenceMap() = default;

  /**
  * @brief Constructor of a ConvergenceMap
  * @param[in] array input values to give to the class
  * @param[in] sizeXaxis number of pixels in the X axis
  * @param[in] sizeYaxis number of pixels in the Y axis
  * @param[in] sizeZaxis number of pixels in the Z axis
  * @param[in] nGalaxies number of galaxies in the map
  *
  * This constructor builds a ConvergenceMap of dimensions sizeXaxis, sizeYaxis, sizeZaxis
  * from the input data array, which should be arranged such that:
  * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
  *
  */
 ConvergenceMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                unsigned long nGalaxies = 0);

 /**
  * @brief Constructor of a ConvergenceMap
  * @param[in] array input values to give to the class
  * @param[in] sizeXaxis number of pixels in the X axis
  * @param[in] sizeYaxis number of pixels in the Y axis
  * @param[in] sizeZaxis number of pixels in the Z axis
  * @param[in] boundaries Boundaries object containing ra, dec and z information
  * @param[in] nGalaxies number of galaxies in the map
  *
  * This constructor builds a ConvergenceMap of dimensions sizeXaxis, sizeYaxis, sizeZaxis
  * from the input data array, which should be arranged such that:
  * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
  *
  */
 ConvergenceMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
     Boundaries &boundaries, unsigned long nGalaxies = 0);

  /**
   * @brief Constructor of a ConvergenceMap
   * @param filename of a FITS input file
   *
   * This constructor builds a ConvergenceMap from provided FITS input file filename
   *
   */
  ConvergenceMap(std::string filename);

  /**
    * @brief Returns a ShearMap using K&S algorithm
    * @return a ShearMap corresponding to the input ConvergenceMap
    *
    * This method creates and returns a ShearMap from the given ConvergenceMap
    * using Kaiser & Squires algorithm
    *
    */
  ShearMap getShearMap();

private:

}; /* End of ConvergenceMap class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
