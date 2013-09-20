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
#ifndef miro_NamingRepository_h
#define miro_NamingRepository_h

#include "Repository.h"

#include "miro_Export.h"

namespace CORBA
{
  class Object;
}

namespace Miro
{
  class miro_Export NamingRepository : public RepositoryBase
  {
    //--------------------------------------------------------------------------
    // private types
    //--------------------------------------------------------------------------
    typedef RepositoryBase Super;

  public:
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------
    typedef CORBA::Object Type;
    //! Map of the added instances, sorted by name.
    typedef std::map<std::string, CORBA::Object *> InstanceMap;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Clean up the repository.
    ~NamingRepository();
    //! Empty the repository.
    void clear() throw();
    //! Register an instance at the repository.
    void add(std::string const& name, CORBA::Object * type) throw(EAlreadyRegistered);
    //! Look up an instance by name.
    CORBA::Object * get(std::string const& name) throw(ENotRegistered);
    //! Remove instance from the repository, deleting it.
    void remove(std::string const& name) throw(ENotRegistered);
    //! Get read-only access to the whole repository.
    InstanceMap const& getMap() const throw() {
      return instances_;
    }

    //--------------------------------------------------------------------------
    // public data
    //--------------------------------------------------------------------------

    //! Accessor to the global instance of the Repository.
    static Singleton<NamingRepository> instance;

  protected:
    //--------------------------------------------------------------------------
    // protected types
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    // protected methods
    //--------------------------------------------------------------------------

    //! Dump the Repository to the specified output stream.
    void printToStream(std::ostream& ostr) const;

    //--------------------------------------------------------------------------
    // protected data
    //--------------------------------------------------------------------------

    //! Map to associate an instance name with an instance.
    InstanceMap instances_;

  private:
    //--------------------------------------------------------------------------
    // private/hidden methods
    //--------------------------------------------------------------------------

    //! There is only one Repository instance.
    NamingRepository() throw();
    //! Copy construction is prohibited
    NamingRepository(const NamingRepository&) throw() {}
    NamingRepository& operator=(NamingRepository const&) { return *this; }

    //--------------------------------------------------------------------------
    // friend declarations
    //--------------------------------------------------------------------------
    //! Grant ostream operator access to the map.
    friend std::ostream& operator << (std::ostream&, const NamingRepository&);
    //! Allow Singleton to create a Repository
    friend class ACE_Singleton<NamingRepository, ACE_Recursive_Thread_Mutex>;
  };

  typedef ACE_Singleton<NamingRepository, ACE_SYNCH_RECURSIVE_MUTEX> NamingRepositorySingleton;
}

MIRO_SINGLETON_DECLARE(ACE_Singleton, Miro::NamingRepository, ACE_SYNCH_RECURSIVE_MUTEX);
#endif // miro_NamingRepository_h
