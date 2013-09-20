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
#include "StructuredPushConsumer.h"
#include "Server.h"
#include "Log.h"
#include "ServerWorker.h"
#include "ClientParameters.h"

#include <tao/Version.h>

#include <cstring>

namespace Miro
{
  using namespace std;

  /**
   * @param _ec Reference to the event channel.
   */
  StructuredPushConsumer::StructuredPushConsumer(CosNotifyChannelAdmin::EventChannel_ptr _ec) :
      serverHelper_(NULL),
      ec_(CosNotifyChannelAdmin::EventChannel::_duplicate(_ec)),
      ifgop_(CosNotifyChannelAdmin::OR_OP),
      consumerAdminId_(),
      consumerAdmin_(),
      connectedMutex_(),
      connected_(false),
      subscribed_(),
      subscriptions_(),
      offers_()
  {
    MIRO_LOG_CTOR("StructuredPushConsumer");

    consumerAdmin_ = ec_->new_for_consumers(ifgop_, consumerAdminId_);

    // remove catch all offer
    CosNotification::EventTypeSeq added;
    CosNotification::EventTypeSeq removed = asterixSubscription();
    consumerAdmin_->subscription_change(added, removed);

    CosNotifyChannelAdmin::ProxySupplier_var proxySupplier =
      consumerAdmin_->
      obtain_notification_push_supplier(CosNotifyChannelAdmin::STRUCTURED_EVENT,
                                        proxySupplierId_);
    MIRO_ASSERT(!CORBA::is_nil(proxySupplier.in()));

    // narrow
    proxySupplier_ =
      CORBA_dynamic_cast<CosNotifyChannelAdmin::StructuredProxyPushSupplier>(proxySupplier.in());

    MIRO_LOG_CTOR_END("StructuredPushConsumer");
  }

  /**
   * @param _ec Reference to the event channel.
   */
  StructuredPushConsumer::StructuredPushConsumer() :
      serverHelper_(NULL),
      ec_(),
      ifgop_(CosNotifyChannelAdmin::OR_OP),
      consumerAdminId_(),
      consumerAdmin_(),
      connectedMutex_(),
      connected_(false),
      subscribed_(),
      subscriptions_(),
      offers_()
  {
    MIRO_LOG_CTOR("StructuredPushConsumer");

    Client client;
    ec_ = client.resolveName<CosNotifyChannelAdmin::EventChannel>(Miro::RobotParameters::instance()->eventChannelName);

    consumerAdmin_ = ec_->new_for_consumers(ifgop_, consumerAdminId_);

    // remove catch all offer
    CosNotification::EventTypeSeq added;
    CosNotification::EventTypeSeq removed = asterixSubscription();
    consumerAdmin_->subscription_change(added, removed);

    CosNotifyChannelAdmin::ProxySupplier_var proxySupplier =
      consumerAdmin_->
      obtain_notification_push_supplier(CosNotifyChannelAdmin::STRUCTURED_EVENT,
                                        proxySupplierId_);
    MIRO_ASSERT(!CORBA::is_nil(proxySupplier.in()));

    // narrow
    proxySupplier_ =
      CORBA_dynamic_cast<CosNotifyChannelAdmin::StructuredProxyPushSupplier>(proxySupplier.in());

    MIRO_LOG_CTOR_END("StructuredPushConsumer");
  }

  StructuredPushConsumer::~StructuredPushConsumer()
  {
    MIRO_LOG_DTOR("StructuredPushConsumer");

    if (serverHelper_ != NULL) {
      MIRO_LOG(LL_NOTICE, "StructuredPushConsumer still connected.");
    }
      
    delete serverHelper_;

    MIRO_LOG_DTOR_END("StructuredPushConsumer");
  }

  void
  StructuredPushConsumer::setHistoryQoS(CORBA::Long historySize, 
		     CORBA::Long order)
  {
    CosNotification::QoSProperties properties;
    properties.length(2);

    // only queue one event per consumer
    properties[0].name = CORBA::string_dup(CosNotification::MaxEventsPerConsumer);
    properties[0].value <<= CORBA::Long(historySize);
    
    // discard older events
    properties[1].name = CORBA::string_dup(CosNotification::DiscardPolicy);
    properties[1].value <<= CORBA::Long(order);

    consumerAdmin_->set_qos(properties);
    
  }


  void
  StructuredPushConsumer::connect()
  {
    MIRO_DBG(MIRO, LL_NOTICE, "Connecting StructuredPushConsumer.");

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    if (connected_)
      throw EAlreadyConnected();

    // create the server class
    serverHelper_ = new Server();
    // Activate the consumer with the bidir POA
    serverHelper_->activate(this, false);
    // get a client reference
    CosNotifyComm::StructuredPushConsumer_var objref = this->_this();
    // connect to the proxy supplier
    proxySupplier_->connect_structured_push_consumer(objref);

    connected_ = true;
  }

