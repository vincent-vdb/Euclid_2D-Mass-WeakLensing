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
 * @file src/lib/GlobalMap.cpp
 * @date 04/27/17
 * @author user
 */

#include "TWOD_MASS_WL_MassMapping/GlobalMap.h"
#include <CCfits/CCfits>

namespace TWOD_MASS_WL_MassMapping {

GlobalMap::~GlobalMap()
{
  delete m_mapValues;
  m_mapValues = nullptr;
}

GlobalMap::GlobalMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                     unsigned long nGalaxies):
m_sizeXaxis(sizeXaxis), m_sizeYaxis(sizeYaxis), m_sizeZaxis(sizeZaxis),
m_numberOfGalaxies(nGalaxies), m_boundaries(0, 0, 0, 0, 0, 0)
{
  typedef boost::multi_array<double, 3>::index index;

  // Declare the map of data
  m_mapValues = new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Assign values from the input array to the map
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*m_mapValues)[i][j][k] = array[m_sizeXaxis*m_sizeYaxis*k+m_sizeXaxis*j+i];
      }
    }
  }
}

GlobalMap::GlobalMap(double* array, unsigned int sizeXaxis, unsigned int sizeYaxis, unsigned int sizeZaxis,
                     Boundaries &boundaries, unsigned long nGalaxies):
m_sizeXaxis(sizeXaxis), m_sizeYaxis(sizeYaxis), m_sizeZaxis(sizeZaxis),
m_numberOfGalaxies(nGalaxies), m_boundaries(boundaries)
{
  typedef boost::multi_array<double, 3>::index index;

  // Declare the map of data
  m_mapValues = new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Assign values from the input array to the map
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*m_mapValues)[i][j][k] = array[m_sizeXaxis*m_sizeYaxis*k+m_sizeXaxis*j+i];
      }
    }
  }
}


GlobalMap::GlobalMap(std::string filename): m_numberOfGalaxies(0), m_boundaries(0, 0, 0, 0, 0, 0)
{
  try
  {
    // Create a FITS pointer
    std::shared_ptr<CCfits::FITS> pInputFile(new CCfits::FITS(filename, CCfits::Read, true));

    // Get the image parameters
    CCfits::PHDU& image = pInputFile->pHDU();

    // Create an array in the right format the get the data
    std::valarray<double> contents;

    // Read the image content
    image.readAllKeys();
    image.read(contents);

    // Define the axis dimensions
    m_sizeXaxis = image.axis(0);
    m_sizeYaxis = image.axis(1);
    m_sizeZaxis = image.axis(2);

    // Write the values into the object map
    typedef boost::multi_array<double, 3> array_type;
    typedef array_type::index index;

    m_mapValues = new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

    // Assign values to the elements
    for (index k = 0; k != m_sizeZaxis; ++k)
    {
      for (index j = 0; j != m_sizeYaxis; ++j)
      {
        for (index i = 0; i != m_sizeXaxis; ++i)
        {
          (*m_mapValues)[i][j][k] = contents[m_sizeXaxis*m_sizeYaxis*k+m_sizeXaxis*j+i];
        }
      }
    }

    // Try to get the number of galaxies in the image if any
    try
    {
      std::string nGalString;
      image.readKey("nGalaxies", nGalString);
      // if found then fill it
      m_numberOfGalaxies = std::stol(nGalString);

      std::string raMin;
      std::string raMax;
      std::string decMin;
      std::string decMax;
      std::string zMin;
      std::string zMax;
      image.readKey("raMin", raMin);
      image.readKey("raMax", raMax);
      image.readKey("decMin", decMin);
      image.readKey("decMax", decMax);
      image.readKey("zMin", zMin);
      image.readKey("zMax", zMax);

      m_boundaries = Boundaries(std::stod(raMin), std::stod(raMax),
                                std::stod(decMin), std::stod(decMax),
                                std::stod(zMin), std::stod(zMax));
    }
    catch (CCfits::FitsException)
    {
      // if not found then put zero
      m_numberOfGalaxies = 0;
    }
  }
  catch (CCfits::FitsException)
  {
    // no FITS image at this path
    std::cout<<filename<<": can not open a FITS image from this file"<<std::endl;
  }
}

