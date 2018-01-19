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
 * @file src/lib/ShearMap.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/ShearMap.h"
#include "TWOD_MASS_WL_MassMapping/ConvergenceMap.h"
#include "fftw3.h"

namespace TWOD_MASS_WL_MassMapping {

ShearMap::ShearMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                   unsigned long nGalaxies):
  GlobalMap(array, sizeXaxis, sizeYaxis, sizeZaxis, nGalaxies)
{
}

ShearMap::ShearMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                   Boundaries &boundaries, unsigned long nGalaxies):
  GlobalMap(array, sizeXaxis, sizeYaxis, sizeZaxis, boundaries, nGalaxies)
{
}

ShearMap::ShearMap(std::string filename):GlobalMap(filename)
{
}

ConvergenceMap ShearMap::getConvergenceMap()
{
  double fftFactor = 1.0/m_sizeXaxis/m_sizeYaxis;

  // Create the complex maps
  fftw_complex *gamma_complex  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex *fft_gamma_complex  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex *Psi_complex  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex *fft_kappa_complex  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex *kappa_complex  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);

  // Create the plans for transformation
  fftw_plan plan_k_backward;
  fftw_plan plan_g_forward;

  #pragma omp critical
  {
    // Create the plans for transformation
    plan_k_backward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, fft_kappa_complex, kappa_complex,
                                       FFTW_BACKWARD,  FFTW_MEASURE);
    plan_g_forward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, gamma_complex, fft_gamma_complex,
                                      FFTW_FORWARD,  FFTW_MEASURE);
  }

  // Fill the complex gamma map with shear map values
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      gamma_complex[j*m_sizeXaxis +i][0] = (*m_mapValues)[i][j][0];
      gamma_complex[j*m_sizeXaxis +i][1] = (*m_mapValues)[i][j][1];
    }
  }

  // Perform the fourier transform of the complex shear map
  fftw_execute(plan_g_forward);

  // Create the P factor
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
//        if (i+j == 0) continue;
      int l1 = (double(i) <= double(m_sizeXaxis)/2. ? i : i - m_sizeXaxis);
      int l2 = (double(j) <= double(m_sizeYaxis)/2. ? j : j - m_sizeYaxis);
      Psi_complex[j*m_sizeXaxis +i][0] = double(l1*l1-l2*l2)/double(l1*l1+l2*l2);
      Psi_complex[j*m_sizeXaxis +i][1] = -double(2.*(l1*l2))/double(l1*l1+l2*l2);
    }
  }
  Psi_complex[0][0] = 0.0;
  Psi_complex[0][1] = 0.0;

  // Create the complex convergence map in Fourier space
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      fft_kappa_complex[j*m_sizeXaxis+i][0] = Psi_complex[j*m_sizeXaxis+i][0]*fft_gamma_complex[j*m_sizeXaxis+i][0]
                                             -Psi_complex[j*m_sizeXaxis+i][1]*fft_gamma_complex[j*m_sizeXaxis+i][1];
      fft_kappa_complex[j*m_sizeXaxis+i][1] = Psi_complex[j*m_sizeXaxis+i][0]*fft_gamma_complex[j*m_sizeXaxis+i][1]
                                             +Psi_complex[j*m_sizeXaxis+i][1]*fft_gamma_complex[j*m_sizeXaxis+i][0];
    }
  }

  // Perform inverse Fourier transform to get the convergence map
  fftw_execute(plan_k_backward);

  // Fill the convergence map
  double *kappaArray = new double[m_sizeXaxis*m_sizeYaxis*m_sizeZaxis];
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      kappaArray[j*m_sizeXaxis +i] = kappa_complex[j*m_sizeXaxis +i][0]*fftFactor;
      kappaArray[m_sizeXaxis*m_sizeYaxis + j*m_sizeXaxis +i] = kappa_complex[j*m_sizeXaxis +i][1]*fftFactor;
    }
  }

  ConvergenceMap kappaMap(kappaArray, m_sizeXaxis, m_sizeYaxis, m_sizeZaxis, m_boundaries, m_numberOfGalaxies);

  // free memory
  delete [] kappaArray;
  kappaArray = nullptr;

  fftw_destroy_plan(plan_g_forward);
  fftw_destroy_plan(plan_k_backward);

  fftw_free(gamma_complex);
  fftw_free(Psi_complex);
  fftw_free(kappa_complex);
  fftw_free(fft_gamma_complex);
  fftw_free(fft_kappa_complex);

  return kappaMap;
}

void ShearMap::computeReducedShear(ConvergenceMap inputConvMap)
{
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      for (unsigned int k=0; k<m_sizeZaxis; k++)
      {
        (*m_mapValues)[i][j][k] /= 1. -  inputConvMap.getBinValue(i, j, k);
      }
    }
  }
}


} // TWOD_MASS_WL_MassMapping namespace
