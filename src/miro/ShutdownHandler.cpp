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
#include "ShutdownHandler.h"
#include "Log.h"
#include "Exception.h"

#include <ace/Thread_Manager.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Signal.h>

namespace
{
  static void signal_handler(int signum)
  {
    // check of sigint and sigterm
    if (signum == SIGINT || signum == SIGTERM) {
      MIRO_LOG_OSTR(LL_CRITICAL, "Miro ShutdownHandler: Signal received (shutting down): " << signum);
      Miro::ShutdownHandler::shutdown();
    }
    else {
      MIRO_LOG_OSTR(LL_ERROR, "Miro ShutdownHanlder: Unhandled signal received (ignored): " << signum);
      
    }
  }

  ACE_Recursive_Thread_Mutex mutex_;
  bool instance_ = false;
  bool init_ = false;
  bool shutdown_ = false;
}

namespace Miro
{
  ShutdownHandler::ShutdownHandler()
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);

    if (instance_)
      throw Miro::Exception("Second ShutdownHandler instanciation. Only one allowed.");
    instance_ = true;

    shutdown_ = false;
  
    // set up signal handler
    ACE_Sig_Action sa((ACE_SignalHandler) signal_handler);
    ACE_Sig_Set ss;
  
    ss.sig_add(SIGINT);
    ss.sig_add(SIGTERM);
  
    sa.mask(ss);
  
    sa.register_action(SIGINT);
    sa.register_action(SIGTERM);

    init_ = true;
  }

  ShutdownHandler::~ShutdownHandler()
  {
    if (init_) {
      // register Signal handler for Ctr+C
      // Signal set to be handled by the signal handler.
      ACE_Sig_Action sa(NULL);
      ACE_Sig_Set ss;

      ss.sig_add(SIGINT);
      ss.sig_add(SIGTERM);

      sa.mask(ss);

      sa.register_action(SIGINT);
      sa.register_action(SIGTERM);
    }

    instance_ = false;
  }

  void
  ShutdownHandler::shutdown()
  {
    shutdown_ = true;
  }

  bool
  ShutdownHandler::isShutdown()
  {
    if (!shutdown_)
      return false;

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);

    if (init_) {
      init_ = false;
    
      // register Signal handler for Ctr+C
      // Signal set to be handled by the signal handler.
      ACE_Sig_Action sa(NULL);
      ACE_Sig_Set ss;

      ss.sig_add(SIGINT);
      ss.sig_add(SIGTERM);

      sa.mask(ss);

      sa.register_action(SIGINT);
      sa.register_action(SIGTERM);

    }
    
    return true;
  }
}
