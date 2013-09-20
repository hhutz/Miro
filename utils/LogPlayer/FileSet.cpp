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
#include "FileSet.h"
#include "ChannelManager.h"

#include "miro/TimeHelper.h"
#include "miro/Log.h"
#include "miro/LogWriter.h"

#define QT_NO_TEXTSTREAM
#include <q3tl.h>

namespace
{
  struct LFLess : public std::binary_function<LogFile const *, LogFile const *, bool> {
    bool operator() (LogFile const * _lhs, LogFile const * _rhs) {
      return _lhs->coursorTime() < _rhs->coursorTime();
    }
  };


  struct LFMore : public std::binary_function<LogFile const *, LogFile const *, bool> {
    bool operator() (LogFile const * _lhs, LogFile const * _rhs) {
      return _lhs->coursorTime() > _rhs->coursorTime();
    }
  };
}

FileSet::FileSet(ChannelManager * _channelManager) :
    channelManager_(_channelManager),
    startCut_(ACE_Time_Value::zero),
    endCut_(ACE_Time_Value::zero),
    forward_(true)
{}

FileSet::~FileSet()
{
  FileVector::iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first)
    delete *first;

}

LogFile *
FileSet::addFile(QString const& _name)
{
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first)
    if ((*first)->name() == _name)
      break;
  MIRO_ASSERT(first == file_.end());

  LogFile * const logFile = new LogFile(_name, channelManager_);
  file_.push_back(logFile);

  return logFile;
}

void
FileSet::delFile(QString const& _name)
{
  // remember coursor time
  ACE_Time_Value t;
  bool parsed = file_.front()->parsed();
  if (parsed)
    t = coursorTime();

  FileVector::iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first)
    if ((*first)->name() == _name)
      break;
  MIRO_ASSERT(first != file_.end());

  delete *first;
  file_.erase(first);

  if (parsed && file_.size() != 0) {
    calcStartEndTime();
    coursorTime(t);
    forward_ = true;
    std::make_heap(file_.begin(), file_.end(), LFMore());
  }
}

void
FileSet::calcStartEndTime()
{
  if (file_.size() != 0) {
    FileVector::const_iterator first, last = file_.end();
    first = file_.begin();
    startTime_ = (*first)->startTime();
    endTime_ = (*first)->endTime();

    for (++first; first != last; ++first ) {
      if ((*first)->startTime() < startTime_) {
        startTime_ = (*first)->startTime();
      }
      if ((*first)->endTime() > endTime_) {
        endTime_ = (*first)->endTime();
      }
    }
    for (first = file_.begin(); first != last; ++first ) {
      (*first)->setTimeOffset(startTime_);
    }
  }

  startCut_ = startTime_;
  endCut_ = endTime_;

  emit intervalChange();
}

void
FileSet::coursorTime(ACE_Time_Value const& _time)
{
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first) {
    (*first)->coursorTime(_time);
  }

  std::make_heap(file_.begin(), file_.end(), LFMore());
  forward_ = true;

  //  std::cout << "new time heap: " << std::endl;
  //  for (first = file_.begin(); first != last; ++first) {
  //    std::cout << (*first)->coursorTime()  - startTime_ << std::endl;
  //  }

  file_.front()->getCurrentEvent();

  emit coursorChange();
}

void
FileSet::coursorTimeRel(ACE_Time_Value const& _time)
{
  ACE_Time_Value t = startTime_ + _time;
  coursorTime(t);
}

