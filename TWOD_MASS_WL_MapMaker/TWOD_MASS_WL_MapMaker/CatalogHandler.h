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
 * @file TWOD_MASS_WL_MapMaker/CatalogHandler.h
 * @date 04/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MAPMAKER_CATALOGHANDLER_H
#define TWOD_MASS_WL_MAPMAKER_CATALOGHANDLER_H

#include <string>

namespace TWOD_MASS_WL_MassMapping {
class ShearMap;
class ConvergenceMap;
class GlobalMap;
class Boundaries;
}

namespace TWOD_MASS_WL_MapMaker {

enum mapEnum {convMap, shearMap};

/**
 * @class CatalogHandler
 * @brief class that allows to handle catalogs.
 *
 * CatalogHandler class provides method to get a ShearMap or ConvergenceMap
 * from a catalog for a given area of the sky.
 *
 */
class CatalogHandler {

public:

  /**
   * @brief Destructor
   */
  virtual ~CatalogHandler();

  /**
   * @brief Constructor of the CatalogHandler
   * @param[in] filename path and name of the catalog
   *
   * This method creates a CatalogHandler for a given catalog input file
   *
   */
  CatalogHandler(std::string filename);

  /**
   * @brief Gets the gnomonic projection of given ra and dec
   * @param[in] ra the input right ascension in degrees
   * @param[in] dec the input declination in degrees
   * @param[in] ra0 the input right ascension on which to perform the projection in degrees
   * @param[in] dec0 the input declination on which to perform the projection in degrees
   *
   * @return a pair containing the projected X and Y values
   *
   * This method returns the gnomonic projection of ra and dec for a given ra0 and dec0 on
   * which to perform the projection. The return value is a pair of doubles containing X and Y projected.
   *
   */
  std::pair<double, double> getGnomonicProjection(double ra, double dec, double ra0, double dec0);

  /**
   * @brief Gets the inverse gnomonic projection of given X and Y
   * @param[in] x the input x
   * @param[in] y the input y
   * @param[in] ra0 the input right ascension on which to perform the projection in degrees
   * @param[in] dec0 the input declination on which to perform the projection in degrees
   *
   * @return a pair containing the backprojected ra and dec values in degrees
   *
   * This method returns the gnomonic backprojection of x and y for a given ra0 and dec0 on
   * which to perform the backprojection. The return value is a pair of doubles containing
   * the right ascension and declination backprojected.
   *
   */
  std::pair<double, double> getInverseGnomonicProjection(double x, double y, double ra0, double dec0);

  /**
   * @brief Checks input parameters ra, dec and z to be ok
   * @param[in] bounds the object containing ra, dec and z min and max
   *
   * @return true if values are ok, false otherwise
   *
   * This method checks whether the input parameters are ok (i.e. min values are lower than max values)
   *
   */
  bool checkRaDecZ(TWOD_MASS_WL_MassMapping::Boundaries &bounds);

  /**
   * @brief Create an array of zeros
   * @param[in] nbBinsX the number of bins on the X axis
   * @param[in] nbBinsY the number of bins on the Y axis
   * @param[in] nbBinsZ the number of bins on the Z axis
   *
   * @return an array of type T* with nbBinsX*nbBinsY*nbBinsZ values
   *
   * This method creates an array of zeros of type T* with nbBinsX*nbBinsY*nbBinsZ values
   *
   */
  template<typename T>
  T* initializeArray(const unsigned int nbBinsX, const unsigned int nbBinsY, const unsigned int nbBinsZ)
  {
    // Create the map array
    T *array = new T[nbBinsX*nbBinsY*nbBinsZ];

    // Initialize it to zeros
    for (unsigned int i = 0; i<nbBinsX*nbBinsY*nbBinsZ; i++)
    {
      array[i] = 0;
    }

    return array;
  }

  /**
   * @brief Create a ConvergenceMap
   * @param[in] arrayPair a pair containing a number of galaxies and an array of
   * the associated image
   * @param[in] nbBinsX the number of bins on the X axis
   * @param[in] nbBinsY the number of bins on the Y axis
   * @param[in] bounds the object containing ra, dec and z min and max
   *
   * @return a ConvergenceMap based on input array and number of galaxies, or nullptr if no information
   *
   * This method creates a ConvergenceMap based on input pair (array and number of galaxies),
   * or nullptr if no information in the pair
   *
   */
  TWOD_MASS_WL_MassMapping::ConvergenceMap* getConvergenceMapFromArray(std::pair<long, double*> arrayPair,
                                                                      const unsigned int nbBinsX,
                                                                      const unsigned int nbBinsY,
                                                                      TWOD_MASS_WL_MassMapping::Boundaries &bounds);

  /**
   * @brief Create a ShearMap
   * @param[in] arrayPair a pair containing a number of galaxies and an array of
   * the associated image
   * @param[in] nbBinsX the number of bins on the X axis
   * @param[in] nbBinsY the number of bins on the Y axis
   * @param[in] bounds the object containing ra, dec and z min and max
   *
   * @return a ShearMap based on input array and number of galaxies, or nullptr if no information
   *
   * This method creates a ShearMap based on input pair (array and number of galaxies),
   * or nullptr if no information in the pair
   *
   */
  TWOD_MASS_WL_MassMapping::ShearMap* getShearMapFromArray(std::pair<long, double*> arrayPair,
                                                          const unsigned int nbBinsX,
                                                          const unsigned int nbBinsY,
                                                          TWOD_MASS_WL_MassMapping::Boundaries &bounds);

  /**
   * @brief Returns a density map if any
   * @return a GlobalMap containing galaxy density after parsing of the catalog
   */
  TWOD_MASS_WL_MassMapping::GlobalMap* getDensityMap() const;

protected:

  std::string m_catalogFilename;
  TWOD_MASS_WL_MassMapping::GlobalMap* m_galDensity;


}; /* End of CatalogHandler class */

} /* namespace TWOD_MASS_WL_MapMaker */


#endif
