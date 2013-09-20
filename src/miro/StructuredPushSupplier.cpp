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
#include "StructuredPushSupplier.h"
#include "Log.h"
#include "Server.h"
#include "ClientParameters.h"
#include "ServerWorker.h"

#include <iostream>
#include <cstring>

namespace Miro
{
  using namespace std;

  /**
   * Create a new proxy supplier.
   *
   * @param _ec Reference to the event channel.
   */
  StructuredPushSupplier::StructuredPushSupplier(CosNotifyChannelAdmin::EventChannel_ptr _ec) :
      serverHelper_(NULL),
      ec_(CosNotifyChannelAdmin::EventChannel::_duplicate(_ec)),
      ifgop_(CosNotifyChannelAdmin::OR_OP),
      supplierAdminId_(),
      supplierAdmin_(),
      connectedMutex_(),
      connected_(false),
      offers_(),
      subscription_()
  {
    MIRO_LOG_CTOR("StructuredPushSupplier");

    // Obtain new admin.
    // As domain/type based filtering is admin based, we create one
    // admin per supplier.
    supplierAdmin_ = ec_->new_for_suppliers(ifgop_, supplierAdminId_);

    // remove catch all offer
    CosNotification::EventTypeSeq added;
    CosNotification::EventTypeSeq removed = asterixOffer();
    supplierAdmin_->offer_change(added, removed);


    // Init proxy consumer
    CosNotifyChannelAdmin::ProxyConsumer_var proxyConsumer =
      supplierAdmin_->
      obtain_notification_push_consumer(CosNotifyChannelAdmin::STRUCTURED_EVENT,
                                        proxyConsumerId_);
    MIRO_ASSERT(!CORBA::is_nil(proxyConsumer.in()));

    // narrow
    proxyConsumer_ =
      CORBA_dynamic_cast<CosNotifyChannelAdmin::StructuredProxyPushConsumer>(proxyConsumer.in());
    MIRO_ASSERT(!CORBA::is_nil(proxyConsumer.in()));

    MIRO_LOG_CTOR_END("StructuredPushSupplier");
  }

  /**
   * Create a new proxy supplier.
   *
   * @param _ec Reference to the event channel.
   */
  StructuredPushSupplier::StructuredPushSupplier() :
      serverHelper_(NULL),
      ec_(),
      ifgop_(CosNotifyChannelAdmin::OR_OP),
      supplierAdminId_(),
      supplierAdmin_(),
      connectedMutex_(),
      connected_(false),
      offers_(),
      subscription_()
  {
    MIRO_LOG_CTOR("StructuredPushSupplier");

    Client client;
    ec_ = client.resolveName<CosNotifyChannelAdmin::EventChannel>(Miro::RobotParameters::instance()->eventChannelName);

   // Obtain new admin.
    // As domain/type based filtering is admin based, we create one
    // admin per supplier.
    supplierAdmin_ = ec_->new_for_suppliers(ifgop_, supplierAdminId_);

    // remove catch all offer
    CosNotification::EventTypeSeq added;
    CosNotification::EventTypeSeq removed = asterixOffer();
    supplierAdmin_->offer_change(added, removed);


    // Init proxy consumer
    CosNotifyChannelAdmin::ProxyConsumer_var proxyConsumer =
      supplierAdmin_->
      obtain_notification_push_consumer(CosNotifyChannelAdmin::STRUCTURED_EVENT,
                                        proxyConsumerId_);
    MIRO_ASSERT(!CORBA::is_nil(proxyConsumer.in()));

    // narrow
    proxyConsumer_ =
      CORBA_dynamic_cast<CosNotifyChannelAdmin::StructuredProxyPushConsumer>(proxyConsumer.in());
    MIRO_ASSERT(!CORBA::is_nil(proxyConsumer.in()));

    MIRO_LOG_CTOR_END("StructuredPushSupplier");
  }

  StructuredPushSupplier::~StructuredPushSupplier()
  {
    MIRO_LOG_DTOR("StructuredPushSupplier");

    if (serverHelper_ != NULL) {
      MIRO_LOG(LL_NOTICE, "StructuredPushConsumer still connected.");
    }
      
    delete serverHelper_;

    MIRO_LOG_DTOR_END("StructuredPushSupplier");
  }

