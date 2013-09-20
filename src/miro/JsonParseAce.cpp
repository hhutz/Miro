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
#include "JsonParseAce.h"

#include <ace/INET_Addr.h>
#include <ace/Sched_Params.h>

#include <qstring.h>

#include "qt_compatibility.h"

namespace Miro
{
  using namespace std;

  void operator <<= (ACE_Time_Value& lhs, const Json::Value& node)
  {
    bool valid;
    QString value = node.asString().c_str();
#if QT_VERSION >= 0x040000
    int dot = value.indexOf('.');
#else
    int dot = value.find('.');
#endif

    QString sec = value;
    QString usec = "0";
    if (dot > 0) {
      sec = value.left(dot);
      if (dot != ((int)value.length()) - 1)
        usec = value.mid(dot + 1);
    }

    // bring usec to 6 digits
    usec += QString("000000");
    usec = usec.left(6);

    lhs.sec(sec.toULong(&valid));
    if (!valid)
      throw Exception("Parse exception (ACE_Time_Value)");
    lhs.usec(usec.toULong(&valid));
    if (!valid)
      throw Exception("Parse exception (ACE_Time_Value)");
  }

  Json::Value operator >>= (const ACE_Time_Value& lhs, Json::Value& _node)
  {
    static const QString zero("000000");

    QString s;
    QString us;
    s.setNum(lhs.sec());
    us.setNum(lhs.usec());
    QString all = s + "." + zero.left(6 - us.length()) + us;
#if QT_VERSION >= 0x040000
    _node = std::string(all.toAscii());
#else
    _node = std::string(all.latin1());
#endif
    return _node;
  }

  static const char* SERIAL_MODES[] = { "none", "even", "odd", "mark", "space", NULL };
  static const char* SERIAL_MODES_NONE = SERIAL_MODES[0];

  void
  operator<<= (ACE_TTY_IO::Serial_Params& _lhs, const Json::Value& _node)
  {
#define JSON_IN_FIELD(field) \
    if (_node.isMember(#field)) { \
      _lhs.field <<= _node[#field]; \
    }

    if (!_node.isNull()) {
      JSON_IN_FIELD(baudrate);
      JSON_IN_FIELD(ctsenb);
      JSON_IN_FIELD(databits);
      JSON_IN_FIELD(modem);
      if (_node.isMember("parityenb")) {
	bool parityenb;
	parityenb <<= _node["parityenb"];
	if (parityenb)
	  throw Exception("Can't upgrade parityenb to paritymode silently (can't interpret 'true' value)");
	_lhs.paritymode = SERIAL_MODES_NONE;
      }
      JSON_IN_FIELD(rcvenb);
      JSON_IN_FIELD(readtimeoutmsec);
      JSON_IN_FIELD(rtsenb);
      JSON_IN_FIELD(stopbits);
      JSON_IN_FIELD(xinenb);
      JSON_IN_FIELD(xofflim);
      JSON_IN_FIELD(xonlim);
      JSON_IN_FIELD(xoutenb);
      if (_node.isMember("paritymode")) {
	string s;
	s <<= _node["paritymode"];
	size_t j;
	for (j = 0; SERIAL_MODES[j] != NULL; ++j)
	  if (strcmp(s.c_str(), SERIAL_MODES[j]) == 0)
	    break;
	_lhs.paritymode = SERIAL_MODES[j];
      }
    }
  }
  Json::Value
  operator>>= (const ACE_TTY_IO::Serial_Params& lhs, Json::Value& _node)
  {
#define JSON_OUT_FIELD(field) \
    lhs.field >>= _node[#field];

    JSON_OUT_FIELD(baudrate);
    JSON_OUT_FIELD(paritymode);
    JSON_OUT_FIELD(databits);
    JSON_OUT_FIELD(stopbits);
    JSON_OUT_FIELD(readtimeoutmsec);
    JSON_OUT_FIELD(modem);
    JSON_OUT_FIELD(rcvenb);
    JSON_OUT_FIELD(ctsenb);
    JSON_OUT_FIELD(rtsenb);
    JSON_OUT_FIELD(xinenb);
    JSON_OUT_FIELD(xoutenb);

    return _node;
  }

  void operator <<= (ACE_INET_Addr& lhs, const Json::Value& node)
  {
    lhs.set(node.asString().c_str());
  }

  Json::Value operator >>= (const ACE_INET_Addr& lhs, Json::Value& _node)
  {
    _node = lhs.get_host_addr();
    return _node;
  }

  void operator <<= (ACE_Sched_Params& _lhs, const Json::Value& _node)
  {
#define JSON_SCHED_IN_FIELD(field, type) \
    if (_node.isMember(#field)) { \
      type p; \
      p <<= _node[#field]; \
      _lhs.field(p); \
    }

    if (!_node.isNull()) {
      JSON_SCHED_IN_FIELD(policy, int);
      JSON_SCHED_IN_FIELD(priority, int);
      JSON_SCHED_IN_FIELD(quantum, ACE_Time_Value);
    }
  }

  Json::Value operator >>= (const ACE_Sched_Params& lhs, Json::Value& _node)
  {
#define JSON_SCHED_OUT_FIELD(field) \
    lhs.field() >>= _node[#field];

    JSON_SCHED_OUT_FIELD(policy);
    JSON_SCHED_OUT_FIELD(priority);
    JSON_SCHED_OUT_FIELD(quantum);

    return _node;
  }
}
