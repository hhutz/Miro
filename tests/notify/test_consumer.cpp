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
#include "EventC.h"

#include "miro/Server.h"
#include "miro/StructuredPushConsumer.h"
#include "miro/Log.h"
#include "miro/TimeHelper.h"
#include "miro/RobotParameters.h"

#include <orbsvcs/Time_Utilities.h>

#include <tao/ORB.h>
#include <tao/AnyTypeCode/TypeCode.h>
#include <tao/Typecode_typesC.h>
#include <tao/DynamicAny/DynamicAny.h>

#include <ace/Get_Opt.h>
#include <ace/OS_NS_sys_time.h>

#include <iostream>
#include <cmath>

using namespace std;

string event_name = "Test";
string channel_name = "NotifyEventChannel";
double time_out = 0.1;
bool verbose = false;

int 
parse_args(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "t:c:e:v?");
  
  while ((c = get_opts()) != -1) {
    switch (c) {
    case 't':
      time_out = atof(get_opts.optarg);
      break;
    case 'c':
      channel_name = get_opts.optarg;
      break;
    case 'e':
      event_name = get_opts.optarg;
      break;
    case 'v':
      verbose = true;
      break;
    case '?':
    default:
      rc = 1;
    }
  }

  if (rc == 1) {
      cerr << "usage: " << argv[0] << "[-t event_type] [-e event_name] [-c channel_name] [-v?]" << endl
	   << "  -t <timeout> timeout between events (default 0.1)" << endl
	   << "  -e <event name> name of the event (default Test)" << endl
	   << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	   << "  -v verbose mode" << endl
	   << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    Miro::Server server;
    
    cout << "event name: " << event_name << endl 
	 << "domain name (naming context): " << Miro::RobotParameters::instance()->namingContextName << endl
	 << "channel name: " << channel_name << endl
	 << "timeout: " << time_out << endl;
  }

  return rc;
}

class MyConsumer : public Miro::StructuredPushConsumer
{
public:
  MyConsumer(CosNotifyChannelAdmin::EventChannel_ptr ec) :
    Miro::StructuredPushConsumer(ec)
  {
    CosNotification::QoSProperties properties;
    properties.length(2);

    // only queue one event per consumer
    properties[0].name = CORBA::string_dup(CosNotification::MaxEventsPerConsumer);
    properties[0].value <<= CORBA::Long(1);
    
    // discard older events
    properties[1].name = CORBA::string_dup(CosNotification::DiscardPolicy);
    properties[1].value <<= CORBA::Long(CosNotification::FifoOrder);

    consumerAdmin_->set_qos(properties);
  }

  virtual void push_structured_event(CosNotification::StructuredEvent const& event)
    throw(CosEventComm::Disconnected)
  {
    cout << "domain_name: " << event.header.fixed_header.event_type.domain_name
	 << "  type_name: " << event.header.fixed_header.event_type.type_name << " " << flush;

    if (false) {
    Miro::Client client;
    DynamicAny::DynAnyFactory_var daf =
      client.resolveInit<DynamicAny::DynAnyFactory>("DynAnyFactory");
    DynamicAny::DynAny_var da = daf->create_dyn_any(event.remainder_of_body);
    
    CORBA::TypeCode_var tc = da->type();

    if (tc->kind() == CORBA::tk_struct) {
      CORBA::String_var name = tc->name();
      CORBA::String_var id = tc->id();

      DynamicAny::DynStruct_var ds =
	DynamicAny::DynStruct::_narrow(da);

      for (CORBA::ULong i = 0; i < ds->component_count(); ++i) {
	DynamicAny::DynAny_var member = ds->current_component();
	CORBA::String_var name = ds->current_member_name();
	if (std::string("timestamp") == name.in()) {
	  long long int i =  member->get_ulonglong();
	  ACE_Time_Value t;
	  ORBSVCS_Time::Absolute_TimeT_to_Time_Value(t, i);
	  cout << "latency: " << ACE_OS::gettimeofday() - t << endl;
	  break;
	}
	ds->next();
      }
    }
    else {
      cerr << "unknown event layout" << endl;
    }
    }
    ACE_Time_Value t;
    t.set(time_out);
    ACE_OS::sleep(t);

    cout << "waking up after sleep" << endl;
  }
};

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Server server(argc, argv);

    if (parse_args(argc, argv) != 0) {
      return 1;
    }

    CosNotifyChannelAdmin::EventChannel_var ec =
      server.resolveName<CosNotifyChannelAdmin::EventChannel>(channel_name);
    server.detach(4);
   
    MyConsumer consumer(ec);
    consumer.setSingleSubscription(event_name);
    consumer.connect();

    server.wait();

    consumer.disconnect();
  }
  catch (CORBA::Exception const& e) {
    cerr << "CORBA Exception: " << e << endl;
  }
  return 0;
}
