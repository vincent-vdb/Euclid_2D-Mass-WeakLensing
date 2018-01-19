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
 * @file TWOD_MASS_WL_Launcher/XMLParser.h
 * @date 06/27/17
 * @author user
 */

#ifndef TWOD_MASS_WL_LAUNCHER_XMLPARSER_H
#define TWOD_MASS_WL_LAUNCHER_XMLPARSER_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>

namespace TWOD_MASS_WL_Launcher {

/**
 * @class XMLParser
 * @brief
 *
 */
class XMLParser {

public:

  /**
   * @brief Destructor
   */
  virtual ~XMLParser();

  /**
   * @brief Constructor
   * @param[in] inputXMLfile the filename of the input XML
   * @param[in] outputXMLfile the filename of the output XML
   * */
  XMLParser(std::string inputXMLfile, std::string outputXMLfile);

  /**
   * @brief Copy constructor
   * @param[in] copyParser the XMLParser to copy
   * */
  XMLParser(XMLParser const& copyParser);

  /**
   * @brief Method that parses the input XML file and calls the PFAlgo
   * @return true if parsing and PFAlgo call were OK, false otherwise
   * */
  bool parseInputFile();

private:

  std::string m_inputXMLfile;
  std::string m_outputXMLfile;
  xercesc::XercesDOMParser *m_fileParser;

}; /* End of XMLParser class */

} /* namespace TWOD_MASS_WL_Launcher */


#endif
