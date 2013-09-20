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
#ifndef XMP_BiDirImpl_h
#define XMP_BiDirImpl_h

#include "BiDirS.h"

namespace XMP
{
  class FooImpl : public POA_XMP::Foo
  {
  public:
    FooImpl();
    virtual ~FooImpl();
    virtual void callMeBack() throw();
  };

  class BarImpl : public POA_XMP::Bar
  {
  public:
    BarImpl();
    virtual ~BarImpl();
    virtual void willCallBack(XMP::Foo_ptr f) throw();

    Foo_var foo;
  };
}

#endif // XMP_BiDirImpl_h
