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
 * @file TWOD_MASS_WL_MassMapping/DataFilesLoader.h
 * @date 01/17/18
 * @author user
 */

#ifndef _TWOD_MASS_WL_MASSMAPPING_DATAFILESLOADER_H
#define _TWOD_MASS_WL_MASSMAPPING_DATAFILESLOADER_H

#include <string>

namespace TWOD_MASS_WL_MassMapping {

/**
 * @class DataFilesLoader
 * @brief
 *
 */
class DataFilesLoader {

public:

  /**
   * @brief Destructor
   */
  virtual ~DataFilesLoader() = default;

  /**
   * Download data from the remote repository
   * @return local repository path
   */
  std::string downloadTestFiles() const;


private:

}; /* End of DataFilesLoader class */

} /* namespace TWOD_MASS_WL_MassMapping */


#endif