  void
  StructuredPushConsumer::disconnect()
  {
    MIRO_DBG(MIRO, LL_NOTICE, "Disconnecting StructuredPushConsumer.");

    Server * s = NULL;
    {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);
      if (!connected_) {
        MIRO_LOG(LL_ERROR, "Already disconnected.");
        return;
      }

      try {

        // there were shutdown issues with this particular version of TAO
        // unfortunately it was used in the hmp field test, so we need to
        // preserve this special-casing
#if !( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 5) && (TAO_BETA_VERSION == 8) )
        proxySupplier_->disconnect_structured_push_supplier();
        consumerAdmin_->destroy();
#endif
      }
      catch (const CORBA::Exception & e) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "StructuredPushConsumer::disconnect() CORBA exception on:\n"
                      << e);
      }

      // prevent double delete, if server owns consumer
      s = serverHelper_;
      serverHelper_ = NULL;
      connected_ = false;
    }

    // ensure no access to member data, as this might result in deletion of consumer
    delete s;
  }

  /**
   * @param type_name Event type name.
   * @param domain_name Event domain name.
   */
  bool
  StructuredPushConsumer::offered(std::string const& _type_name, std::string const& _domain_name) const
  {
    std::string domain_name(_domain_name);
    if (domain_name.length() == 0)
      domain_name = ClientParameters::instance()->namingContextName;

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    for (unsigned int i = 0; i < subscriptions_.length(); ++i) {
      if (_type_name == static_cast<char const *>(subscriptions_[i].type_name) &&
                  domain_name == static_cast<char const *>(subscriptions_[i].domain_name))
        return offers_[i];
    }
    return false;
  }

  void
  StructuredPushConsumer::subscribe()
  {
    subscribed_ = true;
  }

  void
  StructuredPushConsumer::unsubscribe()
  {
    subscribed_ = false;
  }

  /**
   * The consumer will register the subscriptions at its event consumer admin.
   * The admin will make sure, the notification channel suppliers are informed
   * about new subscriptions. That way, suppliers can query whether events it
   * offers are currently subscribed by any consumer.
   *
   * Internally the consumer keeps track of the events offered by suppliers.
   * A user of the consumer can query at the consumer for each subscribed event,
   * whether it is offered by any supplier or not. This way an event sink
   * can determine whether the demanded information is produced by the system.
   *
   * For efficiency, offer/subscription matches can be queried by the index,
   * they apeared within the orgument vector of this method.
   *
   * @param offers The vector of new subscirptions.
   */
  void
  StructuredPushConsumer::setSubscriptions(CosNotification::EventTypeSeq const& _newSubscriptions)
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    CORBA::ULong offersLen = subscriptions_.length();
    CORBA::ULong newOffersLen = _newSubscriptions.length();

    // actually added events
    CosNotification::EventTypeSeq added;
    // dummy: always empty
    CosNotification::EventTypeSeq removed;

    CORBA::ULong addedIndex = 0;
    CORBA::ULong removedIndex = 0;

    // enlarge the offers vector by the maximum required size
    added.length(newOffersLen);
    removed.length(offersLen);

    for (unsigned int i = 0; i < newOffersLen; ++i) {
      unsigned int j;
      for (j = 0; j < offersLen; ++j) {
        // search whether already subscribed
        if (strcmp(_newSubscriptions[i].type_name, subscriptions_[j].type_name) == 0 &&
                    strcmp(_newSubscriptions[i].domain_name, subscriptions_[j].domain_name) == 0) {
          break;
        }
      }

      // if not, add new subscription to list of added subscriptions
      if (j == offersLen) {
        added[addedIndex] = _newSubscriptions[i];
        ++addedIndex;
      }
    }

    for (unsigned int i = 0; i < offersLen; ++i) {
      unsigned int j;
      for (j = 0; j < newOffersLen; ++j) {
        // search whether still offered
        if (strcmp(subscriptions_[i].type_name, _newSubscriptions[j].type_name) == 0 &&
                    strcmp(subscriptions_[i].domain_name, _newSubscriptions[j].domain_name) == 0) {
          break;
        }
      }

      // if not, add old subscription to list of removed subscriptions
      if (j == newOffersLen) {
        removed[removedIndex] = subscriptions_[i];
        ++removedIndex;
      }
    }

    // resize offers vector to actual size
    added.length(addedIndex);
    removed.length(removedIndex);

    // overwrite offers vector
    subscriptions_ = _newSubscriptions;

    // do the offers change and performe bookkeeping
    initiateSubscriptionChange(added, removed);
  }

  void
  StructuredPushConsumer::setSingleSubscription(std::string const& _type_name,
      std::string const& _domain_name)
  {
    std::string domain_name(_domain_name);
    if (domain_name.length() == 0) {
      domain_name = ClientParameters::instance()->namingContextName;
    }

    CosNotification::EventTypeSeq subscription;
    // an empty subscription is now handled as an asterix subscription...
    if (_type_name.length() != 0) {
      subscription.length(1);
      subscription[0].domain_name = domain_name.c_str();
      subscription[0].type_name = _type_name.c_str();
    }
    setSubscriptions(subscription);

    if (_type_name.length() != 0 &&
                !offered(0)) {
      MIRO_LOG_OSTR(LL_WARNING,
                    "StructuredPushConsumer - event subscribed not (yet) offered: domain_name=" <<
                    domain_name << " type_name=" << _type_name);
    }
  }

  void
  StructuredPushConsumer::offer_change(const CosNotification::EventTypeSeq& added,
                                       const CosNotification::EventTypeSeq& removed)
  throw(CosNotifyComm::InvalidEventType)
  {
    MIRO_DBG_OSTR(MIRO,
                  LL_PRATTLE,
                  "StructuredPushConsumer: offer change\nadded messages:");

    for (unsigned int i = 0; i < added.length(); ++i) {
      for (unsigned int j = 0; j < subscriptions_.length(); ++j) {
        if (strcmp(added[i].type_name, subscriptions_[j].type_name) == 0 &&
                    strcmp(added[i].domain_name, subscriptions_[j].domain_name) == 0) {
          offers_[j] = true;
          break;
        }
      }
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    " " << added[i].domain_name << "\t"
                    << "  " << added[i].type_name);
    }
    MIRO_DBG(MIRO, LL_PRATTLE, "removed messages:");
    for (unsigned int i = 0; i < removed.length(); ++i) {
      for (unsigned int j = 0; j < subscriptions_.length(); ++j) {
        if (strcmp(removed[i].type_name, subscriptions_[j].type_name) == 0 &&
                    strcmp(removed[i].domain_name, subscriptions_[j].domain_name) == 0) {
          offers_[j] = false;
          break;
        }
      }
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    "  " << removed[i].domain_name << "\t"
                    << "  " << removed[i].type_name);
    }
  }

  void
  StructuredPushConsumer::push_structured_event(const CosNotification::StructuredEvent & /*notification*/)
  throw(CosEventComm::Disconnected)
  {
    MIRO_LOG(LL_ERROR, "You have to overwrite StructuredPushConsumer::push_structured_event!");
  }

  void
  StructuredPushConsumer::disconnect_structured_push_consumer()
  throw()
  {
    MIRO_DBG(MIRO, LL_NOTICE, "StructuredPushConsumer: disconnect consumer.");
    disconnect();
  }

  void
  StructuredPushConsumer::initiateSubscriptionChange(CosNotification::EventTypeSeq const& _added,
      CosNotification::EventTypeSeq const& _removed)
  {
    // reserve space in subscriptions vector
    offers_.resize(subscriptions_.length(), false);

    MIRO_DBG(MIRO, LL_TRACE, "StructuredPushConsumer subscription change\nadded messages:");

    for (unsigned int i = 0; i < _added.length(); ++i) {
      MIRO_DBG_OSTR(MIRO,
                    LL_TRACE,
                    "  " << _added[i].domain_name << "\t" << "  " << _added[i].type_name);
    }

    MIRO_DBG(MIRO, LL_TRACE, "removed messages:");
    for (unsigned int i = 0; i < _removed.length(); ++i) {
      MIRO_DBG_OSTR(MIRO,
                    LL_TRACE,
                    "  " << _removed[i].domain_name << "\t" << "  " << _removed[i].type_name);
    }
    // inform the supplier admin about the new offers
    proxySupplier_->subscription_change(_added, _removed);

    // generate list of subscribed offers
    CosNotification::EventTypeSeq_var offers =
      proxySupplier_->obtain_offered_types(CosNotifyChannelAdmin::ALL_NOW_UPDATES_ON);

    for (unsigned int i = 0; i < subscriptions_.length(); ++i) {
      offers_[i] = false;
      for (unsigned int j = 0; j < offers->length(); ++j) {
        if (strcmp(subscriptions_[i].type_name, offers[j].type_name) == 0 &&
                    strcmp(subscriptions_[i].domain_name, offers[j].domain_name) == 0) {
          offers_[i] = true;
          break;
        }
      }
    }
  }

  CosNotification::EventTypeSeq
  StructuredPushConsumer::asterixSubscription()
  {
    CosNotification::EventTypeSeq offer;

    offer.length(1);
    offer[0].type_name = CORBA::string_dup("*");
    offer[0].domain_name = CORBA::string_dup("*");

    return offer;
  }
}

