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
#ifndef miro_SturcturedPushSupplier_h
#define miro_SturcturedPushSupplier_h

#include "Log.h"
#include "Exception.h"
#include "TimeHelper.h"
#include "miro_Export.h"

#include <orbsvcs/CosNotifyChannelAdminS.h>
#include <orbsvcs/CosNotifyCommC.h>

#include <ace/Synch.h>
#include <ace/OS_NS_sys_time.h>

#include <string>
#include <vector>

namespace Miro
{
  // forward declaration
  class Server;

  //! StructuredPushSupplier interface implementation.
  /**
   * This class implements the StructuredPushSupplier interface of the
   * CORBA notification service. It performs bookkeeping of
   * subscriptions from consumers to the events offered by this
   * supplier and allowes efficient querying of this information for
   * event producers.
   */
  class miro_Export StructuredPushSupplier : public POA_CosNotifyComm::StructuredPushSupplier
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
    StructuredPushSupplier(CosNotifyChannelAdmin::EventChannel_ptr _ec);
    //! Initializing constructor.
    StructuredPushSupplier();

    //! Destructor
    virtual ~StructuredPushSupplier();

    //! Connect to proxy consumer.
    void connect();
    //! Disconnect supplier to proxy consumer.
    void disconnect();
    //! Report the connection status.
    bool connected() const throw();

    //! Test whether an offered event is subscribed.
    bool subscribed(unsigned int _index) const;
    //! Test whether an offered event is subscribed.
    bool subscribed(std::string const& _domain, std::string const& _type = "") const;

    //! Set the set of offers to the notification channel.
    void setOffers(CosNotification::EventTypeSeq const& _offers);
    //! Set the set of offers to the notification channel.
    void setSingleOffer(std::string const& _type_name, std::string const& _domain_name = "");

    //! Send one event.
    void sendEvent(const CosNotification::StructuredEvent& event);

    //--------------------------------------------------------------------------
    // public static methods
    //--------------------------------------------------------------------------
    static void initStructuredEvent(CosNotification::StructuredEvent& _event,
                                    std::string const& _domainName,
                                    std::string const& _typeName = "");
  protected:
    //--------------------------------------------------------------------------
    // protected types
    //--------------------------------------------------------------------------

    typedef std::vector<bool> SubscriptionVector;

    //--------------------------------------------------------------------------
    // protected methods
    //--------------------------------------------------------------------------

    //! @{ inherited IDL interfae

    //! CosNotifyComm::StructuredPushSupplier interface method implementation.
    virtual void subscription_change(const CosNotification::EventTypeSeq & added,
                                     const CosNotification::EventTypeSeq & removed)
    throw(CosNotifyComm::InvalidEventType);

    //! CosNotifyComm::StructuredPushSupplier interface method implementation.
    virtual void disconnect_structured_push_supplier()
    throw();

    //! @}

    //! Tell the admin about an offer change and update the subscription vector.
    void initiateOfferChange(CosNotification::EventTypeSeq const& _added,
                             CosNotification::EventTypeSeq const& _removed);

    //--------------------------------------------------------------------------
    // protected static methods
    //--------------------------------------------------------------------------

    //! Helper method to initialize the default offer.
    static CosNotification::EventTypeSeq asterixOffer();

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
    //! The suppllier admin id returned on supplier creation.
    CosNotifyChannelAdmin::AdminID supplierAdminId_;
    //! The supplier admin used.
    CosNotifyChannelAdmin::SupplierAdmin_var supplierAdmin_;

    //! The proxy that we are connected to.
    CosNotifyChannelAdmin::StructuredProxyPushConsumer_var proxyConsumer_;
    //! This supplier's id.
    CosNotifyChannelAdmin::ProxyID proxyConsumerId_;


    //! Lock for the connected_ flag.
    mutable ACE_Recursive_Thread_Mutex connectedMutex_;

  private:
    bool connected_;

    //! Vector of offered event types
    CosNotification::EventTypeSeq offers_;
    //! Vector matching subscriptions to offered events.
    SubscriptionVector subscription_;
  };

  inline
  void
  StructuredPushSupplier::sendEvent(const CosNotification::StructuredEvent& event)
  {
    if (serverHelper_) {
      ACE_Time_Value before = ACE_OS::gettimeofday();
      proxyConsumer_->push_structured_event(event);
      ACE_Time_Value after = ACE_OS::gettimeofday();
      ACE_Time_Value elapsed = after - before;

      if (elapsed > ACE_Time_Value(0, 400000)) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "StructuredPushSupplier: needed " << elapsed << "s to send a "
                      << event.header.fixed_header.event_type.type_name);
      }
    }
  }


  inline
  bool
  StructuredPushSupplier::connected() const throw()
  {
    return connected_;
  }

  /**
   * @param index The index of the event in the offer vector.  This
   * index is returned as a vector from addOffers. Offers specified as
   * constructor the argument offer are indexed in ascending order,
   * starting with 0.
   */
  inline
  bool
  StructuredPushSupplier::subscribed(unsigned int _index) const
  {
    MIRO_ASSERT(_index < subscription_.size());
    return subscription_[_index];
  }
}
#endif
