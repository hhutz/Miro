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
#ifndef miro_ServerWorker_h
#define miro_ServerWorker_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ace/Signal.h>
#include <ace/Task.h>

#ifndef MIRO_NO_CORBA
#  include <tao/PortableServer/PortableServer.h>
#endif // MIRO_NO_CORBA

#include "miro_Export.h"

//! forward declaration
class ACE_Reactor;

namespace Miro
{
  //! Detached server thread pool.
  /**
   *   Uses the ACE_Task_Base class to run server threads
   */
  class miro_Export ServerWorker : public ACE_Task_Base
  {
    typedef ACE_Task_Base Super;

  public:
    //! The thread entry point.
    virtual int svc();

  private:
    //! Singleton instance of worker threads.
    static ServerWorker * instance();
    friend class Server;

  public:
    //! Shutting down the threads
    static void shutdown() throw();
    //! Is the thread canceled?
    static bool isShutdown() throw();

#ifndef MIRO_NO_CORBA
    //! Accessor which returns the root poa.
    PortableServer::POA_ptr rootPoa() const;
    //! Accessor which returns the child poa.
    /** The policies are named and persistent. */
    PortableServer::POA_ptr npPoa() const;
    //! Accessor which returns the child poa.
    /** The policy is bidirectional giop. */
    PortableServer::POA_ptr bdPoa() const;
#endif // MIRO_NO_CORBA

  private:
    //@{
    /**
     * @name Reference Counting Operations
     */
    //! Increase reference count by one.
    void add_ref();

    /**
     * Decreases reference count by one; if the resulting reference
     * count equals zero, _remove_ref invokes delete on its this pointer
     * in order to destroy the servant.
     */
    void remove_ref();

    /**
     * Returns the current reference count value.
     */
    unsigned int refcount_value() const;
    //@}

  public:

#ifndef MIRO_NO_CORBA
    //! Returns the reactor instance used by the ORB core.
    ACE_Reactor * tao_reactor();
#endif // MIRO_NO_CORBA

  private:
    /** Flag indicating, the server was canceld  */
    static bool shutdown_;

#ifndef MIRO_NO_CORBA
    void init_child_poa();

    //! The orb.
    CORBA::ORB_var orb_;

    //! Reference to Root POA.
    PortableServer::POA_var poa_;

    //! Named and persistent poa under the root POA.
    PortableServer::POA_var child_poa_;

    //! BiDir GIOP poa under the root POA.
    PortableServer::POA_var bidir_poa_;

    //!  Reference to POA manager.
    PortableServer::POAManager_var poa_mgr_;
#endif // MIRO_NO_CORBA

    //! Reference counter.
    ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> ref_count_;

    static ACE_Thread_Mutex instance_mutex_;
    static ServerWorker * instance_ ;


    //! Initializing constructor.
    ServerWorker();
    ServerWorker(ServerWorker const&);
    virtual ~ServerWorker();
    ServerWorker& operator=(ServerWorker const&);
  };
}
#endif // miro_ServerWorker_h




