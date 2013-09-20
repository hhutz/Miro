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
#include "HelloWorldImpl.h"

#include "miro/Server.h"
#include "miro/Log.h"

#include <iostream>

int main (int argc, char * argv[])
{ 
  // initialize log output
  Miro::Log::init(argc, argv);
  try {
    // server helper class
    Miro::Server server(argc, argv);
    
    // create the servant   
    XMP::HelloWorldImpl * servant = new XMP::HelloWorldImpl();

    // and hand it to the orb 
    server.activateNamedObject("MyWorld", servant);

    // run the server event loop in the background
    server.detach();

    // get a reference to the proxy object
    XMP::HelloWorld_var hello_world = server.resolveName<XMP::HelloWorld>("MyWorld");
    
    // invoke a method on the remote object
    hello_world->say("Hello World!");
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
