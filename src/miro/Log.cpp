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
#include "Log.h"
#include "Exception.h"
#include "miro_Export.h"

#include <ace/ACE.h>
#include <ace/Arg_Shifter.h>
#include <ace/Log_Record.h>
#include <ace/OS_NS_strings.h>

namespace
{

  ACE_TCHAR const * const MIRO_LOG_LEVEL = ACE_TEXT("-MiroLogLevel");
  ACE_TCHAR const * const MIRO_LOG_FILTER = ACE_TEXT("-MiroLogFilter");
  ACE_TCHAR const * const MIRO_LOG_LEVEL_SHORT = ACE_TEXT("-MLL");
  ACE_TCHAR const * const MIRO_LOG_FILTER_SHORT = ACE_TEXT("-MLF");
  ACE_TCHAR const * const MIRO_LOG_DEVICE   = ACE_TEXT("-MiroLogDevice");
  ACE_TCHAR const * const helpKey = "-MiroHelp";
  ACE_TCHAR const * const shortHelpKey = "-?";

  bool printHelp = false;
  char const * const logHelp =
    "Miro log parameters:\n"
    "  -MLL (-MiroLogLevel) <0-9> level of log verbosity (default: 5, LL_WARNING)\n"
    "  -MLL (-MiroLogFilter) <name> enable debug output for specific category\n";

  std::string logDevice;
}

//
// a log facility, providing loglevels and output
//

namespace Miro
{
  namespace LC {
    ACE_TCHAR const * const MIRO_NAME = ACE_TEXT("MIRO");
    ACE_TCHAR const * const NMC_NAME = ACE_TEXT("NMC");
  }

  unsigned int Log::level_ = LL_WARNING;
  unsigned int Log::mask_ = LC::MIRO;
  char const * Log::format_ = "%D %s\n";
  ACE_Log_Priority Log::aceLM_[MAX_DBG_LEVEL + 1] = {
    LM_EMERGENCY,
    LM_ALERT,
    LM_CRITICAL,
    LM_ERROR,
    LM_WARNING,
    LM_NOTICE,
    LM_DEBUG,
    LM_DEBUG,
    LM_DEBUG,
    LM_DEBUG
  };


  /**
   * Adds the log cathegory clear names corresponding to the cathegory
   * integer constants to the ACE_Log_Record.
   *
   * Parses the argument vector for the Miro command line option:
   * -MiroLogLevel, -MiroLogFilter and -MiroLogDevice.
   *
   * @TODO -MiroLogDevice has no effect. - Needs to be implemented.
   */
  void
  Log::init(int& argc, ACE_TCHAR * argv[])
  {
    static bool initialized = false;
    if (initialized)
      return;
    initialized = true;

#if (GCC_MAJOR_VERSION >= 3)
    std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif

    mask_ = 0;
    // register a logging cathegories at the log record
    register_category(Miro::LC::MIRO, LC::MIRO_NAME);
    // register a logging cathegories at the log record
    register_category(Miro::LC::NMC, LC::NMC_NAME);

    // parse command line arguments
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
      const ACE_TCHAR *currentArg = arg_shifter.get_current();

      if (ACE_OS::strcasecmp(MIRO_LOG_LEVEL, currentArg) == 0 ||
                  ACE_OS::strcasecmp(MIRO_LOG_LEVEL_SHORT, currentArg) == 0) {
        arg_shifter.consume_arg();
        if (arg_shifter.is_parameter_next()) {
          level_ = strtol(arg_shifter.get_current(), (char**)NULL, 10);
          if (level_ == 0 && errno == ERANGE)
            throw Miro::CException(errno, "Error parsing -MiroLogLevel");
          arg_shifter.consume_arg();
        }
      }
      else if (ACE_OS::strcasecmp(MIRO_LOG_FILTER, currentArg) == 0 ||
                  ACE_OS::strcasecmp(MIRO_LOG_FILTER_SHORT, currentArg) == 0) {
        arg_shifter.consume_arg();

        while (arg_shifter.is_parameter_next()) {
          unsigned int i;
          for (i = ACE::log2(LC::MIRO); i < 32; ++i) {
            if (ACE_OS::strcasecmp(ACE_Log_Record::priority_name(ACE_Log_Priority(1 << i)), arg_shifter.get_current()) == 0) {
              ACE_Log_Msg *l = ACE_Log_Msg::instance();
              unsigned int p = l->priority_mask(ACE_Log_Msg::PROCESS);
              p |= 1 << i;
              l->priority_mask(p, ACE_Log_Msg::PROCESS);

              break;
            }
          }
          if (i == 32)
            throw Miro::Exception(std::string("Unknown parameter for -MiroLogFilter: ") +
                                  arg_shifter.get_current());

          arg_shifter.consume_arg();
        }
      }
      else if (ACE_OS::strcasecmp(MIRO_LOG_DEVICE, currentArg) == 0) {
        arg_shifter.consume_arg();

        if (arg_shifter.is_parameter_next()) {
          logDevice = arg_shifter.get_current();
        }
      }
      else if (ACE_OS::strcasecmp(currentArg, helpKey) == 0 ||
               ACE_OS::strcasecmp(currentArg, shortHelpKey) == 0) {
        arg_shifter.ignore_arg();

        printHelp = true;                 
      }
      else {
        arg_shifter.ignore_arg();
      }
    }

    mask_ = ACE_Log_Msg::instance()->priority_mask(ACE_Log_Msg::PROCESS);

//     for (unsigned int i = 0; i < 32; ++i) {
//       unsigned int p = 1UL << i;
//       if (mask_ & p) {
// 	std::cout << "logged cathegory: "
// 		  << ACE_Log_Record::priority_name((ACE_Log_Priority) p)
// 		  << std::endl;
//       }
//    }
    if (printHelp) {
      MIRO_LOG(LL_NOTICE, logHelp);
    }
  }

  void
  Log::register_category(unsigned int _category, ACE_TCHAR const * _name)
  {
    // register a logging cathegories at the log record
    ACE_Log_Record::priority_name((ACE_Log_Priority)_category, _name);
  }
}
