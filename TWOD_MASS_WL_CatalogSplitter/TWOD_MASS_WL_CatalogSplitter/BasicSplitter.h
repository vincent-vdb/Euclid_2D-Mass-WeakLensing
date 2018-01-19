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
 * @file TWOD_MASS_WL_CatalogSplitter/BasicSplitter.h
 * @date 07/27/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_CATALOGSPLITTER_BASICSPLITTER_H
#define _TWOD_MASS_WL_CATALOGSPLITTER_BASICSPLITTER_H

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include <string>
#include <vector>

namespace TWOD_MASS_WL_CatalogSplitter {

/**
 * @class BasicSplitter
 * @brief Class that performs a basic catalog split, based only on the number of splits wanted for
 * right ascension, declination and redshift over the full sky
 */
class BasicSplitter {

public:

  /**
   * @brief Destructor
   */
  virtual ~BasicSplitter() = default;

  /**
   * @brief Constructor
   * @param[in] inputCatalogFilename the input catalog to split
   * @param[in] outputCatalogRootName the root name of the output catalogs, that will be completed
   * by the value of the right ascension, declination and redshift indices
   * @param[in] nbCatalogsRa the number of subcatalogs to create over the right ascension
   * @param[in] nbCatalogsDec the number of subcatalogs to create over the declination
   * @param[in] nbCatalogsZ the number of subcatalogs to create over the redshift
   */
  BasicSplitter(std::string inputCatalogFilename, std::string outputCatalogRootName,
                unsigned int nbCatalogsRa, unsigned int nbCatalogsDec, unsigned int nbCatalogsZ,
                TWOD_MASS_WL_MassMapping::Boundaries &boundaries);

  /**
   * @brief Method to split the catalog
   * @return true if the split worked fine, false otherwise
   */
  virtual bool splitCatalog();

  /**
   * @brief Method to split the catalog
   * @return true if the split worked fine, false otherwise
   */
  virtual bool splitCatalogWithMask();

  /**
   * @brief Method that splits the full sky in patches of 10x10 degrees
   * @return a vector of pairs containing the center coordinates of the patches
   */
  std::vector<std::pair<float, float> > getCenterOfPatches();

  /**
   * @brief Method that parses the all catalog and retrieves patch where information exists
   * @param[in] a vector of the coordinates of the patch centers
   * @return a vector of pairs containing the center coordinates of patches where information exists
   */
  std::vector<std::pair<float, float> > getMaskOfCatalog(std::vector<std::pair<float, float> > &patchCenters);

  /**
   * @brief Method that gets the closest patch center of a given coordinate
   * @param[in] a vector of the coordinates of the patch centers
   * @param[in] ra the right ascension of the input coordinate
   * @param[in] dec the declination of the input coordinate
   * @return the index in the given vector of patches the closest to the given (ra, dec)
   */
  unsigned int getClosestPatch(std::vector<std::pair<float, float> > const &patchCenters, float ra, float dec);

private:

  std::string m_inputCatalogFilename;
  std::string m_outputCatalogRootName;
  const unsigned int m_nbCatalogsRa;
  const unsigned int m_nbCatalogsDec;
  const unsigned int m_nbCatalogsZ;
  TWOD_MASS_WL_MassMapping::Boundaries m_boundaries;

}; /* End of BasicSplitter class */

} /* namespace TWOD_MASS_WL_CatalogSplitter */


#endif
