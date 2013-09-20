// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#include "PayloadC.h"
#include "SharedBeliefStateC.h"
#include "idl/MotionStatusC.h"
#include "idl/RangeEventC.h"


#include "miro/Server.h"
#include "miro/StructuredPushSupplier.h"
#include "miro/LogNotifyConsumer.h"
#include "miro/Log.h"

#include <orbsvcs/Notify/Notify_EventChannelFactory_i.h>
#include <orbsvcs/Notify/Notify_Default_CO_Factory.h>
#include <orbsvcs/Notify/Notify_Default_POA_Factory.h>
#include <orbsvcs/Notify/Notify_Default_Collection_Factory.h>
#include <orbsvcs/Notify/Notify_Default_EMO_Factory.h>

#include <ace/Get_Opt.h>
#include <ace/Sample_History.h>
#include <ace/High_Res_Timer.h>
#include <ace/Sched_Params.h>
#include <ace/Version.h>
#if (ACE_MAJOR_VERSION > 5) || \
  ( (ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION > 5) ) || \
  ( (ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION == 5) && (ACE_BETA_VERSION >= 10) )
#  include <ace/Throughput_Stats.h>
#else
#  include <ace/Stats.h>
#endif


#include <iostream>

enum PayloadID {
  NONE, 
  OCTED_STREAM_1K, OCTET_STREAM_100K,
  INT_ARRAY_1K, INT_ARRAY_100K,
  MOTION_STATUS, RANGE_EVENT,
  SHARED_BELIEF, SHARED_BELIEF_FULL
};


const unsigned int NUM_PAYLOADS = 9;
char const * const payloadName[NUM_PAYLOADS] = {
  "None",
  "OctetStream1K",
  "OctetStream100K",
  "IntArray1K",
  "IntArray10K",
  "MotionStatus",
  "RangeSensor",
  "SharedBelief",
  "SharedBeliefFull"
};

bool verbose = false;
PayloadID payload = OCTED_STREAM_1K;
std::string fileName;
int iterations = 10000;


void 
producePayload(CosNotifyChannelAdmin::EventChannel_ptr _ec)
{
  CosNotification::EventTypeSeq offers;
  offers.length(1);
  offers[0].domain_name = CORBA::string_dup("Miro");
  offers[0].type_name = CORBA::string_dup("Log");
  Miro::StructuredPushSupplier supplier(_ec,
					"Miro",
					true,
					offers);
  CosNotification::StructuredEvent event;
  Miro::StructuredPushSupplier::initStructuredEvent(event, "Miro", "Log");

  switch(payload) {
  case NONE: 
    {
      break;
    }
  case OCTED_STREAM_1K: 
    {
      OctStr_var load = new OctStr;
      load->length(1024);
      for (unsigned int i = 0; i < load->length(); ++i)
	load[i] = (CORBA::Octet) i;
      event.remainder_of_body <<= load._retn();
      break;
    }
  case OCTET_STREAM_100K: 
    {
      OctStr_var load = new OctStr;
      load->length(1024 * 100);
      for (unsigned int i = 0; i < load->length(); ++i)
	load[i] = (CORBA::Octet) i;
      event.remainder_of_body <<= load._retn();
      break;
    }
  case INT_ARRAY_1K: 
    {
      I1K * load = new I1K();
      for (unsigned int i = 0; i < 256; ++i)
	load->array[i] = i;
      event.remainder_of_body <<= load;
      break;
    }
  case INT_ARRAY_100K: 
    {
      I10K * load = new I10K();
      for (unsigned int i = 0; i < 2560; ++i)
	load->array[i] = i;
      event.remainder_of_body <<= load;
      break;
    }
  case MOTION_STATUS:
    {
      Miro::MotionStatusIDL * load = new Miro::MotionStatusIDL();;
      std::cerr << "MotionStautsIDL size: " << sizeof(*load);
      load->time.sec = 0xffffffff;
      load->time.sec = 0x0f0f0f0f;
      event.remainder_of_body <<= load;
      break;
    }
  case RANGE_EVENT:
    {
      Miro::RangeScanEventIDL * load = new Miro::RangeScanEventIDL();
      load->time.sec = 0xffffffff;
      load->time.sec = 0x0f0f0f0f;
      load->range.length(1);
      load->range[0].length(361);
      event.remainder_of_body <<= load;
      break;
    }
  case SHARED_BELIEF: 
    {
      MSL::SharedBeliefState01 * load = new MSL::SharedBeliefState01();
      event.remainder_of_body <<= load;
      break;
    }
  case SHARED_BELIEF_FULL: 
    {
      MSL::SharedBeliefState01 * load = new MSL::SharedBeliefState01();

      load->time = 0x123456789abcdef0LL;
      load->egoBelief.oppGoal.length(2);
      load->egoBelief.ownGoal.length(2);
      load->egoBelief.observation.length(15);
      for (unsigned int i = 0; i < 15; ++i) {
	load->egoBelief.observation[i].classification.length(2);
      }
      event.remainder_of_body <<= load;
      break;
    }
  default: 
    {
      std::ostringstream ostr;
      ostr << "Unknown payload: " << payload;
      throw Miro::Exception(ostr.str());
    }
  }

  ACE_Sample_History history (iterations);
  

  // send two dummies, to get the initializing done...
 
  ACE_hrtime_t test_start = ACE_OS::gethrtime ();
  for (int i = 0; i < iterations; ++i) {
    ACE_hrtime_t start = ACE_OS::gethrtime ();
    supplier.sendEvent(event);
    ACE_hrtime_t now = ACE_OS::gethrtime ();
    history.sample (now - start);
  }

  ACE_hrtime_t test_end = ACE_OS::gethrtime ();

  ACE_DEBUG ((LM_DEBUG, "test finished\n"));
  
  ACE_DEBUG ((LM_DEBUG, "High resolution timer calibration...."));
  ACE_UINT32 gsf = ACE_High_Res_Timer::global_scale_factor ();
  ACE_DEBUG ((LM_DEBUG, "done\n"));
  
  std::cerr << "Scale facor: " << gsf << std::endl;
  //  history.dump_samples ("HISTORY", gsf);
  
  ACE_Basic_Stats stats;
  history.collect_basic_stats (stats);
  stats.dump_results ("Total", gsf);
  
  ACE_Throughput_Stats::dump_throughput ("Total", gsf,
					 test_end - test_start,
					 stats.samples_count ());
}

