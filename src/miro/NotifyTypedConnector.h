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
#ifndef miro_NotifyTypedConnector_h
#define miro_NotifyTypedConnector_h

#include "StructuredPushConsumer.h"

namespace Miro
{
  //! StructuredPushConsumer interface implementation.
  /**
   * This class implements the StructuredPushConnector interface of the
   * CORBA notification service. It performs bookkeeping of offers
   * from suplliers to the events subscribed by this consumer and
   * allowes efficient querying of this information for event
   * consumers.
   */
  template<typename TYPE, typename TARGET_HANDLER>
  class NotifyTypedConnector : public StructuredPushConsumer
  {
  public:
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------

    typedef TARGET_HANDLER TargetHandler;
    typedef TYPE Type;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializing constructor.
    NotifyTypedConnector(TargetHandler * eventHandler,
                         CosNotifyChannelAdmin::EventChannel_ptr ec,
                         std::string const& typeName,
                         std::string const& domainName = "",
			 CORBA::Long history = -1);
    NotifyTypedConnector(TargetHandler * eventHandler,
                         std::string const& typeName,
                         std::string const& domainName = "",
			 CORBA::Long history = -1);
    ~NotifyTypedConnector();

    TargetHandler * eventHandler() throw() {
      return _handleEvent;
    }
    TargetHandler const * eventHandler() const throw() {
      return _handleEvent;
    }

  protected:
    //! Callback for the admin to push events to the client.
    virtual void push_structured_event(const CosNotification::StructuredEvent & notification) throw();

  private:
    TargetHandler * _handleEvent;
  };

  template<typename T, typename H>
  inline
  NotifyTypedConnector<T, H>::NotifyTypedConnector(TargetHandler * eventHandler,
						   CosNotifyChannelAdmin::EventChannel_ptr ec,
						   std::string const& typeName,
						   std::string const& domainName,
						   CORBA::Long history) :
      StructuredPushConsumer(ec),
      _handleEvent(eventHandler)
  {
    setSingleSubscription(typeName, domainName);
    if (history != -1)
      setHistoryQoS(history);
    connect();
  }

  template<typename T, typename H>
  inline
  NotifyTypedConnector<T, H>::NotifyTypedConnector(TargetHandler * eventHandler,
						   std::string const& typeName,
						   std::string const& domainName,
						   CORBA::Long history) :
      StructuredPushConsumer(),
      _handleEvent(eventHandler)
  {
    setSingleSubscription(typeName, domainName);
    if (history != -1)
      setHistoryQoS(history);
    connect();
  }

  template<typename T, typename H>
  inline
  NotifyTypedConnector<T, H>::~NotifyTypedConnector()
  {
    if (connected()) {
      disconnect();
    }
  }

  template<typename T, typename H>
  inline
  void
  NotifyTypedConnector<T, H>::push_structured_event(const CosNotification::StructuredEvent & notification) throw()
  {
    Type t;
    notification.remainder_of_body >>= t;
    (*_handleEvent)(t);
  }
}
#endif // miro_NotifyTypedConnector_h
