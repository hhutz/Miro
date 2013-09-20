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
#include "ChannelManager.h"

#include "miro/ServerWorker.h"
#include "miro/Log.h"

#include <orbsvcs/Notify/Notify_EventChannelFactory_i.h>

using std::cerr;
using std::endl;

ChannelManager::ChannelManager(int& argc, char * argv[],
                               bool _shared, bool _unified,
                               QString const& _channelName) :
    Super(argc, argv),
    shared_(_shared),
    unified_(_unified),
    channelName_(_channelName)
{
  if (!shared_) {
    notifyFactory_ = resolveName<CosNotifyChannelAdmin::EventChannelFactory>(qPrintable(_channelName+"Factory"));
  }
  if (unified_) {

    if (!shared_) {
      // Channel identifer.
      CosNotifyChannelAdmin::ChannelID id;
      // Initial qos specified to the factory when creating the EC.
      CosNotification::QoSProperties initialQos;
      // Initial admin props specified to the factory when creating the EC.
      CosNotification::AdminProperties initialAdmin;

      unifiedChannel_ = notifyFactory_->create_channel(initialQos, initialAdmin, id);
    }
    else {
      unifiedChannel_ =
        this->resolveName<CosNotifyChannelAdmin::EventChannel>(qPrintable(_channelName));
    }
  }
}

ChannelManager::~ChannelManager()
{
  MIRO_LOG_DTOR("ChannelManager");

}

CosNotifyChannelAdmin::EventChannel_ptr
ChannelManager::getEventChannel(QString const& _domainName)
{
  ChannelMap::iterator channel = channel_.find(_domainName);

  if (channel == channel_.end()) {

    // The channel reference we need to get - somehow
    CosNotifyChannelAdmin::EventChannel_ptr ec;
    QString fullName = "/" + _domainName + "/" + channelName_;

    if (shared_) {
      if (unifiedChannel_) {
        ec = unifiedChannel_.in();
      }
      else {
        ec = resolveName<CosNotifyChannelAdmin::EventChannel>(qPrintable(fullName));
      }
    }
    else {
      // Channel identifer.
      CosNotifyChannelAdmin::ChannelID id;
      // Initial qos specified to the factory when creating the EC.
      CosNotification::QoSProperties initialQos;
      // Initial admin props specified to the factory when creating the EC.
      CosNotification::AdminProperties initialAdmin;

      if (!unified_) {
        ec = notifyFactory_->create_channel(initialQos, initialAdmin, id);
        addToNameService(ec, qPrintable(fullName));
      }
      else {
        ec = unifiedChannel_.in();
      }
    }
    channel = channel_.insert(std::make_pair(_domainName,
                              CosNotifyChannelAdmin::EventChannel::_duplicate(ec))).first;
  }

  return channel->second;
}
