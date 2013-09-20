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
#include "HelloWorldC.h"

#include "miro/Client.h"
#include "miro/Log.h"

#include <iostream>

int main (int argc, char * argv[])
{
  // initialize log output
  Miro::Log::init(argc, argv);

  try {
    // client helper class instance
    Miro::Client client(argc, argv);
   
    // resolve the object reference
    XMP::HelloWorld_var hello_world = client.resolveName<XMP::HelloWorld>("HelloWorld");
    
    // invoke a method on the remote object
    hello_world->say("Hello World");
  }
  catch (CORBA::Exception const& e) {
    std::cerr << "CORBA Exception: " << e << std::endl;
  }
  catch (Miro::Exception const& e) {
    std::cerr << "Miro Exception: " << e << std::endl;
  }
  catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }
  return 0;
}
