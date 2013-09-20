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
#ifndef miro_Singleton_h
#define miro_Singleton_h

#include <ace/Synch.h>
#include <ace/Singleton.h>

namespace Miro
{
  //! Templated implementation of the singleton pattern.
  /**
   * This implements a heap allocated global variable
   * with a global helper method as a accessor method.
   *
   * Example code:
   *
   * To get a pseudo global variable for class T it looks the following.
   *
   * class T
   * {
   *   ...
   *   Miro::Singleton<T> instance;
   * };
   *
   * Access:
   *
   * T * t = T::instance();
   *
   * There's a paper about the singleton pattern and
   * double checked locking on the ACE web pages.
   */
  template < class TYPE,
  class LOCK = ACE_Recursive_Thread_Mutex,
  template<class TYPE, class LOCK> class ACE_SINGLETON = ACE_Singleton >
  class Singleton
  {
  public:
    //! Access operator to the global variable.
    TYPE * operator()();
    //! Public type for friend declarations.
    typedef ACE_SINGLETON<TYPE, LOCK> ACE_Singleton_Type;
  };

  //--------------------------------------------------------------------------
  // Inlined methods
  //--------------------------------------------------------------------------
  template < class TYPE,
  class LOCK,
  template<class TYPE, class LOCK> class ACE_SINGLETON >
  inline
  TYPE *
  Singleton<TYPE, LOCK, ACE_SINGLETON>::operator()()
  {

    return ACE_SINGLETON<TYPE, LOCK>::instance();
  }
}

#endif // miro_Singleton_h

