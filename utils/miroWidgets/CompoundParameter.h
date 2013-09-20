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
#ifndef CompoundParameter_h
#define CompoundParameter_h

#include "ParameterXML.h"
#include "params/Type.h"

#include "miroWidgets_Export.h"

//! Class representing compound parameter items.
class miroWidgets_Export CompoundParameter : public ParameterXML
{
  Q_OBJECT
  
  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef ParameterXML Super;
public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  CompoundParameter(Miro::CFG::Type const& _type,
		    QDomNode const& _node,
		    Q3ListViewItem * _listViewItem, Q3ListViewItem * _pre,
		    QObject * _parent, const char * _name);

  //! Initializing constructor. For toplevel QListViewItem.
  CompoundParameter(Miro::CFG::Type const& _type,
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

  //! Translates a XML node and name into a CFG::Type.
  static Miro::CFG::Type const& type(QDomNode const& _node,
				     QString const& _name);

protected:
  //! Type description of the compound parameter.
  Miro::CFG::Type type_;
};

#endif
