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
#ifndef miro_Client_h
#define miro_Client_h

#include "Exception.h"
#include "miro_Export.h"

#include <tao/Version.h>
#include <tao/Object.h>

#include <ace/ace_wchar.h>

#include <string>
#include <iostream>

// forward declarations
#  if TAO_MAJOR_VERSION > 1 || \
  (TAO_MAJOR_VERSION == 1  && TAO_MINOR_VERSION > 5) ||			\
  (TAO_MAJOR_VERSION == 1 && TAO_MINOR_VERSION == 5 && TAO_BETA_VERSION >= 3)
#include <tao/PortableServer/PS_ForwardC.h>
#  else
class TAO_ServantBase;
class TAO_ServantBase_var;
namespace PortableServer
{
  typedef ::TAO_ServantBase ServantBase;
  typedef ::TAO_ServantBase_var ServantBase_var;
}
#  endif
namespace CosNaming
{
  class Name;
  class NamingContext;
}

namespace Miro
{
  // forward declarations
  class RobotParameters;
  typedef RobotParameters ClientParameters;
  class ClientData;


  //! Global helper method. Matches CORBA down-casts to the C++ dynamic cast syntax.
  /* It takes a pointer to a CORBA object and returns a pointer to the object type
   * identified in the template parameter.
   */
  template<class T>
  T *
  CORBA_dynamic_cast(CORBA::Object * _obj)
  {
    return T::_narrow(_obj);
  }

  //! Base class for all MIRO client applications.
  /**
   * It tries to encapsulate the CORBA stuff, so that you need
   * only to have a minimal idea of what CORBA is to use MIRO.
   * Anyhow, it doesn't restrict you in the use of CORBA.
   *
   * Note that you can have multiple Client objects. But make sure,
   * you passed the command line parameters to the first instance, if
   * you want to allow the processing of ORB command line parameters.
   */
  class miro_Export Client
  {
  public:

    //! Initializing constructor.
    /**
     * The ORB is initialized on construction of this class.  The argc
     * and argv are passed on the CORBA::ORB_init() global helper
     * function and the ORB specific command line options are removed
     * from the argument vector.
     *
     * Feel free to call it with argc=0 and argv=NULL if you happen to
     * need another client instance, and have done command line argument
     * parsing somewhen before.
     */
    Client(int& argc = _dummy_argc, ACE_TCHAR *argv[] = NULL);

    /**
     * Destructor.
     */
    ~Client();

    //! Accessor which returns the ORB pointer.
    /**
     * Following the normal CORBA memory management rules of return
     * values from functions, this function duplicates the orb return
     * value before returning it.
     */
    CORBA::ORB_ptr orb() const;

    //! Helper functions to resolve initial contexts from the ORB.
    /**
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * resolveInit(char const * _id) const
    throw(CORBA::Exception);
    //! Helper functions to resolve initial contexts from the ORB.
    /**
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * resolveInit(std::string const& _id) const
    throw(CORBA::Exception);

    /**
     * Helper functions to resolve names from the name service,
     * using the clients naming context.
     *
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * resolveName(char const * _name) const
    throw(CORBA::Exception, Miro::Exception);
    /**
     * Helper functions to resolve names from the name service,
     * using the clients naming context.
     *
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * resolveName(std::string const& _name) const
    throw(CORBA::Exception, Miro::Exception);

    /**
     * Helper functions to resolve names from the name service,
     * using the clients naming context.
     *
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * tryResolveName(char const * _name) const
    throw();
    /**
     * Helper functions to resolve names from the name service,
     * using the clients naming context.
     *
     * See "Advanced CORBA programming in C++" for details.
     */
    template<class T>
    T * tryResolveName(std::string const& _name) const
    throw();

    std::string name2FullName(char const * _path) const
    throw();

    void path2Name(char const * _path, CosNaming::Name& _name) const
    throw();

    CORBA::Object *
    setRoundTripTimeout(CORBA::Object * obj, ACE_Time_Value const& _timeout) const;

    CORBA::Object *
    resolveInitialObject(char const * _id) const
    throw(CORBA::Exception);
    CosNaming::NamingContext * getNamingContext(CosNaming::Name const& path) const;

    CORBA::Object *
    resolveNamedObject(char const * _name) const
    throw(CORBA::Exception, Miro::Exception);
    CORBA::Object *
    tryResolveNamedObject(char const * _name) const
    throw();

  protected:
    ClientParameters * const _params;
    ClientData * _data;

    static int _dummy_argc;

  private:
    //! Copy construction prohibited.
    Client(Client const&) : _params(NULL) {}
    //! Assignement operation prohibited.
    Client & operator = (Client const&) {
      return *this;
    }
  };

  //----------------------------------------------------------------------------
  // Template implementations
  //----------------------------------------------------------------------------
  template<class T>
  T *
  Client::resolveInit(char const * _id) const throw(CORBA::Exception)
  {
    CORBA::Object_var obj = resolveInitialObject(_id);

    typename T::_var_type ref;
    try {
      ref = CORBA_dynamic_cast<T>(obj);
    }
    catch (const CORBA::Exception & /*e*/) {
      std::cerr << "Cannot narrow reference for " << _id << std::endl;
      throw;
    }
    if (CORBA::is_nil(ref.in())) {
      std::cerr << "Incorrect type of reference: " << _id << std::endl;
      throw 0;
    }
    return ref._retn();
  }

  //----------------------------------------------------------------

  template<class T>
  T *
  Client::resolveName(char const * _name)
  const throw(CORBA::Exception, Miro::Exception)
  {
    CORBA::Object_var obj = resolveNamedObject(_name);

    typename T::_var_type ref;
    try {
      ref = CORBA_dynamic_cast<T>(obj);
    }
    catch (const CORBA::Exception & /*e*/) {
      throw Exception(std::string("Cannot narrow reference for ") + _name);
    }
    if (CORBA::is_nil(ref.in())) {
      throw Exception(std::string("Reference has incorrect type: ") + _name);
    }
    return ref._retn();
  }

  template<class T>
  T *
  Client::tryResolveName(char const * _name)
  const throw()
  {
    CORBA::Object_var obj = tryResolveNamedObject(_name);

    typename T::_var_type ref;
    try {
      ref = CORBA_dynamic_cast<T>(obj);
    }
    catch (const CORBA::Exception & /*e*/) {
    }
    return ref._retn();
  }

  template<class T>
  T *
  Client::resolveInit(std::string const& _id) const throw(CORBA::Exception)
  {
    return this->resolveInit<T>(_id.c_str());
  }

  template<class T>
  T *
  Client::resolveName(std::string const& _name)
  const throw(CORBA::Exception, Miro::Exception)
  {
    return this->resolveName<T>(_name.c_str());
  }

  template<class T>
  T *
  Client::tryResolveName(std::string const& _name)
  const throw()
  {
    return this->tryResolveName<T>(_name.c_str());
  }
}

#endif // miro_Client_h




