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
#ifndef XMP_BasicImpl_h
#define XMP_BasicImpl_h

#include "BasicS.h"

namespace XMP
{
  class  BasicImpl : public virtual POA_XMP::Basic
  {
  public:
    virtual void s0 ()
      throw (::CORBA::SystemException);
  
    virtual ::CORBA::Long s1 (::CORBA::Long p1, ::CORBA::Long_out p2, ::CORBA::Long & p3) 
      throw (::CORBA::SystemException);
    
    virtual char * s2 (const char * p1, ::CORBA::String_out p2, char *& p3)
      throw (::CORBA::SystemException);
    
    virtual ::XMP::Foo s3 (const ::XMP::Foo & p1, ::XMP::Foo_out p2, ::XMP::Foo & p3)
      throw (::CORBA::SystemException);
    
    virtual
    ::XMP::FooSequence * s4 (const ::XMP::FooSequence & p1, ::XMP::FooSequence_out p2, ::XMP::FooSequence & p3)
      throw (::CORBA::SystemException);
    
    virtual ::XMP::Bar * s5 (const ::XMP::Bar & p1, ::XMP::Bar_out p2, ::XMP::Bar & p3)
      throw (::CORBA::SystemException);
    
    virtual ::CORBA::Object_ptr s6 (::CORBA::Object_ptr p1, ::CORBA::Object_out p2, ::CORBA::Object_ptr & p3)
      throw (::CORBA::SystemException);
    
    virtual void e0 ()
      throw (::CORBA::SystemException, ::XMP::EFoo);
  };
}
#endif // XMP_BasicImpl_h
