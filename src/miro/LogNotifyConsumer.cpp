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
#include "LogNotifyConsumer.h"
#include "LogWriter.h"

#include "Server.h"
#include "Exception.h"
#include "TimeHelper.h"
#include "Log.h"

#include <ace/OS_NS_sys_time.h>
#include <ace/Sample_History.h>
#include <ace/Version.h>
#if (ACE_MAJOR_VERSION > 5) || \
  ( (ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION > 5) ) || \
  ( (ACE_MAJOR_VERSION == 5) && (ACE_MINOR_VERSION == 5) && (ACE_BETA_VERSION >= 10) )
#  include <ace/Throughput_Stats.h>
#else
#  include <ace/Stats.h>
#endif

#include <sstream>

namespace Miro
{
  using namespace std;

  LogNotifyConsumer::LogNotifyConsumer(CosNotifyChannelAdmin::EventChannel_ptr _ec,
                                       std::string const& _domainName,
                                       std::string const& _fileName,
                                       LogNotifyParameters const& _parameters) :
      Super(_ec),
      parameters_(_parameters),
      domainName_(_domainName),
      fileName_((_fileName.size() == 0) ? defaultFileName() : _fileName),
      mutex_(),
      logWriter_(new LogWriter(fileName_ + "-00.mlog", _parameters)),
      logNum_(0),
      history_(NULL),
      nTimes_(0)
  {
    MIRO_LOG_CTOR("LogNotifyConsumer");

    CosNotification::EventTypeSeq added;
    added.length(parameters_.typeName.size() + parameters_.event.size());

    if (parameters_.typeName.size() == 0 &&
                parameters_.event.size() == 0) {
      added.length(1);
      added[0].domain_name = CORBA::string_dup("*");
      added[0].type_name = CORBA::string_dup("*");
    }
    else {

      int index = 0;
      for (unsigned int i = 0; i < parameters_.typeName.size(); ++i, ++index) {
        added[index].domain_name =  CORBA::string_dup(domainName_.c_str());
        added[index].type_name =
          CORBA::string_dup(parameters_.typeName[i].c_str());
      }

      for (unsigned int i = 0; i < parameters_.event.size(); ++i, ++index) {
        added[index].domain_name =
          CORBA::string_dup(parameters_.event[i].domain.c_str());
        added[index].type_name =
          CORBA::string_dup(parameters_.event[i].type.c_str());
      }
    }
    setSubscriptions(added);
    connect();
  }

  LogNotifyConsumer::~LogNotifyConsumer()
  {
    MIRO_LOG_DTOR("LogNotifyConsumer");

    delete logWriter_;
    delete history_;
  }

  void
  LogNotifyConsumer::closeWriter()
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
    delete logWriter_;
    logWriter_ = 0;
  }

  void
  LogNotifyConsumer::push_structured_event(const CosNotification::StructuredEvent & notification)
  throw(CosEventComm::Disconnected)
  {
    ACE_hrtime_t start = ACE_OS::gethrtime();

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);

    if (logWriter_ && connected()) {
      if (!logWriter_->logEvent(ACE_OS::gettimeofday(), notification)) {
        MIRO_LOG(LL_NOTICE,
                 "Event log consumer max file size reached. - Starting new log file.");

        delete logWriter_;
        ++logNum_;
        std::stringstream num;
        num << "-";
        num.width(2);
        num.fill('0');
        num << logNum_ << ".mlog";
        logWriter_ = new LogWriter(fileName_ + num.str(), parameters_);
        logWriter_->logEvent(ACE_OS::gettimeofday(), notification);
      }
    }

    // performance measurement
    if (nTimes_ > 0) {
      ACE_hrtime_t now = ACE_OS::gethrtime();
      history_->sample(now - start);

      --nTimes_;
    }
  }

  void
  LogNotifyConsumer::measureTiming(unsigned int _nTimes)
  {
    nTimes_ = _nTimes;
    delete history_;
    history_ = new ACE_Sample_History(nTimes_);
    testStart_ = ACE_OS::gethrtime();
  }


  void
  LogNotifyConsumer::evaluateTiming()
  {
    ACE_hrtime_t testEnd = ACE_OS::gethrtime();
    ACE_DEBUG((LM_DEBUG, "Logging client test finished\n"));

    ACE_DEBUG((LM_DEBUG, "High resolution timer calibration...."));
    ACE_UINT32 gsf = ACE_High_Res_Timer::global_scale_factor();
    ACE_DEBUG((LM_DEBUG, "done\n"));

    //history_->dump_samples ("HISTORY", gsf);

    ACE_Basic_Stats stats;
    history_->collect_basic_stats(stats);
    stats.dump_results("Total", gsf);

    ACE_Throughput_Stats::dump_throughput("Logging client total", gsf,
                                          testEnd - testStart_,
                                          stats.samples_count());
    delete history_;
    history_ = NULL;
  }

  std::string
  LogNotifyConsumer::defaultFileName() const
  {
    return path() + domainName_ + "_" + timeStringMinute();
  }

  std::string
  LogNotifyConsumer::path()
  {
    char * miro_log = ACE_OS::getenv("MIRO_LOG");
    std::string p("./");

    if (miro_log)
      p = miro_log;

    if (p.size() > 0 && p[p.size() - 1] != '/')
      p += "/";
    return p;
  }
}
