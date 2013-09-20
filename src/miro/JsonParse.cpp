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
#include "JsonParse.h"
#include "Angle.h"
#include "Text.h"

#include <qstring.h>

#include <algorithm>
#include <climits>
#include <limits>

#include "qt_compatibility.h"

namespace Miro
{
  using namespace std;

  void operator <<= (bool& lhs, const Json::Value& node)
  {
    lhs = node.asBool();
  }

  Json::Value operator >>= (const bool& lhs, Json::Value& _node)
  {
    _node = lhs;
    return _node;
  }

  void operator <<= (char& lhs, const Json::Value& node)
  {
    QString value = node.asString().c_str();
    if (value.length() != 1)
      throw Miro::Exception("expected char");

#if QT_VERSION >= 0x040000
    lhs = value[0].toAscii();
#else
    lhs = value[0].latin1();
#endif
  }

  Json::Value operator >>= (const char& lhs, Json::Value& _node)
  {
    std::string s(1, lhs);
    _node = s;
    return _node;
  }

  void operator <<= (signed char& lhs, const Json::Value& node)
  {
    Json::Value::Int v = node.asInt();
    if (v < SCHAR_MIN || v > SCHAR_MAX)
      throw Miro::Exception("expected signed char");
    lhs = v;
  }

  Json::Value operator >>= (const signed char& lhs, Json::Value& _node)
  {
    _node = (Json::Value::Int) lhs;
    return _node;
  }

  void operator <<= (unsigned char& lhs, const Json::Value& node)
  {
    Json::Value::UInt v = node.asUInt();
    if (v > UCHAR_MAX)
      throw Miro::Exception("expected unsigned char");
    lhs = v;
  }

  Json::Value operator >>= (const unsigned char& lhs, Json::Value& _node)
  {
    _node = (Json::Value::UInt) lhs;
    return _node;
  }

#define JSON_PARSE_QSTRING_IMPL(type, asFunc, castTo) \
  void operator <<= (type& lhs, const Json::Value& node) \
  { \
    lhs = node.asFunc(); \
  } \
  Json::Value operator >>= (const type& lhs, Json::Value& _node) \
  { \
    _node = (castTo) lhs; \
    return _node; \
  }

  JSON_PARSE_QSTRING_IMPL(short, asInt, Json::Value::Int);
  JSON_PARSE_QSTRING_IMPL(unsigned short, asUInt, Json::Value::UInt);
  JSON_PARSE_QSTRING_IMPL(int, asInt, Json::Value::Int);
  JSON_PARSE_QSTRING_IMPL(unsigned int, asUInt, Json::Value::UInt);
  JSON_PARSE_QSTRING_IMPL(long, asInt, Json::Value::Int);
  JSON_PARSE_QSTRING_IMPL(unsigned long, asUInt, Json::Value::UInt);
  JSON_PARSE_QSTRING_IMPL(float, asDouble, double);
  JSON_PARSE_QSTRING_IMPL(double, asDouble, double);

  void operator <<= (Angle& lhs, const Json::Value& node)
  {
    lhs = deg2Rad(node.asDouble());
  }

  Json::Value operator >>= (const Angle& lhs, Json::Value& _node)
  {
    _node = (double) rad2Deg(lhs);
    return _node;
  }

  void operator <<= (std::string& lhs, const Json::Value& node)
  {
    lhs = node.asString();
  }

  Json::Value operator >>= (const std::string& lhs, Json::Value& _node)
  {
    _node = lhs;
    return _node;
  }

  void operator <<= (Text& lhs, const Json::Value& node)
  {
    lhs = node.asString();
  }

  Json::Value operator >>= (const Text& lhs, Json::Value& _node)
  {
    _node = lhs;
    return _node;
  }
}
