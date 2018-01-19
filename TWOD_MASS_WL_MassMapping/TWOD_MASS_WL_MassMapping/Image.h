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
 * @file TWOD_MASS_WL_MassMapping/Image.h
 * @date 07/07/17
 * @author user
 */

#ifndef _TWOD_MASS_WL_MASSMAPPING_IMAGE_H
#define _TWOD_MASS_WL_MASSMAPPING_IMAGE_H

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class Image
 * @brief class representing an image, containing X and Y dimension and pixel values.
 * Allows to perform basic operation on image and to get and set the pixel values.
 *
 */
class Image {

public:

  /**
   * @brief Destructor
   */
  virtual ~Image();

  /**
   * @brief Constructor of an Image object
   * @param[in] sizeXaxis the number of pixels in the X axis
   * @param[in] sizeYaxis the number of pixels in the Y axis
   * @param[in] values (optional) the input values to fill in the image (has to be a table
   * of double of expected size sizeXaxis*sizeYaxis). If not provided image filled with zeros.
   */
  Image(unsigned int sizeXaxis, unsigned int sizeYaxis, double *values=nullptr);

  /**
   * @brief Copy constructor of an Image object
   * @param[in] copy the image to copy
   */
  Image(const Image& copy);

  /**
   * @brief Operator = method
   * @param[in] copy the Image to assign
   * @return a copy of the input image
   */
  Image& operator= (const Image& copy);

  /**
    * @brief get the number of pixels on X axis
    * @return the number of pixels on X axis
    */
  unsigned int getXdim() const;

  /**
    * @brief get the number of pixels on X axis
    * @return the number of pixels on X axis
    */
  unsigned int getYdim() const;

  /**
   * @brief get the image value at position (x, y)
   * @param[in] x the pixel position on X axis
   * @param[in] y the pixel position on Y axis
   * @return the value of the pixel at position (x, y)
   */
  double getValue(int x, int y) const;

  /**
   * @brief get the max image value
   * @return the max image value
   */
  double getMax() const;

  /**
   * @brief get the standard deviation in the image
   * @return the standard deviation in the image
   */
  double getStandardDeviation() const;

  /**
   * @brief set the image value at position (x, y) to value
   * @param[in] x the pixel position on X axis
   * @param[in] y the pixel position on Y axis
   * @param[in] value the value to assign to the pixel at position (x, y)
   */
  void setValue(unsigned int x, unsigned int y, double value);

  /**
   * @brief sets all values below a threshold to zero
   * @param[in] threshold the threshold below which all values will be zeros
   */
  void applyThreshold(double threshold);

  /**
   * @brief returns true if the pixel (x , y) is a local max
   * @param[in] x the position of the pixel on X axis
   * @param[in] y the position of the pixel on Y axis
   * @return true if the pixel (x , y) is a local max, false otherwise
   */
  bool isLocalMax(unsigned int x, unsigned int y) const;

  /**
   * @brief returns an image which the addition of this and the input image
   * @param[in] image the image to add to this
   * @return an image which the addition of this and the input image
   */
  Image add(Image image);

  /**
   * @brief returns an image which the substraction of this and the input image
   * @param[in] image the image to substract to this
   * @return an image which the substraction of this and the input image
   */
  Image substract(Image image);

  /**
   * @brief returns an image which the multiplicator of this and the input factor
   * @param[in] factor the factor to multiply to the image
   * @return an image which the multiplication of this and the input factor
   */
  Image multiply(double factor);

  /**
   * @brief returns the double* array containing values of the pixels
   * @return a double* array of dimension sizeXaxis*sizeYaxis
   */
  double* getArray() const;

private:

  double *m_values;
  unsigned int m_sizeXaxis;
  unsigned int m_sizeYaxis;

}; /* End of Image class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
