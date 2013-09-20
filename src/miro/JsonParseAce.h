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
#ifndef JsonParseAce_h
#define JsonParseAce_h

#include "JsonParse.h"
#include "miroJson_Export.h"

#include <ace/TTY_IO.h>

// forward declarations
class ACE_Time_Value;
class ACE_INET_Addr;
class ACE_Sched_Params;

namespace Miro
{
  miroJson_Export void operator <<= (ACE_Time_Value& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (ACE_TTY_IO::Serial_Params& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (ACE_INET_Addr& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (ACE_Sched_Params& lhs, const Json::Value& node);

  miroJson_Export Json::Value operator >>= (const ACE_Time_Value& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const ACE_TTY_IO::Serial_Params& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const ACE_INET_Addr& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const ACE_Sched_Params& lhs, Json::Value& node);
}


#endif // JsonParseAce_h
