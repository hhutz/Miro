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
#include <tao/ORB.h>
#include <tao/PolicyC.h>

#include <iostream>

int main (int argc, char * argv[])
{
   CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

   try {
     CORBA::Object_var obj = orb->string_to_object(argv[1]);
     CORBA::PolicyList_var pl;
     obj->_validate_connection(pl);
   }
   catch(CORBA::Exception const& e) {
     std::cerr << "Exception: " << e << std::endl;
   }
   return 0;
}
