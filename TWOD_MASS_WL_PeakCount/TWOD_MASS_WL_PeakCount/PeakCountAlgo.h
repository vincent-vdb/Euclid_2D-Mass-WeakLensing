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
 * @file TWOD_MASS_WL_PeakCount/PeakCountAlgo.h
 * @date 05/22/17
 * @author user
 */

#ifndef TWOD_MASS_WL_PEAKCOUNT_PEAKCOUNTALGO_H
#define TWOD_MASS_WL_PEAKCOUNT_PEAKCOUNTALGO_H

#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "TWOD_MASS_WL_MassMapping/ImProcessing.h"
#include "TWOD_MASS_WL_MapMaker/FITSCatalogHandler.h"
#include "TWOD_MASS_WL_MassMapping/Image.h"

namespace TWOD_MASS_WL_PeakCount {

/**
 * @class PeakCountAlgo
 * @brief
 *
 */
class PeakCountAlgo {

public:

  /**
   * @brief Destructor
   */
  virtual ~PeakCountAlgo() = default;

  /**
   * @brief Constructor
   * @param[in] convMap the convergence map on which to compute the peak counting
   * @param[in] densityMap the galaxy density map needed for SNR estimation
   */
  PeakCountAlgo(TWOD_MASS_WL_MassMapping::ConvergenceMap &convMap, TWOD_MASS_WL_MassMapping::GlobalMap &densityMap);

  /**
   * @brief Global method computing the peak counting and saving it as a FITS catalog
   * @param[in] filename the FITS catalog output filename
   *
   * @return true if a peak catalog is well created, false otherwise
   */
  bool savePeakCatalog(std::string filename);

  /**
   * @brief method that returns the peaks from a vector of bands
   * @param[in] myBand the vector containing all the scales of the wavelet decomposition of the convergence map
   *
   * @return a vector containing all the peak information: right ascension, declination, redshift, scale and SNR
   */
  std::vector<std::vector<double> > getPeaks(std::vector<TWOD_MASS_WL_MassMapping::Image> &myBand);

  /**
   * @brief method that returns the SNR image from an input image and a noise value
   * @param[in] inputKappaImage the input image on which to measure the SNR
   * @param[in] globalNoise the input noise to be used to measure the SNR
   *
   * @return a SNR image
   */
  TWOD_MASS_WL_MassMapping::Image getSNRimage(TWOD_MASS_WL_MassMapping::Image inputKappaImage, double globalNoise);

private:

  TWOD_MASS_WL_MassMapping::ConvergenceMap m_convMap;
  TWOD_MASS_WL_MassMapping::GlobalMap m_densityMap;

  TWOD_MASS_WL_MassMapping::ImProcessing m_IP;

  unsigned int m_sizeXaxis;
  unsigned int m_sizeYaxis;
  unsigned int m_sizeZaxis;

}; /* End of PeakCountAlgo class */

} /* namespace TWOD_MASS_WL_PeakCount */


#endif
