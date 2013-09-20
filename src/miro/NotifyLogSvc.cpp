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
#include "NotifyLogSvc.h"

#include "Log.h"
#include "Server.h"
#include "ServerWorker.h"
#include "ClientParameters.h"
#include "LogNotifyConsumer.h"
#include "Configuration.h"

#include <orbsvcs/Notify/Service.h>
#include <orbsvcs/CosNotifyChannelAdminC.h>
#include <orbsvcs/CosNamingC.h>

#include <ace/OS_NS_strings.h>
#include <ace/Get_Opt.h>

namespace Miro
{
  using namespace std;

  NotifyLogSvc::NotifyLogSvc() :
      _consumer(NULL)
  {
    MIRO_LOG_CTOR("Miro::NotifyLogSvc");
  }

  NotifyLogSvc::~NotifyLogSvc()
  {
    MIRO_LOG_DTOR("Miro::NotifyLogSvc");
  }

  int
  NotifyLogSvc::init(int argc, ACE_TCHAR *argv[])
  {
    MIRO_LOG(LL_NOTICE, "Miro::NotifyLogSvc::init()");

    // parasing the arguments
    if (parse_args(argc, argv) != 0)
      return -1;

    try {
      Client client;

      CosNotifyChannelAdmin::EventChannel_var ec =
        client.resolveName<CosNotifyChannelAdmin::EventChannel>(RobotParameters::instance()->eventChannelName);

      // The consumer, that gets the events
      _consumer =
        new Miro::LogNotifyConsumer(ec,
                                    RobotParameters::instance()->namingContextName,
                                    _fileName,
                                    *Miro::LogNotifyParameters::instance());
    }
    catch (Miro::Exception const& e) {
      MIRO_LOG_OSTR(LL_CRITICAL, "Miro exception constructing LogNotifyConsumer:\n" << e.what());
      return -1;
    }
    catch (CORBA::Exception const& e) {
      MIRO_LOG_OSTR(LL_CRITICAL, "CORBA exception constructing LogNotifyConsumer:\n" << e);
      return -1;
    }
    catch (std::exception const& e) {
      MIRO_LOG_OSTR(LL_CRITICAL, "Std exception constructing LogNotifyConsumer: " << e.what());
      return -1;
    }
    catch (...) {
      MIRO_LOG_OSTR(LL_CRITICAL, "Unknown exception constructing LogNotifyConsumer.");
      return -1;
    }

    return 0;
  }

  int
  NotifyLogSvc::info(ACE_TCHAR **bufferp, size_t len) const
  {
    static const char i[] = "MIRO Notification loader Service \n";

    if (*bufferp == 0)
      *bufferp = ACE::strnew(i);
    else
      ACE_OS_String::strncpy(*bufferp, i, len);

    return ACE_OS_String::strlen(*bufferp);
  }


  int
  NotifyLogSvc::fini()
  {
    MIRO_LOG(LL_NOTICE, "Miro::NotifyLogSvc::fini()");

    if (_consumer) {
      _consumer->closeWriter();
      delete _consumer;
    }
    _consumer = NULL;

    MIRO_LOG(LL_NOTICE, "Miro::NotifyLogSvc::fini() done");

    return 0;
  }

  int
  NotifyLogSvc::parse_args(int& argc, char* argv[])
  {
    int rc = 0;
    int c;

    // initialize parameters from config file
    Miro::LogNotifyParameters * params =
      Miro::LogNotifyParameters::instance();
    // reset to default parameters
    *params = Miro::LogNotifyParameters();
    _fileName    = "";
    _verbose     = false;

    Miro::ConfigDocument * config = Miro::Configuration::document();
    config->setSection("Notification");
    config->getParameters("Miro::LogNotifyParameters", *params);

    // initialize parameters from command line
    ACE_Get_Opt get_opts(argc, argv, "n:v?");

    while ((c = get_opts()) != -1) {
      switch (c) {
        case 'n':
          _fileName = get_opts.optarg;
          break;
        case 'v':
          _verbose = true;
          break;
        case '?':
        default:
          rc = 1;
      }
    }

    if (rc) {
      cerr << "usage: " << argv[0] << "[-n <file name> -v?]" << endl
           << "  -n name of the log file to create" << endl
           << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
           << "  -v verbose mode" << endl
           << "  -MCF <filename.xml> Miro Configuration File specifying type_name list to record" << endl
           << "  -? help: emit this text and stop" << endl;
    }

    if (_verbose) {
      cerr << "NotifyLogSvc parameters:" << endl
           << *params << endl;
    }
    return rc;
  }
}

// Create an object that will insert the <Timer_Service> into the list
// of statically linked services that the <ACE_Service_Config> will
// process at run-time.
ACE_FACTORY_NAMESPACE_DEFINE(miro, Miro_NotifyLogSvc, Miro::NotifyLogSvc);
ACE_STATIC_SVC_DEFINE(Miro_NotifyLogSvc,
                      ACE_TEXT("NotifyLogSvc"),
                      ACE_SVC_OBJ_T,
                      &ACE_SVC_NAME(Miro_NotifyLogSvc),
                      ACE_Service_Type::DELETE_THIS | ACE_Service_Type::DELETE_OBJ,
                      0);
ACE_STATIC_SVC_REQUIRE(Miro_NotifyLogSvc);
