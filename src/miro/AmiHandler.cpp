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
#include "AmiHandler.h"
#include "Log.h"

namespace Miro
{
  AmiHandlerBase::AmiHandlerBase() throw() :
      _mutex(),
      _condition(_mutex),
      _finished(false)
  {
    MIRO_LOG_CTOR("AmiHandlerBase");
  }

  AmiHandlerBase::~AmiHandlerBase()
  {
    MIRO_LOG_DTOR("AmiHandlerBase");
  }

  void
  AmiHandlerBase::wait(ACE_Time_Value const& timeout) const throw(::Miro::ETimeout, ::Miro::EInterrupted)
  {
    ACE_Time_Value const * t = NULL;
    if (timeout != ACE_Time_Value::zero)
      t = &timeout;
    ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
    if (_finished)
      return;

    int rc = _condition.wait(t);
    if (rc == -1)
      throw Miro::EInterrupted("Interrupted by the system while waiting on AMI reply.");
    if (t != NULL && (*t) != ACE_Time_Value::zero)
      throw Miro::ETimeout("Timeout while waiting on AMI reply.");

  }

  void
  AmiHandlerBase::throwOnException() const throw(CORBA::Exception, ENotYetCompleted)
  {
    ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
    if (!_finished)
      throw ENotYetCompleted("AMI call did not complete, yet.");
    if (_exception_holder != NULL)
      _exception_holder->raise_exception();
  }

  void
  AmiHandlerBase::finishByException(Messaging::ExceptionHolder * exception_holder) throw()
  {
    {
      ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
      exception_holder->_add_ref();
      _exception_holder = exception_holder;
      _finished = true;
      _condition.broadcast();
    }
    finish();
  }

  void
  AmiHandlerBase::finish() throw()
  {
#ifndef MIRO_NO_CORBA
    try {
      // Get reference to Root POA.
      PortableServer::POA_var poa = _default_POA();
      // Deactivate.
      PortableServer::ObjectId_var oid = poa->reference_to_id(_this());
      // deactivate from the poa.
      poa->deactivate_object(oid.in());
    }
    catch (CORBA::Exception const& e) {
      MIRO_LOG_OSTR(LL_ERROR, "AmiHandlerBase::finish() raises exception: " << e);
    }
#else
    //   _decr_refcnt();
#endif


  }

  //////////////////////////////////////////////////////////////////////////////

  void
  VoidAmiHandler::finish() throw()
  {
    {
      ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
      _finished = true;
      _condition.broadcast();
    }
    Super::finish();
  }
}
