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
#include "NotifyMulticastTest.h"

#include "idl/OdometryC.h"
#include "idl/RangeEventC.h"
#include "idl/ButtonsC.h"

#include "miro/TimeHelper.h"
#include "miro/StructuredPushConsumer.h"

#include <ace/Arg_Shifter.h>
#include <ace/OS.h>
#include <ace/Event_Handler.h>
#include <tao/CDR.h>

#include <orbsvcs/Notify/Notify_EventChannelFactory_i.h>
#include <orbsvcs/Notify/Notify_Default_CO_Factory.h>
#include <orbsvcs/Notify/Notify_Default_POA_Factory.h>
#include <orbsvcs/Notify/Notify_Default_Collection_Factory.h>
#include <orbsvcs/Notify/Notify_Default_EMO_Factory.h>

#include <iostream>

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::cin;

using CosNotifyChannelAdmin::EventChannelFactory;
using CosNotifyChannelAdmin::EventChannel;

#define NOTIFY_FACTORY_NAME "NotifyEventChannelFactory"

CosNaming::Name channelFactoryName;

class Consumer : public Miro::StructuredPushConsumer {
    public:
        struct EventInfo {
            public:
                EventInfo() {
                    reset();
                }

                ~EventInfo() {
                }

                EventInfo(const EventInfo &ei) {
                    this->medianDataSize = ei.medianDataSize;
                    this->trafficVolume = ei.trafficVolume;
                    this->messageCount = ei.messageCount;
                }
                
                inline void reset() {
                    this->medianDataSize = 0;
                    this->trafficVolume = 0;
                    this->messageCount = 0;
                }

                inline void copy(EventInfo &ei) {
                    ei = *this;
                    reset();
                }
                 
                int medianDataSize;
                int trafficVolume;
                int messageCount;
        };
       
        typedef std::map<std::string, EventInfo> EventInfoMap;
        EventInfoMap eventInfoMap;

    public:
        Consumer(CosNotifyChannelAdmin::EventChannel_ptr ec,
                 CosNotification::EventTypeSeq ets,
                 bool _auto) :
            Miro::StructuredPushConsumer(ec)
        {
            CosNotification::EventTypeSeq none(0);
            std::cout << "subscription change (" << ets.length() << ")" << std::endl;
            try {
                consumerAdmin_->subscription_change(ets, none);
            } catch (...) {
                std::cout << "Uncaught exception while subscribing events" << std::endl;
            }

            auto_ = _auto;
        }

        ~Consumer() {
        }

        void push_structured_event(
                const CosNotification::StructuredEvent &e
                ACE_ENV_ARG_DECL_WITH_DEFAULTS)
            throw(CORBA::SystemException, CosEventComm::Disconnected)
        {
            TAO_OutputCDR cdr;

            cdr << e;
        
            std::string domain = (const char *)e.header.fixed_header.event_type.domain_name;
            std::string type = (const char *)e.header.fixed_header.event_type.type_name;
            std::string id = domain + "/" + type;
            EventInfoMap::iterator itr = eventInfoMap.find(id);
            if (itr == eventInfoMap.end()) {
                EventInfo ei;
                eventInfoMap[id] = ei;
                itr = eventInfoMap.find(id);
            }
            itr->second.messageCount += 1;
            itr->second.trafficVolume += cdr.total_length();
            itr->second.medianDataSize = (itr->second.medianDataSize + cdr.total_length()) / 2;
//            std::cout << "Got event from " << e.header.fixed_header.event_type.domain_name << "/" << e.header.fixed_header.event_type.type_name << std::endl;
        }

        void offer_change(
                const CosNotification::EventTypeSeq & added,
                const CosNotification::EventTypeSeq & removed
                ACE_ENV_ARG_DECL_WITH_DEFAULTS)
            ACE_THROW_SPEC ((
                        CORBA::SystemException,
                        CosNotifyComm::InvalidEventType))
        {
            std::cout << "> Added events:" << std::endl;
            for (unsigned int i = 0; i < added.length(); i++) {
                std::cout << "> + " << added[i].domain_name << "/" << added[i].type_name << std::endl;
            }

            std::cout << "> Removed events:" << std::endl;
            for (unsigned int i = 0; i < removed.length(); i++) {
                std::cout << "> - " << removed[i].domain_name << "/" << removed[i].type_name << std::endl;
            }
                
            if (auto_) {
                consumerAdmin_->subscription_change(added, removed);
            }
        }

    protected:
        bool auto_;

};
     
class StatisticHandler : public ACE_Event_Handler {
    public:
        StatisticHandler(Consumer *_consumer) {
            consumer = _consumer;
        }
        
        ~StatisticHandler() {
        }

        int handle_timeout(
                const ACE_Time_Value &/*currentTime*/,
                const void * /*act*/ = 0) {
            std::cout << "> DUMP" << std::endl;
            for (Consumer::EventInfoMap::iterator itr = consumer->eventInfoMap.begin();
                 itr != consumer->eventInfoMap.end();
                 itr++) {
                std::cout << itr->first << ":\t\t (M:" << itr->second.medianDataSize << ", V:" << itr->second.trafficVolume << ", C:" << itr->second.messageCount << ")" << std::endl;
                itr->second.reset();
            }
            return 0;
        }

