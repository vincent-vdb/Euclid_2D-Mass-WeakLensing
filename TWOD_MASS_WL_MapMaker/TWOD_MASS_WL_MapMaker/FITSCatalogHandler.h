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
 * @file TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h
 * @date 04/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MAPMAKER_FITSCATALOGHANDLER_H
#define TWOD_MASS_WL_MAPMAKER_FITSCATALOGHANDLER_H

#include "TWOD_MASS_WL_MapMaker/CatalogHandler.h"
#include <valarray>

namespace TWOD_MASS_WL_MapMaker {

/**
 * @class FITSCatalogHandler
 * @brief Class that allows to extract maps from a FITS catalog. Deriving from CatalogHandler.
 *
 */
class FITSCatalogHandler : public CatalogHandler {

public:

  /**
   * @brief Destructor
   */
  virtual ~FITSCatalogHandler() = default;

  /**
   * @brief Constructor of the FITSCatalogHandler
   * @param[in] filename path and name of the catalog
   *
   * This method creates a FITSCatalogHandler for a given catalog input file
   *
   */
  FITSCatalogHandler(std::string filename);

  /**
   * @brief Gets the ShearMap from the FITS catalog and returns it
   * @param[in] bounds the object containing ra, dec and z min and max
   * @param[in] nbBinsX the number of bins needed on the X axis
   * @param[in] nbBinsY the number of bins needed on the Y axis
   *
   * @return a ShearMap containing the values extracted from the catalog.
   * Pointer -> need to manage memory!
   *
   * This method returns a ShearMap containing the data extracted from the input FITS catalog.
   * Only galaxies satisfying redshift, right ascension and declination (min and max) are taken into account.
   *
   */
  TWOD_MASS_WL_MassMapping::ShearMap* getShearMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                  const unsigned int nbBinsX,
                                                  const unsigned int nbBinsY,
                                                  bool squareMap = false);

  /**
   * @brief Gets the ConvergenceMap from the FITS catalog and returns it
   * @param[in] bounds the object containing ra, dec and z min and max
   * @param[in] nbBinsX the number of bins needed on the X axis
   * @param[in] nbBinsY the number of bins needed on the Y axis
   * @param[in] squareMap a bool to be set to true to have a square map on projected plan
   *
   * @return a ConvergenceMap containing the values extracted from the FITScatalog.
   * Pointer -> need to manage memory!
   *
   * This method returns a ConvergenceMap containing the data extracted from the input FITS catalog.
   * Only galaxies satisfying redshift, right ascension and declination (min and max) are taken into account.
   *
   */
  TWOD_MASS_WL_MassMapping::ConvergenceMap* getConvergenceMap(TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                                              const unsigned int nbBinsX,
                                                              const unsigned int nbBinsY,
                                                              bool squareMap = false);

  /**
   * @brief Saves the input data into a FITS catalog
   * @param[in] inputData a vector of vector containing the input data to save into a catalog
   *
   * @return true if a FITS catalog was successfully created, false otherwise
   */
  bool saveAsFitsCatalog(std::vector<std::vector<double> > inputData);

  /**
   * @brief Saves the input data into a FITS catalog
   * @param[in] inputData a vector of vector containing the input data to save into a catalog
   * @param[in] columns a vector of vector containing the label for the columns names, forms and units
   *
   * @return true if a FITS catalog was successfully created, false otherwise
   */
  bool saveAsFitsCatalog(std::vector<std::vector<double> > inputData, std::vector<std::vector<std::string> > columns);

private:

  /**
   * @brief Returns a pair containing the number of galaxies selected as well as an array of the map data
   * @param[in] mapType an enum to tell what type of map is needed
   * @param[in] bounds the object containing ra, dec and z min and max
   * @param[in] nbBinsX the number of bins needed on the X axis
   * @param[in] nbBinsY the number of bins needed on the Y axis
   * @param[in] squareMap a bool to be set to true to have a square map on projected plan
   *
   * @return a pair containing the number of galaxies selected as well as an array of the map data
   *
   * This method is generic to both kind of maps (shear and convergence) and returns the
   * number of galaxies selected as well as the array with the data of the map for all
   * given input parameters (z, ra, dec and binning)
   *
   */
  std::pair<long, double*> getMap(mapEnum mapType, TWOD_MASS_WL_MassMapping::Boundaries &bounds,
                                  const unsigned int nbBinsX, const unsigned int nbBinsY,
                                  bool squareMap);

}; /* End of FITSCatalogHandler class */

} /* namespace TWOD_MASS_WL_MapMaker */


#endif
