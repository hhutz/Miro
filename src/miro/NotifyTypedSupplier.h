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
#ifndef miro_NotifyTypedSupplier_h
#define miro_NotifyTypedSupplier_h

#include "StructuredPushSupplier.h"
#include "TimeHelper.h"

#include <ace/Time_Value.h>

namespace Miro
{
  //! StructuredPushSupplierr interface implementation.
  /**
   * This class implements the StructuredPushSupplier interface of the
   * CORBA notification service. It performs bookkeeping of offers
   * from suplliers to the events subscribed by this consumer and
   * allowes efficient querying of this information for event
   * consumers.
   */
  template<class PAYLOAD_TYPE>
  class NotifyTypedSupplier : public StructuredPushSupplier
  {
  public:
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------

    typedef PAYLOAD_TYPE Payload_Type;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializing constructor.
    NotifyTypedSupplier(CosNotifyChannelAdmin::EventChannel_ptr ec,
                        std::string const& type_name,
                        std::string const& domain_name = "");
    //! Initializing constructor.
    NotifyTypedSupplier(std::string const& type_name,
                        std::string const& domain_name = "");
    ~NotifyTypedSupplier();

    void sendEvent(Payload_Type const& notification);

    CosNotification::StructuredEvent& getStructuredEvent() throw();
  private:
    CosNotification::StructuredEvent _event;
  };

  template<class E>
  inline
  NotifyTypedSupplier<E>::NotifyTypedSupplier(CosNotifyChannelAdmin::EventChannel_ptr ec,
      std::string const& type_name,
      std::string const& domain_name) :
      StructuredPushSupplier(ec)
  {
    setSingleOffer(type_name, domain_name);
    connect();

    initStructuredEvent(_event, type_name, domain_name);
  }

  template<class E>
  inline
  NotifyTypedSupplier<E>::NotifyTypedSupplier(std::string const& type_name,
      std::string const& domain_name) :
      StructuredPushSupplier()
  {
    setSingleOffer(type_name, domain_name);
    connect();

    initStructuredEvent(_event, type_name, domain_name);
  }

  template<class E>
  inline
  NotifyTypedSupplier<E>::~NotifyTypedSupplier()
  {
    if (connected()) {
      disconnect();
    }
  }

  template<class E>
  inline
  void
  NotifyTypedSupplier<E>::sendEvent(Payload_Type const& payload)
  {
    ACE_Time_Value before = ACE_OS::gettimeofday();
    _event.remainder_of_body <<= payload;
    StructuredPushSupplier::sendEvent(_event);
    ACE_Time_Value after = ACE_OS::gettimeofday();

    MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                  "Shipping event: " <<
                  _event.header.fixed_header.event_type.type_name << " :" <<
                  (after - before) << "s");
  }

  template<class E>
  inline
  CosNotification::StructuredEvent&
  NotifyTypedSupplier<E>::getStructuredEvent() throw()
  {
    return _event;
  }
}
#endif // miro_NotifyTypedSupplier_h
