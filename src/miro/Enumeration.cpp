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
#include "Enumeration.h"

#include <iostream>
#include <algorithm>

namespace Miro
{
  using namespace std;

  Enumeration::Enumeration(string const& _enum, string const& _values) throw(EInvalid, EDuplicates)
  {
    // tokenize input values
    int pos = 0;
    while (_values.find(" ", pos) < _values.size()) {
      string tmp = _values.substr(pos, _values.find(" ", pos) - pos);
      if ((tmp != " ") && (tmp != ""))
        values_.push_back(tmp);
      pos = _values.find(" ", pos) + 1;
    }
    string tmp = _values.substr(pos, _values.size());
    if ((tmp != " ") && (tmp != ""))
      values_.push_back(tmp);

    makeSet();
    value(_enum);
  }


  Enumeration::Enumeration(string const& _enum, vector<string> const& _values) throw(EInvalid, EDuplicates) :
      values_(_values)
  {
    makeSet();
    value(_enum);
  }

  void
  Enumeration::value(string const& _value) throw(EInvalid, EDuplicates)
  {
    StringVector::const_iterator i;

    i = lower_bound(values_.begin(), values_.end(), _value);
    if (i == values_.end())
      throw EInvalid("Enumeration: tried to set value not in list of available values: " + _value);

    enum_ = i;
  }

  void
  Enumeration::makeSet() throw(EDuplicates)
  {
    sort(values_.begin(), values_.end());
    StringVector::iterator i = unique(values_.begin(), values_.end());
    if (i != values_.end())
      throw EDuplicates("Duplicates in enumeration set.");
  }

  ostream&
  operator<<(std::ostream& ostr, Enumeration const& _enum)
  {
    return ostr << _enum.value();
  }

  istream&
  operator>>(std::istream& istr, Enumeration& _enum)
  {
    string tmp;
    istr >> tmp;
    _enum.value(tmp); // this way, error handling is done by value()
    return istr;
  }
}

