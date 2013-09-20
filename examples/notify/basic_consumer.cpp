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
#include "orbsvcs/CosNamingC.h"
#include "orbsvcs/CosNotifyCommC.h"
#include "orbsvcs/CosNotifyChannelAdminS.h"

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

int main(int argc, char * argv[])
{
  CORBA::Object_var obj;

  //----------------------------------------------------------------------------
  // initialize ORB

  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

  //----------------------------------------------------------------------------
  // initialize POA

  obj = orb->resolve_initial_references("RootPOA");
  assert(!CORBA::is_nil(obj));
  PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);
  PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();
  poa_manager->activate();

  //----------------------------------------------------------------------------
  // get naming service reference

  obj = orb->resolve_initial_references("NameService");
  assert(!CORBA::is_nil(obj));
  CosNaming::NamingContext_var naming_service =
    CosNaming::NamingContext::_narrow(obj);

  //----------------------------------------------------------------------------
  // get event channel reference

  // resolving name in robots naming context
  assert(argc >= 2);
  std::string naming_context = argv[1];

  CosNaming::Name name;
  name.length(2);
  name[0].id = naming_context.c_str();
  name[1].id = string("NotifyEventChannel").c_str();

  obj = naming_service->resolve(name);

  // The event channel
  CosNotifyChannelAdmin::EventChannel_var ec =
    CosNotifyChannelAdmin::EventChannel::_narrow(obj);

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
    obtain_offered_types(CosNotifyChannelAdmin::NONE_NOW_UPDATES_OFF);

  // tell about the events we gonna subscribe
  CosNotification::EventTypeSeq added;
  CosNotification::EventTypeSeq removed;

  added.length(1);
  added[0].domain_name = string("Robot_Name").c_str();
  added[0].type_name = string("Event_Name").c_str();  added.length(1);
  // remove default subscription
  removed.length(1);
  removed[0].domain_name = string("*").c_str();
  removed[0].type_name = string("*").c_str();
  consumer_admin->subscription_change(added, removed);

  //----------------------------------------------------------------------------
  // create and connect a supplier class
  
  class MyConsumer : public POA_CosNotifyComm::StructuredPushConsumer
  {
  public:
    void push_structured_event(CosNotification::StructuredEvent const& event)
      throw()
    {
      cout << "domain name: " 
	   << event.header.fixed_header.event_type.domain_name << endl
	   << "type name: "
	   << event.header.fixed_header.event_type.type_name << endl;
    }
    void offer_change(CosNotification::EventTypeSeq const& added,
			     CosNotification::EventTypeSeq const& removed)
      throw(CosNotifyComm::InvalidEventType)
    {
      cerr << "offer_change() called" << endl;
    }
    void disconnect_structured_push_consumer() throw() 
    {
      cerr << "disconnect_structured_push_consumer() called" << endl;
    }
  };

  MyConsumer * consumer = new MyConsumer();

  // activate supplier with poa
  CosNotifyComm::StructuredPushConsumer_var objref = consumer->_this();
  // connect
  proxy_supplier->connect_structured_push_consumer(objref);


  //----------------------------------------------------------------------------
  // receiving events
  orb->run();

  return 0;
}
