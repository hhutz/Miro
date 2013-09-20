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
#ifndef NotifyMulticastTest_h
#define NotifyMulticastTest_h

#include "miro/Server.h"
#include "miro/StructuredPushSupplier.h"
#include "miro/NotifyMulticastAdapter.h"

#include <orbsvcs/CosNotifyChannelAdminS.h>
#include <orbsvcs/CosNotifyCommC.h>

#include <ace/Reactor.h>

class Consumer;

class StatisticHandler;

class NotifyMulticastTest : public Miro::Server
{
  typedef Miro::Server Super;
  typedef NotifyMulticastTest self;

public:
  // Initialization and Termination methods.
  NotifyMulticastTest(int argc, char *argv[], bool colocated);
  // Constructor.

  ~NotifyMulticastTest();
  // Destructor.

  // protected:

  bool colocated_;

  // Notification Service

  /** Channel factory. */
  CosNotifyChannelAdmin::EventChannelFactory_var notifyFactory_;
  /** Channel identifer. */
  CosNotifyChannelAdmin::ChannelID id_;
  /** The one channel that we create using the factory. */
  CosNotifyChannelAdmin::EventChannel_var ec_;
  /** The group operator between admin-proxy's. */
  CosNotifyChannelAdmin::InterFilterGroupOperator ifgop_;
  /** Initial qos specified to the factory when creating the EC. */
  CosNotification::QoSProperties initialQos_;
  /** Initial admin props specified to the factory when creating the EC. */
  CosNotification::AdminProperties initialAdmin_;

  Consumer * consumer;

  Miro::NMC::Adapter *nmcAdapter;
  
  CosNotification::EventTypeSeq ets;

  ACE_Reactor *reactor_;

  StatisticHandler *sh_;
  int shId_;

  ACE_Time_Value shTime_;
};
#endif




