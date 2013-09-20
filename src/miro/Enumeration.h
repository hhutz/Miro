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
#ifndef miro_Enumeration_h
#define miro_Enumeration_h

#include "Exception.h"

#include <string>
#include <vector>

namespace Miro
{
  class Enumeration;

  std::ostream& operator<<(std::ostream& ostr, Enumeration const& _enum);
  std::istream& operator>>(std::istream& istr,  Enumeration& _enum);

  class Enumeration
  {
  public:
    MIRO_EXCEPTION_TYPE(EInvalid);
    MIRO_EXCEPTION_TYPE(EDuplicates);

    typedef std::vector<std::string> StringVector;

    Enumeration(std::string const& _enum, std::string const& _values)
    throw(EInvalid, EDuplicates);
    Enumeration(std::string const &_enum, std::vector<std::string> const& _values)
    throw(EInvalid, EDuplicates);

    void value(std::string const& _value) throw(EInvalid, EDuplicates);
    std::string const& value() const throw();
    StringVector const& assortment() const throw();

  private:
    void makeSet() throw(EDuplicates);

    StringVector::const_iterator enum_;
    StringVector values_;
  };

  inline
  std::string const&
  Enumeration::value() const throw()
  {
    return *enum_;
  }

  inline
  std::vector<std::string> const&
  Enumeration::assortment() const throw()
  {
    return values_;
  }
}

#endif // miro_Enumeration_h
