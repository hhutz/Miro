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
#ifndef ParameterEdit_h
#define ParameterEdit_h

#include <qobject.h>
#include <qdom.h>

#include "miroWidgets_Export.h"

// forward declarations
namespace Miro {
  namespace CFG {
    struct Parameter;
  }
}
class ParameterDialog;
class ItemXML;

//! Virtual base class for the edit fields.
class miroWidgets_Export ParameterEdit : public QObject
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef QObject Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  ParameterEdit(Miro::CFG::Parameter const& _parameter,
		QDomNode& _parentNode, QDomNode& _node,
		ItemXML * _parentItem, ItemXML * _item,
		QWidget * _parent, const char * _name);

  QWidget * editWidget();
  //! Updates the xml tree with the data provided in the edit field.
  virtual void setXML() = 0;
  //! True, if the user entered something into the dialog.
  virtual bool modified() const = 0;

protected:
  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Description of the parameter to edit.
  Miro::CFG::Parameter const& parameter_;

  //! Parent node of the parameter in the xml tree.
  /** Needed if the parameter does not have an xml entry yet. */
  QDomNode parentNode_;
  //! The xml node of the parameter.
  QDomNode node_;

  QWidget * editWidget_;

  //! Parent item in the QListView.
  /** Needed if the parameter does not have an xml entry yet. */
  ItemXML * parentItem_;
  //! Item in the QListView.
  ItemXML * item_;

  //----------------------------------------------------------------------------
  // protected static data
  //----------------------------------------------------------------------------

  //! Tag denoting a parameter.
  static QString const XML_TAG_PARAMETER;
  //! The key attribute of the parameter.
  static QString const XML_ATTRIBUTE_KEY;
  //! The value attribute of the parameter.
  static QString const XML_ATTRIBUTE_VALUE;
};

inline
QWidget *
ParameterEdit::editWidget() {
  return editWidget_;
}
#endif // ParameterEdit_h
