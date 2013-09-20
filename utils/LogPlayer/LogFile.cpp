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
#include "LogFile.h"
#include "ChannelManager.h"
#include "EventView.h"

#include "miro/StructuredPushSupplier.h"
#include "miro/TimeHelper.h"

#include <cstring>
#include <iostream>

LogFile::LogFile(QString const& _name,
                 ChannelManager * _channelManager) :
    name_(_name),
    channelManager_(_channelManager),
    coursor_(timeVector_.end()),
    currentDomainName_(""),
    logReader_(name_.latin1()),
    counter_(0),
    parsed_(false)
{
}

LogFile::~LogFile()
{

  {
    // free strings
    CStringMap::iterator first, last = eventTypes_.end();
    for (first = eventTypes_.begin(); first != last; ++first) {
      CStringSet::iterator f2, l2 = first->second.end();
      for (f2 = first->second.begin(); f2 != l2; ++f2) {
        delete[] (*f2);
      }
    }
  }

  {
    SupplierVector::const_iterator first, last = suppliers_.end();
    for (first = suppliers_.begin(); first != last; ++first) {
      first->second->disconnect();
      delete first->second;
    }
  }
}

unsigned int
LogFile::parse()
{
  unsigned int rc = 0;
  ACE_Time_Value timeStamp;
  CosNotification::FixedEventHeader header;

  bool notEof = true;
  while (( logReader_.version() >= 3 &&
              ++counter_ <= logReader_.events() &&
              ( notEof = logReader_.parseTimeStamp(timeStamp) ) ) ||
              ( logReader_.version() < 3 &&
                ( notEof = logReader_.parseTimeStamp(timeStamp) ) ) ) {

    timeVector_.push_back(std::make_pair(timeStamp, logReader_.rdPtr()));

    logReader_.parseEventHeader(header);

    // get domain name slot
    if (strcmp(currentDomainName_,
                (char const *)header.event_type.domain_name) != 0) {
      currentDomainEvents_ =
        eventTypes_.find((char const *)header.event_type.domain_name);
      if (currentDomainEvents_ == eventTypes_.end()) {
        currentDomainEvents_ =
          eventTypes_.insert(std::make_pair((char const *)
                                            CORBA::string_dup(header.event_type.domain_name),
                                            CStringSet())).first;
      }
      currentDomainName_ = currentDomainEvents_->first;
    }
    if (currentDomainEvents_->second.find(header.event_type.type_name) ==
                currentDomainEvents_->second.end())

      currentDomainEvents_->second.insert(CORBA::string_dup(header.event_type.type_name));

    // skip event
    logReader_.skipEventBody();


    // break parsing up to advance status bar
    if (!(timeVector_.size() % 2048))
      break;
  }

  if (timeVector_.size() == 0)
    throw Miro::Exception("Logfile contains no data.");

  // EOF
  if (!notEof ||
              ( logReader_.version() >= 3 && counter_ >= logReader_.events()) ) {
    coursor_ = timeVector_.begin();

    suppliers_.reserve(eventTypes_.size());
    CStringMap::const_iterator first, last = eventTypes_.end();
    for (first = eventTypes_.begin(); first != last; ++first) {
      CosNotifyChannelAdmin::EventChannel_ptr ec =
        channelManager_->getEventChannel(first->first);
      Miro::StructuredPushSupplier * supplier =
        new Miro::StructuredPushSupplier(ec);
      suppliers_.push_back(std::make_pair(first->first, supplier));

      CosNotification::EventTypeSeq offers;
      offers.length(first->second.size());

      CStringSet::const_iterator typeName = first->second.begin();
      for (unsigned int i = 0; i < offers.length(); ++i, ++ typeName) {
        offers[i].domain_name = CORBA::string_dup(first->first);
        offers[i].type_name = CORBA::string_dup(*typeName);
      }
      supplier->setOffers(offers);
      supplier->connect();
    }

    rc = 100;
    parsed_ = true;
  }
  // End Of Work Packet
  else
    rc = logReader_.progress();
  return rc;
}

void
LogFile::sendEvent()
{
  MIRO_ASSERT(coursor_ != timeVector_.end());

  emit notifyEvent(domainName_ + " - " + typeName_);

  if (suppliers_.size() == 1) {
    suppliers_.front().second->sendEvent(event_);
  }
  else {
    SupplierVector::const_iterator supplier =
      std::lower_bound(suppliers_.begin(), suppliers_.end(),
                       std::make_pair((char const *)event_.header.fixed_header.event_type.domain_name,
                                      (Miro::StructuredPushSupplier *) NULL),
                       LtStrFirst());
    MIRO_ASSERT(supplier != suppliers_.end());
    supplier->second->sendEvent(event_);
  }
}

