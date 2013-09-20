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
#include "miro/Log.h"
#include "miro/Client.h"

#include <orbsvcs/Time_Utilities.h>
#include <tao/AnyTypeCode/Any.h>
#include <tao/Messaging/Messaging_TypesC.h>
#include <tao/PolicyC.h>

#include <iostream>

using namespace std;

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  Miro::Client client(argc, argv);

  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " object_name\n";
    return 1;
  }

  try {
    ACE_Time_Value timeout(0, 300000);
    TimeBase::TimeT roundTripTimeoutVal;
    ORBSVCS_Time::Time_Value_to_TimeT(roundTripTimeoutVal, timeout);
      
    CORBA::Any anyObjectVal;
    anyObjectVal <<= roundTripTimeoutVal;
    CORBA::PolicyList polList (1);
    polList.length (1);
    
    CORBA::ORB_var orb = client.orb();
    polList[0] = orb->create_policy (Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
				     anyObjectVal);
    
    CORBA::Object_var obj = client.resolveName<CORBA::Object>(argv[1]);
    cout << ".." << endl;

    CORBA::Object_var obj2 = obj->_set_policy_overrides(polList, CORBA::SET_OVERRIDE);
    polList[0]->destroy();
    polList[0] = CORBA::Policy::_nil ();

    cout << "asdf" << endl;
    CORBA::PolicyList_var pl;
    obj2->_validate_connection(pl);

    cout << "finished" << endl;
  }
  catch (CORBA::Exception const& e) {
    cerr << "Caught CORBA exception: " << endl 
	 << e << endl;
  } 
  return 0;
}
