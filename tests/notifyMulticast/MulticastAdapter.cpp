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
#include "miro/NotifyMulticast.h"
#include "miro/Server.h"
#include "miro/Configuration.h"
#include "miro/TimeHelper.h"
#include "miro/Log.h"
#include "miro/NotifyMulticastParameters.h"

int main(int argc, char *argv[])
{
  int rc = 1;

  // read syslog options
  Miro::Log::init(argc, argv);

  // Parameters to be passed to the services
  Miro::RobotParameters * robotParameters = Miro::RobotParameters::instance();
  Miro::NMC::Parameters * nmcParameters = Miro::NMC::Parameters::instance();

  // Config file processing
  Miro::ConfigDocument * config =  Miro::Configuration::document();
  config->setSection("Robot");
  config->getParameters("Miro::RobotParameters", *robotParameters);
  config->setSection("Notification");
  config->getParameters("Miro::NMC::Parameters", *nmcParameters);
  
  MIRO_LOG_OSTR(LL_NOTICE, "  robot parameters:\n" << *robotParameters);
  MIRO_LOG_OSTR(LL_NOTICE, "  multicast parameters:\n" << *nmcParameters);

  MIRO_LOG(LL_NOTICE,"Initialize server daemon.");
  Miro::Server server(argc, argv);

  try {
    // The one channel that we create using the factory.
    CosNotifyChannelAdmin::EventChannel_var ec =
      server.resolveName<CosNotifyChannelAdmin::EventChannel>("EventChannel");
    
    Miro::NMC::Adapter mcAdapter(argc, argv, &server, ec, server.namingContextName);
    mcAdapter.init();
  
    try {
      MIRO_LOG(LL_NOTICE , "Loop forever handling events." );
      server.run();
      MIRO_LOG(LL_NOTICE , "Server loop ended, exiting." );
      rc = 0;
    }
    catch (const Miro::Exception& e) {
      MIRO_LOG_OSTR(LL_CRITICAL , "Uncought Miro excepition: " << e);
    }

    server.detach(1);
    mcAdapter.fini();

    MIRO_LOG(LL_NOTICE , "shutting down server.");
    server.shutdown();
    server.wait();
  }
  catch (const CORBA::Exception & e) {
    MIRO_LOG_OSTR(LL_CRITICAL , "Uncaught CORBA exception: " << e);
  }
  return rc;
}
