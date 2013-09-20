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
#include "BasicImpl.h"
#include "miro/Client.h"

#include <iostream>

namespace XMP
{
  using namespace std;

  void BasicImpl::s0 ()
    throw (::CORBA::SystemException)
  {
    //we do nothing here

    cout << "s0 called" << endl;
  }

  /** Server side mapping for basic types.
   * Pretty straight forward, even though the _out paramter suggests differently.
   * In reality its just a typedef for a non-const reference.
   */
  ::CORBA::Long BasicImpl::s1 (::CORBA::Long p1, ::CORBA::Long_out p2, ::CORBA::Long & p3)
    throw (::CORBA::SystemException)
  {
    std::cout << "received p1 in: " << p1 << std::endl;
    std::cout << "received p3 inout: " << p3 << std::endl;

    p2 = p3;
    p3 = p1;

    return p1;
  }

  /** Server side mapping for strings.
   * We do not use wide strings.
   */
  char * BasicImpl::s2 (const char * p1, ::CORBA::String_out p2, char *& p3)
    throw (::CORBA::SystemException)
  {
    std::cout << "received p1 in: " << p1 << std::endl;
    std::cout << "received p3 inout: " << p3 << std::endl;

    // out parameters are unassigned when entering the method
    p2 = CORBA::string_dup(p3);
    // but we need to free the string in the inout parameter
    // if we assign it something new...
    CORBA::string_free(p3);
    p3 = CORBA::string_dup(p1);

    // we release ownership of all memory we point to in return values
    // so also the return value needs to be freshly allocated
    return CORBA::string_dup(p1);
  }

  /** Server side mapping for basic types.
   * Pretty straight forward, even though the _out paramter suggests differently.
   * In reality its just a typedef for a non-const reference.
   */
  ::XMP::Foo BasicImpl::s3 (const ::XMP::Foo & p1, ::XMP::Foo_out p2, ::XMP::Foo & p3)
    throw (::CORBA::SystemException)
  {
    std::cout << "received p1 in: " << p1.f[0] << " " << p1.f[1] << std::endl;
    std::cout << "received p3 inout: " << p3.f[0] << " " << p3.f[1]  << std::endl;

    p2 = p3;
    p3 = p1;

    return p1;
  }

  /** Server side mapping for variable length data types.
   * These behave similar to strings and raw memory arrays, except, 
   * that we can rely on the builtin allocators (new/delete).
   */
  ::XMP::FooSequence * BasicImpl::s4 (const ::XMP::FooSequence & p1, ::XMP::FooSequence_out p2, ::XMP::FooSequence & p3)
    throw (::CORBA::SystemException)
  {
    std::cout << "received p1 in: ";
    for (unsigned int i = 0; i < p1.length(); ++i)
      std::cout << p1[i].f[0] << " " << p1[i].f[1] << ",";
    std::cout << std::endl;
    std::cout << "received p3 inout: ";
    for (unsigned int i = 0; i < p3.length(); ++i)
      std::cout << p3[i].f[0] << " " << p3[i].f[1] << ",";
    std::cout << std::endl;

    // out paramters are uninizialized when entering the method
    // it is actually a reference to a pointer, so we heap-allocate it
    p2 = new ::XMP::FooSequence(p3);
    // inout parameters are passed by reference, which simplifies things.
    p3 = p1;

    // return values are handled just like out parameters.
    return new ::XMP::FooSequence(p1);
  }

  /** Server side mapping for variable length data types.
   * Structs which hold sequences or strings are considered
   * variable length data types just the same.
   * These behave similar to strings and raw memory arrays, except, 
   * that we can rely on the builtin allocators (new/delete).
   */
  ::XMP::Bar * BasicImpl::s5 (const ::XMP::Bar & p1, ::XMP::Bar_out p2, ::XMP::Bar & p3)
    throw (::CORBA::SystemException)
  {
    std::cout << "received p1 in: ";
    for (unsigned int i = 0; i < p1.b.length(); ++i)
      std::cout << p1.b[i] << " ";
    std::cout << std::endl;
    std::cout << "received p3 inout: ";
    for (unsigned int i = 0; i < p3.b.length(); ++i)
      std::cout << p3.b[i] << " ";
    std::cout << std::endl;

    // out paramters are uninizialized when entering the method
    // it is actually a reference to a pointer, so we heap-allocate it
    p2 = new ::XMP::Bar(p3);
    // inout parameters are passed by reference, which simplifies things.
    p3 = p1;

    // return values are handled just like out parameters.
    return new ::XMP::Bar(p1);
  }

  ::CORBA::Object_ptr BasicImpl::s6 (::CORBA::Object_ptr p1, ::CORBA::Object_out p2, ::CORBA::Object_ptr & p3)
    throw (::CORBA::SystemException)
  {
    // transform the object references to strings
    // this is only for the ostream operator
    // so we do not need this actually...

    // obtain a reference to the orb. 
    // as we know that Client holds a reference,
    // we get that
    Miro::Client c;
    CORBA::ORB_var orb = c.orb();
    // stringify the object references
    // this does not provide very meaningfull output, though...
    CORBA::String_var s_p1 = orb->object_to_string (p1);
    CORBA::String_var s_p3 = orb->object_to_string (p3);


    std::cout << "received p1 in: " << s_p1 << std::endl;
    std::cout << "received p3 inout: " << s_p3 << std::endl;

    // This creates a new servant proxy, which becomes the
    // responsibility of the caller.
    p2 = _this();
    // CORBA does reference counting on object references, 
    // so we need to deminish the reference count, if we 
    // reset an inout argument
    CORBA::release(p3);
    // we need to 
    p3 = CORBA::Object::_duplicate(p1);

    return CORBA::Object::_duplicate(p1);
  }

  void BasicImpl::e0 ()
  throw (::CORBA::SystemException, ::XMP::EFoo)
  {
    throw EFoo("This is an expected exception!");
  }
}

