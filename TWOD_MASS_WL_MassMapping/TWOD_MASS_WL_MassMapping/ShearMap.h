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
 * @file TWOD_MASS_WL_MassMapping/ShearMap.h
 * @date 04/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MASSMAPPING_SHEARMAP_H
#define TWOD_MASS_WL_MASSMAPPING_SHEARMAP_H

#include "TWOD_MASS_WL_MassMapping/GlobalMap.h"

namespace TWOD_MASS_WL_MassMapping {

class ConvergenceMap;


/**
 * @class ShearMap
 * @brief map that contains shear data, deriving from GlobalMap class
 *
 */
class ShearMap : public GlobalMap {

public:

  /**
   * @brief Destructor
   */
  virtual ~ShearMap() = default;

  /**
   * @brief Constructor of a ShearMap
   * @param array input values to give to the class
   * @param sizeXaxis number of pixels in the X axis
   * @param sizeYaxis number of pixels in the Y axis
   * @param sizeZaxis number of pixels in the Z axis
   * @param[in] nGalaxies number of galaxies in the map
   *
   * This constructor builds a ShearMap of dimensions sizeXaxis, sizeYaxis, sizeZaxis
   * from the input data array, which should be arranged such that:
   * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
   *
   */
  ShearMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
           unsigned long nGalaxies = 0);

  /**
   * @brief Constructor of a ShearMap
   * @param array input values to give to the class
   * @param sizeXaxis number of pixels in the X axis
   * @param sizeYaxis number of pixels in the Y axis
   * @param sizeZaxis number of pixels in the Z axis
   * @param[in] nGalaxies number of galaxies in the map
   * @param[in] boundaries Boundaries object containing ra, dec and z information
   *
   * This constructor builds a ShearMap of dimensions sizeXaxis, sizeYaxis, sizeZaxis
   * from the input data array, which should be arranged such that:
   * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
   *
   */
  ShearMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
           Boundaries &boundaries, unsigned long nGalaxies = 0);

  /**
   * @brief Constructor of a ShearMap
   * @param filename of a FITS input file
   *
   * This constructor builds a ShearMap from provided FITS input file filename
   *
   */
  ShearMap(std::string filename);

  /**
    * @brief Returns a ConvergenceMap using K&S algorithm
    * @return a ConvergenceMap corresponding to the input ShearMap
    *
    * This method creates and returns a ConvergenceMap from the given ShearMap
    * using Kaiser & Squires algorithm
    *
    */
  ConvergenceMap getConvergenceMap();

  /**
    * @brief Computes the reduced shear
    *
    * This method computes the reduced shear from the original shear map and the
    * provided convergence map
    *
    */
  void computeReducedShear(ConvergenceMap inputConvMap);

private:

}; /* End of ShearMap class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
