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
 * @file src/lib/ConvergenceMap.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "fftw3.h"

namespace TWOD_MASS_WL_MassMapping {

ConvergenceMap::ConvergenceMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                               unsigned long nGalaxies):
  GlobalMap(array, sizeXaxis, sizeYaxis, sizeZaxis, nGalaxies)
{
}

ConvergenceMap::ConvergenceMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                               Boundaries &boundaries, unsigned long nGalaxies):
  GlobalMap(array, sizeXaxis, sizeYaxis, sizeZaxis, boundaries, nGalaxies)
{
}

ConvergenceMap::ConvergenceMap(std::string filename):GlobalMap(filename)
{
}

ShearMap ConvergenceMap::getShearMap()
{
  double fftFactor = 1.0/m_sizeXaxis/m_sizeYaxis;

  // Create the complex maps
  fftw_complex* kappa_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);
  fftw_complex* fft_kappa_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);
  fftw_complex* Psi_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex* gamma_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex* fft_gamma_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);

  // Create the plans for transformation
  fftw_plan plan_k_forward;
  fftw_plan plan_g_backward;

  #pragma omp critical
  {
    plan_k_forward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, kappa_complex, fft_kappa_complex,
                                      FFTW_FORWARD,  FFTW_MEASURE);
    plan_g_backward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, fft_gamma_complex, gamma_complex,
                                       FFTW_BACKWARD,  FFTW_MEASURE);
  }

  // Fill the complex kappa map with convergence map values
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      kappa_complex[j*m_sizeXaxis +i][0] = (*m_mapValues)[i][j][0];
      kappa_complex[j*m_sizeXaxis +i][1] = (*m_mapValues)[i][j][1];
    }
  }

  // Perform the fourier transform of the complex convergence map
  fftw_execute(plan_k_forward);

  // Create the P factor
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
//        if (i+j == 0) continue;
      int l1 = (double(i) <= double(m_sizeXaxis)/2. ? i : i - m_sizeXaxis);
      int l2 = (double(j) <= double(m_sizeYaxis)/2. ? j : j - m_sizeYaxis);
      Psi_complex[j*m_sizeXaxis +i][0] = double(l1*l1-l2*l2)/double(l1*l1+l2*l2);
      Psi_complex[j*m_sizeXaxis +i][1] = double(2.*(l1*l2))/double(l1*l1+l2*l2);
   }
  }
  Psi_complex[0][0] = 0.0;
  Psi_complex[0][1] = 0.0;


  // Create the complex shear map in Fourier space
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      fft_gamma_complex[j*m_sizeXaxis+i][0] = Psi_complex[j*m_sizeXaxis+i][0]*fft_kappa_complex[j*m_sizeXaxis+i][0]
                                             -Psi_complex[j*m_sizeXaxis+i][1]*fft_kappa_complex[j*m_sizeXaxis+i][1];
      fft_gamma_complex[j*m_sizeXaxis+i][1] = Psi_complex[j*m_sizeXaxis+i][0]*fft_kappa_complex[j*m_sizeXaxis+i][1]
                                             +Psi_complex[j*m_sizeXaxis+i][1]*fft_kappa_complex[j*m_sizeXaxis+i][0];
    }
  }

  // Perform inverse Fourier transform to get the shear map
  fftw_execute(plan_g_backward);

  // Fill the shear map
  double *gammaArray = new double[m_sizeXaxis*m_sizeYaxis*m_sizeZaxis];

  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      gammaArray[j*m_sizeXaxis +i] = gamma_complex[j*m_sizeXaxis +i][0]*fftFactor;
      gammaArray[m_sizeXaxis*m_sizeYaxis + j*m_sizeXaxis +i] = gamma_complex[j*m_sizeXaxis +i][1]*fftFactor;
   }
  }

  ShearMap gammaMap(gammaArray, m_sizeXaxis, m_sizeYaxis, m_sizeZaxis, m_boundaries, m_numberOfGalaxies);

  // free memory
  delete [] gammaArray;
  gammaArray = nullptr;

  fftw_destroy_plan(plan_k_forward);
  fftw_destroy_plan(plan_g_backward);

  fftw_free(gamma_complex);
  fftw_free(Psi_complex);
  fftw_free(kappa_complex);
  fftw_free(fft_gamma_complex);
  fftw_free(fft_kappa_complex);

  return gammaMap;
}

} // TWOD_MASS_WL_MassMapping namespace
