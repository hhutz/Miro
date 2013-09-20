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
#include "TimeHelper.h"

#include <ace/Date_Time.h>
#include <ace/OS_NS_sys_time.h>

#include <iostream>
#include <sstream>

#if ((ACE_MAJOR_VERSION <= 5) && (ACE_MINOR_VERSION < 8)) 
std::ostream&
operator << (std::ostream& ostr, const ACE_Time_Value& rhs)
{
  ostr << rhs.sec() << ".";
  ostr.width(6);
  ostr.fill('0');
  ostr << ::abs(rhs.usec());
  return ostr;
}
#endif

std::istream&
operator >> (std::istream& istr, ACE_Time_Value& rhs)
{
  char c;
  unsigned long t;

  istr >> t >> c;
  rhs.sec(t);

  if (c != '.')
    return istr;

  if (istr >> t)
    rhs.usec(t);
  return istr;
}

namespace Miro
{
  std::string
  timeString()
  {
    ACE_Date_Time dt;
    dt.microsec(ACE_OS::gettimeofday().usec());

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.second() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << (dt.microsec() / 10000);

    return ostr.str();
  }

  std::string
  timeStringSec()
  {
    ACE_Date_Time dt;

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.second();

    return ostr.str();
  }

  std::string
  timeStringMinute()
  {
    ACE_Date_Time dt;

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute();

    return ostr.str();
  }

#if ACE_MAJOR_VERSION > 5 || (ACE_MAJOR_VERSION == 5  && ACE_MINOR_VERSION >= 4)

  std::string
  timeString(ACE_Time_Value const& _stamp)
  {
    ACE_Date_Time dt(_stamp);

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.second() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << (dt.microsec() / 10000);

    return ostr.str();
  }

  std::string
  timeStringSec(ACE_Time_Value const& _stamp)
  {
    ACE_Date_Time dt(_stamp);

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.second();

    return ostr.str();
  }

  std::string
  timeStringMinute(ACE_Time_Value const& _stamp)
  {
    ACE_Date_Time dt(_stamp);

    std::ostringstream ostr;

    ostr << dt.year() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.month() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.day() << "-";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.hour() << ".";
    ostr.width(2);
    ostr.fill('0');
    ostr << dt.minute();

    return ostr.str();
  }
#endif
}


