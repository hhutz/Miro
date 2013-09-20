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
#ifndef miro_EnumerationMultiple_h
#define miro_EnumerationMultiple_h

#include "Exception.h"

#include <string>
#include <vector>

namespace Miro
{
  class EnumerationMultiple;

  std::ostream& operator<<(std::ostream& ostr, EnumerationMultiple const& _enum);
  std::istream& operator>>(std::istream& istr,  EnumerationMultiple& _enum);

  class EnumerationMultiple
  {
  public:
    MIRO_EXCEPTION_TYPE(EInvalid);
    MIRO_EXCEPTION_TYPE(EDuplicates);

    typedef std::vector<std::string> StringVector;

    EnumerationMultiple(std::string const& _enum, std::string const& _values)
    throw(EInvalid, EDuplicates);
    EnumerationMultiple(std::vector<std::string> const& _enum, std::vector<std::string> const& _values)
    throw(EInvalid, EDuplicates);

    void value(std::string const& _value) throw(EInvalid, EDuplicates);
    void value(std::vector<std::string> const& _value) throw(EInvalid, EDuplicates);

    std::vector<std::string> const& value() const throw();
    std::vector<std::string> const& assortment() const throw();

  private:

    static void makeSet(std::vector<std::string>& v) throw(EDuplicates);
    static std::vector<std::string> tokenizer(std::string const& _values);
    void checkAvailability(std::vector<std::string> const& v) throw(EInvalid);

    std::vector<std::string> enum_;
    std::vector<std::string> values_;
  };

  inline
  std::vector<std::string> const&
  EnumerationMultiple::value() const throw()
  {
    return enum_;
  }

  inline
  std::vector<std::string> const&
  EnumerationMultiple::assortment() const throw()
  {
    return values_;
  }

}

#endif // miro_EnumerationMultiple_h