GlobalMap::GlobalMap(GlobalMap const& copyMap):
m_sizeXaxis(copyMap.m_sizeXaxis), m_sizeYaxis(copyMap.m_sizeYaxis), m_sizeZaxis(copyMap.m_sizeZaxis),
m_numberOfGalaxies(copyMap.m_numberOfGalaxies), m_boundaries(copyMap.m_boundaries)
{
  typedef boost::multi_array<double, 3>::index index;

  // Declare the map of data
  m_mapValues = new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Assign values from the input array to the map
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*m_mapValues)[i][j][k] = (*copyMap.m_mapValues)[i][j][k];
      }
    }
  }
}

double GlobalMap::getBinValue(unsigned int binx, unsigned int biny, unsigned int binz) const
{
  // Make sure the bin values are not wrong
  if (binx >= m_sizeXaxis)
  {
    binx = m_sizeXaxis-1;
  }
  if (biny >= m_sizeYaxis)
  {
    biny = m_sizeYaxis-1;
  }
  if (binz >= m_sizeZaxis)
  {
    binz = m_sizeZaxis-1;
  }
  // Return the value of the map for the (binx, biny, binz)
  return (*m_mapValues)[binx][biny][binz];
}

unsigned int GlobalMap::getXdim() const
{
  return m_sizeXaxis;
}

unsigned int GlobalMap::getYdim() const
{
  return m_sizeYaxis;
}

unsigned int GlobalMap::getZdim() const
{
  return m_sizeZaxis;
}

bool GlobalMap::saveToFITSfile(std::string filename, bool overwrite)
{
  std::map<std::string, po::variable_value> param;

  return saveToFITSfile(filename, overwrite, param);
/*


  if (overwrite)
  {
    filename = "!"+filename;
  }

  // Create a FITS pointer and fill it if a file can be created
  std::shared_ptr<CCfits::FITS> pFits(nullptr);
  try
  {
    long naxes[3] = {m_sizeXaxis, m_sizeYaxis, m_sizeZaxis};

    // Create a FITS file with name filename, of floats, 3 axis of sizes naxes
    pFits.reset(new CCfits::FITS(filename, FLOAT_IMG, 3, naxes));
  }
  catch (CCfits::FITS::CantCreate)
  {
    return false;
  }

  // Create an array to write in the file with the right format
  std::valarray<double> array(m_sizeXaxis*m_sizeYaxis*m_sizeZaxis);

  // Assign values to the elements
  for (unsigned int k = 0; k != m_sizeZaxis; ++k)
  {
    for (unsigned int j = 0; j != m_sizeYaxis; ++j)
    {
      for (unsigned int i = 0; i != m_sizeXaxis; ++i)
      {
        array[m_sizeXaxis*m_sizeYaxis*k+m_sizeXaxis*j+i] = (*m_mapValues)[i][j][k];
      }
    }
  }

  // Write the total number of galaxies into the FITS file
  pFits->pHDU().addKey("nGalaxies", std::to_string(m_numberOfGalaxies), "total number of galaxies into the map");

  // Write the boundaries into the FITS file
  pFits->pHDU().addKey("raMin", std::to_string(m_boundaries.getRaMin()), "right ascension min of the map");
  pFits->pHDU().addKey("raMax", std::to_string(m_boundaries.getRaMax()), "right ascension max of the map");
  pFits->pHDU().addKey("decMin", std::to_string(m_boundaries.getDecMin()), "declination min of the map");
  pFits->pHDU().addKey("decMax", std::to_string(m_boundaries.getDecMax()), "declination max of the map");
  pFits->pHDU().addKey("zMin", std::to_string(m_boundaries.getZMin()), "redshift min of the map");
  pFits->pHDU().addKey("zMax", std::to_string(m_boundaries.getZMax()), "redshift max of the map");

  // Write the array image to the FITS file
  pFits->pHDU().write(1, m_sizeXaxis*m_sizeYaxis*m_sizeZaxis, array);

////////////////////////////////////////////////////////////////////////////

  struct wcsprm *myWCS;//new wcsprm;
  int iniOK = wcsini(true, 2, myWCS);

  std::cout<<"value of the ini: "<<iniOK<<std::endl;
//  myWCS.naxis = 2;
  int numberOfHeaders;
  char* header;
  int headerok = wcshdo(1, myWCS, &numberOfHeaders, &header);

  std::cout<<"wcshdo output: "<<headerok<<std::endl;
  std::cout<<"number of header: "<<numberOfHeaders<<std::endl;
  std::cout<<"header 0: "<<header<<std::endl;

////////////////////////////////////////////////////////////////////////////

  return true;
*/
}


