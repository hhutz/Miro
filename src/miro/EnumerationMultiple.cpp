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
#include "EnumerationMultiple.h"
#include "Exception.h"

#include <iostream>
#include <algorithm>

using std::string;
using std::vector;


namespace Miro
{
  EnumerationMultiple::EnumerationMultiple(string const& _enum,
      string const& _values)
  throw(EInvalid, EDuplicates) :
      enum_(tokenizer(_enum)),
      values_(tokenizer(_values))
  {
    makeSet(enum_);
    makeSet(values_);
    checkAvailability(enum_);
  }


  EnumerationMultiple::EnumerationMultiple(vector<string> const& _enum,
      vector<string> const& _values)
  throw(EInvalid, EDuplicates) :
      enum_(_enum),
      values_(_values)
  {
    makeSet(enum_);
    makeSet(values_);
    checkAvailability(enum_);
  }


  void
  EnumerationMultiple::value(string const& _value) throw(EInvalid, EDuplicates)
  {
    StringVector v(tokenizer(_value));
    makeSet(v);
    checkAvailability(v);

    enum_.swap(v);
  }


  void
  EnumerationMultiple::value(vector<string> const& _value) throw(EInvalid, EDuplicates)
  {
    StringVector v(_value);
    makeSet(v);
    checkAvailability(v);

    enum_.swap(v);
  }

  std::vector<std::string>
  EnumerationMultiple::tokenizer(std::string const& _values)
  {
    int pos = 0;
    std::vector<std::string> values;
    while (_values.find(" ", pos) < _values.size()) {
      string tmp = _values.substr(pos, _values.find(" ", pos) - pos);
      if ((tmp != " ") && (tmp != ""))
        values.push_back(tmp);
      pos = _values.find(" ", pos) + 1;
    }
    string tmp = _values.substr(pos, _values.size());
    if ((tmp != " ") && (tmp != ""))
      values.push_back(tmp);

    return values;
  }

  void
  EnumerationMultiple::makeSet(vector<string>& v) throw(EDuplicates)
  {
    sort(v.begin(), v.end());
    StringVector::iterator i = unique(v.begin(), v.end());
    if (i != v.end())
      throw EDuplicates("Duplicates in enumeration set.");
  }

  void
  EnumerationMultiple::checkAvailability(vector<string> const& v)  throw(EInvalid)
  {
    if (!includes(values_.begin(), values_.end(), v.begin(), v.end())) {
      throw EInvalid("Enumeration: tried to set value not in list of available values");
    }
  }


  std::ostream&
  operator<<(std::ostream& ostr, EnumerationMultiple const& _enum)
  {
    vector<string> const& v = _enum.value();
    vector<string>::const_iterator first, last = v.end();

    for (first = v.begin(); first != last; ++first)
      ostr << *first;
    return ostr;
  }


  std::istream&
  operator>>(std::istream& istr, EnumerationMultiple& _enum)
  {
    std::string tmp;
    istr >> tmp;
    _enum.value(tmp); // this way, error handling is done by value()

    return istr;
  }
}

