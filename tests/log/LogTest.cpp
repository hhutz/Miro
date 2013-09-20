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
#include "miro/MiroIO.h"
#include "miro/Log.h"


#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::cin;

int
main(int argc, char * argv[])
{
  if (!::Miro::Log::compiledWithLog())
  {
    cerr << "Error: Miro not compiled with Log support\n";
    return 1;
  }
  if (!::Miro::Log::compiledWithDebug())
  {
    cerr << "Error: Miro not compiled with Debug support\n";
    return 1;
  }

  ::Miro::Log::mask(0xFFFFFFFF);
  ::Miro::Log::level(::Miro::Log::MAX_DBG_LEVEL);

  ::Miro::Log::init(argc, argv);

  cout << "Testing log levels\n";
  MIRO_LOG(LL_EMERGENCY,"MIRO_LOG LL_EMERGENCY");
  MIRO_LOG(LL_ALERT,"MIRO_LOG LL_ALERT");
  MIRO_LOG(LL_CRITICAL,"MIRO_LOG LL_CRITICAL");
  MIRO_LOG(LL_ERROR,"MIRO_LOG LL_ERROR");
  MIRO_LOG(LL_WARNING,"MIRO_LOG LL_WARNING");
  MIRO_LOG(LL_NOTICE,"MIRO_LOG LL_NOTICE");

  cout << "Testing debug levels\n";
  MIRO_DBG(MIRO,LL_EMERGENCY,"MIRO_DBG LL_EMERGENCY");
  MIRO_DBG(MIRO,LL_ALERT,"MIRO_DBG LL_ALERT");
  MIRO_DBG(MIRO,LL_CRITICAL,"MIRO_DBG LL_CRITICAL");
  MIRO_DBG(MIRO,LL_ERROR,"MIRO_DBG LL_ERROR");
  MIRO_DBG(MIRO,LL_WARNING,"MIRO_DBG LL_WARNING");
  MIRO_DBG(MIRO,LL_NOTICE,"MIRO_DBG LL_NOTICE");
  MIRO_DBG(MIRO,LL_CTOR_DTOR,"MIRO_DBG LL_CTOR_DTOR");
  MIRO_DBG(MIRO,LL_DEBUG,"MIRO_DBG LL_DEBUG");
  MIRO_DBG(MIRO,LL_TRACE,"MIRO_DBG LL_TRACE");
  MIRO_DBG(MIRO,LL_PRATTLE,"MIRO_DBG LL_PRATTLE");

  return 0;
}