bool
LogFile::nextEvent()
{
  MIRO_ASSERT (coursor_ != timeVector_.end());

  do {
    ++coursor_;
    if (coursor_ == timeVector_.end()) {
      return false;
    }

    logReader_.rdPtr(coursor_->second);
    logReader_.parseEventHeader(event_.header.fixed_header);
  }
  while (!validEvent());
  return true;
}

bool
LogFile::getCurrentEvent()
{
  if (coursor_ == timeVector_.end())
    return false;

  logReader_.rdPtr(coursor_->second);
  logReader_.parseEventHeader(event_.header.fixed_header);
  while (!validEvent()) {
    ++coursor_;
    if (coursor_ == timeVector_.end()) {
      return false;
    }

    logReader_.rdPtr(coursor_->second);
    logReader_.parseEventHeader(event_.header.fixed_header);
  }

  parseEvent();

  return true;
}

bool
LogFile::prevEvent()
{
  do {
    if (coursor_ == timeVector_.begin())
      return false;
    --coursor_;

    logReader_.rdPtr(coursor_->second);
    logReader_.parseEventHeader(event_.header.fixed_header);
  }
  while (!validEvent());

  return true;
}

bool
LogFile::assertBefore(ACE_Time_Value const& _t)
{
  bool valid = true;
  while (valid && coursor_->first >= _t) {
    valid = prevEvent();
  }
  return valid;
}

bool
LogFile::assertAfter(ACE_Time_Value const& _t)
{
  bool valid = true;
  while (valid && coursor_->first <= _t) {
    valid = nextEvent();
  }
  return valid;
}

void
LogFile::clearExclude()
{
  exclude_.clear();
}

void
LogFile::addExclude(QString const& _domainName, QString const& _typeName)
{
  CStringMap::const_iterator domain = eventTypes_.find(_domainName.latin1());
  if (domain == eventTypes_.end())
    return;

  CStringSet::const_iterator type = domain->second.find(_typeName.latin1());
  if (type == domain->second.end())
    return;

  CStringMap::iterator eDomain = exclude_.find(domain->first);
  if (eDomain == exclude_.end()) {
    eDomain = exclude_.insert(std::make_pair(domain->first, CStringSet())).first;
  }
  eDomain->second.insert(*type);
}

void
LogFile::delExclude(QString const& _domainName, QString const& _typeName)
{
  CStringMap::iterator domain = exclude_.find(_domainName.latin1());
  if (domain == exclude_.end())
    return;

  CStringSet::iterator type = domain->second.find(_typeName.latin1());
  if (type == domain->second.end())
    return;

  domain->second.erase(type);
  if (domain->second.size() == 0)
    exclude_.erase(domain);
}

bool
LogFile::validEvent()
{
  // skip excluded events
  CStringMap::const_iterator eDomain =
    exclude_.find(event_.header.fixed_header.event_type.domain_name);
  if (eDomain != exclude_.end()) {
    if (eDomain->second.find(event_.header.fixed_header.event_type.type_name) !=
                eDomain->second.end()) {
      logReader_.skipEventBody();
      return false;
    }
  }
  logReader_.parseEventBody(event_);
  return true;
}

void
LogFile::parseEvent()
{
  if (coursor_ == timeVector_.end())
    return;

  // localize debug hack
  //  if (channelManager_->debugLocalize() && typeName_ == "LineSamples")
  //   event_.header.fixed_header.event_type.type_name = CORBA::string_dup( "RawLineSamples" );

  domainName_ = (char const *)event_.header.fixed_header.event_type.domain_name;
  typeName_ = (char const *)event_.header.fixed_header.event_type.type_name;
  eventName_ = (char const *)event_.header.fixed_header.event_name;

  ACE_Time_Value t = coursorTime() - timeOffset_;
  char const * null = "000000";
  QString sec;
  QString usec;
  sec.setNum(t.sec());
  usec.setNum(t.usec());
  stamp_ = (null + sec.length()) + sec + "." + (null + usec.length()) + usec;

  emit newEvent(stamp_, domainName_, typeName_, eventName_);
}