void
FileSet::playEvents(ACE_Time_Value const& _time)
{
  MIRO_ASSERT(file_.size() != 0);

  if (coursorTime() > endCut_)
    return;

  // correct heap if we change direction
  if (!forward_) {
    FileVector::const_iterator first, last = file_.end();
    first = file_.begin();
    for (++first; first != last; ++first)
      (*first)->assertAfter(file_.front()->coursorTime());
    std::make_heap(file_.begin(), file_.end(), LFMore());
    forward_ = true;
  }

  while (file_.front()->coursorTime() <= _time) {
    std::pop_heap(file_.begin(), file_.end(), LFMore());
    file_.back()->sendEvent();
    file_.back()->nextEvent();
    std::push_heap(file_.begin(), file_.end(), LFMore());

    // restore heap attibute
    //    std::cout << "new time heap: " << std::endl;
    //    FileVector::const_iterator first, last = file_.end();
    //    for (first = file_.begin(); first != last; ++first) {
    //      std::cout << (*first)->coursorTime() - startTime_ << std::endl;
    //    }

    file_.front()->parseEvent();
  }

  emit coursorChange();
}

void
FileSet::playBackwards()
{
  MIRO_ASSERT(file_.size() != 0);
  if (coursorTime() <= startCut_)
    return;

  // correct heap if we change direction
  if (forward_) {
    FileVector::const_iterator first, last = file_.end();
    first = file_.begin();
    for (++first; first != last; ++first)
      (*first)->assertBefore(file_.front()->coursorTime());

    std::make_heap(file_.begin(), file_.end(), LFLess());
    forward_ = false;
  }

  std::pop_heap(file_.begin(), file_.end(), LFLess());
  file_.back()->sendEvent();
  file_.back()->prevEvent();
  std::push_heap(file_.begin(), file_.end(), LFLess());

//   std::cout << "new time heap: " << std::endl;
//   FileVector::const_iterator first, last = file_.end();
//   for (first = file_.begin(); first != last; ++first) {
//     std::cout << (*first)->coursorTime()  - startTime_ << std::endl;
//   }

  file_.front()->parseEvent();

  emit coursorChange();
}

void
FileSet::getEvents(ACE_Time_Value const& _time, unsigned int _num)
{
  // prepare log file set
  ACE_Time_Value now = coursorTime();

  // set new time
  coursorTime(_time);

  while (file_.front()->coursorTime() <= endCut_ && --_num > 0) {
    std::pop_heap(file_.begin(), file_.end(), LFMore());
    file_.back()->nextEvent();
    std::push_heap(file_.begin(), file_.end(), LFMore());
    file_.front()->parseEvent();
  }

  // restor coursor time
  coursorTime(now);
}

void
FileSet::saveCut(QString const& _fileName)
{
  // create the new log file
  Miro::LogWriter writer(std::string(_fileName.latin1()));

  // save the current coursor position
  ACE_Time_Value now = coursorTime();

  coursorTime(cutStartTime());

  while (file_.front()->coursorTime() <= endCut_) {
    writer.logEvent(file_.front()->coursorTime(), file_.front()->currentEvent());
    std::pop_heap(file_.begin(), file_.end(), LFMore());
    file_.back()->nextEvent();
    std::push_heap(file_.begin(), file_.end(), LFMore());
  }

  // restore coursor position
  coursorTime(now);
}

QStringList
FileSet::files() const
{
  QStringList l;
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first)
    l.append((*first)->name());
  qSort(l);

  return l;
}

FileSet::DNETMap
FileSet::typeNames() const
{
  DNETMap tree;
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first) {

    DNETMap const& fileTree((*first)->eventTypes());
    DNETMap::const_iterator f2, l2 = fileTree.end();
    for (f2 = fileTree.begin(); f2 != l2; ++f2) {
      DNETMap::iterator iter = tree.find(f2->first);
      if (iter == tree.end()) {
        tree.insert(*f2);
      }
      else {
        iter->second.insert(f2->second.begin(), f2->second.end());
      }
    }
  }

  return tree;
}

void
FileSet::clearExclude()
{
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first) {
    (*first)->clearExclude();
  }
}

void
FileSet::addExclude(QString const& _domainName, QString const& _typeName)
{
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first) {
    (*first)->addExclude(_domainName, _typeName);
  }
}

void
FileSet::delExclude(QString const& _domainName, QString const& _typeName)
{
  FileVector::const_iterator first, last = file_.end();
  for (first = file_.begin(); first != last; ++first) {
    (*first)->delExclude(_domainName, _typeName);
  }
}

