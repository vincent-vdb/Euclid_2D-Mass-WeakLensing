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
 * @file src/lib/DataFilesLoader.cpp
 * @date 01/17/18
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/DataFilesLoader.h"

#include <vector>

namespace TWOD_MASS_WL_MassMapping {

std::string DataFilesLoader::downloadTestFiles() const {

  std::vector<std::string> dataFiles = std::vector<std::string>();
  // MassMapping module test files
  dataFiles.push_back("convMap.fits");
  dataFiles.push_back("convMap.txt");
  dataFiles.push_back("convMapGaus.txt");
  dataFiles.push_back("conv_holes_test.fits");
  dataFiles.push_back("shearBackMap.txt");
  dataFiles.push_back("shearMap.txt");
  dataFiles.push_back("shear_holes_test.fits");
  dataFiles.push_back("testMap.fits");

  // MapMaker module test files
  dataFiles.push_back("FITScatalog_noDec_noz.fits");
  dataFiles.push_back("FITScatalog_noG1_noz.fits");
  dataFiles.push_back("FITScatalog_noG2_noz.fits");
  dataFiles.push_back("FITScatalog_noK_noz.fits");
  dataFiles.push_back("FITScatalog_noK_withZ.fits");
  dataFiles.push_back("FITScatalog_noRa_noz.fits");
  dataFiles.push_back("FITScatalog_ok_noz.fits");
  dataFiles.push_back("SSVcatalog_noDec_noz.txt");
  dataFiles.push_back("SSVcatalog_noG1_noz.txt");
  dataFiles.push_back("SSVcatalog_noG2_noz.txt");
  dataFiles.push_back("SSVcatalog_noK_noz.txt");
  dataFiles.push_back("SSVcatalog_noRa_noz.txt");
  dataFiles.push_back("SSVcatalog_ok_noz.txt");
  dataFiles.push_back("SSVcatalog_ok_withz.txt");

  // PeakCount module test files
  dataFiles.push_back("shearMap.fits");
  dataFiles.push_back("densMap.fits");
  dataFiles.push_back("shearMap_density.fits");

  // PeakCount module test files
  dataFiles.push_back("badXML.xml");
  dataFiles.push_back("emptyXML.xml");

  std::string protocol = std::string("https://");
  std::string server = std::string("ccirdeuclid.in2p3.fr");
  std::string port = std::string(":443");

  std::string remotePath = std::string("/euclid-fr/home/pfle3write/wl/2dmass-wl/tests/data/");
  std::string localPath = std::string("ccirdeuclid.in2p3.fr/euclid-fr/home/pfle3write/wl/2dmass-wl/tests/data/");
  std::string cli = std::string("wget -r -nc --no-check-certificate --user=pfle3write --password=aldI78PFIqb7Z");
  cli.append(" ");
  cli.append(protocol);
  cli.append(server);
  cli.append(port);
  cli.append(remotePath);

  //
  // -- create data files
  //
  for (unsigned int i=0; i != dataFiles.size() ; i++ )
  {
    std::string command = cli;
    command.append(std::string(dataFiles[i]));
    int status = system(command.c_str());
    // In case of error return an empty string
    if (status == -1)
    {
      return "";
    }
  }

  //
  // -- create tmp directory
  //
  std::string command = cli;
  command.append("/tmp/");
  int status = system(command.c_str());
  // In case of error return an empty string
  if (status == -1)
  {
    return "";
  }

  return localPath;
}


} // TWOD_MASS_WL_MassMapping namespace
