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
#include "Client.h"
#include "ClientParameters.h"
#include "ClientData.h"
#include "Robot.h"
#include "Log.h"
#include "NamingRepository.h"
#include "Configuration.h"

#  include <orbsvcs/CosNamingC.h>
#  include <orbsvcs/Time_Utilities.h>

// Added here to link statically with the advanced resource factory
#  include <tao/Strategies/advanced_resource.h>

#  include <tao/AnyTypeCode/Any.h>
#  include <tao/Messaging/Messaging_TypesC.h>
#  include <tao/PolicyC.h>

#include <ace/Time_Value.h>
#include <ace/Arg_Shifter.h>
#include <ace/OS_NS_strings.h>

#include <string>
#include <vector>

namespace Miro
{
  using namespace std;
  typedef vector<string> StringVector;

  int Client::_dummy_argc = 0;

  Client::Client(int& argc, ACE_TCHAR* argv[]) :
      _params(ClientParameters::instance()),
      _data(ClientData::instance(argc, argv))
  {
    MIRO_LOG_CTOR("Client");

    if (_data->refcount_value() == 1) {

      Robot::init(argc, argv);

      // Resolve the naming context in use
      if (!_params->noNaming) {
        // Get reference to NameService
        try {
          CORBA::Object_var obj = resolveInitialObject("NameService");
          obj = setRoundTripTimeout(obj,  _params->namingServiceTimeout);
          _data->initialNamingContext = CORBA_dynamic_cast<CosNaming::NamingContext>(obj);
        }
        catch (CORBA::SystemException const& e) {
          MIRO_LOG_OSTR(LL_CRITICAL, "Exception resolving root naming context: " << e);
          throw;
        }

        // Attempt to create naming context.
        CosNaming::Name n;
        n.length(1);
        n[0].id = _params->namingContextName.c_str();
        _data->namingContext = getNamingContext(n);
      }
    }
    MIRO_LOG_CTOR_END("Client");
  }

  Client::~Client()
  {
    _data->remove_ref();
  }

  // Return the corba orb reference.
  CORBA::ORB_ptr
  Client::orb() const
  {
    return CORBA::ORB::_duplicate(_data->orb);
  }

  CORBA::Object *
  Client::setRoundTripTimeout(CORBA::Object * obj, ACE_Time_Value const& _timeout) const
  {
    if (_timeout == ACE_Time_Value::zero)
      return CORBA::Object::_duplicate(obj);

    TimeBase::TimeT roundTripTimeoutVal;
    ORBSVCS_Time::Time_Value_to_TimeT(roundTripTimeoutVal, _timeout);

    CORBA::Any anyObjectVal;
    anyObjectVal <<= roundTripTimeoutVal;
    CORBA::PolicyList polList(1);
    polList.length(1);

    polList[0] = _data->orb->create_policy(Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
                                           anyObjectVal);

    try {
      CORBA::Object_var obj2 = obj->_set_policy_overrides(polList, CORBA::SET_OVERRIDE);
      polList[0]->destroy();
      return obj2._retn();

    }
    catch (CORBA::Exception const& /*e*/) {
      polList[0]->destroy();
      throw;
    }
  }

  CORBA::Object *
  Client::resolveInitialObject(char const * _id) const
  throw(CORBA::Exception)

  {
    CORBA::Object_var obj;
    try {
      obj = _data->orb->resolve_initial_references(_id);
    }
    catch (const CORBA::ORB::InvalidName & /*e*/) {
      throw;
    }
    catch (const CORBA::Exception & /*e*/) {
      std::cerr << "Cannot get initial reference for " << _id << std::endl;
      throw;
    }

    if (CORBA::is_nil(obj.in())) {
      std::cerr << "Cannot get initial reference for " << _id << std::endl;
      throw 0;
    }

    return obj._retn();
  }

  void
  Client::path2Name(char const * _path, CosNaming::Name& name) const throw()
  {
    std::string fullName = name2FullName(_path);
    StringVector path;
    unsigned int first = 0;
    unsigned int last = 0;

    while (last < fullName.length()) {
      first = last;
      unsigned int current;
      do {
        current = last;
        ++last;
      }
      while (current < fullName.length() && fullName[current] != '/');

      if (first < current) {
        path.push_back(fullName.substr(first, current - first));
      }
    }

    name.length(path.size());
    for (unsigned int i = 0; i < path.size(); ++i) {
      name[i].id = path[i].c_str();
    }
  }

  CORBA::Object *
  Client::resolveNamedObject(char const * name)  const
  throw(CORBA::Exception, Miro::Exception)
  {
    CORBA::Object_var obj;
    if (_params->noNaming) {
      obj = NamingRepository::instance()->get(name2FullName(name));
    }
    else {
      CosNaming::Name n;
      path2Name(name, n);
      try {
        obj = _data->initialNamingContext->resolve(n);
      }
      catch (CORBA::Exception const& e) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "Exception resolving object reference at naming service: "
                      << name << endl
                      << e);
        throw;
      }

      if (CORBA::is_nil(obj)) {
        throw Exception(std::string("Nil binding in Naming Service for ") + name);
      }
    }

    return obj._retn();
  }

  CORBA::Object *
  Client::tryResolveNamedObject(char const * name) const throw()
  {
    CORBA::Object_var obj = NULL;

    if (_params->noNaming) {
      try {
        obj = NamingRepository::instance()->get(name2FullName(name));
      }
      catch (Exception const&) {
      }
    }
    else {
      try {
        CosNaming::Name n;
        path2Name(name, n);

        obj = _data->initialNamingContext->resolve(n);
      }
      catch (const CORBA::Exception & e) {
        MIRO_LOG_OSTR(LL_NOTICE,
                      "Exception on tryResolve on object reference at naming service: "
                      << name << endl
                      << e);
      }
    }

    return obj._retn();
  }

  std::string
  Client::name2FullName(char const * path) const throw()
  {
    if (path[0] == '/') {
      return path;
    }
    return _params->namingContextName + "/" + path;
  }


  CosNaming::NamingContext *
  Client::getNamingContext(CosNaming::Name const& path) const
  {
    CosNaming::NamingContext_var context = _data->initialNamingContext;
    for (CORBA::ULong i = 0; i < path.length(); ++i) {

      // Attempt to create naming context.
      CosNaming::Name n;
      n.length(1);
      n[0] = path[i];
      try {
        CORBA::Object_var obj = context->resolve(n);
        CORBA::Object_var obj2 = setRoundTripTimeout(obj, ACE_Time_Value(5));
        context = CORBA_dynamic_cast<CosNaming::NamingContext>(obj2);
      }
      catch (const CosNaming::NamingContext::NotFound & /*e*/) {
        context = context->bind_new_context(n);
      }
    }
    return context._retn();
  }

}
