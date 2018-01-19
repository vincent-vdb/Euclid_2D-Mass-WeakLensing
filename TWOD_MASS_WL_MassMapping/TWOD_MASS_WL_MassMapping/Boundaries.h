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
 * @file TWOD_MASS_WL_MassMapping/Boundaries.h
 * @date 05/23/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MASSMAPPING_BOUNDARIES_H
#define TWOD_MASS_WL_MASSMAPPING_BOUNDARIES_H

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class Boundaries
 * @brief
 *
 */
class Boundaries {

public:

  /**
   * @brief Destructor
   */
  virtual ~Boundaries() = default;

  /**
   * @brief Constructor of a Boundaries object
   * @param[in] raMin the minimum right ascension
   * @param[in] raMax the maximum right ascension
   * @param[in] decMin the minimum declination
   * @param[in] decMax the maximum declination
   * @param[in] zMin the minimum redshift
   * @param[in] zMax the maximum redshift
   *
   */
  Boundaries(double raMin, double raMax, double decMin, double decMax, double zMin, double zMax);

  /**
   * @brief Returns the value of the minimum right ascension
   * @return the value of the minimum right ascension
   */
  double getRaMin() const;

  /**
   * @brief Returns the value of the maximum right ascension
   * @return the value of the maximum right ascension
   */
  double getRaMax() const;

  /**
    * @brief Returns the value of the minimum declination
    * @return the value of the minimum declination
    */
  double getDecMin() const;

  /**
    * @brief Returns the value of the maximum declination
    * @return the value of the maximum declination
    */
  double getDecMax() const;

  /**
    * @brief Returns the value of the minimum redshift
    * @return the value of the minimum redshift
    */
  double getZMin() const;

  /**
    * @brief Returns the value of the maximum redshift
    * @return the value of the maximum redshift
    */
  double getZMax() const;


private:

  double m_raMin;
  double m_raMax;
  double m_decMin;
  double m_decMax;
  double m_zMin;
  double m_zMax;


}; /* End of Boundaries class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
