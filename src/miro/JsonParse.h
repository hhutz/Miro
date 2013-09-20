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
#ifndef JsonParse_h
#define JsonParse_h

#include "Exception.h"
#include "miroJson_Export.h"

#include "JsonCpp.h"

#include <string>
#include <vector>
#include <set>


#define EXCEPTION_SUBTYPE(type, parent) \
  class type : public parent \
  { \
    typedef parent Super; \
  public: \
    /*! Default constructor. */ \
    type() throw() : Super() {} \
    /*! Initializing constructor */ \
    type(const std::string& _what) throw() : Super(_what) {} \
  }

namespace Miro
{
  // forward declarations
  class Angle;
  class Text;

  // JSON parsing exceptions

  EXCEPTION_SUBTYPE(JsonException, Exception);

  miroJson_Export void operator <<= (bool& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (char& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (unsigned char& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (short& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (unsigned short& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (int& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (unsigned int& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (long& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (unsigned long& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (float& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (double& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (Angle& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (std::string& lhs, const Json::Value& node);
  miroJson_Export void operator <<= (Text& lhs, const Json::Value& node);

  template<class T>
  void operator <<= (std::vector<T>& lhs, const Json::Value& node)
  {
    lhs.clear();
    Json::Value::UInt i, n = node.size();
    for (i=0; i < n; ++i) {
	T t;
	t <<= node[i];
	lhs.push_back(t);
    }
  }

  template<class T>
  void operator <<= (std::set<T>& lhs, const Json::Value& node)
  {
    lhs.clear();
    Json::Value::UInt i, n = node.size();
    for (i=0; i < n; ++i) {
	T t;
	t <<= node[i];
	lhs.insert(t);
    }
  }

  miroJson_Export Json::Value operator >>= (const bool& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const char& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const unsigned char& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const short& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const unsigned short& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const int& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const unsigned int& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const long& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const unsigned long& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const float& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const double& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const Angle& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const std::string& lhs, Json::Value& node);
  miroJson_Export Json::Value operator >>= (const Text& lhs, Json::Value& node);

  template<class T>
  Json::Value operator >>= (const std::vector<T>& lhs, Json::Value& node)
  {
    node = Json::Value(Json::arrayValue); // initialize to empty array
    typename std::vector<T>::const_iterator f, l = lhs.end();
    Json::Value::UInt i;
    for (i = 0, f = lhs.begin(); f != l; ++f, ++i) {
      (*f) >>= node[i];
    }
    return node;
  }
  template<class T>
  Json::Value operator >>= (const std::set<T>& lhs, Json::Value& node)
  {
    node = Json::Value(Json::arrayValue); // initialize to empty array
    typename std::set<T>::const_iterator f, l = lhs.end();
    Json::Value::UInt i;
    for (i = 0, f = lhs.begin(); f != l; ++f, ++i) {
      (*f) >>= node[i];
    }
    return node;
  }
}


#endif // JsonParse_h
