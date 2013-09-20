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
#ifndef FileManager_h
#define FileManager_h

#include "LogFile.h"
#include "miro/Log.h"

#include <qstring.h>
#include <qstringlist.h>

#include <vector>
#include <map>

// forward declarations
class ChannelManager;

class FileSet : public QObject
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------
  typedef QObject Super;

public:
  //----------------------------------------------------------------------------
  // public types
  //----------------------------------------------------------------------------
  typedef std::vector<LogFile *> FileVector;
  typedef LogFile::CStringMap DNETMap; // domain_name type_came map

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
  FileSet(ChannelManager * _channelManager);
  virtual ~FileSet();

  LogFile * addFile(QString const& _name);
  void delFile(QString const& _name);

  unsigned int size() const;
  QStringList files() const;

  void calcStartEndTime();

  ACE_Time_Value const& cutStartTime() const;
  ACE_Time_Value const& cutEndTime() const;

  void cutStart();
  void cutEnd();
  void cutUndo();


  //! Save the current cut to one file.
  void saveCut(QString const& _fileName);

  //! Play events till specified time
  void playEvents(ACE_Time_Value const& _t);
  void playBackwards();

  //! Get @ref _num events from specified time.
  void getEvents(ACE_Time_Value const& _t, unsigned int _num);

  DNETMap typeNames() const;
  void clearExclude();

  FileVector& fileVector() {
    return file_;
  }

  //! Current coursor event time.
  ACE_Time_Value const& coursorTime() const;
  void coursorTime(ACE_Time_Value const& _t);
  void coursorTimeRel(ACE_Time_Value const& _t);

  ChannelManager const * channelManager() const {
    return channelManager_;
  }

  //----------------------------------------------------------------------------
  // public slots
  //----------------------------------------------------------------------------
public slots:
  void addExclude(QString const& _domainName, QString const& _typeName);
  void delExclude(QString const& _domainName, QString const& _typeName);

  //----------------------------------------------------------------------------
  // signals
  //----------------------------------------------------------------------------
signals:
  void intervalChange();
  void coursorChange();

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  ACE_Time_Value const& startTime() const;
  ACE_Time_Value const& endTime() const;

  //----------------------------------------------------------------------------
  // private data
  //----------------------------------------------------------------------------
private:
  //! Reference to the channel manager.
  ChannelManager * const channelManager_;
  //! The set of log files to work on.
  FileVector file_;

  //! The base time of all log files.
  ACE_Time_Value startTime_;
  //! The ent time of all log files.
  ACE_Time_Value endTime_;

  //! The start of the log files after cutting.
  ACE_Time_Value startCut_;
  //! The end of the log files after cutting.
  ACE_Time_Value endCut_;

  //! Flag indicating the direction we play the events.
  bool forward_;
};

inline
unsigned int
FileSet::size() const
{
  return file_.size();
}

inline
ACE_Time_Value const&
FileSet::startTime() const
{
  MIRO_ASSERT(file_.size() != 0);
  return startTime_;
}

inline
ACE_Time_Value const&
FileSet::endTime() const
{
  MIRO_ASSERT(file_.size() != 0);
  return endTime_;
}

inline
ACE_Time_Value const&
FileSet::cutStartTime() const
{
  MIRO_ASSERT(file_.size() != 0);
  return startCut_;
}

inline
ACE_Time_Value const&
FileSet::cutEndTime() const
{
  MIRO_ASSERT(file_.size() != 0);
  return endCut_;
}

inline
void
FileSet::cutStart()
{
  MIRO_ASSERT(file_.size() != 0);
  startCut_ = file_.front()->coursorTime();
  emit intervalChange();
}

inline
void
FileSet::cutEnd()
{
  MIRO_ASSERT(file_.size() != 0);
  endCut_ = file_.front()->coursorTime();
  emit intervalChange();
}

inline
void
FileSet::cutUndo()
{
  startCut_ = startTime_;
  endCut_ = endTime_;
  emit intervalChange();
}

inline
ACE_Time_Value const&
FileSet::coursorTime() const
{
  MIRO_ASSERT(file_.size() != 0);
  return (file_.front()->coursorTime() != ACE_Time_Value(0, 0))?
         file_.front()->coursorTime() : endTime_;
}

#endif // FileManager_h
