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
#include <tao/DynamicInterface/Request.h>
#include <tao/PI_Server/ServerRequestInfoC.h>
#include <tao/PortableServer/PortableServer_Functions.h>

#include "LogInterceptor.h"
#include "RobotParameters.h"
#include "AnyPrinter.h"
#include "Log.h"
#include "Parameters.h"
#include "TimeHelper.h"
#include "NotifyTypedSupplier.h"
#include "Client.h"

#include "SCmdLogC.h"

#include <orbsvcs/Time_Utilities.h>

#include <iostream>


namespace Miro
{
  using namespace std;

  LogInterceptor::LogInterceptor() :
      m_params(LogInterceptorParameters::instance()),
      m_requestSupplier(NULL),
      m_replySupplier(NULL),
      m_anyPrinter(NULL),
      m_notifyFailure(false)
  {
    MIRO_LOG_CTOR("LogInterceptor");
  }

  LogInterceptor::~LogInterceptor()
  {
    MIRO_LOG_DTOR("LogInterceptor");
    delete m_anyPrinter;
  }

  char *
  LogInterceptor::name()
  throw(CORBA::SystemException)
  {
    cout << "Calling LogInterceptor name() method" << endl;
    return CORBA::string_dup(m_params->name.c_str());
  }

  void
  LogInterceptor::destroy()
  throw(CORBA::SystemException)
  {
    cout << "Calling destroy()." << endl;
  }

