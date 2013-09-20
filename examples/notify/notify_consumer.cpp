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
#include "miro/NotifyTypedConsumer.h"
#include "miro/Log.h"

#include <functional>
#include <iostream>

std::ostream& operator << (std::ostream& ostr, XMP::Payload const& rhs)
{
  ostr << "a=" << rhs.a << " b=" << rhs.b;
  return ostr;
}

class My_Notify_Event_Handler : public std::unary_function<XMP::Payload const *, void>
{
public:
  result_type operator() (argument_type event) throw() {
    std::cout << (*event) << std::endl;
  }
};

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Server server(argc, argv);
    CosNotifyChannelAdmin::EventChannel_var ec =
      server.resolveName<CosNotifyChannelAdmin::EventChannel>("NotifyEventChannel");
    server.detach();
   
    My_Notify_Event_Handler event_handler; 
    Miro::NotifyTypedConsumer<My_Notify_Event_Handler> 
      event_consumer(event_handler, ec, "test", "my_robot");
    server.wait();

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
