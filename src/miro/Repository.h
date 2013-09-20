// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#ifndef Miro_Repository_h
#define Miro_Repository_h

#include "Singleton.h"
#include "Exception.h"

#include "miroCore_Export.h"

#include <map>
#include <string>
#include <iostream>

namespace Miro
{
  // forward declarations
  template<class T>
  class Repository;

  //! Ostream operator for debug purposes.
  template<class T>
  std::ostream&
  operator << (std::ostream& ostr, Repository<T> const& factory);

  //! Base class for repository templates.
  /**
   * @author Hans Utz
   */
  class miroCore_Export RepositoryBase
  {
  public:
    //--------------------------------------------------------------------------
    // pulic types
    //--------------------------------------------------------------------------

    MIRO_EXCEPTION_TYPE(EAlreadyRegistered);
    MIRO_EXCEPTION_TYPE(ENotRegistered);
  };

  //! Repository of available instances of a type.
  /**
   * @author Hans Utz
   */
  template<class T>
  class Repository : public RepositoryBase
  {
    //--------------------------------------------------------------------------
    // private types
    //--------------------------------------------------------------------------
    typedef Repository<T> Self;
    typedef RepositoryBase Super;

  public:
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------
    typedef T Type;
    //! Map of the added instances, sorted by name.
    typedef std::map<std::string, Type *> InstanceMap;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Clean up the repository.
    ~Repository() throw();
    //! Empty the repository.
    void clear() throw();
    //! Register an instance at the repository.
    void add(std::string const& name, Type * type) throw(EAlreadyRegistered);
    //! Look up an instance by name.
    Type * get(std::string const& name) throw(ENotRegistered);
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
    static Singleton<Self> instance;

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

  protected:
    //--------------------------------------------------------------------------
    // private/hidden methods
    //--------------------------------------------------------------------------

    //! There is only one Repository instance.
    Repository() throw();
    //! Copy construction is prohibited
    Repository(const Repository<T>&) throw() {}
    Repository& operator=(Repository const&) { return *this; }

    //--------------------------------------------------------------------------
    // friend declarations
    //--------------------------------------------------------------------------
    //! Grant ostream operator access to the map.
    friend std::ostream& operator << <Type>(std::ostream&, const Self&);
    //! Allow Singleton to create a Repository
    friend class ACE_Singleton<Self, ACE_Recursive_Thread_Mutex>;
  };


  /**
   * To get a pointer to the global Repository instance,
   * simply call Repository::instance();
   */
  // removing this for sake of windows stupidity:
  // please declare your own, unique instance of the repository singleton
  // or maybe it _is_ doing good. - at least it seems to work (tm)
  template<class T>
  Singleton<Repository<T> > Repository<T>::instance;

  template<class T>
  Repository<T>::Repository() throw()
  {}

  template<class T>
  Repository<T>::~Repository() throw()
  {
    clear();
  }

  /** Deletes all objects registered at the repository. */
  template<class T>
  void
  Repository<T>::clear() throw()
  {
    typename InstanceMap::iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first)
      delete first->second;
    instances_.clear();
  }

  /** The repository takes ownership of the instance. */
  template<class T>
  void
  Repository<T>::add(std::string const& _name, T * _instance)
  throw(EAlreadyRegistered)
  {
    if (instances_.find(_name) == instances_.end()) {
      instances_.insert(std::make_pair(_name, _instance));
      return;
    }
    throw EAlreadyRegistered("add failed, \""+_name+"\" already in repository");
  }

  /**
   * @return Instance registered under the specified name.
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  template<class T>
  T *
  Repository<T>::get(const std::string& _name) throw(ENotRegistered)
  {
    typename InstanceMap::const_iterator i = instances_.find(_name);
    if (i != instances_.end())
      return i->second;
    throw ENotRegistered("get failed, \""+_name+"\" not in Repository");
  }

  /**
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  template<class T>
  void
  Repository<T>::remove(const std::string& _name) throw(ENotRegistered)
  {
    typename InstanceMap::iterator i = instances_.find(_name);
    if (i != instances_.end()) {
      delete i->second;
      instances_.erase(i);
      return;
    }
    throw ENotRegistered("remove failed, \""+_name+"\" not in repository");
  }

  /**
   * Prints the names of all registered instances to the stream
   */
  template<class T>
  void
  Repository<T>::printToStream(std::ostream& _ostr) const
  {
    typename InstanceMap::const_iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first) {
      _ostr << first->first << std::endl;
    }
  }

  template<class T>
  std::ostream&
  operator << (ostream& _ostr, Repository<T> const& _repository)
  {
    _repository.printToStream(_ostr);
    return _ostr;
  }


  // forward declarations
  template<class T>
  class UnmanagedRepository;

  //! Ostream operator for debug purposes.
  template<class T>
  std::ostream&
  operator << (std::ostream& ostr, UnmanagedRepository<T> const& factory);

