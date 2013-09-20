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
#ifndef miroReactorTask_hh
#define miroReactorTask_hh

#include "miroCore_Export.h"

#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/Sched_Params.h>

namespace Miro
{
  class miroCore_Export ReactorTask : public ACE_Task_Base
  {
  public:
    ReactorTask(ACE_Sched_Params * pschedp = NULL, bool shutdownOnException = true, int size = 20);
    virtual ~ReactorTask();

    void shutdown(bool waitFinished = true) throw();

    // methods defined by ACE_Task_Base
    virtual int svc();

  private:
    void conditionalShutdown();

    ACE_Sched_Params schedp_;
    ACE_Reactor reactor_;
    bool shutdownOnException_;
  };
}

#endif




