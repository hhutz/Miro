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
#include "miro/Log.h"
#include "miro/Server.h"
#include "miro/TimeHelper.h"
#include "miro/RobotParameters.h"

#include <orbsvcs/CosNotifyCommC.h>
#include <orbsvcs/CosNotifyChannelAdminS.h>

#include <ace/Time_Value.h>
#include <ace/Get_Opt.h>

#include <iostream>
#include <string>
#include <set>
#include <map>

using namespace std;

string channelName("NotifyEventChannel");
string eventName = "*";
bool verbose = false;

map<string, unsigned int> sizeMap;

int 
parseArgs(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "c:e:v?");
  
  while ((c = get_opts()) != -1) {
    switch (c) {
      break;
    case 'c':
      channelName = get_opts.optarg;
      break;
    case 'v':
      verbose = true;
      break;
    case 'e':
      eventName = get_opts.optarg;
      break;
    case '?':
    default:
      rc = 1;
    }
  }


  if (rc) {
    cerr << "usage: " << argv[0] << "-e event_name [-c channel_name] [-v?]" << endl
	 << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	 << "  -e <event name> name of the event_type to listen for (default: *)" << endl
	 << "  -v verbose mode" << endl
	 << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    cerr << "event name: " << eventName << endl
	 << "channel name: " << channelName << endl;
  }

  return rc;
}

int main(int argc, char * argv[])
{
  Miro::Log::init(argc, argv);

  Miro::Server server(argc, argv);

  if (parseArgs(argc, argv) != 0)
    return 1;

  // The event channel
  CosNotifyChannelAdmin::EventChannel_var ec =
    server.resolveName<CosNotifyChannelAdmin::EventChannel>(channelName);
  
  //----------------------------------------------------------------------------
  // creating a notification service supplier admin

  // The group operator between admin-proxy's.
  CosNotifyChannelAdmin::InterFilterGroupOperator ifgop = 
    CosNotifyChannelAdmin::OR_OP;
  // The id of the created adin.
  CosNotifyChannelAdmin::AdminID admin_id;
  // The supplier admin used by supplier.
  CosNotifyChannelAdmin::ConsumerAdmin_var consumer_admin;

  consumer_admin = ec->new_for_consumers(ifgop, admin_id);

  //----------------------------------------------------------------------------
  // create a notification service proxy consumer, to send events to

  // Initial qos specified to the factory when creating the EC.
  CosNotification::QoSProperties initial_qos;
  // Initial admin props specified to the factory when creating the EC.
  CosNotification::AdminProperties initial_admin;
  // This proxy's id.
  CosNotifyChannelAdmin::ProxyID proxy_id;
   // The proxy that we are connected to.
  CosNotifyChannelAdmin::StructuredProxyPushSupplier_var proxy_supplier;


  CORBA::Object_var obj;
  obj = consumer_admin->
    obtain_notification_push_supplier(CosNotifyChannelAdmin::STRUCTURED_EVENT,
				      proxy_id);
  assert(!CORBA::is_nil(obj));

  // narrow
  proxy_supplier = 
    CosNotifyChannelAdmin::StructuredProxyPushSupplier::_narrow(obj);

  //----------------------------------------------------------------------------
  // offer/subscription management

  // don't call us on offer changes
  CosNotification::EventTypeSeq_var events = 
    proxy_supplier->
    obtain_offered_types(CosNotifyChannelAdmin::ALL_NOW_UPDATES_OFF);

  bool offered = false;
  for (CORBA::ULong i = 0; i < events->length(); ++i) {
    if (events[i].domain_name.in() == Miro::RobotParameters::instance()->namingContextName &&
	events[i].type_name.in() == eventName) {
      offered = true;
    }
  }
  if (!offered) {
    cerr << "WARNING: The event you are listening for is apparently not offered." << endl;
  }

  //----------------------------------------------------------------------------
  // create and connect a supplier class
  
  class MyConsumer : public POA_CosNotifyComm::StructuredPushConsumer
  {
  public:
    void push_structured_event(CosNotification::StructuredEvent const& event)
      throw()
    {
      TAO_OutputCDR ostr;
      ostr << event.remainder_of_body.type();

      if (verbose)
	cout <<"domain name: " 
	     << event.header.fixed_header.event_type.domain_name
	     << ":  type name: "
	     << event.header.fixed_header.event_type.type_name
	     << "   type-code size: " << ostr.total_length() << endl;

      if (sizeMap[string(event.header.fixed_header.event_type.type_name)] < ostr.total_length())
	sizeMap[string(event.header.fixed_header.event_type.type_name)] = ostr.total_length();
    }

    void offer_change(CosNotification::EventTypeSeq const&,
		      CosNotification::EventTypeSeq const&)
      throw(CosNotifyComm::InvalidEventType)
    {}

    void disconnect_structured_push_consumer() throw() 
    {
      cerr << "disconnect_structured_push_consumer() called" << endl;
    }
  };

  MyConsumer * consumer = new MyConsumer();

  // activate supplier with poa
  server.activate(consumer);

  CosNotifyComm::StructuredPushConsumer_var objref = consumer->_this();
  // connect
  proxy_supplier->connect_structured_push_consumer(objref);


  cerr << "listening for the subscribed event" << endl;

  //----------------------------------------------------------------------------
  // receiving events
  server.run(1);

  map<string, unsigned int>::const_iterator first, last = sizeMap.end();
  for (first = sizeMap.begin(); first != last; ++first) {
    cout << first->first << " " << first->second << endl;
  }
  

  proxy_supplier->disconnect_structured_push_supplier();
  consumer_admin->destroy();

  return 0;
}
