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
 * @file src/lib/CatalogHandler.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MapMaker/CatalogHandler.h"
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include <cmath>
#include <cstring>


const double pi = 3.14159265358979323846;

using namespace TWOD_MASS_WL_MassMapping;

namespace TWOD_MASS_WL_MapMaker {

CatalogHandler::~CatalogHandler()
{
  if (m_galDensity!=nullptr)
  {
    delete m_galDensity;
    m_galDensity = nullptr;
  }
}

CatalogHandler::CatalogHandler(std::string filename): m_catalogFilename(filename), m_galDensity(nullptr)
{
}

std::pair<double, double> CatalogHandler::getGnomonicProjection(double ra, double dec, double ra0, double dec0)
{
  ra *= pi/180.;
  dec *= pi/180.;
  ra0 *= pi/180.;
  dec0 *= pi/180.;

  double cosc = sin(dec0)*sin(dec) + cos(dec0)*cos(dec)*cos(ra-ra0);
  double x = 1./cosc*cos(dec)*sin(ra-ra0);
  double y = 1./cosc*(cos(dec0)*sin(dec) - sin(dec0)*cos(dec)*cos(ra-ra0));
/*
  std::cout<<"x, y old school: "<<x<<" "<<y<<std::endl;

  ///////////////////////////////////////////
  wcserr_enable(1);
  struct prjprm prj;
//  prjini(&prj);
  std::cout<<"status of prjini: "<<prjini(&prj)<<std::endl;
//  prj.phi0 = 0.;//ra0*180./pi;
//  prj.theta0 = 0.;//dec0*180./pi;
  prj.r0 = 0;
  strncpy(prj.code, "TAN", 4);
  prj.flag = 0;
*/

/*
  // Perform the tanset stuffs

  prj.flag = 103;
  strcpy(prj.code, "TAN");

  if (prj.r0 == 0.0) prj.r0 = 180./pi;;

  strcpy(prj.name, "gnomonic");
  prj.category  = ZENITHAL;
  prj.pvrange   = 0;
  prj.simplezen = 1;
  prj.equiareal = 0;
  prj.conformal = 0;
  prj.global    = 0;
  prj.divergent = 1;

  prj.prjx2s = tanx2s;
  prj.prjs2x = tans2x;

  // Perform the projoff stuffs
  int    stat;
  double x0, y0;

  prj.x0 = 0.0;
  prj.y0 = 0.0;

  if (prj.phi0 == 987654321.0e99 || prj.theta0 == 987654321.0e99) {
    // Set both to the projection-specific default if either undefined.
    prj.phi0   = 0.;
    prj.theta0 = 90.;

  } else {
    if (prj.prjs2x(&prj, 1, 1, 1, 1, &(prj.phi0), &(prj.theta0), &x0, &y0,
                    &stat)) {
      std::cout<<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"<<std::endl;
    }

    prj.x0 = x0;
    prj.y0 = y0;
  }
*/
/*

  std::cout<<"phi0, theta0: "<<prj.phi0<<" "<<prj.theta0<<std::endl;

  std::cout<<"status of tanset: "<<tanset(&prj)<<std::endl;

  double phi = 0.;//ra;
  double theta = 0.;//dec;
  double xx, yy;
  int status;

  std::cout<<"status of the projection: "<<tans2x(&prj, 1, 1, 1, 1 , &phi, &theta, &xx, &yy, &status)<<std::endl;
//  std::cout<<"status of the projection: "<<prjs2x(&prj, 1, 1, 1, 1 , &phi, &theta, &xx, &yy, &status)<<std::endl;

  std::cout<<"errors outputs: "<<prj.err->msg<<std::endl;
  std::cout<<"x, y: "<<xx<<" "<<yy<<std::endl;

  std::cout<<"phi0, theta0: "<<prj.phi0<<" "<<prj.theta0<<std::endl;
  std::cout<<"x0, y0: "<<prj.x0<<" "<<prj.y0<<std::endl;
  std::cout<<"flag value: "<<prj.flag<<std::endl;
  std::cout<<"code value: "<<prj.code<<std::endl;
  std::cout<<"r0 value: "<<prj.r0<<std::endl;
  std::cout<<"bounds value: "<<prj.bounds<<std::endl;
  std::cout<<"pv[0] value: "<<prj.pv[0]<<std::endl;
*/
  //////////////////////////////////////

  return std::pair<double, double> (x, y);
}

std::pair<double, double> CatalogHandler::getInverseGnomonicProjection(double x, double y, double ra0, double dec0)
{
  // Handle the case where x = y = 0 when ra = ra0 and dec = dec0
  if (fabs(x)<0.01 && fabs(y)<0.01)
  {
    return std::pair<double, double> (ra0, dec0);
  }
  ra0 *= pi/180.;
  dec0 *= pi/180.;

  double rho = sqrt(x*x + y*y);
  double c = atan(rho);

  double dec = asin(cos(c)*sin(dec0) + y*sin(c)*cos(dec0)/rho);
  double ra = ra0 + atan( x*sin(c)/(rho*cos(dec0)*cos(c) - y*sin(dec0)*sin(c)) );

  dec *= 180./pi;
  ra *= 180./pi;

  return std::pair<double, double> (ra, dec);
}

bool CatalogHandler::checkRaDecZ(TWOD_MASS_WL_MassMapping::Boundaries &bounds)
{
  // Check ra min is lower than ra max
  if (bounds.getRaMin()>=bounds.getRaMax())
  {
    return false;
  }
  // Check dec min in lower than dec max
  if (bounds.getDecMin()>=bounds.getDecMax())
  {
    return false;
  }
  // Check z min in lower than a max
  if (bounds.getZMin()>=bounds.getZMax())
  {
    return false;
  }
  // Check values are in the good range
  if (bounds.getRaMin()<0. || bounds.getRaMax()>360 || bounds.getDecMin()<-90 || bounds.getDecMax()>90)
  {
    return false;
  }
  return true;
}

ConvergenceMap* CatalogHandler::getConvergenceMapFromArray(std::pair<long, double*> arrayPair,
                                                           const unsigned int nbBinsX, const unsigned int nbBinsY,
                                                           TWOD_MASS_WL_MassMapping::Boundaries &bounds)
{
  // If there is no output return a nullptr
  if (arrayPair.second==nullptr)
  {
    return nullptr;
  }
  if (arrayPair.first==0)
  {
    delete [] arrayPair.second;
    arrayPair.second = nullptr;
    return nullptr;
  }

  // Otherwise create the map
  ConvergenceMap *myConvMap = new ConvergenceMap(arrayPair.second, nbBinsX, nbBinsY, 2, bounds, arrayPair.first);

  // Destroy the memory of the array
  delete [] arrayPair.second;
  arrayPair.second = nullptr;

  // And finally return the map
  return myConvMap;
}

ShearMap* CatalogHandler::getShearMapFromArray(std::pair<long, double*> arrayPair,
                                               const unsigned int nbBinsX, const unsigned int nbBinsY,
                                               TWOD_MASS_WL_MassMapping::Boundaries &bounds)
{
  // If there is no output return a nullptr
  if (arrayPair.second==nullptr)
  {
    return nullptr;
  }
  if (arrayPair.first==0)
  {
    delete [] arrayPair.second;
    arrayPair.second = nullptr;
    return nullptr;
  }

  // Otherwise create the map
  ShearMap *myShearMap = new ShearMap(arrayPair.second, nbBinsX, nbBinsY, 2, bounds, arrayPair.first);

  // Destroy the memory of the array
  delete [] arrayPair.second;
  arrayPair.second = nullptr;

  // And finally return the map
  return myShearMap;
}

TWOD_MASS_WL_MassMapping::GlobalMap* CatalogHandler::getDensityMap() const
{
  return m_galDensity;
}

} // TWOD_MASS_WL_MapMaker namespace
