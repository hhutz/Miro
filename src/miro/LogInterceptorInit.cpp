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
#include "LogInterceptorInit.h"
#include "LogInterceptor.h"

#include <iostream>
#include <exception>

namespace Miro
{
  using namespace std;

  LogInterceptorInit::LogInterceptorInit()
  {
  }

  void
  LogInterceptorInit::pre_init(PortableInterceptor::ORBInitInfo_ptr)
  throw(CORBA::SystemException)
  {
  }

  void
  LogInterceptorInit::post_init(PortableInterceptor::ORBInitInfo_ptr info)
  throw(CORBA::SystemException)
  {
    // Create and register the request interceptors.
    try {
      PortableInterceptor::ServerRequestInterceptor_var si_interceptor =
        new LogInterceptor();
      info->add_server_request_interceptor(si_interceptor);
    }
    catch (exception const&) {
      cerr << "Std exception occured trying to create LogInterceptor." << endl;
    }
  }
}
