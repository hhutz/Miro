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
#include "miro/TimeHelper.h"

#include <ace/Time_Value.h>
#include <ace/Date_Time.h>
#include <ace/Version.h>

#include <string>
#include <iostream>
#include <sstream>

int main (int argc, char * argv[])
{
#if ACE_MAJOR_VERSION > 5 || (ACE_MAJOR_VERSION == 5  && ACE_MINOR_VERSION >= 4)
  unsigned int sec = 0;
  unsigned int usec = 0;
  ACE_Time_Value t;

  if (argc < 2 || argc >3) {
    std::cout << "usage: " << argv[0] << " <sec> .|' ' [<usec>]\n";
    return 1;
  }

  if (argc == 2) {
    std::stringstream s(argv[1]);
    s >> t;
  }
  else if (argc == 3) {

    sec = atoi(argv[1]);
    usec = atoi(argv[2]);

    ACE_Time_Value stamp(sec, usec);
    t = stamp;
  }

  std::cout << Miro::timeString(t) << std::endl;

  return 0;
#else
  std::cout << "This is supported only by ACE Version 5.4 or higher!\n";

  return 1;
#endif

}
