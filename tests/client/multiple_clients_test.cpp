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
#include "miro/Log.h"
#include "miro/Client.h"

#include <iostream>

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);

  Miro::Client client1(argc, argv);
  Miro::Client client2;

  CORBA::ORB_var orb1 = client1.orb();
  CORBA::ORB_var orb2 = client2.orb();

  std::cout << "Orb ID client1: \"" << orb1->id() << "\"" <<std::endl;
  std::cout << "Orb ID client2: \"" << orb2->id() << "\"" << std::endl;

  return 0;
}
