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
#ifndef miro_StructuredPushConsumer_h
#define miro_StructuredPushConsumer_h


#include "Log.h"
#include "Exception.h"
#include "miro_Export.h"

#include <orbsvcs/CosNotifyChannelAdminS.h>
#include <orbsvcs/CosNotifyCommC.h>

#include <ace/Synch.h>

#include <vector>
#include <string>

namespace Miro
{
  // forward declaration
  class Server;

  //! StructuredPushConsumerr interface implementation.
  /**
   * This class implements the StructuredPushConsumer interface of the
   * CORBA notification service. It performs bookkeeping of offers
   * from suplliers to the events subscribed by this consumer and
   * allowes efficient querying of this information for event
   * consumers.
   */
  class miro_Export StructuredPushConsumer : public POA_CosNotifyComm::StructuredPushConsumer
  {
  public:
    MIRO_EXCEPTION_TYPE(EAlreadyConnected);

    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------

    typedef std::vector<unsigned int> IndexVector;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializing constructor.
    StructuredPushConsumer();
    StructuredPushConsumer(CosNotifyChannelAdmin::EventChannel_ptr _ec);

    //! Destructor
    virtual ~StructuredPushConsumer();

    //! Connect to proxy supplier.
    void connect();
    //! Disconnect from proxy supplier.
    void disconnect();
    //! Report the connection status.
    bool connected() const throw();

    //! Subscribe the subscriptions from the notifcation channel.
    /**
     * This is for deferred subscription after
     * @ref setSubscriptions() or addSubscriptons() calls or
     * resubscripiton after @ref unsubscribe() calls.
     */
    void subscribe();
    //! Unsubscribe the subscriptions from the notifcation channel.
    /**
     * Temporarily unsubscribe the set messages.
     */
    void unsubscribe();
    //! Report the subscription status.
    bool subscribed() const;

    //! Test whether an subscribed event is offered.
    bool offered(unsigned int _index) const;
    //! Test whether an event is offered.
    bool offered(std::string const& _type_name, std::string const& _domain_name = "") const;

    //! Set the set of subscriptions from the notification channel.
    void setSubscriptions(CosNotification::EventTypeSeq const& _subscriptions);
    //! Set the set of subscriptions from the notification channel.
    void setSingleSubscription(std::string const& _type_name, std::string const& _domain_name = "");

    //! Helper method to set history QoS
    /** This is a bit over-simplified, but should work for the remaining time we use the
     * Notification service.
     */
    void setHistoryQoS(CORBA::Long historySize = 1, 
		       CORBA::Long order = CosNotification::FifoOrder);

  protected:
    //--------------------------------------------------------------------------
    // protected type definitions
    //--------------------------------------------------------------------------

    typedef std::vector<bool> OfferVector;

    //--------------------------------------------------------------------------
    // protected methods
    //--------------------------------------------------------------------------

    //! @{ inherited IDL interface

    //! Callback for the admin to inform about changes from the supplier side.
    virtual void offer_change(const CosNotification::EventTypeSeq & added,
                              const CosNotification::EventTypeSeq & removed)
    throw(CosNotifyComm::InvalidEventType);

    //! Callback for the admin to push events to the client.
    virtual void push_structured_event(const CosNotification::StructuredEvent & notification)
    throw(CosEventComm::Disconnected);

    //! Callback for the admin to tell the consumer about a disconnect.
    virtual void disconnect_structured_push_consumer()
    throw();

    //! @}

    //! Tell the admin about an subscription change and update the offer vector.
    void initiateSubscriptionChange(CosNotification::EventTypeSeq const& _added,
                                    CosNotification::EventTypeSeq const& _removed);

    //--------------------------------------------------------------------------
    // protected static methods
    //--------------------------------------------------------------------------

    //! Helper method to initialize the default subscription.
    static CosNotification::EventTypeSeq asterixSubscription();

    //--------------------------------------------------------------------------
    // private data
    //--------------------------------------------------------------------------
  private:
    Server * serverHelper_;

  protected:
    //! The channel we connect to.
    CosNotifyChannelAdmin::EventChannel_var ec_;
    //! The group operator between admin-proxy's.
    CosNotifyChannelAdmin::InterFilterGroupOperator ifgop_;
    //! The id returned on creation of the consumer
    CosNotifyChannelAdmin::AdminID consumerAdminId_;
    //! The consumer admin we use.
  public:
    CosNotifyChannelAdmin::ConsumerAdmin_var consumerAdmin_;

  protected:
    //! The proxy that we are connected to.
    CosNotifyChannelAdmin::StructuredProxyPushSupplier_var proxySupplier_;
    //! The proxy_supplier id.
    CosNotifyChannelAdmin::ProxyID proxySupplierId_;

    //! Lock for the connected_ flag.
    mutable ACE_Recursive_Thread_Mutex connectedMutex_;

  private:
    bool connected_;

  protected:
    //! If true, the subscribtions are actually registered at the event channel.
    bool subscribed_;

    //! Vector of subscribed event types
    CosNotification::EventTypeSeq subscriptions_;
    //! Vector matching subscriptions to offered events.
    OfferVector offers_;
  };

  inline
  bool
  StructuredPushConsumer::connected() const throw()
  {
    return connected_;
  }
  inline
  bool
  StructuredPushConsumer::subscribed() const
  {
    return subscribed_;
  }
  /**
   * @param index The index of the event in the subscription vector.
   * This index is returned as a vector from
   * addSubscripitons. Subscriptions specified as constructor the
   * argument subscription are indexed in ascending order, starting
   * with 0.
   */
  inline
  bool
  StructuredPushConsumer::offered(unsigned int _index) const
  {
    MIRO_ASSERT(_index < offers_.size());
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(connectedMutex_);
    return offers_[_index];
  }
}
#endif // miro_StructuredPushConsumer_h
