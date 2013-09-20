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
#include "PayloadC.h"

#include "miro/Server.h"
#include "miro/NotifyTypedSupplier.h"
#include "miro/Log.h"

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Server server(argc, argv);         // a event suppliers is a servers
    CosNotifyChannelAdmin::EventChannel_var ec = // get ec reference
      server.resolveName<CosNotifyChannelAdmin::EventChannel>("NotifyEventChannel");
    server.detach(1);                       // start server event loop

    // we create a supplier for
    // "test" events from "my_robot" with payload <XMP::Payload>
    Miro::NotifyTypedSupplier<XMP::Payload> supplier(ec, "test", "my_robot");

    XMP::Payload payload = { 3, 4.f };      // initialize payload
    supplier.sendEvent(payload);           // send 1 event
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
