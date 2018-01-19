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
 * @file TWOD_MASS_WL_Launcher/PFAlgo.h
 * @date 06/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_LAUNCHER_PFALGO_H
#define TWOD_MASS_WL_LAUNCHER_PFALGO_H

#include <string>
#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

namespace TWOD_MASS_WL_Launcher {

/**
 * @class PFAlgo
 * @brief
 *
 */
class PFAlgo {

public:

  /**
   * @brief Destructor
   */
  virtual ~PFAlgo() = default;

  /**
   * @brief Constructor
   * @param[in] bModes set to true to force the B modes at zero in the iterations
   * @param[in] nbIterInpainting the number of iterations in the inpainting
   * @param[in] nbIterReducedShear the number of iterations in the reduced shear
   * @param[in] nbScaleInpainting the number of scales in the inpainting
   * @param[in] variancePerScale set to true to force same variance in and out of the mask
   * @param[in] gaussianSmoothing the value of the gaussian smoothing
   * @param[in] denoisingVal the value of the denoising
   * @param[in] inputFITSCatalog the filename of the input FITS catalog
   * @param[in] inputSSVCatalog the filename of the input SSV catalog
   * @param[in] outputPeakCatalog the root filename of the output peak catalog
   * @param[in] outputConvergenceMap root the filename of the output convergence map
   * @param[in] raStep the expected right ascension step between two patches
   * @param[in] raDec the expected declination step between two patches
   * @param[in] zStep the expected redshift step between two patches
   * @param[in] squareMap set to true to have a square map in the projection plane
   * @param[in] boundaries the right ascension, declination and redshift min and max if only one patch to process
   * (set to zeros by default and not taken into account)
   *
   */
  PFAlgo(bool bModes, unsigned int nbIterInpainting, unsigned int nbIterReducedShear,
         unsigned int nbScaleInpainting, bool variancePerScale, float gaussianSmoothing, float denoisingVal,
         std::string inputFITSCatalog, std::string inputSSVCatalog, std::string outputPeakCatalog,
         std::string outputConvergenceMap, float raStep, float decStep,
         float zStep, bool squareMap,
         TWOD_MASS_WL_MassMapping::Boundaries boundaries=TWOD_MASS_WL_MassMapping::Boundaries(0, 0, 0, 0, 0, 0));

  /**
   * @brief Method to check the input parameters
   * @return true if the parameters are OK, false otherwise
   */
  bool checkParameters();

  /**
   * @brief Method to perform all the processing function
   * @param[in] outputPeakMap the filename of the output peak catalog
   * @param[in] outputConvMap the filename of the output convergence map
   * @param[in] boundaries the right ascension, declination and redshift min and max
   * @return true if the processing function is well performed, false otherwise
   */
  bool performProcessingFunction(std::string outputPeakMap, std::string outputConvMap,
      TWOD_MASS_WL_MassMapping::Boundaries boundaries);

  /**
   * @brief Method that computes the mask and launch the PF parallelized on every patches inside the mask
   * @return true if every patches went through the PF process, false otherwise
   */
  bool launchParalPF();

private:

  bool m_bModes;
  unsigned int m_nbIterInpainting;
  unsigned int m_nbIterReducedShear;
  unsigned int m_nbScaleInpainting;
  bool m_variancePerScale;
  float m_gaussianSmoothing;
  float m_denoisingVal;
  std::string m_inputFITSCatalog;
  std::string m_inputSSVCatalog;
  std::string m_outputPeakCatalog;
  std::string m_outputConvergenceMap;
  float m_raStep;
  float m_decStep;
  float m_zStep;
  bool m_squareMap;
  TWOD_MASS_WL_MassMapping::Boundaries m_boundaries;


}; /* End of PFAlgo class */

} /* namespace TWOD_MASS_WL_Launcher */


#endif
