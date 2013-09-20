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
#ifndef miro_NotifyTypedConsumer_h
#define miro_NotifyTypedConsumer_h

#include "StructuredPushConsumer.h"

namespace Miro
{
  //! StructuredPushConsumerr interface implementation.
  /**
   * This class implements the StructuredPushConsumer interface of the
   * CORBA notification service. It performs bookkeeping of offers
   * from suplliers to the events subscribed by this consumer and
   * allowes efficient querying of this information for event
   * consumers.
   */
  template<class TYPED_EVENT_HANDLER>
  class NotifyTypedConsumer : public StructuredPushConsumer
  {
  public:
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------

    typedef TYPED_EVENT_HANDLER Typed_Event_Handler;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializing constructor.
    NotifyTypedConsumer(Typed_Event_Handler const& event_handler,
                        CosNotifyChannelAdmin::EventChannel_ptr ec,
                        std::string const& type_name,
                        std::string const& domain_name = "",
			CORBA::Long history = -1);
    NotifyTypedConsumer(Typed_Event_Handler const& event_handler,
                        std::string const& type_name,
                        std::string const& domain_name = "",
			CORBA::Long history = -1);
    ~NotifyTypedConsumer();

    Typed_Event_Handler& event_handler() {
      return _handle_event;
    }
    Typed_Event_Handler const& event_handler() const {
      return _handle_event;
    }

  protected:
    //! Callback for the admin to push events to the client.
    virtual void push_structured_event(const CosNotification::StructuredEvent & notification) throw();

  private:
    Typed_Event_Handler _handle_event;
  };

  template<class E>
  inline
  NotifyTypedConsumer<E>::NotifyTypedConsumer(Typed_Event_Handler const& event_handler,
					      CosNotifyChannelAdmin::EventChannel_ptr ec,
					      std::string const& type_name,
					      std::string const& domain_name,
					      CORBA::Long history) :
      StructuredPushConsumer(ec),
      _handle_event(event_handler)
  {
    setSingleSubscription(type_name, domain_name);
    if (history != -1)
      setHistoryQoS(history);
    connect();
  }

  template<class E>
  inline
  NotifyTypedConsumer<E>::NotifyTypedConsumer(Typed_Event_Handler const& event_handler,
					      std::string const& type_name,
					      std::string const& domain_name,
					      CORBA::Long history) :
      StructuredPushConsumer(),
      _handle_event(event_handler)
  {
    setSingleSubscription(type_name, domain_name);
    if (history != -1)
      setHistoryQoS(history);
    connect();
  }

  template<class E>
  inline
  NotifyTypedConsumer<E>::~NotifyTypedConsumer()
  {
    if (connected()) {
      disconnect();
    }
  }

  template<class E>
  inline
  void
  NotifyTypedConsumer<E>::push_structured_event(const CosNotification::StructuredEvent & notification) throw()
  {
    typename Typed_Event_Handler::argument_type t;
    notification.remainder_of_body >>= t;
    _handle_event(t);
  }
}
#endif // miro_NotifyTypedConsumer_h
