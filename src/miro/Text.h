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
#ifndef miro_Text_h
#define miro_Text_h

#include <string>

namespace Miro
{
  /** Text class for the parameters framework.
   *
   * In contrast to the string parameter, this provides a multi-line edit field
   * and is stored in a DomText node, instead of an attribute.
   */
  class Text : public std::string
  {
  public:
    Text(Text const& str) : std::string(str) {}
    Text(std::string const& str) : std::string(str) {}
    Text& operator = (Text const& str) {
      std::string::operator=(str); return *this;
    }
    Text& operator = (std::string const& str) {
      std::string::operator=(str); return *this;
    }
  };
}

#endif // miro_Text_h
