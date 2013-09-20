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
#include "Server.h"
#include "ServerWorker.h"
#include "ServerData.h"
#include "ClientParameters.h"
#include "ClientData.h"
#include "Log.h"
#include "Exception.h"
#include "NamingRepository.h"

#ifndef MIRO_NO_CORBA
#  include <tao/ORB_Core.h>
#  include <tao/PortableServer/Servant_Base.h>
#  include <orbsvcs/CosNamingC.h>
#endif // MIRO_NO_CORBA

namespace Miro
{
  using namespace std;

  Server::Server(int& argc, ACE_TCHAR *argv[]) :
      Super(argc, argv),
      _worker(ServerWorker::instance()),
      _data(new ServerData())
  {
    MIRO_LOG_CTOR("Miro::Server");
  }

  Server::~Server()
  {
    MIRO_LOG_DTOR("Miro::Server");

#ifndef MIRO_NO_CORBA
    {
      PortableServer::POA_var poa = _worker->rootPoa();
      ServerData::ObjectIdVector::const_iterator first, last = _data->poaObjects.end();
      for (first = _data->poaObjects.begin(); first != last; ++first) {
        try {
          CORBA::String_var name =
            PortableServer::ObjectId_to_string(first->in());
          MIRO_DBG_OSTR(MIRO, LL_DEBUG, "Deactivating object: " << name.in());
        }
        catch (CORBA::Exception& e) {
          MIRO_LOG_OSTR(LL_ERROR, "Failed to convert object id to string: " << e);
        }
        poa->deactivate_object(first->in());
      }
    } {
      PortableServer::POA_var child_poa = _worker->npPoa();
      ServerData::ObjectIdVector::const_iterator first, last = _data->npPoaObjects.end();
      for (first = _data->npPoaObjects.begin(); first != last; ++first) {
        try {
          CORBA::String_var name =
            PortableServer::ObjectId_to_string(first->in());
          MIRO_DBG_OSTR(MIRO, LL_DEBUG, "Deactivating object: " << name.in());
        }
        catch (CORBA::Exception& e) {
          MIRO_LOG_OSTR(LL_ERROR, "Failed to convert object id to string: " << e);
        }
        child_poa->deactivate_object(first->in());
      }
    } {
      PortableServer::POA_var child_poa = _worker->bdPoa();
      ServerData::ObjectIdVector::const_iterator first, last = _data->bdPoaObjects.end();
      for (first = _data->bdPoaObjects.begin(); first != last; ++first) {
        try {
          CORBA::String_var name =
            PortableServer::ObjectId_to_string(first->in());
          MIRO_DBG_OSTR(MIRO, LL_DEBUG, "Deactivating bidir object: " << name.in());
        }
        catch (CORBA::Exception& e) {
          MIRO_LOG_OSTR(LL_ERROR, "Failed to convert object id to string: " << e);
        }
        child_poa->deactivate_object(first->in());
      }
    }
#else // MIRO_NO_CORBA
    {
      ServerData::ObjectIdVector::const_iterator first, last = _data->poaObjects.end();
      for (first = _data->poaObjects.begin(); first != last; ++first) {
        MIRO_DBG(MIRO, LL_DEBUG, "Deleting default POA servant object.");
        delete *first;
      }
      MIRO_DBG(MIRO, LL_DEBUG, "Deleting default POA servants done.");
    } {
      ServerData::ObjectIdVector::const_iterator first, last = _data->npPoaObjects.end();
      for (first = _data->npPoaObjects.begin(); first != last; ++first) {
        MIRO_DBG(MIRO, LL_DEBUG, "Deleting persistent POA servant object.");
        delete *first;
      }
      MIRO_DBG(MIRO, LL_DEBUG, "Deleting persistent POA servant done.");
    } {
      ServerData::ObjectIdVector::const_iterator first, last = _data->bdPoaObjects.end();
      for (first = _data->bdPoaObjects.begin(); first != last; ++first) {
        MIRO_DBG(MIRO, LL_DEBUG, "Deleting bidir POA servant object.");
        delete *first;
      }
      MIRO_DBG(MIRO, LL_DEBUG, "Deleting bidir POA servant done.");
    }
#endif // MIRO_NO_CORBA

    if (!_params->noNaming) {
#ifndef MIRO_NO_CORBA

      ServerData::StringVector::const_iterator i;
      try {
        CosNaming::Name n;
        for (i = _data->nameVector.begin(); i != _data->nameVector.end(); ++i) {
          path2Name(i->c_str(), n);

          Client::_data->initialNamingContext->unbind(n);
        }
      }
      catch (const CORBA::Exception& e) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "Caught CORBA exception on unbinding: " << *i
                      << "\nProbably the NameSevice went down while we run:\n" << e);
      }
#else // MIRO_NO_CORBA
      MIRO_ASSERT(0);
#endif // MIRO_NO_CORBA
    }
    else {
      ServerData::StringVector::const_iterator i;
      try {
        for (i = _data->nameVector.begin(); i != _data->nameVector.end(); ++i) {
          std::string n = name2FullName(i->c_str());
          MIRO_DBG_OSTR(MIRO, LL_DEBUG, "Deleting naming repository entry: " << n);
          NamingRepository::instance()->remove(n);
        }
        MIRO_DBG(MIRO, LL_DEBUG, "Deleting naming repository done.");
      }
      catch (NamingRepository::ENotRegistered const& /*e*/) {
        MIRO_LOG_OSTR(LL_ERROR, "Caught exception on unbinding: " << *i
                      << "\nThere seems to be an name clash somewhere!");

      }
    }

    delete _data;
    _worker->remove_ref();

    MIRO_LOG_DTOR_END("Miro::Server");
  }

  /**
   * Starts the server main loop as a parallel threadpool. Returns
   * emediately.
   *
   * @param nthreads Number of threads in the threadpool.
   */
  void
  Server::detach(unsigned int nthreads)
  {
    _worker->shutdown_ = false;
    if (_worker->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, nthreads, 1) != 0)
      throw Miro::Exception("Miro::Server: Cannot activate client threads.");
  }


  void
  Server::wait()
  {
    _worker->wait();
  }

  /**
   * Starts the server main loop, waits for shutdown
   * of the server.
   *
   * @param nthreads Number of threads in the threadpool.
   */
  void
  Server::run(unsigned int nthreads)
  {
    detach(nthreads);
    _worker->wait();
  }

  /**
   * Adds an IOR at the naming service in the servers
   * naming context.
   */
  void
  Server::addToNameService(CORBA::Object * _object, const std::string& _name)
  {
    if (!_params->noNaming) {
#ifndef MIRO_NO_CORBA
      CosNaming::Name n;
      path2Name(_name.c_str(), n);

      if (n.length() < 1) {
        MIRO_LOG(LL_ERROR, "Object name empty.");
        return;
      }

      CosNaming::Name path = n;
      path.length(n.length() - 1);
      CosNaming::NamingContext_var nc = getNamingContext(path);

      CosNaming::Name name;
      name.length(1);
      name[0] = n[n.length() - 1];

      // Force binding of references to make
      // sure they are always up-to-date.
      try {
        nc->unbind(name);
        MIRO_LOG_OSTR(LL_ERROR, "Object still bound in naming service: " << _name
                      << "\nRebinding it.");
      }
      catch (...) {
      }

      try {
        nc->bind(name, _object);
      }
      catch (CosNaming::NamingContext::AlreadyBound&) {
        MIRO_LOG_OSTR(LL_ERROR, "Object is already bound in naming service: "
                      << _name);
        throw(0);
      }

#else // MIRO_NO_CORBA
      MIRO_ASSERT(0);
#endif // MIRO_NO_CORBA
    }
    else {
      NamingRepository::instance()->add(name2FullName(_name.c_str()), _object);
    }
    _data->nameVector.push_back(_name);
  }

  /**
   * Signals the server main loop to stop. The call returns
   * emediately.  Use @ref wait() to wait for completion.
   */
  void
  Server::shutdown()
  {
    // tell the orb to return from run()
    ServerWorker::shutdown();
  }

  bool
  Server::isShutdown() const throw()
  {
    return ServerWorker::isShutdown();
  }

  void
  Server::activate(PortableServer::ServantBase * servant, bool pass_ownership)
  {
#ifndef MIRO_NO_CORBA
    PortableServer::POA_var poa = _worker->rootPoa();
    PortableServer::ObjectId_var id = poa->activate_object(servant);

    if (pass_ownership) {
      servant->_remove_ref();
    }
    _data->poaObjects.push_back(id);
#else // MIRO_NO_CORBA

    if (pass_ownership)
      _data->poaObjects.push_back(servant);
#endif // MIRO_NO_CORBA
  }
  void
  Server::activateBiDir(PortableServer::ServantBase * servant, bool pass_ownership)
  {
#ifndef MIRO_NO_CORBA
    PortableServer::POA_var poa = _worker->bdPoa();
    PortableServer::ObjectId_var id = poa->activate_object(servant);

    if (pass_ownership) {
      servant->_remove_ref();
    }
    _data->bdPoaObjects.push_back(id);

#else // MIRO_NO_CORBA

    if (pass_ownership)
      _data->bdPoaObjects.push_back(servant);
#endif // MIRO_NO_CORBA
  }

  void
  Server::activateNamedObject(std::string const& object_name,
                              PortableServer::ServantBase * servant,
                              bool pass_ownership)
  {
#ifndef MIRO_NO_CORBA
    MIRO_ASSERT(object_name.length() != 0);

    PortableServer::POA_var child_poa = _worker->npPoa();
    PortableServer::ObjectId_var id =
      PortableServer::string_to_ObjectId(object_name.c_str());

    child_poa->activate_object_with_id(id.in(), servant);

    CORBA::Object_var obj = child_poa->id_to_reference(id.in());

    if (pass_ownership) {
      servant->_remove_ref();
    }
    _data->npPoaObjects.push_back(id);
#else // MIRO_NO_CORBA
    CORBA::Object * obj = servant;
    if (pass_ownership)
      _data->npPoaObjects.push_back(servant);
#endif // MIRO_NO_CORBA

    addToNameService(obj, object_name);
  }
}
