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
 * @file src/lib/XMLParser.cpp
 * @date 06/27/17
 * @author user
 */

#include "TWOD_MASS_WL_Launcher/XMLParser.h"
#include <xercesc/util/PlatformUtils.hpp>
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

#include <iostream>

using namespace xercesc;

namespace TWOD_MASS_WL_Launcher {

XMLParser::XMLParser(std::string inputXMLfile, std::string outputXMLfile):
    m_inputXMLfile(inputXMLfile), m_outputXMLfile(outputXMLfile)
{
  // Initialize the xml toolkit
  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (XMLException& exception)
  {
    // throw exception here to return ERROR_XERCES_INIT
    std::string message = XMLString::transcode(exception.getMessage());
    std::cout<<"XML toolkit initialization error: "<<message<<std::endl;
  }

  // Allocate the pointer
  m_fileParser = new XercesDOMParser;
}

XMLParser::XMLParser(XMLParser const& copyParser):
    m_inputXMLfile(copyParser.m_inputXMLfile), m_outputXMLfile(copyParser.m_outputXMLfile)
{
  // Allocate the pointer
  m_fileParser = new XercesDOMParser;
}

XMLParser::~XMLParser()
{
  // Delete the pointer
  delete m_fileParser;
  m_fileParser = nullptr;

  // Terminate the xerces toolkit
  try
  {
    XMLPlatformUtils::Terminate();
  }
  catch (XMLException& exception)
  {
    std::string message = XMLString::transcode(exception.getMessage());
    std::cout<<"XML toolkit teardown error: "<<message<<std::endl;
  }
}


bool XMLParser::parseInputFile()
{

  // Configure DOM parser
  m_fileParser->setValidationScheme(XercesDOMParser::Val_Never);
  m_fileParser->setDoNamespaces(false);
  m_fileParser->setDoSchema(false);
  m_fileParser->setLoadExternalDTD(false);

  try
  {
    // Give the input XML to parse
    m_fileParser->parse(m_inputXMLfile.c_str());

    // no need to free this pointer - owned by the parent parser object
    DOMDocument* xmlDoc = m_fileParser->getDocument();
    if (xmlDoc==nullptr)
    {
      std::cout<<"not valid input XML"<<std::endl;
      return false;
    }

    // Get the top-level element: Name is "root". No attributes for "root"
    DOMElement* elementRoot = xmlDoc->getDocumentElement();
    if (elementRoot==nullptr)
    {
      std::cout<<"no valid root element in the XML"<<std::endl;
      return false;
    }

    // Parse XML file for tags of interest: "ApplicationSettings"
    // Look one level nested within "root". (child of root)
    DOMNodeList* children = elementRoot->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    // For all nodes, children of "root" in the XML tree.
    for (XMLSize_t iNode = 0; iNode < nodeCount; ++iNode)
    {
      DOMNode* currentNode = children->item(iNode);
      if (currentNode->getNodeType() &&  // true is not NULL
          currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
      {
        // Found node which is an Element. Re-cast node as element
        DOMElement* currentElement = dynamic_cast<xercesc::DOMElement*>(currentNode);
        if (XMLString::equals(currentElement->getTagName(), XMLString::transcode("ApplicationSettings")))
        {
          // Already tested node as type element and of name "ApplicationSettings".
          // Read attributes of element "ApplicationSettings".
          const XMLCh* xmlch_OptionA = currentElement->getAttribute(XMLString::transcode("option_a"));
          //m_OptionA = XMLString::transcode(xmlch_OptionA);
          std::cout<<"option A:"<<XMLString::transcode(xmlch_OptionA)<<std::endl;

          const XMLCh* xmlch_OptionB = currentElement->getAttribute(XMLString::transcode("option_b"));
          //m_OptionB = XMLString::transcode(xmlch_OptionB);
          std::cout<<"option B:"<<XMLString::transcode(xmlch_OptionB)<<std::endl;
        }
        else if (XMLString::equals(currentElement->getTagName(), XMLString::transcode("OtherStuff")))
        {
          // Already tested node as type element and of name "OtherStuff".
          // Read attributes of element "OtherStuff".
          const XMLCh* xmlch_OptionX = currentElement->getAttribute(XMLString::transcode("option_x"));
          //m_OptionA = XMLString::transcode(xmlch_OptionA);
          std::cout<<"option X:"<<XMLString::transcode(xmlch_OptionX)<<std::endl;
        }
      }
    }
  }
  catch (xercesc::XMLException& exception)
  {
    std::string message = xercesc::XMLString::transcode(exception.getMessage());
    std::cout<<"Error parsing file: "<<message<<std::endl;
    return false;
  }

  return true;
}

} // TWOD_MASS_WL_Launcher namespace
