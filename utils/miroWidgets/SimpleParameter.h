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
#ifndef SimpleParameter_h
#define SimpleParameter_h

#include "ParameterXML.h"
#include "params/Parameter.h"

#include <qstring.h>

#include "miroWidgets_Export.h"

// forward declarations
class ConfigFile;
class Q3ListView;
class Q3ListViewItem;

class miroWidgets_Export SimpleParameter : public ParameterXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef ParameterXML Super;
public:

  //----------------------------------------------------------------------------
  // public types
  //----------------------------------------------------------------------------

  enum Type {
    NONE,
    BOOL, CHAR, SHORT, USHORT, INT, UINT, LONG, ULONG,
    FLOAT, DOUBLE,
    ANGLE, MIRO_ANGLE, STRING, TEXT,
    ACE_TIME_VALUE, ACE_INET_ADDR,
    ENUMERATION, ENUMERATIONMULTIPLE
  };

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  SimpleParameter(Miro::CFG::Parameter const& _param,
		  QDomNode const& _node,
		  Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
		  QObject * _parent, const char * _name);

  SimpleParameter(Miro::CFG::Parameter const& _param,
		  QDomNode const& _node,
		  Q3ListView * _list, Q3ListViewItem * _pre,
		  QObject * _parent, const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------------------

  static Type typeFromName(QString const& _name);

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  static QString const XML_ATTRIBUTE_VALUE;

protected:
  Miro::CFG::Parameter param_;
};

#endif