  void
  StructuredPushSupplier::connect()
  {
    MIRO_DBG(MIRO, LL_PRATTLE, "Connecting StructuredPushSupplier.");

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    if (connected_)
      throw EAlreadyConnected();

    // create the server class
    serverHelper_ = new Server();
    // Activate the consumer with the bidir POA
    serverHelper_->activate(this, false);
    // get the object reference
    CosNotifyComm::StructuredPushSupplier_var objref = this->_this();
    // connect to the proxy consumer.
    proxyConsumer_->connect_structured_push_supplier(objref);

    connected_ = true;
  }

  void
  StructuredPushSupplier::disconnect()
  {
    MIRO_DBG(MIRO, LL_PRATTLE, "Disconnecting StructuredPushSupplier.");

    Server * s = NULL;
    {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);
      if (!connected_) {
        MIRO_LOG(LL_ERROR, "Already disconnected.");
        return;
      }

      try {
        proxyConsumer_->disconnect_structured_push_consumer();
        supplierAdmin_->destroy();

      }
      catch (const CORBA::Exception & e) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "StructuredPushSupplier::disconnect() CORBA exception on:\n"
                      << e);
      }

      // prevent double delete, if server owns supplier
      s = serverHelper_;
      serverHelper_ = NULL;
      connected_ = false;
    }

    // ensure no access to member data, as this might result in deletion of supplier
    delete s;
  }

  /**
   * @param domain Event domain name.
   * @param type Event type name.
   */
  bool
  StructuredPushSupplier::subscribed(std::string const& _domain_name,
                                     std::string const& _type_name) const
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    std::string domain_name(_domain_name);
    if (domain_name.length() == 0)
      domain_name = ClientParameters::instance()->namingContextName;
    for (unsigned int i = 0; i < offers_.length(); ++i) {
      if (_type_name == static_cast<char const *>(offers_[i].type_name) &&
                  domain_name == static_cast<char const *>(offers_[i].domain_name))
        return subscription_[i];
    }
    return false;
  }

  /**
   * The supplier will register the offers at its event supplier admin.
   * The admin will make sure, the notification channel consumers are informed
   * about new offers. That way, consumers can query whether the events it
   * subscribes is currently offered by any supplier.
   *
   * Internally the supplier keeps track of the events subscirbed by consumers.
   * A user of the supplier can query at the supplier for each offered event,
   * whether it is subscribed by any consumer or not. This way an event source
   * can skip the production of any event that no consumer currently is
   * subscribed for.
   *
   * For efficiency, subscription/offer matches can be queried by the index,
   * they apeared within the orgument vector of this method.
   *
   * @param offers The vector of new offers.
   */
  void
  StructuredPushSupplier::setOffers(CosNotification::EventTypeSeq const& _newOffers)
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);

    CORBA::ULong offersLen = offers_.length();
    CORBA::ULong newOffersLen = _newOffers.length();

    // actually added events
    CosNotification::EventTypeSeq added;
    // dummy: allway empty
    CosNotification::EventTypeSeq removed;

    CORBA::ULong addedIndex = 0;
    CORBA::ULong removedIndex = 0;

    // enlarge the offers vector by the maximum required size
    added.length(newOffersLen);
    removed.length(offersLen);

    for (CORBA::ULong i = 0; i < newOffersLen; ++i) {
      CORBA::ULong j;
      for (j = 0; j < offersLen; ++j) {
        // search whether already offered
        if (strcmp(_newOffers[i].type_name, offers_[j].type_name) == 0 &&
                    strcmp(_newOffers[i].domain_name, offers_[j].domain_name) == 0) {
          break;
        }
      }
      // if not, add new offer to list of new offers
      if (j == offersLen) {
        added[addedIndex] = _newOffers[i];
        ++addedIndex;
      }
    }

    for (CORBA::ULong i = 0; i < offersLen; ++i) {
      CORBA::ULong j;
      for (j = 0; j < newOffersLen; ++j) {
        // search whether still offered
        if (strcmp(offers_[i].type_name, _newOffers[j].type_name) == 0 &&
                    strcmp(offers_[i].domain_name, _newOffers[j].domain_name) == 0) {
          break;
        }
      }

      // if not, add old offer to list of removed offers
      if (j == newOffersLen) {
        removed[removedIndex] = offers_[i];
        ++removedIndex;
      }
    }

    // resize offers vector to actual size
    added.length(addedIndex);
    removed.length(removedIndex);

    // overwrite offers vector
    offers_ = _newOffers;

    // do the offers change and performe bookkeeping
    initiateOfferChange(added, removed);
  }

  void
  StructuredPushSupplier::setSingleOffer(std::string const& _type_name,
                                         std::string const& _domain_name)
  {
    std::string domain_name(_domain_name);
    if (domain_name.length() == 0) {
      // an empty subscription is now handled as an asterix subscription...
      if (_type_name.length() == 0) {
        return;
      }
      domain_name = ClientParameters::instance()->namingContextName;
    }

    CosNotification::EventTypeSeq offer;
    if (_type_name.length() != 0) {
      offer.length(1);
      offer[0].domain_name = domain_name.c_str();
      offer[0].type_name = _type_name.c_str();
    }
    setOffers(offer);
  }

  void
  StructuredPushSupplier::subscription_change(CosNotification::EventTypeSeq const& added,
      CosNotification::EventTypeSeq const& removed)
  throw(CosNotifyComm::InvalidEventType)
  {
    MIRO_DBG(MIRO, LL_PRATTLE, "StructuredPushSupplier subscription change\nadded messages:");

    for (unsigned int i = 0; i < added.length(); ++i) {
      for (unsigned int j = 0; j < offers_.length(); ++j) {
        if (strcmp(added[i].type_name, offers_[j].type_name) == 0 &&
                    strcmp(added[i].domain_name, offers_[j].domain_name) == 0) {
          subscription_[j] = true;
          break;
        }
      }
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    "  " << added[i].domain_name << "\t" << "  " << added[i].type_name);
    }

    MIRO_DBG(MIRO, LL_PRATTLE, "removed messages:");
    for (unsigned int i = 0; i < removed.length(); ++i) {
      for (unsigned int j = 0; j < offers_.length(); ++j) {
        if (strcmp(removed[i].type_name, offers_[j].type_name) == 0 &&
                    strcmp(removed[i].domain_name, offers_[j].domain_name) == 0) {
          subscription_[j] = false;
          break;
        }
      }
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    "  " << removed[i].domain_name << "\t" << "  " << removed[i].type_name);
    }
  }

  void
  StructuredPushSupplier::disconnect_structured_push_supplier()
  throw()
  {
    MIRO_DBG(MIRO, LL_PRATTLE, "disconnect supplier.");
    disconnect();
  }

  void
  StructuredPushSupplier::initiateOfferChange(CosNotification::EventTypeSeq const& _added,
      CosNotification::EventTypeSeq const& _removed)
  {
    // reserve space in subscriptions vector
    subscription_.resize(offers_.length(), false);

    MIRO_DBG(MIRO, LL_PRATTLE, "StructuredPushSupplier offer change\nadded messages:");

    for (unsigned int i = 0; i < _added.length(); ++i) {
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    "  " << _added[i].domain_name << "\t" << "  " << _added[i].type_name);
    }

    MIRO_DBG(MIRO, LL_PRATTLE, "removed messages:");
    for (unsigned int i = 0; i < _removed.length(); ++i) {
      MIRO_DBG_OSTR(MIRO,
                    LL_PRATTLE,
                    "  " << _removed[i].domain_name << "\t" << "  " << _removed[i].type_name);
    }
    // inform the supplier admin about the new offers
    proxyConsumer_->offer_change(_added, _removed);

    // generate list of subscribed offers
    CosNotification::EventTypeSeq_var subscritpions =
      proxyConsumer_->obtain_subscription_types(CosNotifyChannelAdmin::ALL_NOW_UPDATES_ON);
    for (unsigned int i = 0; i < offers_.length(); ++i) {
      subscription_[i] = false;
      for (unsigned int j = 0; j < subscritpions->length(); ++j) {
        if (strcmp(offers_[i].type_name, subscritpions[j].type_name) == 0 &&
                    strcmp(offers_[i].domain_name, subscritpions[j].domain_name) == 0) {
          subscription_[i] = true;
          break;
        }
      }
    }
  }

  CosNotification::EventTypeSeq
  StructuredPushSupplier::asterixOffer()
  {
    CosNotification::EventTypeSeq offer;

    offer.length(1);
    offer[0].type_name = CORBA::string_dup("*");
    offer[0].domain_name = CORBA::string_dup("*");

    return offer;
  }

  void
  StructuredPushSupplier::initStructuredEvent(CosNotification::StructuredEvent& _event,
      std::string const& _type_name,
      std::string const& _domain_name)
  {
    std::string domain_name(_domain_name);
    if (domain_name.length() == 0)
      domain_name = ClientParameters::instance()->namingContextName;

    _event.header.fixed_header.event_type.domain_name = domain_name.c_str();
    _event.header.fixed_header.event_type.type_name = _type_name.c_str();
  }
}
