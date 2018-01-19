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
 * @file src/lib/Image.cpp
 * @date 07/07/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/Image.h"
#include "math.h"

namespace TWOD_MASS_WL_MassMapping {


Image::~Image()
{
  // Delete the m_values array if it exists
  if (m_values!=nullptr)
  {
    delete [] m_values;
    m_values = nullptr;
  }
}


Image::Image(unsigned int sizeXaxis, unsigned int sizeYaxis, double *values)
: m_sizeXaxis(sizeXaxis), m_sizeYaxis(sizeYaxis)
{
  // Allocate memory to the m_values array
  m_values = new double[m_sizeXaxis*m_sizeYaxis];

  // If no values are provided initialize to zeros
  if (values==nullptr)
  {
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        m_values[j*m_sizeXaxis + i] = 0;
      }
    }
  }
  // Else initialize to the input values
  else
  {
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        m_values[j*m_sizeXaxis + i] = values[j*m_sizeXaxis + i];
      }
    }
  }
}

Image::Image(const Image& copy): m_sizeXaxis(copy.m_sizeXaxis), m_sizeYaxis(copy.m_sizeYaxis)
{
  // Allocate memory to the m_values array
  m_values = new double[m_sizeXaxis*m_sizeYaxis];

  // Initialize to the input values
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      m_values[j*m_sizeXaxis + i] = copy.m_values[j*m_sizeXaxis + i];
    }
  }
}

Image& Image::operator= (const Image& copy)
{
  // If objects are different, set members to same values (but different pointer memory)
  if (this!=&copy)
  {
    m_sizeXaxis = copy.m_sizeXaxis;
    m_sizeYaxis = copy.m_sizeYaxis;
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        m_values[j*m_sizeXaxis + i] = copy.m_values[j*m_sizeXaxis + i];
      }
    }
  }
  return *this;
}


unsigned int Image::getXdim() const
{
  return m_sizeXaxis;
}

unsigned int Image::getYdim() const
{
  return m_sizeYaxis;
}

double Image::getValue(int x, int y) const
{
  // Handle out of borders cases by returning always the border values
  if (x<0)
  {
    x=0;
  }
  else if (x>=int(m_sizeXaxis))
  {
    x=m_sizeXaxis-1;
  }
  if (y<0)
  {
    y=0;
  }
  else if (y>=int(m_sizeYaxis))
  {
    y=m_sizeYaxis-1;
  }
  return m_values[y*m_sizeXaxis + x];
}

double Image::getMax() const
{
  double max = m_values[0];

  // Loop over all values to find the max
  for (unsigned int i=1; i<m_sizeXaxis*m_sizeYaxis; i++)
  {
    if (m_values[i]>max)
    {
      max = m_values[i];
    }
  }

  return max;
}

double Image::getStandardDeviation() const
{
  double mean(0);
  double squareMean(0);
  unsigned int counts(0);

  // Loop over all values to find the max
  for (unsigned int i=1; i<m_sizeXaxis*m_sizeYaxis; i++)
  {
    mean += m_values[i];
    squareMean += m_values[i]*m_values[i];
    counts++;
  }

  double stdev = sqrt(squareMean/counts - mean*mean/counts/counts);

  return stdev;
}

void Image::setValue(unsigned int x, unsigned int y, double value)
{
  m_values[y*m_sizeXaxis + x] = value;
}

void Image::applyThreshold(double threshold)
{
  // Loop over all values to apply the threshold
  for (unsigned int i=1; i<m_sizeXaxis*m_sizeYaxis; i++)
  {
    if (fabs(m_values[i])<threshold)
    {
      m_values[i]=0;
    }
  }
}

bool Image::isLocalMax(unsigned int x, unsigned int y) const
{
  double localVal = this->getValue(x, y);

  // Loop over all neighboors
  for (int i=-1; i<2; i++)
  {
    for (int j=-1; j<2; j++)
    {
      // Do not check the pixel itself
      if (i!=0 || j!=0)
      {
        // If a neighboor is greater or equal then this is not a local max
        if (this->getValue(x+i, y+j)>=localVal)
        {
          return false;
        }
      }
    }
  }
  return true;
}

Image Image::add(Image image)
{
  // Check if the sizes are the same
  if (m_sizeXaxis==image.getXdim() && m_sizeYaxis==image.getYdim())
  {
    // Create an image which is the sum of this and the input image
    Image output(m_sizeXaxis, m_sizeYaxis);
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        output.setValue(i, j, this->getValue(i, j) + image.getValue(i, j));
      }
    }
    return output;
  }
  else
  {
    return Image(0, 0, nullptr);
  }
}

Image Image::substract(Image image)
{
  // Check if the sizes are the same
  if (m_sizeXaxis==image.getXdim() && m_sizeYaxis==image.getYdim())
  {
    // Create an image which is the substraction of this and the input image
    Image output(m_sizeXaxis, m_sizeYaxis);
    for (unsigned int i=0; i<m_sizeXaxis; i++)
    {
      for (unsigned int j=0; j<m_sizeYaxis; j++)
      {
        output.setValue(i, j, this->getValue(i, j) - image.getValue(i, j));
      }
    }
    return output;
  }
  else
  {
    return Image(0, 0, nullptr);
  }
}

Image Image::multiply(double factor)
{
  // Create an image which is the multiplication of this and the input factor
  Image output(m_sizeXaxis, m_sizeYaxis);
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      output.setValue(i, j, this->getValue(i, j)*factor);
    }
  }
  return output;
}

double* Image::getArray() const
{
  return m_values;
}

} // TWOD_MASS_WL_MassMapping namespace