bool GlobalMap::saveToFITSfile(std::string filename, bool overwrite, std::map<std::string, po::variable_value> param)
{
  if (overwrite)
  {
    filename = "!"+filename;
  }

  // Create a FITS pointer and fill it if a file can be created
  std::shared_ptr<CCfits::FITS> pFits(nullptr);
  try
  {
    long naxes[3] = {m_sizeXaxis, m_sizeYaxis, m_sizeZaxis};

    // Create a FITS file with name filename, of floats, 3 axis of sizes naxes
    pFits.reset(new CCfits::FITS(filename, FLOAT_IMG, 3, naxes));
  }
  catch (CCfits::FITS::CantCreate)
  {
    return false;
  }

  // Create an array to write in the file with the right format
  std::valarray<double> array(m_sizeXaxis*m_sizeYaxis*m_sizeZaxis);

  // Assign values to the elements
  for (unsigned int k = 0; k != m_sizeZaxis; ++k)
  {
    for (unsigned int j = 0; j != m_sizeYaxis; ++j)
    {
      for (unsigned int i = 0; i != m_sizeXaxis; ++i)
      {
        array[m_sizeXaxis*m_sizeYaxis*k+m_sizeXaxis*j+i] = (*m_mapValues)[i][j][k];
      }
    }
  }

  // Write the total number of galaxies into the FITS file
  pFits->pHDU().addKey("nGalaxies", std::to_string(m_numberOfGalaxies), "total number of galaxies into the map");

  // Write the boundaries into the FITS file
  pFits->pHDU().addKey("raMin", std::to_string(m_boundaries.getRaMin()), "right ascension min of the map");
  pFits->pHDU().addKey("raMax", std::to_string(m_boundaries.getRaMax()), "right ascension max of the map");
  pFits->pHDU().addKey("decMin", std::to_string(m_boundaries.getDecMin()), "declination min of the map");
  pFits->pHDU().addKey("decMax", std::to_string(m_boundaries.getDecMax()), "declination max of the map");
  pFits->pHDU().addKey("zMin", std::to_string(m_boundaries.getZMin()), "redshift min of the map");
  pFits->pHDU().addKey("zMax", std::to_string(m_boundaries.getZMax()), "redshift max of the map");


  // Fill all the required fields of the fits header
  if (param.find("bModeZeros")!=param.end())
  {
    pFits->pHDU().addKey("BMODGAP", param["bModeZeros"].as<int>()==1?"True":"False",
                         "True if B-mode forced to zero in the gaps");
  }
  else
  {
    pFits->pHDU().addKey("BMODGAP", "False", "True if B-mode forced to zero in the gaps");
  }

  if (param.find("sigmaBounded")!=param.end())
  {
    pFits->pHDU().addKey("VARPERSC", param["sigmaBounded"].as<int>()==1?"True":"False",
                         "True if equal variance per scale forced in and out of masked area");
  }
  else
  {
    pFits->pHDU().addKey("VARPERSC", "False", "True if equal variance per scale forced in and out of masked area");
  }

  if (param.find("numberScales")!=param.end())
  {
    pFits->pHDU().addKey("NSCINP", std::to_string(param["numberScales"].as<int>()),
                         "Number of scales for inpainting");
  }
  else
  {
    pFits->pHDU().addKey("NSCINP", "Auto", "Number of scales for inpainting");
  }

  if (param.find("numberIteration")!=param.end())
  {
    pFits->pHDU().addKey("NITINP", std::to_string(param["numberIteration"].as<int>()),
                         "Number of iterations for inpainting");
  }
  else
  {
    pFits->pHDU().addKey("NITINP", "0", "Number of iterations for inpainting");
  }

  if (param.find("sigmaShearMap")!=param.end())
  {
    pFits->pHDU().addKey("STDGAUS", std::to_string((param["sigmaShearMap"].as<std::vector<float> >())[0]),
                         "Standard deviation of gaussian smoothing");
  }
  else
  {
    pFits->pHDU().addKey("STDGAUS", "0", "Standard deviation of gaussian smoothing");
  }

  if (param.find("ReducedShearIteration")!=param.end())
  {
    pFits->pHDU().addKey("NITREDSH", std::to_string(param["ReducedShearIteration"].as<int>()),
                         "Number of iterations for reduced shear");
  }
  else
  {
    pFits->pHDU().addKey("NITREDSH", "0", "Number of iterations for reduced shear");
  }

  //  pFits->pHDU().addKey("DENTYPE", "RA-TAN", "Denoising type");
  //  pFits->pHDU().addKey("FDRVAL", "RA-TAN", "False discovery rate threshold");


  pFits->pHDU().addKey("CTYPE1", "RA-TAN", "First parameter for reference");
  pFits->pHDU().addKey("CTYPE2", "DEC-TAN", "Second parameter for reference");

  pFits->pHDU().addKey("CPIX1", std::to_string(floor(m_sizeXaxis/2)),
                       "Pixel number of reference pixel in first dimension");
  pFits->pHDU().addKey("CPIX2", std::to_string(floor(m_sizeYaxis/2)),
                       "Pixel number of reference pixel in second dimension");

  float cd1 = (m_boundaries.getRaMax()-m_boundaries.getRaMin())/m_sizeXaxis;
  float cd2 = (m_boundaries.getDecMax()-m_boundaries.getDecMin())/m_sizeYaxis;
  float crval1 = (floor(m_sizeXaxis/2)+0.5)*cd1 + m_boundaries.getRaMin();
  float crval2 = (floor(m_sizeYaxis/2)+0.5)*cd2 + m_boundaries.getDecMin();

  pFits->pHDU().addKey("CRVAL1", std::to_string(crval1), "First reference pixel value for CTYPE1");
  pFits->pHDU().addKey("CRVAL2", std::to_string(crval2), "Second reference pixel value for CTYPE2");

  pFits->pHDU().addKey("CD1_1", std::to_string(cd1), "Component (1,1) of the coordinate transformation matrix");
  pFits->pHDU().addKey("CD1_2", std::to_string(0), "Component (1,2) of the coordinate transformation matrix");
  pFits->pHDU().addKey("CD2_1", std::to_string(0), "Component (2,1) of the coordinate transformation matrix");
  pFits->pHDU().addKey("CD2_2", std::to_string(cd2), "Component (2,2) of the coordinate transformation matrix");

  pFits->pHDU().addKey("CUNIT1", "deg", "Unit of the first coordinate value");
  pFits->pHDU().addKey("CUNIT2", "deg", "Unit of the second coordinate value");
  pFits->pHDU().addKey("CREATOR", "2D-MASS-WL", "Software used to create the product");
  pFits->pHDU().addKey("VERSION", "0.1", "Software version");

  // Write the array image to the FITS file
  pFits->pHDU().write(1, m_sizeXaxis*m_sizeYaxis*m_sizeZaxis, array);

  return true;
}
/*
fftw_complex* GlobalMap::performFFTforward(fftw_complex* inputMap)
{
  // Declare the output map
  fftw_complex* fft_outputMap = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  // Create the plan
  fftw_plan plan_forward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, inputMap,
                                            fft_outputMap, FFTW_FORWARD,  FFTW_MEASURE);
  // Perform the transform
  fftw_execute(plan_forward);
  return fft_outputMap;
}

fftw_complex* GlobalMap::performFFTbackward(fftw_complex* inputFFTMap)
{
  // Declare the output map
  fftw_complex* outputMap = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  // Create the plan
  fftw_plan plan_backward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis, inputFFTMap,
                                             outputMap, FFTW_BACKWARD,  FFTW_MEASURE);
  // Perform the transform
  fftw_execute(plan_backward);
  return outputMap;
}
*/
boost::multi_array<double, 2> GlobalMap::makeGaussianKernel(int sizeX, int sizeY, float sigmaX, float sigmaY)
{
  // Create the multi_array output gaussian kernel
  boost::multi_array<double, 2> kernel(boost::extents[sizeX][sizeY]);
  typedef boost::multi_array<double, 2>::index index;

  // Fill in the gaussian kernel
  double sum(0.);
  for (index j = 0; j != sizeY; ++j)
  {
    float y(j-0.5*(sizeY-1));
    for (index i = 0; i != sizeX; ++i)
    {
      float x(i-0.5*(sizeX-1));
      kernel[i][j]=exp(-(x*x/(2*sigmaX*sigmaX) + y*y/(2*sigmaY*sigmaY)));///(2.*pi*sigma*sigma);
      sum+=kernel[i][j];
    }
  }

  double normSum(0.);
  // Normalize the kernel
  for (index j = 0; j != sizeY; ++j)
  {
    for (index i = 0; i != sizeX; ++i)
    {
      kernel[i][j] /= sum;
      normSum +=kernel[i][j];
//        std::cout<<kernel[i][j]<<" ";
    }
//      std::cout<<std::endl;
  }
//    std::cout<<"normalized sum: "<<normSum<<std::endl;

  return kernel;
}

