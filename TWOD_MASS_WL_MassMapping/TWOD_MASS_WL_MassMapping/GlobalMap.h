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
 * @file TWOD_MASS_WL_MassMapping/GlobalMap.h
 * @date 04/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_MASSMAPPING_GLOBALMAP_H
#define TWOD_MASS_WL_MASSMAPPING_GLOBALMAP_H

#include "TWOD_MASS_WL_MassMapping/Boundaries.h"

#include "fftw3.h"
#include "boost/multi_array.hpp"
#include <string>
#include <vector>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class GlobalMap
 * @brief Generic class for maps
 *
 */
class GlobalMap {

public:

  /**
   * @brief Destructor
   */
  virtual ~GlobalMap();

  /**
   * @brief Constructor of a Map
   * @param[in] array input values to give to the class
   * @param[in] sizeXaxis number of pixels in the X axis
   * @param[in] sizeYaxis number of pixels in the Y axis
   * @param[in] sizeZaxis number of pixels in the Z axis
   * @param[in] nGalaxies number of galaxies in the map
   *
   * This constructor builds a generic Map of dimensions sizeXaxis, sizeYaxis, sizeZaxis
   * from the input data array, which should be arranged such that:
   * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
   *
   */
  GlobalMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
            unsigned long nGalaxies = 0);

  /**
   * @brief Constructor of a Map
   * @param[in] array input values to give to the class
   * @param[in] sizeXaxis number of pixels in the X axis
   * @param[in] sizeYaxis number of pixels in the Y axis
   * @param[in] sizeZaxis number of pixels in the Z axis
   * @param[in] boundaries Boundaries object containing ra, dec and z information
   * @param[in] nGalaxies number of galaxies in the map
   *
   * This constructor builds a generic Map of dimensions sizeXaxis, sizeYaxis, sizeZaxis
   * from the input data array, which should be arranged such that:
   * array(i,j,k) = array[i + j*sizeXaxis + k*sizeXaxis*sizeYaxis]
   *
   */
  GlobalMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
            Boundaries &boundaries, unsigned long nGalaxies = 0);


  /**
   * @brief Constructor of a Map
   * @param[in] filename name of a FITS input file
   *
   * This constructor builds a generic Map from provided FITS input file filename.
   * The provided FITS file should have the image as Primary Header in order to be
   * read properly.
   *
   */
  GlobalMap(std::string filename);

  /**
   * @brief Copy constructor of a Map
   * @param[in] copyMap the Map to copy
   *
   * This copy constructor builds a copy of the input Map
   *
   */
  GlobalMap(GlobalMap const& copyMap);

  /**
   * @brief Returns the value of the needed bin in the map
   * @param[in] binx bin on x axis
   * @param[in] biny bin on y axis
   * @param[in] binz bin on z axis
   * @return the value of the bin (double)
   *
   * This method returns the value of the bin (binx, biny, binz) in the map
   *
   */
  double getBinValue(unsigned int binx, unsigned int biny, unsigned int binz) const;

  /**
   * @brief Returns the value of the X dimension
   * @return the value of the X dimension (unsigned int)
   *
   * This method returns the value of the X dimension
   *
   */
  unsigned int getXdim() const;

  /**
   * @brief Returns the value of the Y dimension
   * @return the value of the Y dimension (unsigned int)
   *
   * This method returns the value of the Y dimension
   *
   */
  unsigned int getYdim() const;

  /**
   * @brief Returns the value of the Z dimension
   * @return the value of the Z dimension (unsigned int)
   *
   * This method returns the value of the Z dimension
   *
   */
  unsigned int getZdim() const;

  /**
   * @brief Saves the map as a FITS file
   * @param[in] filename name of the file where to save the map
   * @param[in] overwrite to be set to true to overwrite eventually already existing file filename
   * @return false if could not save properly (e.g. file already exists and overwrite to false) true otherwise
   *
   * This method saves the map as a FITS file into the provided filename
   *
   */
  bool saveToFITSfile(std::string filename, bool overwrite);

  /**
   * @brief Saves the map as a FITS file
   * @param[in] filename name of the file where to save the map
   * @param[in] overwrite to be set to true to overwrite eventually already existing file filename
   * @param[in] param a map that contains information to be saved in the header of the FITS image
   * @return false if could not save properly (e.g. file already exists and overwrite to false) true otherwise
   *
   * This method saves the map as a FITS file into the provided filename
   *
   */
  bool saveToFITSfile(std::string filename, bool overwrite, std::map<std::string, po::variable_value> param);

  /**
   * @brief Performs a forward Fourier transform
   * @param[in] inputMap the input map on which to perform the Fourier transform
   * @return an outputMap of the Fourier transform of the input map
   *
   * This method performs a forward Fourier transform on the inputMap and returns an outputMap
   *
   */