int 
parseArgs(int& argc, char* argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "n:f:p:v?");
  
  int rc = 0;
  int c;
  
  while ((c = get_opts()) != -1) {
    switch (c) {
    case 'f':
      fileName = get_opts.optarg;
      break;
    case 'n':
      iterations = atoi(get_opts.optarg);
      break;
    case 'p':
      for (unsigned int i = 0; i < NUM_PAYLOADS; ++i) {
	if (strcmp(payloadName[i], get_opts.optarg) == 0) {
	  payload = (PayloadID) i;
	  break;
	}
      }
      break;
    case 'v':
      verbose = true;
      break;
    case '?':
    default:
      std::cerr << "usage: " << argv[0] << "[-fpv?]" << std::endl
		<< "  -n <iterations> number of iterations" << std::endl
		<< "  -f <file name> log file name" << std::endl
		<< "  -p <payload> the type of payload for the test:" << std::endl;
      for (unsigned int i = 0; i < NUM_PAYLOADS; ++i) {
	std::cerr << "      " << payloadName[i] << std::endl;
      }
      std::cerr << "  -v verbose mode" << std::endl
		<< "  -? help: emit this text and stop" << std::endl;
      rc = -1;
    }
  }

  if (verbose) {
    std::cout << "iterations: " << iterations << std::endl
	      << "log file name: " << fileName << std::endl
	      << "payload: " << payloadName[payload] << std::endl;
  }
  return rc;
}



int
main (int argc, char * argv[])
{
  int rc = 1;

  try {
    Miro::Log::init(argc, argv);
    Miro::Server server(argc, argv);

    if (parseArgs(argc, argv) != 0)
      return 1;
    
    TAO_Notify_Default_CO_Factory::init_svc();
    TAO_Notify_Default_POA_Factory::init_svc();
    TAO_Notify_Default_Collection_Factory::init_svc();
    TAO_Notify_Default_EMO_Factory::init_svc();
    
    try {

      ACE_Sched_Params schedp(ACE_SCHED_RR, 10, ACE_SCOPE_PROCESS);
      if (ACE_OS::sched_params(schedp) == -1)
	std::cout << "Warning: Could not set rt scheduler." << std::endl;

      // start server thread in the background
      server.detach(1);

      // build an event channel

      // Notification Channel Factory
      CosNotifyChannelAdmin::EventChannelFactory_var notifyFactory =
	TAO_Notify_EventChannelFactory_i::create(server.poa.in() ACE_ENV_ARG_PARAMETER);
      // Initial qos specified to the factory when creating the EC.
      CosNotification::QoSProperties initialQos;
      // Initial admin props specified to the factory when creating the EC.
      CosNotification::AdminProperties initialAdmin;
      // Channel identifer
      CosNotifyChannelAdmin::ChannelID id;
      // The one channel that we create using the factory
      CosNotifyChannelAdmin::EventChannel_var ec =
	notifyFactory->create_channel(initialQos, initialAdmin, id);

      Miro::LogNotifyParameters params;
      params.event.resize(1);
      params.event[0].domain = "Miro";
      params.event[0].type = "Log";
      Miro::LogNotifyConsumer consumer(server, ec, server.namingContextName, fileName, params);
      consumer.measureTiming(iterations);

      producePayload(ec);

      consumer.evaluateTiming();

      server.shutdown();
      server.wait();
      rc = 0;
    }
    catch(CORBA::Exception const& e) {
      std::cerr << "Uncought CORBA exception:\n" << e << std::endl;
    }
   
  }
  catch (Miro::Exception const& e)
  {
    std::cerr << "Uncought Miro exception:\n" << e << std::endl;
  }
  return rc;
}

