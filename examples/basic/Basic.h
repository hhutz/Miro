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
#ifndef XMP_Basic_h
#define XMP_Basic_h

#include "miro/DummyCORBA.h"
#include "miro/Sequence.h"

namespace POA_XMP
{
  class  Basic;
}

namespace XMP
{
  struct  Foo
  {
    int f[2];
  };
  typedef Miro::Sequence<Foo> FooSequence;

  struct  Bar
  {
    Miro::Sequence<int> b;
  };

  class EFoo
  {
  public:
    std::string what;

    EFoo() {}
    EFoo(std::string const& w) : what(w) {}
  };

  using ::XMP::Basic;
  typedef ::XMP::Basic * Basic_var;
}

namespace POA_XMP
{
  class  Basic : public virtual Miro::ServantBase
  {
  public:
    virtual void s0 () throw () = 0;  
    virtual int s1 (int p1, int& p2, int& p3) throw () = 0;   
    virtual char * s2 (const char * p1, char * p2, char *& p3) throw () = 0;
    virtual XMP::Foo s3 (const XMP::Foo & p1, XMP::Foo * p2, XMP::Foo & p3) throw () = 0;
    virtual XMP::FooSequence * s4 (const XMP::FooSequence & p1, XMP::FooSequence * p2, XMP::FooSequence & p3) throw () = 0;
    virtual XMP::Bar * s5 (const ::XMP::Bar & p1, ::XMP::Bar_out p2, ::XMP::Bar & p3) throw () = 0;
    virtual Object * s6 (Object * p1, Object * p2, Object * & p3) throw () = 0;
    virtual void e0 () throw (XMP::EFoo) = 0;
  };
}

#endif // XMP_BasicImpl_h
