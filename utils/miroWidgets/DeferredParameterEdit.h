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
#ifndef DeferredParameterEdit_h
#define DeferredParameterEdit_h

#include "ParameterEdit.h"

// forward declarations
class ConfigFile;
class QPushButton;
class QString;

#include "miroWidgets_Export.h"

//! Deferred parameter edit field.
/*
 * The edit field is actually a button that, when pressen, launches
 * another input dialog. This is necessary for nested compound types,
 * vectors and sets.
 */
class miroWidgets_Export DeferredParameterEdit : public ParameterEdit
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef ParameterEdit Super;

public:
  //----------------------------------------------------------------------------
  // public types
  //----------------------------------------------------------------------------

  //! Contained type of the deferred edit field.
  enum EditType { NESTED_PARAMETER, VECTOR, SET };

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  DeferredParameterEdit(EditType _type,
			Miro::CFG::Parameter const& _parameter,
			QDomNode& _parentNode, QDomNode& _node,
			ItemXML * _parentItem, ItemXML * _item,
			QWidget * _parent, const char * _name);

  //! Inherited method.
  virtual void setXML();
  //! Inherited method.
  virtual bool modified() const;

  //! Translate the type name into an edit type.
  static EditType editType(QString const& _name);

protected slots:
  //----------------------------------------------------------------------------
  // protected slots
  //----------------------------------------------------------------------------

  //! Launch deferred input dialog.
  void deferredEdit();

protected:
  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Type information on the parameter.
  EditType type_;
  //! Type information of vector/set parameter.
  QString nestedType_;
  //! Flag indicating if the dialog is modified.
  bool modified_;

  //! A temporary document, holding a copy of the XML node.
  QDomDocument tmpDocument_;
  //! The parent node in the temporary document.
  QDomNode tmpParentNode_;
  //! The temporary dublicate of the node.
  QDomNode tmpNode_;
};

#endif // DeferredParameterEdit_h
