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
#ifndef SingleParameterDialog_h
#define SingleParameterDialog_h

#include "DialogXML.h"
#include "params/Parameter.h"

#include <vector>

#include "miroWidgets_Export.h"

// forward declarations
class ParameterEdit;
class ConfigFile;

//! Dialog for editing a single parameter value.
class miroWidgets_Export SingleParameterDialog : public DialogXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef DialogXML Super;

public:
  //----------------------------------------------------------------------------
  // pbulic methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  SingleParameterDialog(Miro::CFG::Parameter const & _parameter,
			QDomNode const& _node,
			ItemXML * _parentItem,
			ItemXML * _item,
			QWidget * _parent, const char * _name);

  //! Inherited method.
  virtual void setXML();

protected:
  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! The parameter description.
  Miro::CFG::Parameter parameter_;
  //! The edit field.
  ParameterEdit * edit_;
};

#endif