//! UnmanagedRepository of available instances of a type.
  /**
   * @author Hans Utz
   */
  template<class T>
  class UnmanagedRepository : public RepositoryBase
  {
    //--------------------------------------------------------------------------
    // private types
    //--------------------------------------------------------------------------
    typedef UnmanagedRepository<T> Self;
    typedef RepositoryBase Super;

  public:
    //--------------------------------------------------------------------------
    // private types
    //--------------------------------------------------------------------------
    typedef T Type;
    typedef Singleton<Self, ACE_Recursive_Thread_Mutex, ACE_Unmanaged_Singleton> SingletonType;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Clean up the repository.
    ~UnmanagedRepository() throw();
    //! Register an instance at the repository.
    void add(std::string const& name, Type * type) throw(EAlreadyRegistered);
    //! Look up an instance by name.
    Type * get(std::string const& name) throw(ENotRegistered);
    //! Remove instance from the repository, deleting it.
    /** An empty repository will delete itself. */
    void remove(std::string const& name) throw(ENotRegistered);

    //--------------------------------------------------------------------------
    // public data
    //--------------------------------------------------------------------------

    //! Accessor to the global instance of the UnmanagedRepository.
    static SingletonType instance;

  protected:
    //--------------------------------------------------------------------------
    // protected types
    //--------------------------------------------------------------------------

    //! Map of the added instances, sorted by name.
    typedef std::map<std::string, Type *> InstanceMap;

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

  protected:
    //--------------------------------------------------------------------------
    // private/hidden methods
    //--------------------------------------------------------------------------

    //! There is only one Repository instance.
    UnmanagedRepository() throw();
    //! Copy construction is prohibited
    UnmanagedRepository(const UnmanagedRepository<T>&) throw() {}
    UnmanagedRepository& operator=(UnmanagedRepository const&) { return *this; } // must return a value

    //--------------------------------------------------------------------------
    // friend declarations
    //--------------------------------------------------------------------------
    //! Grant ostream operator access to the map.
    friend std::ostream& operator << <Type>(std::ostream&, const Self&);
    //! Allow Singleton to create a Repository
    friend class ACE_Singleton<Self, ACE_Recursive_Thread_Mutex>;
  };

  /**
   * To get a pointer to the global Repository instance,
   * simply call Repository::instance();
   */
  //  template<class T>
  //typename UnmanagedRepository<T>::SingletonType UnmanagedRepository<T>::instance;

  template<class T>
  UnmanagedRepository<T>::UnmanagedRepository() throw()
  {}

  template<class T>
  UnmanagedRepository<T>::~UnmanagedRepository() throw()
  {
    // just to make sure, the repository should actually be empty at this point.
    typename InstanceMap::iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first)
      delete first->second;
    instances_.clear();
  }

  /** The repository takes ownership of the instance. */
  template<class T>
  void
  UnmanagedRepository<T>::add(std::string const& _name, T * _instance)
  throw(EAlreadyRegistered)
  {
    if (instances_.find(_name) == instances_.end()) {
      instances_.insert(std::make_pair(_name, _instance));
      return;
    }
    throw EAlreadyRegistered("add failed, \""+_name+"\" already in repository");
  }

  /**
   * @return Instance registered under the specified name.
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  template<class T>
  T *
  UnmanagedRepository<T>::get(const std::string& _name) throw(ENotRegistered)
  {
    typename InstanceMap::const_iterator i = instances_.find(_name);
    if (i != instances_.end())
      return i->second;
    throw ENotRegistered("get failed, \""+_name+"\" not in repository");
  }

  /**
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  template<class T>
  void
  UnmanagedRepository<T>::remove(const std::string& _name) throw(ENotRegistered)
  {
    typename InstanceMap::iterator i = instances_.find(_name);
    if (i != instances_.end()) {
      delete i->second;
      instances_.erase(i);
      if (instances_.size() == 0)
        SingletonType::ACE_Singleton_Type::close();
      return;
    }
    throw ENotRegistered("remove failed, \""+_name+"\" not in repository");
  }

  /**
   * Prints the names of all registered instances to the stream
   */
  template<class T>
  void
  UnmanagedRepository<T>::printToStream(std::ostream& _ostr) const
  {
    typename InstanceMap::const_iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first) {
      _ostr << first->first << std::endl;
    }
  }

  template<class T>
  std::ostream&
  operator << (ostream& _ostr, UnmanagedRepository<T> const& _repository)
  {
    _repository.printToStream(_ostr);
    return _ostr;
  }
}
#endif // Miro_Repository_h


