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
  CosNotifyChannelAdmin::SupplierAdmin_var supplier_admin;

  supplier_admin = ec->new_for_suppliers(ifgop, admin_id);

  //----------------------------------------------------------------------------
  // create a notification service proxy consumer, to send events to

  // This proxy's id.
  CosNotifyChannelAdmin::ProxyID proxy_id;
   // The proxy that we are connected to.
  CosNotifyChannelAdmin::StructuredProxyPushConsumer_var proxy_consumer;


  CORBA::Object_var obj;
  obj = supplier_admin->
    obtain_notification_push_consumer(CosNotifyChannelAdmin::STRUCTURED_EVENT,
				      proxy_id);
  assert(!CORBA::is_nil(obj));

  // narrow
  proxy_consumer = 
    CosNotifyChannelAdmin::StructuredProxyPushConsumer::_narrow(obj);

  //----------------------------------------------------------------------------
  // offer/subscription management

  // register the events we gonna offer
  CosNotification::EventTypeSeq added;
  CosNotification::EventTypeSeq removed;

  // remove default offer
  added.length(0);
  removed.length(1);
  removed[0].domain_name = string("*").c_str();
  removed[0].type_name = string("*").c_str();
  supplier_admin->offer_change(added, removed);

  //----------------------------------------------------------------------------
  // create and connect a supplier class
  
  class MySupplier : public POA_CosNotifyComm::StructuredPushSupplier
  {
  protected:
    void subscription_change(CosNotification::EventTypeSeq const& added,
			     CosNotification::EventTypeSeq const& removed)
      throw(CosNotifyComm::InvalidEventType) {
      cout << "subscription change (subscription management): " << endl;
      for (CORBA::ULong i = 0; i < added.length(); ++i) {
	cout << "added -  domain name: " 
	     << added[i].domain_name << "    "
	     << "type name: "
	     << added[i].type_name << endl;
      }
      for (CORBA::ULong i = 0; i < removed.length(); ++i) {
	cout << "removed - domain name: " 
	     << removed[i].domain_name << "    "
	     << "type name: "
	     << removed[i].type_name << endl;
      }
    }
    void disconnect_structured_push_supplier() throw() {
      cerr << "disconnect_structured_push_supplier() called" << endl;
    }
  };

  //----------------------------------------------------------------------------
  // run even loop
  server.detach(1);

  MySupplier * supplier = new MySupplier();

  // activate supplier with poa
  server.activate(supplier);

  CosNotifyComm::StructuredPushSupplier_var objref = supplier->_this();
  // connect
  proxy_consumer->connect_structured_push_supplier(objref);


  cout << "monitoring subscribed event set." << endl;


  server.wait();

  proxy_consumer->disconnect_structured_push_consumer();
  supplier_admin->destroy();

  return 0;
}
