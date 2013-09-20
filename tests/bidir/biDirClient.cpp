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
#include "BiDirImpl.h"

#include "miro/Server.h"
#include "miro/Log.h"

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Server server(argc, argv);
    std::string name = "Foo";
    if (argc > 1)
      name = argv[1];
    XMP::FooImpl * servant = new XMP::FooImpl();
    server.activateNamedObject(name, servant);
    server.detach();

    XMP::Bar_var bar =
      server.resolveName<XMP::Bar>("Bar");
    XMP::Foo_var foo = servant->_this();

    bar->willCallBack(foo);

    server.run();
  }
  catch (CORBA::Exception const& e) {
    MIRO_LOG_OSTR(LL_CRITICAL, "CORBA exception:\n" << e);
  }
  return 0;
}