std::vector<double> GlobalMap::getMeanValues()
{
  // Create the vector of mean values
  std::vector<double> meanValues;

  // Loop over all the values of the map
  typedef boost::multi_array<double, 3>::index index;

  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    double mean(0.);
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        mean += (*m_mapValues)[i][j][k];
      }
    }
    // Scale the sum to the mean
    mean /= m_sizeXaxis*m_sizeYaxis;
    // Add that value to the output vector
    meanValues.push_back(mean);
  }

  return meanValues;
}

void GlobalMap::removeOffset(std::vector<double> offset)
{
  typedef boost::multi_array<double, 3>::index index;

  // For each value in the map, add the offset value
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*m_mapValues)[i][j][k] -= offset[k];
      }
    }
  }
}

/*
void GlobalMap::readTableFITS(std::string filename)
{
  try
  {
    // Create a FITS pointer
    std::shared_ptr<CCfits::FITS> pInputFile(new CCfits::FITS(filename, CCfits::Read));

    // Reading the HDU
    CCfits::ExtHDU& table = pInputFile->extension(1);//"x38i0101t.c0h.tab");

    // Reading the data in the table
    std::valarray<double> dataMin;
    table.column("DATAMIN").read(dataMin, 1, table.column("DATAMIN").rows());

    std::cout<<"value of datamin: "<<dataMin[0]<<std::endl;
  }
  catch (CCfits::FitsException&)
  {
    std::cout<<"exception thrown when opening/reading the file"<<std::endl;
  }
}
*/

