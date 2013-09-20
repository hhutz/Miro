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
#include "NamingRepository.h"

#include <tao/Object.h>

namespace Miro
{
  Singleton<NamingRepository, ACE_SYNCH_RECURSIVE_MUTEX> 
  NamingRepository::instance = 
    Singleton<NamingRepository, ACE_SYNCH_RECURSIVE_MUTEX>();


  /**
   * To get a pointer to the global Repository instance,
   * simply call Repository::instance();
   */
  // removing this for sake of windows stupidity:
  // please declare your own, unique instance of the repository singleton
  //  template<class T>
  //  Singleton<Repository<T> > Repository<T>::instance;

  NamingRepository::NamingRepository() throw()
  {}

  NamingRepository::~NamingRepository()
  {
    clear();
  }

  void
  NamingRepository::clear() throw()
  {
    InstanceMap::iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first)
      first->second->_remove_ref();
    instances_.clear();
  }

  /** The repository takes ownership of the instance. */
  void
  NamingRepository::add(std::string const& _name, CORBA::Object * _instance)
  throw(EAlreadyRegistered)
  {
    if (instances_.find(_name) == instances_.end()) {
      instances_.insert(std::make_pair(_name, CORBA::Object::_duplicate(_instance)));
      return;
    }
    throw EAlreadyRegistered(_name);
  }

  /**
   * @return Instance registered under the specified name.
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  CORBA::Object *
  NamingRepository::get(const std::string& _name) throw(ENotRegistered)
  {
    InstanceMap::const_iterator i = instances_.find(_name);
    if (i != instances_.end())
      return CORBA::Object::_duplicate(i->second);
    throw ENotRegistered(_name);
  }

  /**
   * @throw ENotRegistered
   * if no instance is registered under the specified name.
   */
  void
  NamingRepository::remove(const std::string& _name) throw(ENotRegistered)
  {
    InstanceMap::iterator i = instances_.find(_name);
    if (i != instances_.end()) {
      i->second->_remove_ref();
      instances_.erase(i);
      return;
    }
    throw ENotRegistered(_name);
  }

  /**
   * Prints the names of all registered instances to the stream
   */
  void
  NamingRepository::printToStream(std::ostream& _ostr) const
  {
    InstanceMap::const_iterator first, last = instances_.end();
    for (first = instances_.begin(); first != last; ++first) {
      _ostr << first->first << std::endl;
    }
  }

  std::ostream&
  operator << (ostream& _ostr, NamingRepository const& _repository)
  {
    _repository.printToStream(_ostr);
    return _ostr;
  }
}
