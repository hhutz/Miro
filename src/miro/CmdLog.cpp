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
#include "CmdLog.h"
#include "LogInterceptorInit.h"
#include "Server.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Log.h"

#include <tao/ORBInitializer_Registry.h>

#include <ace/Arg_Shifter.h>

#include <vector>
#include <algorithm>

namespace
{
  const char cmdTraceOpt[] = "-MiroCmdTrace";
  const char shortCmdTraceOpt[] = "-MCT";
}

namespace Miro
{
  using namespace std;

  inline
  static
  bool operator < (IncludeParameters const& lhs, IncludeParameters const& rhs)
  {
    return (lhs.name < rhs.name);
  }


  void
  CmdLog::init(int& argc, char * argv[])
  {
    // initialize parameters from config file
    LogInterceptorParameters * params =
      LogInterceptorParameters::instance();

    Configuration::init(argc, argv);
    ConfigDocument * config = Configuration::document();
    config->setSection("Notification");
    config->getParameters("Miro::LogInterceptorParameters", *params);

    // override defaults from command line
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
      const ACE_TCHAR *current_arg = arg_shifter.get_current();

      if ((ACE_OS::strcasecmp(cmdTraceOpt, current_arg) == 0) ||
                  (ACE_OS::strcasecmp(shortCmdTraceOpt, current_arg) == 0)) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          if (string("ascii") == arg_shifter.get_current())
            params->asciiLog = true;
          if (string("notify") == arg_shifter.get_current())
            params->notifyLog = true;
          arg_shifter.consume_arg();
        }
      }
      else
        arg_shifter.ignore_arg();
    }

    if ((params->asciiLog || params->notifyLog) &&
                (params->objects.size() == 0 && params->interfaces.size() == 0))
      MIRO_LOG(LL_WARNING, "CmdLog: output requested, but no interface nor object name specified.");

    // sort parameters for higher search efficiency
    vector<IncludeParameters>::iterator first, last = params->objects.end();
    for (first = params->objects.begin(); first != last; ++first) {
      sort(first->excludePrefix.begin(), first->excludePrefix.end());
      sort(first->explicitInclude.begin(), first->explicitInclude.end());
    }
    sort(params->objects.begin(), params->objects.end());

    last = params->interfaces.end();
    for (first = params->interfaces.begin(); first != last; ++first) {
      sort(first->excludePrefix.begin(), first->excludePrefix.end());
      sort(first->explicitInclude.begin(), first->explicitInclude.end());
    }

    MIRO_LOG_OSTR(LL_NOTICE, "LogInterceptorParameters: " << *params);

    // register interceptor at ORB
    PortableInterceptor::ORBInitializer_var orb_initializer =
      new LogInterceptorInit();
    PortableInterceptor::register_orb_initializer(orb_initializer);
    CORBA::ORB_init(argc, argv); // init orb
  }
}
