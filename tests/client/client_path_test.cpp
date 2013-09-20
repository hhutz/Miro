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

#include <orbsvcs/CosNamingC.h>
#include <iostream>

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  Miro::Client client(argc, argv);

  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <path>\n";
    return 1;
  }

  CosNaming::Name n;
  client.path2Name(argv[1], n);

  std::cout << "fully qualified path in naming service:\n";
  for (unsigned int i = 0; i < n.length(); ++i)
    std::cout << "-- " << n[i].id << std::endl;
  
  return 0;
}