unsigned int GlobalMap::pixelate(float galaxiesPerBin)
{
  // Check if the current number of galaxies in the map is known
  // if not, not possible to perform the rebinning -> return 1
  if (m_numberOfGalaxies==0)
  {
    return 1;
  }

  // Compute the current number of galaxies per bin if known
  float curGalPerBin = float(m_numberOfGalaxies)/(m_sizeXaxis*m_sizeYaxis);
  // Check if the current number of galaxies per bin is lower than the one needed
  // if not, return 1
  if (curGalPerBin>galaxiesPerBin)
  {
    return 1;
  }

  // Compute here the pixelation....

  // Check what binning would make it closer to the expected value
  bool goodBinning(false);
  int binning(1);
  while (goodBinning==false)
  {
    if (fabs(curGalPerBin*binning*4-galaxiesPerBin)>=fabs(galaxiesPerBin-curGalPerBin))
    {
      goodBinning=true;
    }
    else
    {
      binning *= 4;
    }
  }

  // If the best choice if 1, then do not change anything and return 1
  if (binning == 1)
  {
    return 1;
  }

  // If the best choice means to have one or less pixels then do nothing and return 1
  if (m_sizeXaxis/int(sqrt(binning)) <= 1 || m_sizeYaxis/int(sqrt(binning)) <= 1)
  {
    return 1;
  }

  // Create a buffer array to reshape the member array
  boost::multi_array<double, 3> *buffMapValues = new boost::multi_array<double, 3>
    (boost::extents[m_sizeXaxis/int(sqrt(binning))][m_sizeYaxis/int(sqrt(binning))][m_sizeZaxis]);

  typedef boost::multi_array<double, 3>::index index;

  // Reassign binned values to the map
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*buffMapValues)[i/int(sqrt(binning))][j/int(sqrt(binning))][k] += (*m_mapValues)[i][j][k];
      }
    }
  }

  // Delete the old array
  delete m_mapValues;
  m_mapValues = buffMapValues;

  // Update the axis dimensions of the map
  m_sizeXaxis /= int(sqrt(binning));
  m_sizeYaxis /= int(sqrt(binning));

  return int(sqrt(binning));
}


