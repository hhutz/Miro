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
#include "miro/NotifyTypedSupplier.h"
#include "miro/Log.h"
#include "miro/TimeHelper.h"
#include "miro/RobotParameters.h"

#include <orbsvcs/Time_Utilities.h>

#include <ace/Get_Opt.h>

#include <iostream>
#include <cmath>

using namespace std;

string event_name = "Test";
string channel_name = "NotifyEventChannel";
double time_out = 0.1;
int payload = 0;
bool verbose = false;

int 
parse_args(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "p:t:c:e:v?");
  
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
    case 'p':
      payload = atoi(get_opts.optarg);
      break;
    case '?':
    default:
      rc = 1;
    }
  }

  if (rc == 1) {
      cerr << "usage: " << argv[0] << "[-t event_type] [-e event_name] [-c channel_name] [-v?]" << endl
	   << "  -t <timeout> timeout between events (defaumlt 0.1)" << endl
	   << "  -e <event name> name of the event (default Test)" << endl
	   << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	   << "  -p <payload size> payload buffer length in bytes (default: 0)" << endl
	   << "  -v verbose mode" << endl
	   << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    Miro::Server server;
    
    cout << "event name: " << event_name << endl 
	 << "domain name (naming context): " << Miro::RobotParameters::instance()->namingContextName << endl
	 << "channel name: " << channel_name << endl
	 << "timeout: " << time_out << endl
	 << "payload: " << payload << endl;
  }

  return rc;
}

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
    server.detach();
   
    // we create a supplier for
    // "test" events from "my_robot" with payload <XMP::Payload>
    Miro::NotifyTypedSupplier<test::Event> supplier(ec, event_name);

    ACE_Time_Value t;
    t.set(time_out);
    test::Event e;
    //    payload.msg.status = 0;
    e.buffer.length(payload);

    while (!server.isShutdown()) {
      ORBSVCS_Time::Absolute_Time_Value_to_TimeT(e.timestamp, ACE_OS::gettimeofday());
      ACE_Time_Value before = ACE_OS::gettimeofday();
      supplier.sendEvent(e);  // send 1 event

      if (verbose)
	cout << "." << flush;

      ACE_Time_Value after = ACE_OS::gettimeofday();
      if (t > after - before) {
	ACE_OS::sleep(t - (after - before));
      }
      else if (time_out > 0.001) {
	cerr << endl << "too high consumer latency: " << (after - before);
      }
    }
  }
  catch (CORBA::Exception const& e) {
    cerr << "CORBA Exception: " << e << endl;
  }

  cout << endl;
  return 0;
}
