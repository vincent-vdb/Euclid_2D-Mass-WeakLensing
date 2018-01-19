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
 * @file src/lib/Boundaries.cpp
 * @date 05/23/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

namespace TWOD_MASS_WL_MassMapping {

Boundaries::Boundaries(double raMin, double raMax, double decMin, double decMax, double zMin, double zMax):
m_raMin(raMin), m_raMax(raMax), m_decMin(decMin), m_decMax(decMax), m_zMin(zMin), m_zMax(zMax)
{
}

double Boundaries::getRaMin() const
{
  return m_raMin;
}

double Boundaries::getRaMax() const
{
  return m_raMax;
}

double Boundaries::getDecMin() const
{
  return m_decMin;
}

double Boundaries::getDecMax() const
{
  return m_decMax;
}

double Boundaries::getZMin() const
{
  return m_zMin;
}

double Boundaries::getZMax() const
{
  return m_zMax;
}

} // TWOD_MASS_WL_MassMapping namespace