bool GlobalMap::pixelate(unsigned int xBinning, unsigned int yBinning)
{
  if (xBinning==0 && yBinning==0)
  {
    return false;
  }

  // Put the binning from power of two to actual values
  xBinning = pow(2, xBinning);
  yBinning = pow(2, yBinning);

  // Check the asked binning is not higher or equal to the number of current bins
  if (xBinning>m_sizeXaxis || yBinning>m_sizeYaxis)
  {
    return false;
  }

  // Create a buffer array to reshape the member array
  boost::multi_array<double, 3> *buffMapValues = new boost::multi_array<double, 3>
    (boost::extents[m_sizeXaxis/xBinning][m_sizeYaxis/yBinning][m_sizeZaxis]);

  typedef boost::multi_array<double, 3>::index index;

  // Reassign binned values to the map
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        (*buffMapValues)[i/xBinning][j/yBinning][k] += (*m_mapValues)[i][j][k]/xBinning/yBinning;
      }
    }
  }

  // Delete the old array
  delete m_mapValues;
  m_mapValues = buffMapValues;

  m_sizeXaxis /= xBinning;
  m_sizeYaxis /= yBinning;

  return true;
}


void GlobalMap::applyGaussianFilter(float sigmax, float sigmay)
{
  double fftFactor = 1.0/m_sizeXaxis/m_sizeYaxis;

  // Generate a normalized gaussian kernel with sigma
  boost::multi_array<double, 2> gaussianKernel = makeGaussianKernel(m_sizeXaxis, m_sizeYaxis, sigmax, sigmay);

  // Create the complex maps
  fftw_complex* kappa_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);
  fftw_complex* fft_kappa_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);

  fftw_complex* kernel_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);
  fftw_complex* fft_kernel_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) *m_sizeXaxis*m_sizeYaxis);

  fftw_complex* kappaGauss_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);
  fftw_complex* fft_kappaGauss_complex = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*m_sizeXaxis*m_sizeYaxis);

  // Create the plans for transformation
  fftw_plan plan_k_forward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis,
                                              kappa_complex, fft_kappa_complex,
                                              FFTW_FORWARD,  FFTW_MEASURE);

  fftw_plan plan_kernel_forward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis,
                                                   kernel_complex, fft_kernel_complex,
                                                   FFTW_FORWARD,  FFTW_MEASURE);

  fftw_plan plan_kappaGauss_backward = fftw_plan_dft_2d(m_sizeXaxis, m_sizeYaxis,
                                                        fft_kappaGauss_complex, kappaGauss_complex,
                                                        FFTW_BACKWARD,  FFTW_MEASURE);


  // Fill the complex kappa map with convergence map values and kernel values
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      kappa_complex[j*m_sizeXaxis +i][0] = (*m_mapValues)[i][j][0];
      kappa_complex[j*m_sizeXaxis +i][1] = 0;//(*m_mapValues)[i][j][1];
      kernel_complex[j*m_sizeXaxis +i][0] = gaussianKernel[i][j];
      kernel_complex[j*m_sizeXaxis +i][1] = 0;//gaussianKernel[i][j];
    }
  }

  // Perform the fourier transform of the complex convergence map ans complex kernel
  fftw_execute(plan_k_forward);
  fftw_execute(plan_kernel_forward);


  // Multiply the gaussian kernel and the convergence map in the fourier space
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      fft_kappaGauss_complex[j*m_sizeXaxis+i][0] = fft_kappa_complex[j*m_sizeXaxis+i][0]
                                                  *fft_kernel_complex[j*m_sizeXaxis+i][0];
      fft_kappaGauss_complex[j*m_sizeXaxis+i][1] = fft_kappa_complex[j*m_sizeXaxis+i][1]
                                                  *fft_kernel_complex[j*m_sizeXaxis+i][1];
    }
  }

  // Apply backward Fourier transform to get the filtered convergence map
  fftw_execute(plan_kappaGauss_backward);

  // Fill the convergence map
  for (unsigned int i=0; i<m_sizeXaxis; i++)
  {
    for (unsigned int j=0; j<m_sizeYaxis; j++)
    {
      int ii = i<m_sizeXaxis/2 ? i+m_sizeXaxis/2 : i-m_sizeXaxis/2;
      int jj = j<m_sizeYaxis/2 ? j+m_sizeYaxis/2 : j-m_sizeYaxis/2;
      (*m_mapValues)[ii][jj][0]=kappaGauss_complex[j*m_sizeXaxis +i][0]*fftFactor;
//        (*m_mapValues)[ii][jj][1]=kappaGauss_complex[j*m_sizeXaxis +i][1]*fftFactor;
    }
  }

  // free memory
  fftw_destroy_plan(plan_k_forward);
  fftw_destroy_plan(plan_kernel_forward);
  fftw_destroy_plan(plan_kappaGauss_backward);

  fftw_free(kappa_complex);
  fftw_free(fft_kappa_complex);
  fftw_free(kernel_complex);
  fftw_free(fft_kernel_complex);
  fftw_free(kappaGauss_complex);
  fftw_free(fft_kappaGauss_complex);
}

