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
#ifndef miro_ClientData_h
#define miro_ClientData_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ace/Thread_Mutex.h>

#ifndef MIRO_NO_CORBA
#  include <orbsvcs/CosNamingC.h>
#endif // MIRO_NO_CORBA

namespace Miro
{
  class ClientData
  {
  public:
#ifndef MIRO_NO_CORBA
    //! Remember our ORB.
    CORBA::ORB_var orb;
    //! Reference to the initial naming context.
    CosNaming::NamingContext_var initialNamingContext;
    //! Reference to the MIRO naming context.
    CosNaming::NamingContext_var namingContext;
#endif // MIRO_NO_CORBA

    ClientData(int& argc, char* argv[]);

  private:
    friend class Client;

    //! Singleton instance of worker threads.
    static ClientData * instance(int& argc, char* argv[]);

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

    //! Reference counter.
    ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> ref_count_;

    static ACE_Thread_Mutex instance_mutex_;
    static ClientData * instance_ ;
  };
}
#endif // miro_ClientData_h
