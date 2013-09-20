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
#include "miro/AnyPrinter.h"
#include "miro/RobotParameters.h"

#include <orbsvcs/CosNotifyCommC.h>
#include <orbsvcs/CosNotifyChannelAdminS.h>

#include <ace/Time_Value.h>
#include <ace/Get_Opt.h>

#include <iostream>
#include <string>
#include <set>

using namespace std;

string channelName("NotifyEventChannel");
string eventName("*");
bool verbose;

bool printName = true;
bool printType = true;
bool printLength = false;
bool printEnums = false;
bool printTc = false;
bool printId = false;

int maxSeqLen = 100;

int
parseArgs(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "e:c:m:ntlsdiNTLSDIv?");

  while ((c = get_opts()) != -1) {
    switch (c) {
      case 'e':
        eventName = get_opts.optarg;
        break;
      case 'c':
        channelName = get_opts.optarg;
        break;
      case 'm':
	maxSeqLen = atoi(get_opts.optarg);
      case 'n':
        printName = true;
        break;
      case 'N':
        printName = false;
        break;
      case 't':
        printType = true;
        break;
      case 'T':
        printType = false;
        break;
      case 'l':
        printLength = true;
        break;
      case 'L':
        printLength = false;
        break;
      case 's':
        printEnums = true;
        break;
      case 'S':
        printEnums = false;
        break;
      case 'd':
        printTc = true;
        break;
      case 'D':
        printTc = false;
        break;
      case 'i':
        printId = true;
        break;
      case 'I':
        printId = false;
        break;
      case 'v':
        verbose = true;
        break;
      case '?':
      default:
        rc = 1;
    }
  }

  if (rc) {
    cerr << "usage: " << argv[0] << "-e eventName [-c channelName] [-v?]" << endl
	 << "  -e <event name> name of the event type to look for (default: *)" << endl
	 << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	 << "  -n|N print field names <true|false> (default is true)" << endl
	 << "  -t|T print field names <true|false> (default is true)" << endl
	 << "  -l|L print length of arrays and sequences <true|false> (default is false)" << endl
	 << "  -s|S print enumeration sets <true|false> (default is false)" << endl
	 << "  -d|D print type codes <true|false> (default is false)" << endl
	 << "  -m maximum length for sequences to be printed (default is 100)" << endl
	 << "  -i|I print type id of user types <true|false> (default is false)" << endl
	 << "  -v verbose mode" << endl
	 << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    cerr << "event name: " << eventName << endl
	 << "channel name: " << channelName << endl
	 << "print names: " << printName << endl
	 << "print types: " << printType << endl
	 << "print length: " << printLength << endl
	 << "print enums: " << printEnums << endl
	 << "print type codes: " << printTc << endl
	 << "print IDs: " << printId << endl
	 << "max seq len: " << maxSeqLen << endl;
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

  // tell about the events we gonna subscribe
  CosNotification::EventTypeSeq added;
  CosNotification::EventTypeSeq removed;

  added.length(1);
  added[0].domain_name = Miro::RobotParameters::instance()->namingContextName.c_str();
  added[0].type_name = eventName.c_str();
  // remove default subscription
  removed.length(1);
  removed[0].domain_name = string("*").c_str();
  removed[0].type_name = string("*").c_str();
  consumer_admin->subscription_change(added, removed);

  //----------------------------------------------------------------------------
  // create and connect a supplier class

  class MyConsumer : public POA_CosNotifyComm::StructuredPushConsumer {
  public:
    MyConsumer() :
        anyPrinter(printName,
                   printType,
                   printLength,
                   printEnums,
                   printTc,
                   printId,
		   maxSeqLen)
    {}

    virtual
    void
    push_structured_event(CosNotification::StructuredEvent const& event)
    throw()
    {
      anyPrinter.print(cout, event.remainder_of_body);
      cout << endl;
    }

    virtual
    void
    offer_change(const CosNotification::EventTypeSeq&, const CosNotification::EventTypeSeq&)
    throw()
    {}

    void disconnect_structured_push_consumer() throw()
    {
      cerr << "disconnect_structured_push_consumer() called" << endl;
    }
  private:
    Miro::AnyPrinter anyPrinter;
  };

  MyConsumer * consumer = new MyConsumer();

  // activate supplier with poa
  server.activate(consumer);

  CosNotifyComm::StructuredPushConsumer_var objref = consumer->_this();
  // connect
  proxy_supplier->connect_structured_push_consumer(objref);


  cout << "entering event loop" << endl;

  //----------------------------------------------------------------------------
  // receiving events
  server.run(1);

  proxy_supplier->disconnect_structured_push_supplier();
  consumer_admin->destroy();

  return 0;
}