void GlobalMap::applyGaussianFilter(float sigma)
{
  applyGaussianFilter(sigma, sigma);
}

unsigned long GlobalMap::getNumberOfGalaxies() const
{
  return m_numberOfGalaxies;
}

Boundaries GlobalMap::getBoundaries() const
{
  return m_boundaries;
}

void GlobalMap::addBorders()
{
  // Update the number of pixels on X and Y axis
  m_sizeXaxis *= 2;
  m_sizeYaxis *= 2;

  typedef boost::multi_array<double, 3>::index index;

  // Declare a new array with the right dimensions
  boost::multi_array<double, 3> *borderedMap =
      new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Assign values from the old array to the new bordered one
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
        if (i<m_sizeXaxis/4 || i>=3*m_sizeXaxis/4 || j<m_sizeYaxis/4 || j>=3*m_sizeYaxis/4)
        {
          (*borderedMap)[i][j][k] = 0.;
        }
        else
        {
          (*borderedMap)[i][j][k] = (*m_mapValues)[i-m_sizeXaxis/4][j-m_sizeYaxis/4][k];
        }
      }
    }
  }

//  m_sizeXaxis /= 2;
//  m_sizeYaxis /= 2;
  // Delete the old map and assign the new one as a member
  delete m_mapValues;
  m_mapValues = borderedMap;
}

void GlobalMap::removeBorders()
{
  m_sizeXaxis /= 2;
  m_sizeYaxis /= 2;

  typedef boost::multi_array<double, 3>::index index;

  // Declare a new array with the right dimensions
  boost::multi_array<double, 3> *borderedMap =
      new boost::multi_array<double, 3>(boost::extents[m_sizeXaxis][m_sizeYaxis][m_sizeZaxis]);

  // Assign values from the old array to the new without borders
  for (index k = 0; k != m_sizeZaxis; ++k)
  {
    for (index j = 0; j != m_sizeYaxis; ++j)
    {
      for (index i = 0; i != m_sizeXaxis; ++i)
      {
          (*borderedMap)[i][j][k] = (*m_mapValues)[i+m_sizeXaxis/2][j+m_sizeYaxis/2][k];
      }
    }
  }

//  m_sizeXaxis *= 2;
//  m_sizeYaxis *= 2;
  // Delete the old map and assign the new one as a member
  delete m_mapValues;
  m_mapValues = borderedMap;
}

} // TWOD_MASS_WL_MassMapping namespace
