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
 * @file src/lib/PFLauncherParser.cpp
 * @date 08/08/17
 * @author user
 */

#include "TWOD_MASS_WL_Launcher/PFLauncherParser.h"
#include "TWOD_MASS_WL_Launcher/XMLParser.h"

namespace po = boost::program_options;

namespace TWOD_MASS_WL_Launcher {

PFLauncherParser::PFLauncherParser(): m_inputXMLfile(""), m_outputXMLfile(""), m_workDir("")
{
}

po::options_description PFLauncherParser::defineSpecificProgramOptions()
{
  po::options_description options {};
  //
  // !!! Implement the program options here !!!
  //
  options.add_options()

      ("m_workDir", po::value<std::string>(), "only for pipeline")
      ("logdir", po::value<std::string>(), "only for pipeline")

      ("inputXMLfile", po::value<std::string>(), "input XML file")
      ("outputXMLfile", po::value<std::string>(), "output XML file");

  return options;
}

Elements::ExitCode PFLauncherParser::mainMethod(std::map<std::string, po::variable_value>& args)
{
  // Perform the parsing of input parameters
  if (parseInputParameters(args) == false)
  {
    return Elements::ExitCode::OK;
  }

  XMLParser myXMLParser(m_inputXMLfile, m_outputXMLfile);

  if (myXMLParser.parseInputFile()==false)
  {
    return Elements::ExitCode::OK;
  }

  return Elements::ExitCode::OK;
}

bool PFLauncherParser::parseInputParameters(std::map<std::string, po::variable_value>& args)
{
  // Reinitialize the counter to zero in case of multiple call of this method
  std::map<std::string, po::variable_value>::iterator it;
  for (it = args.begin(); it!= args.end(); ++it)
  {
    if (it->first=="m_workDir")
    {
      m_workDir = args["m_workDir"].as<std::string>() + std::string("/");
    }
    else if (it->first=="inputXMLfile")
    {
      m_inputXMLfile = args["inputXMLfile"].as<std::string>();
      //        std::cout<<"value of inputXMLfile: "<<m_inputXMLfile<<std::endl;
    }
    else if (it->first=="outputXMLfile")
    {
      m_outputXMLfile = args["outputXMLfile"].as<std::string>();
      //        std::cout<<"value of outputXMLfile: "<<m_outputXMLfile<<std::endl;
    }
  }

  // Check all values are well filled
  if (m_inputXMLfile.empty()==true || m_outputXMLfile.empty()==true)
  {
    std::cout<<"Input and/or output XML filenames not provided"<<std::endl;
    return false;
  }

  return true;
}

} // TWOD_MASS_WL_Launcher namespace
