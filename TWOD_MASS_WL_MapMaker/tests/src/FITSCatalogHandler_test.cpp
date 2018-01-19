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
 * @file tests/src/FITSCatalogHandler_test.cpp
 * @date 04/27/17
 * @author user
 */

#include <boost/test/unit_test.hpp>

#include "TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h"

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

using namespace TWOD_MASS_WL_MassMapping;
using namespace TWOD_MASS_WL_MapMaker;

DataFilesLoader myLoader;
std::string pathFiles = myLoader.downloadTestFiles();

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FITSCatalogHandler_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( goodCatalogNoZ_test ) {

  // Open a FITS catalog with data about ra, dec, gamma and kappa
  std::string catalogPath(pathFiles+"FITScatalog_ok_noz.fits");
  FITSCatalogHandler myCatalog(catalogPath);

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

BOOST_AUTO_TEST_CASE( goodCatalogWithZ_test ) {

  // Open a FITS catalog with data about ra, dec, gamma and kappa
  std::string catalogPath =
  pathFiles+"FITScatalog_noK_withZ.fits";
  FITSCatalogHandler myCatalog(catalogPath);

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
}

BOOST_AUTO_TEST_CASE( fakeCatalog_test ) {

  // Provide a FITS file that does not exist
  std::string catalogPath(pathFiles+"dummy.fits");
  FITSCatalogHandler myCatalogNoFile(catalogPath);

  // It should not be possible to create a shear map out of this catalog
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ShearMap *myShearMap = myCatalogNoFile.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

  // It should not be possible to create a convergence map out of this catalog
  ConvergenceMap *myConvMap = myCatalogNoFile.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
}

BOOST_AUTO_TEST_CASE( missingRaDecCatalog_test ) {

  // Open a FITS catalog without data about ra
  std::string catalogPath(pathFiles+"FITScatalog_noRa_noz.fits");
  FITSCatalogHandler myCatalogNoRa(catalogPath);

  // It should return a nullptr for any kind of map
  Boundaries bounds(40, 50, 0, 10, 0, 10);
  ConvergenceMap *myConvMap = myCatalogNoRa.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
  ShearMap *myShearMap = myCatalogNoRa.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);


  // Open a FITS catalog without data about dec
  catalogPath = pathFiles+"FITScatalog_noDec_noz.fits";
  FITSCatalogHandler myCatalogNoDec(catalogPath);

  // It should return a nullptr for any kind of map
  myConvMap = myCatalogNoDec.getConvergenceMap(bounds, 256, 256);
  BOOST_CHECK(myConvMap==nullptr);
  myShearMap = myCatalogNoDec.getShearMap(bounds, 256, 256);
  BOOST_CHECK(myShearMap==nullptr);

}

BOOST_AUTO_TEST_CASE( missingKappaCatalog_test ) {

  // Open a FITS catalog without data about Kappa
  std::string catalogPath(pathFiles+"FITScatalog_noK_noz.fits");
  FITSCatalogHandler myCatalogNoKappa(catalogPath);

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

  // Open a FITS catalog without data about Gamma1
  std::string catalogPath(pathFiles+"FITScatalog_noG1_noz.fits");
  FITSCatalogHandler myCatalogNoGamma1(catalogPath);

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

  // Open a FITS catalog without data about Gamma1
  std::string catalogPath(pathFiles+"FITScatalog_noG2_noz.fits");
  FITSCatalogHandler myCatalogNoGamma2(catalogPath);

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

BOOST_AUTO_TEST_CASE( saveCatalog_test ) {

  // Provide a FITS file for output where to save
  std::string catalogPath(pathFiles+"tmp/trash.fits");
  FITSCatalogHandler myCatalogNoFile(catalogPath);

  // Create an input vector of vector containing catalog inputs
  std::vector<double> raArray(5, 10.);
  std::vector<double> decArray(5, 18.);
  std::vector<double> SNRArray(5, 2.8);
  std::vector<double> zArray(5, 1.3);
  std::vector<double> scaleArray(5, 3.);
  std::vector< std::vector<double>> data;
  data.push_back(raArray);
  data.push_back(decArray);
  data.push_back(SNRArray);
  data.push_back(zArray);

  // There is not the needed number of rows in that catalog, it should not work
  BOOST_CHECK(myCatalogNoFile.saveAsFitsCatalog(data)==false);

  // Then add the last column to the vector
  data.push_back(scaleArray);

  // Now the saving of catalog should work fine
  BOOST_CHECK(myCatalogNoFile.saveAsFitsCatalog(data)==true);

}
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
