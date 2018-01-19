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
 * @file tests/src/SSVCatalogHandler_test.cpp
 * @date 04/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MapMaker/SSVCatalogHandler.h"

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;
using namespace TWOD_MASS_WL_MapMaker;

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SSVCatalogHandler_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( goodCatalogNoZ_test ) {

  // Open a SSV catalog with data about ra, dec, gamma and kappa but no z
  std::string ssvcatalogPath(pathFiles+"SSVcatalog_ok_noz.txt");
  SSVCatalogHandler myCatalog(ssvcatalogPath);

  // It should be possible to create a shear map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ShearMap *myShearMap = myCatalog.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap!=nullptr);

  // Delete the pointer
  delete myShearMap;
  myShearMap = nullptr;


  // It should be possible to create a convergence map out of this catalog
  ConvergenceMap *myConvMap = myCatalog.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap!=nullptr);

  // Check the dimensions of the created map are the one asked
  BOOST_CHECK(myConvMap->getXdim()==256);
  BOOST_CHECK(myConvMap->getYdim()==256);
  BOOST_CHECK(myConvMap->getZdim()==2);

  // Delete the pointer
  delete myConvMap;
  myConvMap = nullptr;


  // It should return a nullptr when providing bad Ra Dec input parameters
  // Here raMin > raMax should return nullptr
  Boundaries badRaBounds(50, 40, 0, 10, 0, 10);
  myConvMap = myCatalog.getConvergenceMap(badRaBounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);

  // Here decMin > decMax should return nullptr
  Boundaries badDecBounds(40, 50, 10, 0, 0, 10);
  myConvMap = myCatalog.getConvergenceMap(badDecBounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);

  // Here zMin > zMax  should return nullptr
  Boundaries badZbounds(40, 50, 0, 10, 10, 0);
  myConvMap = myCatalog.getConvergenceMap(badZbounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);

}

BOOST_AUTO_TEST_CASE( goodCatalogwithZ_test ) {

  // Open a SSV catalog with data about ra, dec, gamma and kappa but no z
  std::string ssvcatalogPath =
  pathFiles+"SSVcatalog_ok_withz.txt";
  SSVCatalogHandler myCatalog(ssvcatalogPath);

  // It should be possible to create a shear map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ShearMap *myShearMap = myCatalog.getShearMap(bounds, 256, 256, true);
  BOOST_CHECK(myShearMap!=nullptr);

  // Check the dimensions of the created map are the one asked
  BOOST_CHECK(myShearMap->getXdim()==256);
  BOOST_CHECK(myShearMap->getYdim()==256);
  BOOST_CHECK(myShearMap->getZdim()==2);

  // Delete the pointer
  delete myShearMap;
  myShearMap = nullptr;


  // It should be possible to create a convergence map out of this catalog
  ConvergenceMap *myConvMap = myCatalog.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap!=nullptr);

  // Delete the pointer
  delete myConvMap;
  myConvMap = nullptr;
}

BOOST_AUTO_TEST_CASE( fakeCatalog_test ) {

  // Provide a SSV file that does not exist
  std::string catalogPath(pathFiles+"dummy.fits");
  SSVCatalogHandler myCatalogNoFile(catalogPath);

  // It should not be possible to create a shear map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ShearMap *myShearMap = myCatalogNoFile.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

  // It should not be possible to create a convergence map out of this catalog
  ConvergenceMap *myConvMap = myCatalogNoFile.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
}

BOOST_AUTO_TEST_CASE( missingRaDecCatalog_test ) {

  // Open a SSV catalog without data about ra
  std::string ssvcatalogPath(pathFiles+"SSVcatalog_noRa_noz.txt");
  SSVCatalogHandler myCatalogNoRa(ssvcatalogPath);

  // It should return a nullptr for any kind of map
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ConvergenceMap *myConvMap = myCatalogNoRa.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
  ShearMap *myShearMap = myCatalogNoRa.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);


  // Open a SSV catalog without data about dec
  ssvcatalogPath = pathFiles+"SSVcatalog_noDec_noz.txt";
  SSVCatalogHandler myCatalogNoDec(ssvcatalogPath);

  // It should return a nullptr for any kind of map
  myConvMap = myCatalogNoDec.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
  myShearMap = myCatalogNoDec.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

}

BOOST_AUTO_TEST_CASE( missingKappaCatalog_test ) {

  // Open a SSV catalog without data about Kappa
  std::string ssvcatalogPath(pathFiles+"SSVcatalog_noK_noz.txt");
  SSVCatalogHandler myCatalogNoKappa(ssvcatalogPath);

  // It should be possible to create a shear map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ShearMap *myShearMap = myCatalogNoKappa.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap!=nullptr);

  // Delete the pointer
  delete myShearMap;
  myShearMap = nullptr;

  // But it should not be able to create a convergence map
  ConvergenceMap *myConvMap = myCatalogNoKappa.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);

}

BOOST_AUTO_TEST_CASE( missingGamma1Catalog_test ) {

  // Open a SSV catalog without data about Gamma1
  std::string ssvcatalogPath(pathFiles+"SSVcatalog_noG1_noz.txt");
  SSVCatalogHandler myCatalogNoGamma1(ssvcatalogPath);

  // It should be possible to create a convergence map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ConvergenceMap *myConvMap = myCatalogNoGamma1.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap!=nullptr);

  // Delete the pointer
  delete myConvMap;
  myConvMap = nullptr;

  // But it should not be able to create a shear map
  ShearMap *myShearMap = myCatalogNoGamma1.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

}

BOOST_AUTO_TEST_CASE( missingGamma2Catalog_test ) {

  // Open a SSV catalog without data about Gamma1
  std::string ssvcatalogPath(pathFiles+"SSVcatalog_noG2_noz.txt");
  SSVCatalogHandler myCatalogNoGamma2(ssvcatalogPath);

  // It should be possible to create a convergence map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ConvergenceMap *myConvMap = myCatalogNoGamma2.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap!=nullptr);

  // Delete the pointer
  delete myConvMap;
  myConvMap = nullptr;

  // But it should not be able to create a shear map
  ShearMap *myShearMap = myCatalogNoGamma2.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


