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
#ifndef ParameterList_h
#define ParameterList_h

#include "ParameterXML.h"
#include "params/Parameter.h"

#include <qstring.h>

#include "miroWidgets_Export.h"

// forward declarations
class ConfigFile;
class Q3ListView;
class Q3ListViewItem;

//! Class representing vector and set items.
class miroWidgets_Export ParameterList : public ParameterXML
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

  //! Type of the parameter list.
  enum Type { NONE, VECTOR, SET};

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  ParameterList(Miro::CFG::Parameter const& _param,
		QDomNode const& _node,
		Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
		QObject * _parent, const char * _name);

  //! Initializing constructor.
  ParameterList(Miro::CFG::Parameter const& _param,
		QDomNode const& _node,
		Q3ListView * _list, Q3ListViewItem * _pre,
		QObject * _parent, const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  //! Inherited method.
  virtual void init();
  //! Inherited method.
  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------------------

  //! Convert type string into a Type.
  static Type typeFromName(QString const& _type);

protected:
  //! Description of the parameter.
  Miro::CFG::Parameter param_;
  //! Type description of the parameter.
  Type type_;
  //! std::vector<int> -> int
  QString nestedTypeName_;
};

#endif
