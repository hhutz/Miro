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
#include "NotifySvc.h"

#include "Log.h"
#include "Server.h"
#include "ServerWorker.h"
#include "ClientParameters.h"
#include "Configuration.h"
#include "miro/Parameters.h"

#include <orbsvcs/Notify/Service.h>
#include <orbsvcs/CosNotifyChannelAdminC.h>
#include <orbsvcs/CosNamingC.h>
#include <orbsvcs/Notify/Properties.h>

#include <tao/Version.h>

#include <ace/OS_NS_strings.h>
#include <ace/Get_Opt.h>

#ifdef WIN32
#include <orbsvcs/Notify/Notify_Default_EMO_Factory.h>
#endif // WIN32

namespace Miro
{
  using namespace std;

  NotifySvc::NotifySvc() :
      m_server(NULL),
      m_properties(NULL),
      m_factory(true),
      m_verbose(false)
  {
    MIRO_LOG_CTOR("Miro::NotifySvc");
  }

  NotifySvc::~NotifySvc()
  {
    MIRO_LOG_DTOR("Miro::NotifySvc");
  }

  int
  NotifySvc::init(int argc, ACE_TCHAR *argv[])
  {
    MIRO_LOG(LL_NOTICE, "Miro::NotifySvc::init()");

    // parasing the arguments
    if (parseArgs(argc, argv) != 0)
      return -1;

// XXX mallan: not sure if there is a more elegant way to do this,
// XXX but the explicit initialization of the service appears to
// XXX be necessary under windows
#ifdef WIN32
    TAO_Notify_Default_EMO_Factory::init_svc();
#endif // WIN32

    m_server = new Server();
    Miro::NotifySvcParameters * params =
      Miro::NotifySvcParameters::instance();

    TAO_Notify_Service*
    notify_service = ACE_Dynamic_Service<TAO_Notify_Service>::instance(TAO_NOTIFY_DEF_EMO_FACTORY_NAME);

    if (notify_service != NULL) {


      // there were static initialization issues with this
      // particular version of TAO
      // unfortunately it was used in the hmp field test, so we need to
      // preserve this special-casing
      // (before it was segfaulting, afterwards the hook was removed)
#if ((TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 5) && (TAO_BETA_VERSION == 8))
      // Set the properties instance before initializing the notify
      // service.  Using the static singleton can cause static
      // destruction issues between the properties instance and
      // statically allocated type code structures.
      m_properties = new TAO_Notify_Properties();
      TAO_Notify_Properties::instance(m_properties);
#endif

      CosNotifyChannelAdmin::EventChannelFactory_var notify_factory;

      if (m_factory) {
        CORBA::ORB_var orb = m_server->orb();
        notify_service->init_service(orb);
        PortableServer::POA_var poa = m_server->worker()->rootPoa();
        notify_factory = notify_service->create(poa);

        m_server->addToNameService(notify_factory, params->factoryName);
      }
      else {
        notify_factory = m_server->resolveName<CosNotifyChannelAdmin::EventChannelFactory>(params->factoryName);
      }

      for (unsigned int i = 0; i < params->channelNames.size(); ++i) {
        CosNotifyChannelAdmin::ChannelID id;
        CosNotification::QoSProperties initial_qos;
        CosNotification::AdminProperties initial_admin;

        CosNotifyChannelAdmin::EventChannel_var ec =
          notify_factory->create_channel(initial_qos, initial_admin, id);

        m_server->addToNameService(ec, params->channelNames[i]);
      }
    }
    else {
      MIRO_LOG(LL_ERROR, "Notification service instanciation failed. Working without notification.");
      return 1;
    }


    return 0;
  }

  int
  NotifySvc::info(ACE_TCHAR **bufferp, size_t len) const
  {
    static const char i[] = "MIRO Notification loader Service \n";

    if (*bufferp == 0)
      *bufferp = ACE::strnew(i);
    else
      ACE_OS_String::strncpy(*bufferp, i, len);

    return ACE_OS_String::strlen(*bufferp);
  }

  int
  NotifySvc::fini()
  {
    MIRO_LOG(LL_NOTICE, "Miro::NotifySvc::fini()");

    delete m_server;
    m_server = NULL;

    // delete m_properties;
    m_properties = NULL;

    MIRO_LOG(LL_NOTICE, "Miro::NotifySvc::fini() done");

    return 0;
  }

  int
  NotifySvc::parseArgs(int& argc, char* argv[])
  {
    int rc = 0;
    int c;

    // initialize parameters from config file
    Miro::NotifySvcParameters * params =
      Miro::NotifySvcParameters::instance();
    // reset to default parameters
    *params = Miro::NotifySvcParameters();
    m_factory = true;
    m_verbose = false;

    Miro::ConfigDocument * config = Miro::Configuration::document();
    config->setSection("Notification");
    config->getParameters("Miro::NotifySvcParameters", *params);

    // initialize parameters from command line
    ACE_Get_Opt get_opts(argc, argv, "Cc:f:Fv?");

    while ((c = get_opts()) != -1) {
      switch (c) {
        case 'F':
          m_factory = false;
          break;
        case 'C':
          params->channelNames.clear();
          break;
        case 'c':
          params->channelNames.push_back(get_opts.optarg);
          break;
        case 'f':
          params->factoryName = get_opts.optarg;
          break;
        case 'v':
          m_verbose = true;
          break;
        case '?':
        default:
          rc = 1;
      }
    }

    if (rc) {
      cerr << "usage: " << argv[0] << "[-f:c:Fv?]" << endl
           << "  -F do _not_ create an event channel factory" << endl
           << "  -f event channel factory name (default: NotifyEventChannelFactory)" << endl
           << "  -C clear the channel list (default & config file channels)" << endl
           << "  -c <name> add this channel to the channel list" << endl
           << "  -v verbose mode" << endl
           << "  -? help: emit this text and stop" << endl;
    }

    if (m_verbose) {
      cerr << "NotifSvc parameters:" << endl
           << *params << endl;
    }
    return rc;
  }
}

// Create an object that will insert the <Timer_Service> into the list
// of statically linked services that the <ACE_Service_Config> will
// process at run-time.
ACE_FACTORY_NAMESPACE_DEFINE(miro, Miro_NotifySvc, Miro::NotifySvc);
ACE_STATIC_SVC_DEFINE(Miro_NotifySvc,
                      ACE_TEXT("NotifySvc"),
                      ACE_SVC_OBJ_T,
                      &ACE_SVC_NAME(Miro_NotifySvc),
                      ACE_Service_Type::DELETE_THIS | ACE_Service_Type::DELETE_OBJ,
                      0);
ACE_STATIC_SVC_REQUIRE(Miro_NotifySvc);
