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
#ifndef xmp_hello_world_impl_h
#define xmp_hello_world_impl_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "HelloWorldS.h"

namespace XMP
{
  class HelloWorldImpl : public virtual POA_XMP::HelloWorld
  {
  public:
    HelloWorldImpl();
    virtual ~HelloWorldImpl();

    virtual void say(char const * hello) throw ();
  };
}
#endif // xmp_hello_world_impl_h
