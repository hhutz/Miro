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

#include <orbsvcs/CosNotifyCommC.h>
#include <orbsvcs/CosNotifyChannelAdminS.h>

#include <ace/Time_Value.h>
#include <ace/Get_Opt.h>
#include <ace/OS_NS_sys_time.h>

#include <iostream>
#include <string>
#include <set>

using namespace std;

string channelName("NotifyEventChannel");
ACE_Time_Value timeout(5);
bool verbose;

int 
parseArgs(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "c:t:?");
  
  while ((c = get_opts()) != -1) {
    switch (c) {
      break;
    case 'c':
      channelName = get_opts.optarg;
      break;
    case 'v':
      verbose = true;
      break;
    case 't':
      timeout.set(atof(get_opts.optarg));
    case '?':
    default:
      rc = 1;
    }
  }


  if (rc) {
    cerr << "usage: " << argv[0] << "[-t timeout [-c channel_name] [-v?]" << endl
	 << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	 << "  -t <sec> timeout (default: 5s)" << endl
	 << "  -v verbose mode" << endl
	 << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    cerr << "timeout: " << timeout << endl
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

  cout << "offered events (subscription management): " << endl;
  for (CORBA::ULong i = 0; i < events->length(); ++i) {
    cout << "domain name: " 
	 << events[i].domain_name << "    "
	 << "type name: "
	 << events[i].type_name << endl;
  }

  //----------------------------------------------------------------------------
  // create and connect a supplier class
  
  class MyConsumer : public POA_CosNotifyComm::StructuredPushConsumer
  {
    typedef set<string> StringSet;
    
  public:
    MyConsumer() :
      start(ACE_OS::gettimeofday()),
      started(false)
    {
      start += timeout;
    }

    ~MyConsumer() { cout << "Ctor of MyConsumer" << endl; }
    
    void push_structured_event(CosNotification::StructuredEvent const& event)
      throw()
    {
      string type = 
	string("domain name: ") +
	string(event.header.fixed_header.event_type.domain_name) +
	string("    ") +
	string("type name: ") +
	string(event.header.fixed_header.event_type.type_name);

      unsigned int size = offers.size();
      offers.insert(type);

      if (ACE_OS::gettimeofday() < start)
	return;

      if (!started || size < offers.size()) {
	cout << endl << endl << "updated offer set (received): " << endl;
	StringSet::const_iterator first, last = offers.end();
	for (first = offers.begin(); first != last; ++first) {
	  cout << "  " << *first << endl;
	}
	started = true;
      }
    }
    void offer_change(CosNotification::EventTypeSeq const& added,
		      CosNotification::EventTypeSeq const& removed)
      throw(CosNotifyComm::InvalidEventType)
    {
      cerr << endl << endl << "offer_change() called" << endl;

      if (added.length() > 0) {
	cout << "added: " << endl;
	for (CORBA::ULong i = 0; i < added.length(); ++i) {
	  cout << "  domain name: "
	       << added[i].domain_name 
	       << "    "
	       << "type name: "
	       << added[i].type_name << endl;
	}
      }
      if (removed.length() > 0) {
	cout << "removed: " << endl;
	for (CORBA::ULong i = 0; i < removed.length(); ++i) {
	  cout << "  domain name: "
	       << removed[i].domain_name 
	       << "    "
	       << "type name: "
	       << removed[i].type_name << endl;
	}
      }
    }

    void disconnect_structured_push_consumer() throw() 
    {
      cerr << "disconnect_structured_push_consumer() called" << endl;
    }

  private:
    ACE_Time_Value start;
    bool started;
    StringSet offers;
  };

  MyConsumer * consumer = new MyConsumer();

  // activate supplier with poa
  server.activate(consumer);

  CosNotifyComm::StructuredPushConsumer_var objref = consumer->_this();
  // connect
  proxy_supplier->connect_structured_push_consumer(objref);


  cout << "collecting offered event set (" << timeout << "s)" << endl;

  //----------------------------------------------------------------------------
  // receiving events
  server.run(1);

  proxy_supplier->disconnect_structured_push_supplier();
  consumer_admin->destroy();

  return 0;
}
