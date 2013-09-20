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
#include "ReactorTask.h"
#include "Exception.h"
#include "Log.h"

#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_signal.h>

namespace Miro
{
  ReactorTask::ReactorTask(ACE_Sched_Params *pschedp,
                           bool shutdownOnException,
                           int size) :
      schedp_(ACE_SCHED_OTHER, 0),
      reactor_(),
      shutdownOnException_(shutdownOnException)
  {
    MIRO_LOG_CTOR("Miro::ReactorTask");

    reactor(&reactor_);
    reactor_.open(size);
    if (pschedp)
      schedp_ = (*pschedp);
  }

  ReactorTask::~ReactorTask()
  {
    MIRO_LOG_DTOR("Miro::ReactorTask");
    reactor_.close();
    MIRO_DBG(MIRO, LL_DEBUG, "Reactor closed.");
    reactor(NULL);
    MIRO_DBG(MIRO, LL_DEBUG, "Reactor canceled.");
  }

  //
  // Now the svc() method where everything interesting happens.
  //
  int
  ReactorTask::svc()
  {
    MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                  "[Miro::ReactorTask] 0x" << (void*)this <<
                  " starts in thread " << ACE_Thread::self());

    ACE_Time_Value timeout(0, 100000); // wait for 100 msec
    ACE_Time_Value delta;              // uninitialized time value

    // set the given thread scheduling policy

    if (ACE_OS::sched_params(schedp_) == -1) {
      MIRO_LOG_OSTR(LL_ERROR, "[Miro::ReactorTask] Could not set sched parameters." << std::endl
                    << "[Miro::ReactorTask] Maybe suid root is missing." << std::endl
                    << "[Miro::ReactorTask] Will work on default scheduling policy." << std::endl);
    }

    // set the thread to be the owner of the reactor,
    //otherwise we will get errors
    reactor_.owner(ACE_OS::thr_self());

    ACE_Thread_Manager * mgr = this->thr_mgr();

    try {
      while (!mgr->testcancel(mgr->thr_self())) {
        // set delta to timeout
        // handle_events decrements the timeout
        delta = timeout;
        // trigger message handling
        reactor_.handle_events(delta);
      }
    }
    catch (const Miro::Exception& e) {
      MIRO_LOG_OSTR(LL_ERROR, "ReactorTask.handleMessage() - Uncaught Miro exception: " << e << std::endl);
      conditionalShutdown();
    }
    catch (...) {
      MIRO_LOG(LL_ERROR, "[Miro::HwReactor] task terminated due to unknown exception.");
      conditionalShutdown();
    }

    return 0;
  }

  void
  ReactorTask::shutdown(bool waitFinished) throw()
  {
    this->thr_mgr()->cancel_task(this);
    if (waitFinished) {
      wait();
    }
  }

  void
  ReactorTask::conditionalShutdown()
  {
    if (shutdownOnException_) {
      MIRO_LOG(LL_CRITICAL, "[Miro::HwReactor] shuting down on exception");
      ACE_OS::kill(ACE_OS::getpid(), SIGINT);
    }
  }
}
