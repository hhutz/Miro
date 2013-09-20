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
#ifndef miro_LogInterceptor_h
#define miro_LogInterceptor_h

#include "miro_Export.h"

#include "tao/PI_Server/ServerRequestInterceptorC.h"

#include <ace/Recursive_Thread_Mutex.h>

namespace Miro
{
  // forward declarations
  class LogInterceptorParameters;
  template<typename PAYLOAD_TYPE>
  class NotifyTypedSupplier;
  class SCmdReq;
  class SCmdRep;
  class AnyPrinter;

  class miro_Export LogInterceptor
        : public PortableInterceptor::ServerRequestInterceptor
  {
  public:
    LogInterceptor(void);
    virtual ~LogInterceptor();

    virtual char * name()
    throw(CORBA::SystemException);

    virtual void destroy()
    throw(CORBA::SystemException);

    virtual void receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw(CORBA::SystemException, PortableInterceptor::ForwardRequest);

    virtual void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw(CORBA::SystemException, PortableInterceptor::ForwardRequest);

    virtual void send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw(CORBA::SystemException);

    virtual void send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw(CORBA::SystemException, PortableInterceptor::ForwardRequest);

    virtual void send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw(CORBA::SystemException, PortableInterceptor::ForwardRequest);

  protected:
    bool isLogged(PortableInterceptor::ServerRequestInfo_ptr ri, char const * objId, char const * operation);

  private:
    LogInterceptorParameters const * m_params;
    NotifyTypedSupplier<Miro::SCmdReq *> * m_requestSupplier;
    NotifyTypedSupplier<Miro::SCmdRep *> * m_replySupplier;
    AnyPrinter * m_anyPrinter;

    ACE_Recursive_Thread_Mutex m_requestMutex;
    ACE_Recursive_Thread_Mutex m_replyMutex;

    bool m_notifyFailure;
  };
}
#endif // miro_LogInterceptor_h
