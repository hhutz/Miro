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
#ifndef miroTimeHelper_hh
#define miroTimeHelper_hh

#include "miroCore_Export.h"

#include <ace/Time_Value.h>
#include <ace/Version.h>

#include <iosfwd>
#include <string>

#if ((ACE_MAJOR_VERSION <= 5) && (ACE_MINOR_VERSION < 8)) 
miroCore_Export std::ostream& operator << (std::ostream& ostr, const ACE_Time_Value& rhs);
#endif
miroCore_Export std::istream& operator >> (std::istream& istr, ACE_Time_Value& rhs);

namespace Miro
{
  miroCore_Export std::string timeString();
  miroCore_Export std::string timeStringSec();
  miroCore_Export std::string timeStringMinute();

#if ACE_MAJOR_VERSION > 5 || (ACE_MAJOR_VERSION == 5  && ACE_MINOR_VERSION >= 4)
  miroCore_Export std::string timeString(ACE_Time_Value const& _stamp);
  miroCore_Export std::string timeStringSec(ACE_Time_Value const& _stamp);
  miroCore_Export std::string timeStringMinute(ACE_Time_Value const& _stamp);
#endif
}

#endif




