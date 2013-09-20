// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2013
// Department of Neural Information Processing, University of Ulm
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#include "Robot.h"
#include "RobotParameters.h"
#include "Configuration.h"
#include "Log.h"

#include <ace/Arg_Shifter.h>

namespace
{
  char const * const robotNameKey = "-MiroRobotName";
  char const * const shortRobotNameKey = "-MRN";
  char const * const namingTimeoutKey = "-MiroNamingTimeout";
  char const * const shortNamingTimeoutKey = "-MNT";
  char const * const namingContextKey = "-MiroNamingContext";
  char const * const shortNamingContextKey = "-MNC";
  char const * const dataRootKey = "-MiroDataRoot";
  char const * const shortDataRootKey = "-MDR";
  char const * const noNamingKey = "-MiroNoNaming";
  char const * const shortNoNamingKey = "-MNN";
  char const * const eventChannelKey = "-MiroEventChannel";
  char const * const shortEventChannelKey = "-MEC";
  char const * const helpKey = "-MiroHelp";
  char const * const shortHelpKey = "-?";

  char const * const robotNameSub = "$MiroRobotName";
  char const * const shortRobotNameSub = "$MRN";
  char const * const namingContextSub = "$MiroNamingContext";
  char const * const shortNamingContextSub = "$MNC";
  char const * const dataRootSub = "$MiroDataRoot";
  char const * const shortDataRootSub = "$MDR";
  char const * const noNamingSub = "$MiroNoNaming";
  char const * const shortNoNamingSub = "$MNN";
  char const * const eventChannelSub = "$MiroEventChannel";
  char const * const shortEventChannelSub = "$MEC";

  bool printHelp = false;
  char const * const miroHelp =
    "Miro Robot parameters:\n"
    "  -MRN (-MiroRobotName) <name> of the robot (default: host name)\n"
    "  -MNC (-MiroNamingContext) <name> of CORBA naming context (default: robot name)\n"
    "  -MNT (-MiroNamingTimeout) <sec> timeout for resolving NamingService (15s)\n"
    "  -MNN (-MiroNoNaming) don't use NamingService\n"
    "  -MDR (-MiroDataRoot) <path> to store robot data products (default: /data)\n"
    "  -MEC (-MiroEventChannel) <name> of EventChannel (default: NotifyEventChannel)\n";
}

namespace Miro
{
  using namespace std;

  void
  Robot::init(int& argc, char * argv[])
  {
    static bool initialized = false;
    if (initialized)
      return;
    initialized = true;

    // if namingContextName is not set explicitly, it is the same as name
    bool explicitNamingContext = false;
    RobotParameters * params = RobotParameters::instance();

    // configuration file processing
    // overwrite default settings
    Miro::Log::init(argc, argv);
    Miro::Configuration::init(argc, argv);
    Miro::ConfigDocument * config = Miro::Configuration::document();
    config->setSection("Robot");
    config->getParameters("Miro::RobotParameters", *params);
    
    // Command line parameter parsing
    // Overwrite config-file settings
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
      const ACE_TCHAR *current_arg = arg_shifter.get_current();
      
      if (ACE_OS::strcasecmp(current_arg, robotNameKey) == 0 ||
          ACE_OS::strcasecmp(current_arg, shortRobotNameKey) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          params->name = arg_shifter.get_current();
          arg_shifter.consume_arg();
          
          if (!explicitNamingContext)
            params->namingContextName = params->name;
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, namingContextKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortNamingContextKey) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          params->namingContextName = arg_shifter.get_current();
          arg_shifter.consume_arg();
          explicitNamingContext = true;
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, namingTimeoutKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortNamingTimeoutKey) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          params->namingServiceTimeout.set(atof(arg_shifter.get_current()));
          arg_shifter.consume_arg();
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, dataRootKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortDataRootKey) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          params->dataRootDir = arg_shifter.get_current();
          arg_shifter.consume_arg();
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, eventChannelKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortEventChannelKey) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          params->eventChannelName = arg_shifter.get_current();
          arg_shifter.consume_arg();
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, noNamingKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortNoNamingKey) == 0) {
        arg_shifter.consume_arg();
        params->noNaming = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, helpKey) == 0 ||
               ACE_OS::strcasecmp(current_arg, shortHelpKey) == 0) {
        arg_shifter.ignore_arg();

        printHelp = true;                 
      }
      else
        arg_shifter.ignore_arg();
    }
    
    // Warning if environment variable is not set
    if (NULL == ACE_OS::getenv("HOSTNAME"))
      MIRO_LOG_OSTR(LL_WARNING,
		    "HOSTNAME env var is empty. Using\"" << params->name << "\" as robot name.");

    MIRO_DBG_OSTR(MIRO, LL_DEBUG, "Robot name is \"" << params->name << "\"");
                                                
    if (printHelp) {
      cerr << miroHelp << endl;
    }
  }

  void
  Robot::substitute(QString& text)
  {
    RobotParameters * params = RobotParameters::instance();

    text.replace(QString(robotNameSub), params->name.c_str());
    text.replace(QString(shortRobotNameSub), params->name.c_str());
    text.replace(QString(namingContextSub), params->namingContextName.c_str());
    text.replace(QString(shortNamingContextSub), params->namingContextName.c_str());
    text.replace(QString(dataRootSub), params->dataRootDir.c_str());
    text.replace(QString(shortDataRootSub), params->dataRootDir.c_str());
    text.replace(QString(eventChannelSub), params->eventChannelName.c_str());
    text.replace(QString(shortEventChannelSub), params->eventChannelName.c_str());
  }
}
