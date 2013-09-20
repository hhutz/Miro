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
#ifndef LogFile_h
#define LogFile_h

#include "miro/Log.h"
#include "miro/LogReader.h"

#include <orbsvcs/CosNotifyChannelAdminC.h>

#include <ace/Mem_Map.h>
#include <ace/Time_Value.h>

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <utility>
#include <cstring>

// forward declaration
class TAO_InputCDR ;
class ChannelManager;
namespace Miro
{
  class StructuredPushSupplier;
};

class LogFile : public QObject
{
  Q_OBJECT

private:
  typedef QObject Super;

protected:
  typedef std::pair< ACE_Time_Value, char const * > TimePair;
  typedef std::vector< TimePair > TimeVector;
  typedef std::vector< QString > QStringVector;
  typedef std::pair< char const *, Miro::StructuredPushSupplier *> SupplierPair;

  struct LtStr : public std::binary_function<char const *, char const *, bool> {
    bool operator()(char const * s1, char const * s2) const {
      return strcmp(s1, s2) < 0;
    }
  };

  struct LtStrFirst : public std::binary_function<SupplierPair const&, SupplierPair const&, bool> {
    bool operator()(SupplierPair const & s1, SupplierPair const & s2) const {
      return strcmp(s1.first, s2.first) < 0;
    }
  };

  struct TVLess : public std::binary_function<TimePair const&, TimePair const &, bool> {
    bool operator() (TimePair const & _lhs, TimePair const & _rhs) {
      return _lhs.first < _rhs.first;
    }
  };

public:
  typedef std::set< char const *, LtStr > CStringSet;
  typedef std::map< char const *, CStringSet, LtStr > CStringMap;
  typedef std::vector< SupplierPair > SupplierVector;

public:
  LogFile(QString const& _name,
          ChannelManager * _channelManager);
  ~LogFile();

  QString const& name() const;
  ACE_Time_Value const& startTime() const;
  ACE_Time_Value const& endTime() const;

  ACE_Time_Value const& coursorTime() const;
  void coursorTime(ACE_Time_Value const& _t);

  void sendEvent();
  bool nextEvent();
  bool prevEvent();
  bool getCurrentEvent();
  CosNotification::StructuredEvent const& currentEvent() const;

  unsigned int parse();
  bool parsed() const;
  void parseEvent();

  void clearExclude();
  void addExclude(QString const& _domainName, QString const& _typeName);
  void delExclude(QString const& _domainName, QString const& _typeName);

  CStringMap const& eventTypes() const;

  void setTimeOffset(ACE_Time_Value const& _offset);
  bool assertBefore(ACE_Time_Value const& _t);
  bool assertAfter(ACE_Time_Value const& _t);


signals:
  void notifyEvent(QString const&);
  void newEvent(QString const&,QString const&,QString const&,QString const&);

protected:
  bool validEvent();

  QString name_;
  ChannelManager * const channelManager_;

  CosNotification::StructuredEvent event_;
  QString stamp_;
  QString domainName_;
  QString typeName_;
  QString eventName_;

  ACE_Time_Value timeOffset_;
  TimeVector timeVector_;
  TimeVector::const_iterator coursor_;

  CStringMap eventTypes_;
  char const * currentDomainName_;
  CStringMap::iterator currentDomainEvents_;

  SupplierVector suppliers_;

  CStringMap exclude_;

  Miro::LogReader logReader_;
  unsigned int counter_;
  bool parsed_;
};

inline
QString const&
LogFile::name() const
{
  return name_;
}

inline
ACE_Time_Value const&
LogFile::startTime() const
{
  assert(timeVector_.size() != 0);
  return timeVector_.front().first;
}

inline
ACE_Time_Value const&
LogFile::endTime() const
{
  assert(timeVector_.size() != 0);
  return timeVector_.back().first;
}

inline
ACE_Time_Value const&
LogFile::coursorTime() const
{
  return (coursor_ != timeVector_.end())? coursor_->first : ACE_Time_Value::max_time;
}

inline
void
LogFile::coursorTime(ACE_Time_Value const& _t)
{
  coursor_ = std::lower_bound(timeVector_.begin(), timeVector_.end(), TimePair(_t, (const char*)NULL), TVLess());
}

inline
bool
LogFile::parsed() const
{
  return parsed_;
}

inline
LogFile::CStringMap const&
LogFile::eventTypes() const
{
  return eventTypes_;
}

inline
void
LogFile::setTimeOffset(ACE_Time_Value const& _offset)
{
  timeOffset_ = _offset;
}

inline
CosNotification::StructuredEvent const&
LogFile::currentEvent() const
{
  return event_;
}
#endif