  void
  LogInterceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo_ptr /*ri*/)
  throw(CORBA::SystemException, PortableInterceptor::ForwardRequest)
  {
  }

  void
  LogInterceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
  throw(CORBA::SystemException, PortableInterceptor::ForwardRequest)
  {
    if (!m_params->asciiLog &&
	!m_params->notifyLog)
      return;

    PortableInterceptor::ObjectId_var id = ri->object_id();
    CORBA::String_var objId = PortableServer::ObjectId_to_string(id);
    CORBA::String_var operation = ri->operation();

    if (isLogged(ri, objId, operation)) {
      cout << "LogInterceptor::receive_request - " << flush;

      ACE_Time_Value timeStamp = ACE_OS::gettimeofday();
      Dynamic::ParameterList_var params = ri->arguments();

      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_requestMutex);

      if (m_params->asciiLog) {
	if (m_params->asciiLog) {
          cout << timeStamp << " "
               << "Request: "
               << objId << "->"
               << operation << "(" << flush;

	  if (m_anyPrinter == NULL)
	    m_anyPrinter = new AnyPrinter();

          for (CORBA::ULong i = 0; i < params->length(); ++i) {
            if (params[i].mode != CORBA::PARAM_OUT) {
              m_anyPrinter->print(cout, params[i].argument);
            }
            else {
              cout << "OUT_PARAM";
            }
            if (i != params->length() - 1)
              cout << ", ";
          }
          cout << ")" << endl;
        }
      }

      if (m_params->notifyLog) {

        if (m_requestSupplier == NULL) {
          try {
            Client client;
            CosNotifyChannelAdmin::EventChannel_var ec =
              client.resolveName<CosNotifyChannelAdmin::EventChannel>(RobotParameters::instance()->eventChannelName);
            m_requestSupplier =
              new NotifyTypedSupplier<Miro::SCmdReq *>(ec, m_params->name + ".SCmdReq");
            m_notifyFailure = false;
          }
          catch (Miro::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Miro exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (CORBA::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "CORBA exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (std::exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Std exception constructing SCmdRep supplier - " << e.what());
            m_notifyFailure = true;
          }
          catch (...) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Unknown exception constructing SCmdRep supplier.");
            m_notifyFailure = true;
          }
        }

        if (m_requestSupplier) {
          try {
            SCmdReq_var sCmdReq = new SCmdReq();
            ORBSVCS_Time::Absolute_Time_Value_to_TimeT(sCmdReq->hdr.time, timeStamp);
            sCmdReq->hdr.status = RET_OKAY;
            sCmdReq->objName = objId._retn();
            sCmdReq->operation = operation._retn();

            CORBA::ULong max = params->maximum();
            CORBA::ULong length = params->length();
            {
              Dynamic::Parameter * buffer = params->get_buffer(true);
              sCmdReq->params.replace(max,
                                      length,
                                      buffer,
                                      true);
            }

            Dynamic::ExceptionList_var exceptions = ri->exceptions();
            max = exceptions->maximum();
            length = exceptions->length();
            {
              CORBA::TypeCode ** buffer = exceptions->get_buffer(true);
              sCmdReq->exceptions.replace(max,
                                          length,
                                          buffer,
                                          true);
            }
            CORBA::Any_var result = ri->result();
            sCmdReq->resultType = result->type();

            m_requestSupplier->sendEvent(sCmdReq._retn());
          }
          catch (CORBA::Exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "CORBA exception sending CmdLog.SCmdReq - " << e);
            delete m_requestSupplier;
            m_requestSupplier = NULL;
          }
          catch (std::exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "Std exception sending CmdLog.SCmdReq - " << e.what());
            delete m_requestSupplier;
            m_requestSupplier = NULL;
          }
          catch (...) {
            MIRO_LOG_OSTR(LL_ERROR, "Unknown exception sending CmdLog.SCmdRep.");
            delete m_requestSupplier;
            m_requestSupplier = NULL;
          }
        }
      }
      //cout << "LogInterceptor::receive_request done." << endl;
    }
  }

  void
  LogInterceptor::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
  throw(CORBA::SystemException)
  {
    if (!m_params->asciiLog &&
	!m_params->notifyLog)
      return;

    PortableInterceptor::ObjectId_var id = ri->object_id();
    CORBA::String_var objId = PortableServer::ObjectId_to_string(id);
    CORBA::String_var operation = ri->operation();

    if (isLogged(ri, objId, operation)) {
      cout << "LogInterceptor::send_reply - " << flush;

      Dynamic::ParameterList_var params = ri->arguments();

      ACE_Time_Value timeStamp = ACE_OS::gettimeofday();
      CORBA::Any_var result = ri->result();

      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_replyMutex);
      if (m_params->asciiLog) {
	cout << timeStamp << " "
             << "Reply: " << flush;
	
	if (m_anyPrinter == NULL)
	  m_anyPrinter = new AnyPrinter();

        m_anyPrinter->print(cout, *result);

        cout << "="
             << objId << "->"
             << operation << "(" << flush;

        for (CORBA::ULong i = 0; i < params->length(); ++i) {
          if (params[i].mode != CORBA::PARAM_IN) {
            m_anyPrinter->print(cout, params[i].argument);
          }
          else {
            cout << "IN_PARAM";
          }
          if (i != params->length() - 1)
            cout << ", ";
        }
        cout << ")" << endl;
      }

      if (m_params->notifyLog) {
        if (m_replySupplier == NULL) {
          try {
            Client client;
            CosNotifyChannelAdmin::EventChannel_var ec =
              client.resolveName<CosNotifyChannelAdmin::EventChannel>(RobotParameters::instance()->eventChannelName);
            m_replySupplier =
              new NotifyTypedSupplier<Miro::SCmdRep *>(ec,  m_params->name + ".SCmdRep");
            m_notifyFailure = false;
          }
          catch (Miro::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Miro exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (CORBA::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "CORBA exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (std::exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Std exception constructing SCmdRep supplier - " << e.what());
            m_notifyFailure = true;
          }
          catch (...) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Unknown exception constructing SCmdRep supplier.");
            m_notifyFailure = true;
          }
        }

        if (m_replySupplier) {
          try {
            SCmdRep_var sCmdRep = new SCmdRep();

            ORBSVCS_Time::Absolute_Time_Value_to_TimeT(sCmdRep->hdr.time, timeStamp);
            sCmdRep->hdr.status = RET_OKAY;
            sCmdRep->objName = objId._retn();
            sCmdRep->operation = operation._retn();

            CORBA::ULong max = params->maximum();
            CORBA::ULong length = params->length();
            Dynamic::Parameter * buffer = params->get_buffer(true);
            sCmdRep->params.replace(max,
                                    length,
                                    buffer,
                                    true);
            sCmdRep->result = *result;

            m_replySupplier->sendEvent(sCmdRep._retn());
          }
          catch (CORBA::Exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "CORBA exception sending CmdLog.SCmdRep - " << e);
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
          catch (std::exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "Std exception sending CmdLog.SCmdRep - " << e.what());
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
          catch (...) {
            MIRO_LOG_OSTR(LL_ERROR, "Unknown exception sending CmdLog.SCmdRep.");
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
        }
      }
      // cout << "LogInterceptor::send_reply done." << endl;
    }
  }

  void
  LogInterceptor::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
  throw(CORBA::SystemException, PortableInterceptor::ForwardRequest)
  {
    if (!m_params->asciiLog &&
                !m_params->notifyLog)
      return;

    PortableInterceptor::ObjectId_var id = ri->object_id();
    CORBA::String_var objId = PortableServer::ObjectId_to_string(id);
    CORBA::String_var operation = ri->operation();

    if (m_params->logAllExceptions ||
        isLogged(ri, objId, operation)) {
      cout << "LogInterceptor::send_exception - " << flush;

      ACE_Time_Value timeStamp = ACE_OS::gettimeofday();
      CORBA::Any_var a = ri->sending_exception();

      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_replyMutex);
      if (m_params->asciiLog) {
        cout << timeStamp << " "
             << "Exception: "
             << objId << "->"
             << operation << "() throw(" ;

	if (m_anyPrinter == NULL)
	  m_anyPrinter = new AnyPrinter();

        m_anyPrinter->print(cout, *a);
        cout << ")" << endl;
      }

      if (m_params->notifyLog) {

        if (m_replySupplier == NULL) {
          try {
            Client client;
            CosNotifyChannelAdmin::EventChannel_var ec =
              client.resolveName<CosNotifyChannelAdmin::EventChannel>(RobotParameters::instance()->eventChannelName);
            m_replySupplier =
              new NotifyTypedSupplier<Miro::SCmdRep *>(ec,  m_params->name + ".SCmdRep");
            m_notifyFailure = false;
          }
          catch (Miro::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Miro exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (CORBA::Exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "CORBA exception constructing SCmdRep supplier - " << e);
            m_notifyFailure = true;
          }
          catch (std::exception const& e) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Std exception constructing SCmdRep supplier - " << e.what());
            m_notifyFailure = true;
          }
          catch (...) {
            if (!m_notifyFailure)
              MIRO_LOG_OSTR(LL_ERROR, "Unknown exception constructing SCmdRep supplier.");
            m_notifyFailure = true;
          }
        }

        if (m_replySupplier) {
          try {
            SCmdRep_var sCmdRep = new SCmdRep();
            ORBSVCS_Time::Absolute_Time_Value_to_TimeT(sCmdRep->hdr.time, timeStamp);
            sCmdRep->hdr.status = RET_EXCEPTION;
            sCmdRep->objName = objId._retn();
            sCmdRep->operation = operation._retn();
            sCmdRep->result = *a;

            m_replySupplier->sendEvent(sCmdRep._retn());
          }
          catch (CORBA::Exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "CORBA exception sending CmdLog.SCmdRep - " << e);
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
          catch (std::exception const& e) {
            MIRO_LOG_OSTR(LL_ERROR, "Std exception sending CmdLog.SCmdRep - " << e.what());
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
          catch (...) {
            MIRO_LOG_OSTR(LL_ERROR, "Unknown exception sending CmdLog.SCmdRep.");
            delete m_replySupplier;
            m_replySupplier = NULL;
          }
        }
      }
      cout << "LogInterceptor::send_exception done." << endl;
    }
  }

  void
  LogInterceptor::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
  throw(CORBA::SystemException, PortableInterceptor::ForwardRequest)
  {
    ACE_UNUSED_ARG(ri);
    cout << "Calling send_other()." << endl;
  }

  inline
  static
  bool operator == (char * const lhs, IncludeParameters const& rhs)
  {
    return (strcmp(lhs, rhs.name.c_str()) == 0);
  }

  inline
  static
  bool operator == (IncludeParameters const& lhs, char const * rhs)
  {
    return (strcmp(lhs.name.c_str(), rhs) == 0);
  }

  inline
  static
  bool operator < (char * const lhs, IncludeParameters const& rhs)
  {
    return (strcmp(lhs, rhs.name.c_str()) < 0);
  }

  inline
  static
  bool operator < (IncludeParameters const& lhs, char const * rhs)
  {
    return (strcmp(lhs.name.c_str(), rhs) < 0);
  }

  inline
  static
  bool operator < (char * const lhs, string const& rhs)
  {
    return (strcmp(lhs, rhs.c_str()) < 0);
  }

  inline
  static
  bool operator < (string const& lhs, char const * rhs)
  {
    return (strcmp(lhs.c_str(), rhs) < 0);
  }

  struct LessPrefix {
    bool operator()(string const& lhs, char const * rhs) {
      return (strncmp(lhs.c_str(), rhs, lhs.length()) < 0);
    }
    bool operator()(char const * lhs, string const& rhs) {
      return (strncmp(lhs, rhs.c_str(), rhs.length()) < 0);
    }
    bool operator()(string const& lhs, string const& rhs) {
      return (strncmp(lhs.c_str(), rhs.c_str(), rhs.length()) < 0);
    }
  };


  bool
  LogInterceptor::isLogged(PortableInterceptor::ServerRequestInfo_ptr ri, char const * objId, char const * operation)
  {
    vector<IncludeParameters>::const_iterator iter =
      find(m_params->objects.begin(), m_params->objects.end(), objId);
    if (iter != m_params->objects.end()) {
      if (!binary_search(iter->excludePrefix.begin(), iter->excludePrefix.end(), operation, LessPrefix()) ||
                  binary_search(iter->explicitInclude.begin(), iter->explicitInclude.end(), operation)) {
        return true;
      }
    }

    vector<IncludeParameters>::const_iterator first, last = m_params->interfaces.end();
    for (first = m_params->interfaces.begin(); first != last; ++first) {
      if (ri->target_is_a(first->name.c_str())) {
        if (!binary_search(first->excludePrefix.begin(), first->excludePrefix.end(), operation, LessPrefix()) ||
                    binary_search(first->explicitInclude.begin(), first->explicitInclude.end(), operation)) {
          return true;
        }
      }
    }

    return false;
  }
}

