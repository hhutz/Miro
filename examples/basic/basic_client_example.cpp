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
#include "BasicC.h"

#include "miro/Client.h"
#include "miro/Log.h"

#include <iostream>

std::ostream& operator << (std::ostream& ostr, XMP::Foo const& rhs) {
  ostr << rhs.f[0] << " " << rhs.f[1];
  return ostr;
} 

std::ostream& operator << (std::ostream& ostr, XMP::FooSequence const& rhs) {
  for (unsigned int i = 0; i < rhs.length(); ++i)
    ostr << "(" << rhs[i] << ")";
  return ostr;
} 

std::ostream& operator << (std::ostream& ostr, XMP::Bar const& rhs) {
    ostr << "(";
  for (unsigned int i = 0; i < rhs.b.length(); ++i)
    ostr << rhs.b[i];
  std::cout << ")";
  return ostr;
} 

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Client client(argc, argv);
    
    std::string name = "Basic";
    if (argc > 1)
      name = argv[1];
    XMP::Basic_var basic = client.resolveName<XMP::Basic>(name);

    //------------------------------------------------------------------------
    // invoke a method without parameters
    basic->s0();

    //------------------------------------------------------------------------
    // invoke a method with a basic type as parameters.
    {
      // CORBA::Long, CORBA::Short, CORBA::Float, CORBA::Double ... are typedefs.
      // using int, short, float & double is reasonable portable
      // also on 64 bit linux, an int is 32 bits
      // and float/double correspond to IEEE floating point
      // the risk is, that other orbs may use i.e. long instead of int
      // this would break out and inout parameters
      // this is not an multi-orb interoperability issue, 
      // but might bite when switching to another orb

      // simplified usage
      {
	int o, io = 2;
	std::cout << "in: p1=" << 1 << " p3=" << io << std::endl;
	int r = basic->s1(1, o, io);
	std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
      }

      // CORBA style
      {
	CORBA::Long o, io = 2;
	std::cout << "in: p1=" << 1 << " p3=" << io << std::endl;
	CORBA::Long r = basic->s1(1, o, io);
	std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
      }
    }

    //------------------------------------------------------------------------
    // invoke a a method with string type parameters.
    {
      // this version directly uses char *
      // memory management has to be done by hand
      // note that an exception in s2() would leave memory leaks

      char const * i = "in";
      char * o, * io = CORBA::string_dup("test");
      std::cout << "in: p1=" << 1 << " p3=" << io << std::endl;
      char * r = basic->s2(i, o, io);
      std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;

      CORBA::string_free(o);  // delete o; works with TAO, but orbs are
      CORBA::string_free(io); // free to implement their own memory management...
    }

    {
      // using the CORBA string helper class
      // it takes care of memory management

      CORBA::String_var in = "in";
      CORBA::String_var o, io = "test";
      std::cout << "in: p1=" << 1 << " p3=" << io << std::endl;
      CORBA::String_var r = basic->s2(in, o, io);
      std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
    }

    {
      // using std::string 
      // with generic conversion to string helpers

      std::string in = "in";
      std::string o, io = "test";
      std::string r;
      std::cout << "in: p1=" << 1 << " p3=" << io << std::endl;

      // generic conversion of method parameters
      CORBA::String_var h_in = in.c_str();
      CORBA::String_var h_o, h_io = io.c_str();

      CORBA::String_var h_r = basic->s2(h_in, h_o, h_io);

      // generic conversion of return parameters
      o = h_o;
      io = h_io;
      r = h_r;

      std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
    }

    //------------------------------------------------------------------------
    // invoke a a method with fixed size structure type parameters.
    {
      XMP::Foo i = { {1, 2} };
      XMP::Foo o, io = { {5, 3} };
      std::cout << "in: p1=" << i << " p3=" << io << std::endl;
      XMP::Foo r = basic->s3(i, o, io);
      std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
    }

    //------------------------------------------------------------------------
    // invoke a a method with variable size structure type parameters.

    // using raw pointer
    {
      XMP::FooSequence i;
      XMP::FooSequence * o;
      XMP::FooSequence io;

      // initializing the in parameter
      i.length(2);
      i[0].f[0] = 0;
      i[0].f[1] = 1;
      i[1].f[0] = 2;
      i[1].f[1] = 3;
      // initializing the inout parameter
      io.length(2);
      io[0].f[0] = 3;
      io[0].f[1] = 2;
      io[1].f[0] = 1;
      io[1].f[1] = 0;

      std::cout << "in: p1=" << i << " p3=" << io << std::endl;
      XMP::FooSequence * r = basic->s4(i, o, io);
      std::cout << "out: r=" << *r << " p2=" << *o << " p3=" << io << std::endl;

      delete r;
      delete o;
    }

    // using CORBA _var helper
    {
      XMP::FooSequence i;
      XMP::FooSequence_var o;
      XMP::FooSequence io;

      // initializing the in parameter
      i.length(2);
      i[0].f[0] = 0;
      i[0].f[1] = 1;
      i[1].f[0] = 2;
      i[1].f[1] = 3;
      // initializing the inout parameter
      io.length(2);
      io[0].f[0] = 3;
      io[0].f[1] = 2;
      io[1].f[0] = 1;
      io[1].f[1] = 0;

      std::cout << "in: p1=" << i << " p3=" << io << std::endl;
      XMP::FooSequence_var r = basic->s4(i, o, io);
      std::cout << "out: r=" << r << " p2=" << o << " p3=" << io << std::endl;
    }

    //------------------------------------------------------------------------
    // invoke a a method with variable size structure type parameters.

    // using raw pointer
    {
      XMP::Bar i;
      XMP::Bar * o; // requires manual memory management
      XMP::Bar io;

      // initializing the in parameter
      i.b.length(2);
      i.b[0] = 0;
      i.b[1] = 1;
      // initializing the inout parameter
      io.b.length(2);
      io.b[0] = 3;
      io.b[1] = 2;

      std::cout << "in: p1=" << i << " p3=" << io << std::endl;
      XMP::Bar * r = basic->s5(i, o, io);
      std::cout << "out: r=" << (*r) << " p2=" << (*o) << " p3=" << io << std::endl;

      // memory management
      delete r;
      delete o;
    }

    // using var helper class
    {
      XMP::Bar i;
      XMP::Bar_var o;
      XMP::Bar io;

      // initializing the in parameter
      i.b.length(2);
      i.b[0] = 0;
      i.b[1] = 1;
      // initializing the inout parameter
      io.b.length(2);
      io.b[0] = 3;
      io.b[1] = 2;

      std::cout << "in: p1=" << i << " p3=" << io << std::endl;
      XMP::Bar_var r = basic->s5(i, o, io);
      std::cout << "out: r=" << (*r) << " p2=" << (*o) << " p3=" << io << std::endl;
    }

    //------------------------------------------------------------------------
    // invoke a a method with object reference parameters.
    {
      // object references are reference counted, 
      // so we need to duplicate them, when assigning it to multiple _var's
      CORBA::Object_var i = XMP::Basic::_duplicate(basic);
      CORBA::Object_var o, io = XMP::Basic::_duplicate(basic);
      //      std::cout << "in: p1=" << i << " p3=" << (*io) << std::endl;
      CORBA::Object_var r = basic->s6(i, o, io);
      //std::cout << "out: r=" << (*r) << " p2=" << (*o) << " p3=" << (*io) << std::endl;
    }

    //------------------------------------------------------------------------
    // invoking a method wich throws an exception.
    try {
      basic->e0();
    }
    catch (XMP::EFoo const& e) {
      // using a contained string read-only is straight forward:
      std::cout << "exception: " << e.what << std::endl;
    }

  }
  catch (CORBA::Exception const& e) {
    MIRO_LOG_OSTR(LL_CRITICAL, "CORBA exception:\n" << e);
  }
  return 0;
}
