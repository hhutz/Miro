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
#ifndef MiroIO_h
#define MiroIO_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
#include <set>
#include <iostream>

#include "miroCore_Export.h"

#define OSTR_OPERATOR_DECL(x) \
  class x; \
  miroCore_Export extern std::ostream & \
  operator<<(std::ostream &ostr, x const &rhs)
#define ISTR_OPERATOR_DECL(x) \
  miroCore_Export extern std::istream & \
  operator<<(std::istream &ostr, x &rhs)

OSTR_OPERATOR_DECL(ACE_INET_Addr);
OSTR_OPERATOR_DECL(ACE_Sched_Params);

namespace std
{
  // set and vector ostream operators
  template<class T>
  std::ostream &
  operator<<(std::ostream &ostr, const std::vector<T> &rhs)
  {
    typename std::vector<T>::const_iterator first, last = rhs.end();
    for (first = rhs.begin(); first != last; ++first) {
      ostr << (*first) << std::endl;
    }
    return ostr;
  }
  template<class T>
  std::ostream &
  operator<<(std::ostream &ostr, const std::set<T> &rhs)
  {
    typename std::set<T>::const_iterator first, last = rhs.end();
    for (first = rhs.begin(); first != last; ++first) {
      ostr << (*first) << std::endl;
    }
    return ostr;
  }
}

namespace Miro
{
  // exceptions

  // basic types
  // interface specific types
}

#endif




