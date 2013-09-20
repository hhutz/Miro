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

// add another log category
namespace Miro
{
  namespace LC
  {
    unsigned int XMP = 0x00008000 ;
    ACE_TCHAR const * const XMP_NAME = ACE_TEXT("XMP");
  }
}
      

int main(int argc, char * argv[])
{
  Miro::Log::register_category(Miro::LC::XMP, Miro::LC::XMP_NAME);
  Miro::Log::init(argc, argv);

  MIRO_LOG(LL_EMERGENCY, "This is an example log message of priority emergency.");
  MIRO_LOG(LL_ALERT, "This is an example log message of priority alert.");
  MIRO_LOG(LL_CRITICAL, "This is an example log message of priority critical.");
  MIRO_LOG(LL_ERROR, "This is an example log message of priority error.");
  MIRO_LOG(LL_WARNING, "This is an example log message of priority warning.");
  MIRO_LOG(LL_NOTICE, "This is an example log message of priority notice.");
  MIRO_DBG(MIRO, LL_DEBUG, "This is a debug message of category MIRO.");
  MIRO_DBG(XMP, LL_DEBUG, "This is a debug message of category XMP.");

  return 0;
}
