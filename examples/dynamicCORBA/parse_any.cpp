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
#include "CORBA_TypesC.h"

#include "miro/Client.h"
#include "miro/AnyPrinter.h"

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
  Miro::Client client(argc, argv);
  CORBA::Any any;

  // any <<= (CORBA::ULong)99;

  XMP::Foo foo;

  foo.a = 3;
  foo.b = 3.141592654;
  foo.c = XMP::THREE;
  foo.d.length(2);
  foo.d[0] = "hi";
  foo.d[1] = "ho";
  foo.e[0] = 1.2;
  foo.e[1] = 1.3;
  foo.e[2] = 1.4;
  foo.e[3] = 1.5;

  any <<= foo;

  Miro::AnyPrinter printer;
  printer.print(cout, any);
  cout << endl;

  return 0;
}