//  fftw_complex* performFFTforward(fftw_complex* inputMap);

  /**
   * @brief Performs a backward Fourier transform
   * @param[in] inputMap the input map on which to perform the backward Fourier transform
   * @return an outputMap of the backward Fourier transform of the input map
   *
   * This method performs a backward Fourier transform on the inputMap and returns an outputMap
   *
   */
//  fftw_complex* performFFTbackward(fftw_complex* inputMap);

  /**
   * @brief Outputs a gaussian kernel
   * @param[in] sizeX size of the kernel on the X direction
   * @param[in] sizeY size of the kernel on the Y direction
   * @param[in] sigmaX sigma of the gaussian kernel on the X direction
   * @param[in] sigmaY sigma of the gaussian kernel on the Y direction
   * @return a boost multi_array of size {sizeX, sizeY}
   *
   * This method outputs a boost multi_array of size {sizeX, sizeY} which contains a gaussian kernel
   * with sigmaX and sigmaY as sigma in the X and Y directions respectively
   *
   */
  boost::multi_array<double, 2> makeGaussianKernel(int sizeX, int sizeY, float sigmaX, float sigmaY);

  /**
   * @brief Outputs the mean values for each Z plans of the map
   * @return vector or double of size m_sizeZaxis containing the mean value
   * of the map for each of those Z plans
   *
   * This method returns a vector of the mean values of each of the Z plans as a vector
   *
   */
  std::vector<double> getMeanValues();

  /**
   * @brief Removes an offset value to each Z plans of the map
   * @param[in] a vector of size m_sizeZaxis with an offset value to remove in those plans
   *
   * This method removes an offset value to the entire map in each Z plan
   *
   */
  void removeOffset(std::vector<double>);

//  void readTableFITS(std::string filename);

  /**
   * @brief Resizes the map to target the galaxiesPerBin number of galaxies per bin
   * @param[in] galaxiesPerBin the target number of galaxies per bin expected
   * @return the binning applied on each dimension, meaning 1 if nothing was done (i.e.
   * the current binning was the closest to the target galaxiesPerBin) or a higher value
   * if binning was applied.
   *
   * This method rebins the map to get a mean number of galaxies per bin the closest
   * to the target input galaxiesPerBin
   *
   */
  unsigned int pixelate(float galaxiesPerBin);

  /**
   * @brief Resizes the map to target the input xBinning and yBinning targets
   * @param[in] xBinning the binning to apply to X dimension as power of two
   * (i.e. 1 to binning 2, 2 to binning 4...)
   * @param[in] yBinning the binning to apply to X dimension as power of two
   * (i.e. 1 to binning 2, 2 to binning 4...)
   * @return true if the binning was applied, false otherwise (e.g. xBinning < 1 or
   * xBinning > log2(bins of X dimension))
   *
   * This method rebins the map with input values xBinning and yBinning
   *
   */
  bool pixelate(unsigned int xBinning, unsigned int yBinning);

  /**
    * @brief Applies a gaussian filter of the ConvergenceMap
    * @param sigmaX sigma of the gaussian kernel on the X direction
    * @param sigmaY sigma of the gaussian kernel on the Y direction
    *
    * This method applies a gaussian filter to the Map of width
    * sigmaX and sigmaY in X and Y directions respectively
    *
    */
  void applyGaussianFilter(float sigmax, float sigmay);

  /**
    * @brief Applies a gaussian filter of the ConvergenceMap
    * @param sigma sigma of the gaussian kernel
    *
    * This method applies a gaussian filter to the Map of width sigma
    *
    */
  void applyGaussianFilter(float sigma);

  /**
    * @brief returns the number of galaxies in the map if known
    * @return the number of galaxies in the map if known, 0 otherwise
    */
  unsigned long getNumberOfGalaxies() const;

  /**
    * @brief returns the boundaries of the map
    * @return the boundaries of the map
    */
  Boundaries getBoundaries() const;

  /**
    * @brief Add borders to the map filled with zeros
    */
  void addBorders();

  /**
    * @brief Removes borders to the map
    */
  void removeBorders();


protected:

  unsigned int m_sizeXaxis;
  unsigned int m_sizeYaxis;
  unsigned int m_sizeZaxis;
  boost::multi_array<double, 3> *m_mapValues;

  unsigned long m_numberOfGalaxies;

  Boundaries m_boundaries;

}; /* End of GlobalMap class */


} /* namespace TWOD_MASS_WL_MassMapping */


#endif