    protected:
        Consumer *consumer;
                
};

        
NotifyMulticastTest::NotifyMulticastTest(int argc, char *argv[], bool _colocated) :
  Super(argc, argv),
  colocated_(_colocated),
  // Notification Channel
  notifyFactory_((colocated_)? 
		 TAO_Notify_EventChannelFactory_i::create(poa.in()
							  ACE_ENV_ARG_PARAMETER)
		 :
		 resolveName<EventChannelFactory>(channelFactoryName)),
  id_(),
  ifgop_(CosNotifyChannelAdmin::OR_OP)
{
    bool autoSense = false;
  try {
    ec_ = resolveName<EventChannel>("EventChannel");
    DBG(cout << "found channel" << endl);
  }
  catch (...) {
    ec_ = notifyFactory_->create_channel(initialQos_, initialAdmin_, id_);
  }

  CosNaming::Name n(1);
  try {
    if (colocated_) {
      initialNamingContext->bind(channelFactoryName, notifyFactory_.in());
      DBG(cout << "colocated factory" << flush);
    }

    n.length(1);
    n[0].id = CORBA::string_dup("EventChannel");
    namingContext->bind(n, ec_.in());

    DBG(cout << "Bound channel in naming service." << endl);
  } catch (...) {
      std::cout << "Error binding naming service" << std::endl;
  }

  /* only even number of args allowed */
  if ((((argc - 1) & 0x01) == 0) && (argc > 1)) {
      ets.length(argc / 2);

      for (int i = 0; i < (argc / 2); i++) {
          ets[i].domain_name = CORBA::string_dup(argv[2 * i + 1]);
          ets[i].type_name = CORBA::string_dup(argv[2 * i + 2]);
          std::cout << "Subscribing " << ets[i].domain_name << "/" << ets[i].type_name << std::endl;
      }
    
  } else if (argc == 1) {
      std::cout << "Autosensing for events" << std::endl;
      autoSense = true;

  } else {
      std::cerr << "Wrong arg count" << std::endl;
      exit(1);
  }

  consumer = new Consumer(ec_.in(), ets, autoSense);

  reactor_ = reactor();

  sh_ = new StatisticHandler(consumer);
  shTime_ = ACE_Time_Value(1, 0);
  
  shId_ = reactor_->schedule_timer(sh_, 0, shTime_, shTime_);

  nmcAdapter = new Miro::NMC::Adapter(argc, argv, this, ec_.in(), namingContextName);

  DBG(cout << "NotifyMulticastTest initialized.." << endl);
}

NotifyMulticastTest::~NotifyMulticastTest()
{
  DBG(cout << "Destructing NotifyMulticastTest." << endl);

  reactor_->cancel_timer(shId_);
  delete sh_;
  delete nmcAdapter;
  delete consumer;

  try {
    CosNaming::Name n;
    n.length(1);
    n[0].id = CORBA::string_dup("EventChannel");
    namingContext->unbind(n);
    initialNamingContext->unbind(channelFactoryName);
  }
  catch (const CORBA::Exception& e) {
    cerr << "Caught CORBA exception on unbind: " << e << endl;
    cerr << "Porbably the NameSevice went down while we run." << endl;
  }

  DBG(cout << "unbound objects from Name Sevice" << endl);

  if (colocated_)
    ec_->destroy();
}

int
main(int argc, char *argv[])
{
  bool colocated = false;
  const char colocatedOpt[] = "-colocated_ec";
  
  ACE_Arg_Shifter arg_shifter (argc, argv);
  while (arg_shifter.is_anything_left ()) {
    const ACE_TCHAR *current_arg = arg_shifter.get_current ();

    if (ACE_OS::strcasecmp(current_arg, colocatedOpt) == 0) {
      arg_shifter.consume_arg();
      colocated = true;
      break;
    }

    arg_shifter.ignore_arg ();
  }

  // Init TAO Factories
  if (colocated) {
    TAO_Notify_Default_CO_Factory::init_svc();
    TAO_Notify_Default_POA_Factory::init_svc();
    TAO_Notify_Default_Collection_Factory::init_svc();
    TAO_Notify_Default_EMO_Factory::init_svc();
  }

  channelFactoryName.length(1);
  channelFactoryName[0].id = CORBA::string_dup(NOTIFY_FACTORY_NAME);

  std::cout << "nmctest" << std::endl;
  try {
  NotifyMulticastTest NotifyMulticastTest(argc, argv, colocated);
  
  try {
    DBG(cout << "Initialize server daemon." << endl);

    NotifyMulticastTest.run();

    DBG(cout << "NotifyMulticastTest ended, exiting." << endl);
  }
  catch (const CORBA::Exception & e) {
    cerr << "Uncaught CORBA exception: " << e << endl;
    return 1;
  }
  catch (...) {
    cerr << "Uncaught exception: " << endl;
    return 1;
  }
  } catch (const CORBA::Exception &e) {
      std::cout << e << std::endl;
  } catch (...) {
      std::cout << "uncaught exception" << std::endl;
  }

  return 0;
}
